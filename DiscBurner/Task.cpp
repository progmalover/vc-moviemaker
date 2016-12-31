#include "StdAfx.h"
#include "Task.h"
#include "Converter.h"
#include "AVIReader.h"
#include "ProfileManager.h"
#include "FileList.h"
#include "filepath.h"
#include "Options.h"
#include "EncoderFFmpeg.h"
#include "EncoderAC3.h"
#include <Shlwapi.h>

#include "../Activation/Activation.h"

#ifdef AC3_AFTEN 
 float  g_fEncoderAC3         = 0.0f;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _BD
	#ifdef _DEBUG
		BOOL g_bIsActivated = TRUE;
	#else
		BOOL g_bIsActivated = FALSE;
	#endif
#endif

DEFINE_REGISTERED_MESSAGE(WM_TASK_ADD)
DEFINE_REGISTERED_MESSAGE(WM_TASK_COMPLETED)
DEFINE_REGISTERED_MESSAGE(WM_TASK_FAILED)
DEFINE_REGISTERED_MESSAGE(WM_TASK_CANCELED)

CBitmap CTask::m_bmpAudio;
CTask::CTask(LPCTSTR lpszFile)
{
	::InitializeCriticalSection(&m_csTask);

	m_hCancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hPauseEvent =  ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hPauseEvent2 =  ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hResumeEvent =  ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hResumeEvent2 =  ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_lpfnConvProc = ConvProc;
	m_hConvThread = NULL;

	m_nStatus = CONV_STATUS_NONE;
	m_nUpdate = 0;

	m_bCanceled = FALSE;
	m_bPaused = FALSE;

	m_nStartTick = 0;
	m_nPauseTick = 0;

	m_strSourceFile = lpszFile;
	
	// An union
	m_fEncodedTime = 0.0;
	ASSERT(abs(m_fEncodedPercent - 0.0) < FLT_EPSILON);

	m_fTrimStart = 0.0;
	m_fTrimEnd = 0.0;
	m_bTrimmed = FALSE;
    m_fDuration = 0.0;

	m_pEncoder = NULL;

	if (m_bmpAudio.m_hObject == NULL)
	{
		HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_FILE_LIST_MUSIC), IMAGE_BITMAP, 
			0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
		m_bmpAudio.Attach(hBitmap);
	}

#ifdef _DEBUG
	m_dwLock = 0;
	m_dwTlsIndex = ::TlsAlloc();
	if (m_dwTlsIndex == TLS_OUT_OF_INDEXES)
		ReportLastError();
#endif

#ifdef AC3_AFTEN
	m_fVideoStream = 0.0;
#endif
}

CTask::~CTask(void)
{
	::DeleteCriticalSection(&m_csTask);
	::CloseHandle(m_hCancelEvent);

	if (m_bmpNonBlankFrame.m_hObject == m_bmpFirstFrame.m_hObject)
		m_bmpNonBlankFrame.Detach();

	if (!m_strTempFolder.IsEmpty() && ::PathIsDirectory(m_strTempFolder))
		::RemoveDirectory(m_strTempFolder, TRUE);

	if (!m_strSourceFileAlt.IsEmpty())
	{
		::DeleteFile(m_strSourceFileAlt);
		CString strTempFolder = ::FileStripFileName(m_strSourceFileAlt);
		if (::PathIsDirectory(strTempFolder))
			::RemoveDirectory(strTempFolder);
	}

	//ClearOldLog();
	//ClearNewLog();

#ifdef _DEBUG
	LPVOID lpData = ::TlsGetValue(m_dwTlsIndex);
	::LocalFree(lpData);
	::TlsFree(m_dwTlsIndex);
	ASSERT(m_dwLock == 0);
#endif
}

void CTask::ClearTasks()
{
	m_Atoms.clear();
}

void CTask::DeleteFiles()
{
	for(int n=0;n<(int)m_Atoms.size();n++)
	{
		CString strOutputFile=m_Atoms[n].outfile;
		if (!strOutputFile.IsEmpty() && ::PathFileExists(strOutputFile))
		{
			::DeleteFile(strOutputFile);
			m_Atoms[n].outfile.Empty();
		}
	}
}

BOOL CTask::Lock()
{
#ifdef _DEBUG
	LPVOID lpData = ::TlsGetValue(m_dwTlsIndex);
	if (lpData == NULL)
	{
		lpData = ::LocalAlloc(LPTR, 4);
		VERIFY(::TlsSetValue(m_dwTlsIndex, lpData));
		ASSERT(::TlsGetValue(m_dwTlsIndex) == lpData);
	}

	ASSERT(*((DWORD *)lpData) == 0);
	(*((DWORD *)lpData))++;
#endif

	::EnterCriticalSection(&m_csTask);

#ifdef _DEBUG
	ASSERT(m_dwLock == 0);
	m_dwLock++;
#endif

	return TRUE;
}

void CTask::Unlock()
{
#ifdef _DEBUG
	LPVOID lpData = ::TlsGetValue(m_dwTlsIndex);
	(*((DWORD *)lpData))--;
#endif

#ifdef _DEBUG
	ASSERT(m_dwLock == 1);
	m_dwLock--;
#endif

	::LeaveCriticalSection(&m_csTask);
}

BOOL CTask::CreateTempFolder()
{
	return ::CreateTempFolder(NULL, m_strTempFolder);
}

BOOL CTask::PrepareThumbnail()
{
#define CHECKRESULT(str) {                                    \
	if (FAILED(hr))                                           \
	{                                                         \
	    CLog::Instance()->AppendLog("%s , hr = 0x%08x.\r\n", str,hr); \
	    goto _cleanup;                                        \
     }                                                        \
}
	ASSERT(m_bmpFirstFrame.m_hObject == NULL && m_bmpNonBlankFrame.m_hObject == NULL);
	ASSERT(!m_strTempFolder.IsEmpty() && ::PathIsDirectory(m_strTempFolder));

	BOOL bUnknownFps = (fabs(m_mi.m_fFrameRate - 0.0) < FLT_EPSILON);
	float fFramerate = min(MAX_FRAMERATE, max(MIN_FRAMERATE, (bUnknownFps ? DEF_FRAMERATE : m_mi.m_fFrameRate)));

	CString strScript;

	// Create preview AVS
	try
	{
		CString strTemplate;
		strTemplate.Format("%s\\Scripts\\Preview.avs", GetModuleFilePath());

		CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
		ULONGLONG len = file.GetLength();
		file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
		strScript.ReleaseBuffer();

		strScript.Replace("$(modulepath)", GetModuleFilePath());
		strScript.Replace("$(input)", m_strSourceFileAlt.IsEmpty() ? m_strSourceFile : m_strSourceFileAlt);

		CString strFileName = m_strSourceFile;
		PathRemoveExtension(strFileName.GetBuffer());
		strFileName.ReleaseBuffer();
		
		CString strSubtitle;
		FindSubtitle(strSubtitle);
		strScript.Replace("$(subtitle)", strSubtitle);

		CString strFramerate;
		strFramerate.Format("%.3f", fFramerate);
		strScript.Replace("$(vframerate)", strFramerate);

		strScript.Replace("$(seekzero)", bUnknownFps ? "true" : "false");

		CString strWidth;
		CString strHeight;
		strWidth.Format("%d", MAX_THUMBNAIL_WIDTH * 4);
		strHeight.Format("%d", MAX_THUMBNAIL_HEIGHT * 4);
		strScript.Replace("$(width)", strWidth);
		strScript.Replace("$(height)", strHeight);

		CString strAspect;
		strAspect.Format("%.6f", m_mi.m_fDisplayRatio);
		strScript.Replace("$(aspect)", strAspect);

		ASSERT(strScript.Find('$') < 0);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	CString strPreview;
	strPreview.Format("%s\\Preview.avs", m_strTempFolder);
	try
	{
		CStdioFile file(strPreview, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(strScript, strScript.GetLength());
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	m_strPreview = strPreview;

	// Create thumbnail AVS
	CString strThumbnail;
	strThumbnail.Format("%s\\Thumbnail.avs", m_strTempFolder);

	CString strLog;
	strLog.Format("%s\\Thumbnail.log", m_strTempFolder);

	try
	{
		CString strTemplate;
		strTemplate.Format("%s\\Scripts\\Thumbnail.avs", GetModuleFilePath());

		CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
		ULONGLONG len = file.GetLength();
		file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
		strScript.ReleaseBuffer();
		
		strScript.Replace("$(modulepath)", GetModuleFilePath());
		strScript.Replace("$(log)", strLog);
		strScript.Replace("$(input)", m_strSourceFileAlt.IsEmpty() ? m_strSourceFile : m_strSourceFileAlt);

		CString strFileName = m_strSourceFile;
		PathRemoveExtension(strFileName.GetBuffer());
		strFileName.ReleaseBuffer();

		CString strSubtitle;
		FindSubtitle(strSubtitle);
		strScript.Replace("$(subtitle)", strSubtitle);

		CString strWidth;
		CString strHeight;
		strWidth.Format("%d", MAX_THUMBNAIL_WIDTH * 4);
		strHeight.Format("%d", MAX_THUMBNAIL_HEIGHT * 4);
		strScript.Replace("$(width)", strWidth);
		strScript.Replace("$(height)", strHeight);
		
		CString strAspect;
		strAspect.Format("%.6f", m_mi.m_fDisplayRatio);
		strScript.Replace("$(aspect)", strAspect);

		ASSERT(strScript.Find('$') < 0);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	try
	{
		CStdioFile file(strThumbnail, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(strScript, strScript.GetLength());
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	HRESULT hr = S_OK;

	CAVIFileReader ar;
	hr = ar.Init(strThumbnail, fFramerate, 1);
	CHECKRESULT("ar.Init(strThumbnail, fFramerate, 1) ");

	// get audio format
	WAVEFORMATEX *wfx = NULL;
	hr = ar.GetFormat_Audio(&wfx);
	CHECKRESULT("ar.GetFormat_Audio(&wfx)");

	// get video format
	BITMAPINFOHEADER *bmih = NULL;
	hr = ar.GetFormat_Video(&bmih);
	CHECKRESULT("ar.GetFormat_Video(&bmih)");

	hr = (wfx == NULL && bmih == NULL) ? E_FAIL : S_OK;
	CHECKRESULT(" (wfx == NULL && bmih == NULL) ? E_FAIL : S_OK");

	BOOL bHasVideo = (bmih != NULL);
	if (bHasVideo)
	{
		HDC hDC = ::GetDC(NULL);
		if (hDC != NULL )
		{
			// Get frame count.
			INT32 nTotalFrames = 0;
			hr =  ar.GetFrameCount(&nTotalFrames);
			CHECKRESULT("ar.GetFrameCount(&nTotalFrames)");

			hr = nTotalFrames > 0 ? S_OK : E_FAIL;
			CHECKRESULT("nTotalFrames > 0 ? S_OK : E_FAIL");

			ar.SeekFirst();

			CStdioFile file;
			int nDiffOld = -1;

			// Iterate frame-by-frame.
			BYTE *pVideoData = NULL;
			DWORD dwVideoDataSize = 0;
			for (INT32 i = 0; i < nTotalFrames && i < 12; i++)		// search every 5 seconds
			{
				INT64 tTimeStamp = 0;

				hr = ar.ReadFrame_Video(&pVideoData, &dwVideoDataSize, &tTimeStamp); 
				CHECKRESULT("ar.ReadFrame_Video(&pVideoData, &dwVideoDataSize, &tTimeStamp)");

				if (i == 0)
				{
					if (!file.Open(strLog, CFile::modeRead | CFile::shareDenyNone))
					{
						CLog::Instance()->AppendLog("file.Open(\"%s\", CFile::modeRead | CFile::shareDenyNone) failed (nTotalFrames = %d).\r\n",strLog,nTotalFrames);
#ifdef _DEBUG
						TRACE("file.Open(\"%s\", CFile::modeRead | CFile::shareDenyNone) failed.\n",strLog);
						ASSERT(FALSE);
#endif
						break;
					}
				}

				CString strLine;
				try
				{
					file.ReadString(strLine);
					TRACE("Log: %s\n", strLine);
				}
				catch (CFileException *e)
				{
#ifdef _DEBUG
					e->ReportError();
#endif
					e->Delete();
					continue;
				}

				int nFrame, nDiff;
				if (_stscanf(strLine, _T("%d:%d"), &nFrame, &nDiff) == 2)
				{
					ASSERT(i == nFrame);
					if (i == nFrame && nDiff > nDiffOld)
					{
						BITMAPINFO bmi;
						ZeroMemory(&bmi, sizeof(bmi));
						memcpy(&bmi, bmih, sizeof(BITMAPINFOHEADER));
						void *pBits = NULL;
						HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);
						if (hBitmap != NULL)
						{
							if (::SetDIBits(hDC, hBitmap, 0, bmi.bmiHeader.biHeight, pVideoData, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight)
							{
								if (i == 0)
								{
									m_bmpFirstFrame.Attach(hBitmap);
									m_bmpNonBlankFrame.Attach(hBitmap);
								}
								else
								{
									if (m_bmpNonBlankFrame.m_hObject != m_bmpFirstFrame.m_hObject)
										m_bmpNonBlankFrame.DeleteObject();
									else
										m_bmpNonBlankFrame.Detach();
									m_bmpNonBlankFrame.Attach(hBitmap);
								}
							}
							else
							{
								::DeleteObject(hBitmap);
							}
						}

						if (nDiff > 16)	// Y' (Luma) is from 16 to 235
							break;

						nDiffOld = nDiff;
					}
				}
			}

			::ReleaseDC(NULL, hDC);
		}
	}

_cleanup:
	ar.Destroy();

#ifndef _DEBUG
	::DeleteFile(strLog);
#endif

	return (!bHasVideo || m_bmpNonBlankFrame.m_hObject != NULL);
}

//A trailing backslash is required. For example, you specify \\MyServer\MyShare as "\\MyServer\MyShare\", or the C drive as "C:\".
BOOL isNTFSVolume(CString PathName)
{
	CString strDrvName = PathName;
	strDrvName.Trim();
	//Removes all parts of the path except for the root information.
	::PathStripToRoot(strDrvName.GetBuffer(_MAX_PATH));
	strDrvName.ReleaseBuffer();
	ASSERT(PathIsRoot(strDrvName));
	::PathAddBackslash(strDrvName.GetBuffer(_MAX_PATH));
	strDrvName.ReleaseBuffer();

	DWORD flag;
	char FileSystemName[10];
	memset(FileSystemName,0,10);
	if(!GetVolumeInformation(strDrvName,NULL,0,NULL,0,&flag,FileSystemName,9))
		return FALSE;
	TRACE(" Folder \"%s\" file system: %s\n",PathName,FileSystemName);
	return strcmp(FileSystemName,"NTFS")==0;
}

BOOL CTask::PrepareConversion()
{
#ifdef _DVD
	BOOL bFunctionLimited = FALSE;
#else
	BOOL bFunctionLimited = !g_bIsActivated;
#endif
	m_fDuration = m_bTrimmed?m_fTrimEnd - m_fTrimStart:m_mi.m_fDuration;
	if(bFunctionLimited) 
		m_fDuration = min(m_fDuration,TRIAL_TIME_LIMIT);

	double fDuration  = m_fDuration;
#ifdef _DVD
	BOOL   bForceSplit = AfxGetApp()->GetProfileInt("Debug","ForceSplit",0);
	double fSegLength = AfxGetApp()->GetProfileInt("Debug","Split",3.5*1024)*1024.0*1024;
#else
	//The file sysytem of COptions::Instance()->m_strTempFolder must be NTFS
	BOOL   bForceSplit = 0;
	double fSegLength =  0;
#endif
	int    iMaxSegTimeLen = 0;	// 以秒为单位的视频段最大时长，0 为不做限制

	if(bForceSplit || !isNTFSVolume(COptions::Instance()->m_strTempFolder))
	{
		CProfileManager *profileManager = CProfileManager::Instance();
		iMaxSegTimeLen = (int)(fSegLength*BITRATE_SAFE_FACTOR*8/(profileManager->m_nVBitrate+profileManager->m_nABitrate))/1000;
	}

	CLog::Instance()->AppendLog("PrepareConversion(): %s\r\n", m_strSourceFile);

	m_pEncoder = new CEncoderFFmpeg();
	m_pEncoder->m_pTask = this;

	taskAtom atom;
	atom.startframe = -1;
	atom.endframe = -1;
	atom.novideo = FALSE;
	double fStartTime = 0;

	CProfileItem *pItem;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pItem);

	do 
	{
		if(fDuration<=0.1) break;

		CString strAVSEncoding;
		for (int i = 0; ; i++)
		{
			strAVSEncoding.Format("%s\\Encoding%d.avs", m_strTempFolder, i);
			if (!PathFileExists(strAVSEncoding)) break;
		}	
		atom.avs=strAVSEncoding;
		// width, height, zoom are not available for audio-only file
		int nWidth, nHeight;
		CString strZoom;

		if (pItem->m_bHasVideo)	// its a video profile
		{
#ifdef _DVD
			CString strStandard;
			pItem->GetProfileParam("vstandard")->GetValue(this, strStandard);
			if (strStandard.CompareNoCase("PAL") == 0)
			{
				nWidth = 720;
				nHeight = 576;
			}
			else
			{
				nWidth = 720;
				nHeight = 480;
			}
#else
			CProfileParamDimension *pParamDimension = (CProfileParamDimension *)pItem->GetProfileParam("vdimension");
			if (pParamDimension == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			CString strDimension;
			if (!pParamDimension->GetValue(this, strDimension))
			{
#ifdef _DEBUG
				AfxMessageBoxEx(0, "CProfileParam::GetValue(\"%s\") failed.\n", pParamDimension->m_strName);
#endif
				return FALSE;
			}

			if (!CProfileParamDimension::GetDimension(strDimension, nWidth, nHeight))
			{
#ifdef _DEBUG
				AfxMessageBoxEx(0, "CProfileParam::GetValue(\"%s\") failed.\n", pParamDimension->m_strName);
#endif
				return FALSE;
			}
#endif

			CProfileParam *pParamZoom = pItem->GetProfileParam("vzoom");
			if (pParamZoom == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			pParamZoom->GetValue(this, strZoom);
		}
		else	// it's an audio only profile
		{
			nWidth = 320;
			nHeight = 240;
			strZoom = "Letterbox";
		}


		CLog::Instance()->AppendLog("PrepareConversion(): GetDimension() = %dx%d\r\n", nWidth, nHeight);

		CString strSampleRate;
		CProfileParam *pParamSampleRate = (CProfileParam *)pItem->GetProfileParam("asamplerate");
		if (pParamSampleRate == NULL)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		pParamSampleRate->GetValue(this, strSampleRate);

		CString strChannels;
		CProfileParam *pParamChannels = (CProfileParam *)pItem->GetProfileParam("achannels");
		if (pParamChannels == NULL)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		pParamChannels->GetValue(this, strChannels);

		BOOL bUnknownFps = (fabs(m_mi.m_fFrameRate - 0.0) < FLT_EPSILON);
		float fFramerate = min(MAX_FRAMERATE, max(MIN_FRAMERATE, (bUnknownFps ? DEF_FRAMERATE : m_mi.m_fFrameRate)));

		CString strScript;
		try
		{
			CString strTemplate;
#ifdef AC3_AFTEN
			strTemplate.Format("%s\\Scripts\\VEncoding.avs", GetModuleFilePath());
#else
			strTemplate.Format("%s\\Scripts\\Encoding.avs", GetModuleFilePath());
#endif

			CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
			ULONGLONG len = file.GetLength();
			file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
			strScript.ReleaseBuffer();

			strScript.Replace("$(modulepath)", GetModuleFilePath());
			strScript.Replace("$(input)", m_strSourceFileAlt.IsEmpty() ? m_strSourceFile : m_strSourceFileAlt);

			strScript.Replace("$(enablevideo)", pItem->m_bHasVideo ? "true" : "false");

			CString strFileName = m_strSourceFile;
			PathRemoveExtension(strFileName.GetBuffer());
			strFileName.ReleaseBuffer();

			CString strSubtitle;
			FindSubtitle(strSubtitle);
			strScript.Replace("$(subtitle)", strSubtitle);

			CString strFramerate;
			strFramerate.Format("%.3f", fFramerate);
			strScript.Replace("$(vframerate)", strFramerate);

			strScript.Replace("$(seekzero)", bUnknownFps ? "true" : "false");

			strScript.Replace("$(asamplerate)", strSampleRate);
			strScript.Replace("$(achannels)", strChannels);

			CString strWidth;
			CString strHeight;
			strWidth.Format("%d", nWidth);
			strHeight.Format("%d", nHeight);
			strScript.Replace("$(width)", strWidth);
			strScript.Replace("$(height)", strHeight);

			CString strAspect;
			strAspect.Format("%.6f", m_mi.m_fDisplayRatio);
			strScript.Replace("$(disaspect)", strAspect);

#ifdef _DVD
			CString strOutptuAspect;
			pItem->GetProfileParam("vaspect")->GetValue(this, strOutptuAspect);
			if (strOutptuAspect.CompareNoCase("4:3") == 0)
				strAspect.Format("%.6f", (float)4/3);
			else
				strAspect.Format("%.6f", (float)16/9);
#else
			strAspect.Format("%.6f", (float)nWidth / nHeight);
#endif

			strScript.Replace("$(outaspect)", strAspect);

			strScript.Replace("$(vzoom)", strZoom);


			double curAtomLen=iMaxSegTimeLen?min(iMaxSegTimeLen,fDuration):fDuration;
			if(m_Atoms.empty() && m_bTrimmed)
				atom.startframe=(__int64)(m_fTrimStart*fFramerate+0.5);
			else
				atom.startframe=atom.endframe+1;

			atom.endframe = (__int64)(curAtomLen*fFramerate)+atom.startframe;
			atom.starttime = fStartTime;
			fStartTime += curAtomLen;
			fDuration -= curAtomLen;

			if(iMaxSegTimeLen || m_bTrimmed || bFunctionLimited)
			{
				CString FrameStr;

				FrameStr.Format("%d",(int)(atom.startframe));
				strScript.Replace("$(trimstart)", FrameStr);
				FrameStr.Format("%d",(int)(atom.endframe));
				strScript.Replace("$(trimend)", FrameStr);
			}
			else
			{
				strScript.Replace("$(trimstart)", "0");
				strScript.Replace("$(trimend)", "0");
			}

			if( AfxGetApp()->GetProfileInt("Debug", "File Log", FALSE))
			{
				CString strLogInfo;
				strLogInfo.Format("   AddLog(\"%s\",10,10)\r\nAddLog(AVLength(last),10,100)\r\nShowFramesNumber(10,200)\n",m_strSourceFile);
				strScript.Replace("$(log)", strLogInfo);
			}
			else
				strScript.Replace("$(log)", "");
			strScript.Replace("$(baudiolog)", "false");
			ASSERT(strScript.Find("$(") < 0);
		}
		catch (CException *e)
		{
			e->ReportError();
			e->Delete();
			return FALSE;
		}

		try
		{
			CStdioFile file(strAVSEncoding, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

			file.Write(strScript, strScript.GetLength());

			file.Flush();
			file.Close();
		}
		catch (CException *e)
		{
			e->ReportError();
			e->Delete();
			return FALSE;
		}

#ifdef _DVD
		atom.cmd=CProfileManager::Instance()->GetEncoderCmd(&atom,this,_AV);
#else
	#ifdef AC3_AFTEN
		atom.cmd=CProfileManager::Instance()->GetEncoderCmd(&atom,this,_VIDEO);
	#else
		atom.cmd=CProfileManager::Instance()->GetEncoderCmd(&atom,this,_AV);
	#endif
#endif
		if(atom.cmd.IsEmpty())
		{
			m_nStatus = CONV_STATUS_FAILED;
			m_nUpdate |= UPDATE_STATUS;
			AfxMessageBox(IDS_E_OPEN_ENCODER);
			delete m_pEncoder;
			return FALSE;
		}

		m_Atoms.push_back(atom);
	} while (1);

#ifdef AC3_AFTEN
	//Enable audio
	CString strEnableAudio="";
	pItem->GetProfileParam("aenable")->GetValue(NULL,strEnableAudio);
	if(strEnableAudio.Compare("-an")!=0)
	{
		if(!AudioAtom(&m_Atoms))
		{
			CLog::Instance()->AppendLog("Error: AudioAtom(): failed.\r\n");
			delete m_pEncoder;
			return FALSE;
		}
	}
#endif

	CLog::Instance()->AppendLog("PrepareConversion(): done.\r\n");
#ifdef AC3_AFTEN
	m_fVideoStream = 0.0f;
#endif
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Change pTask->m_nStatus to CONV_STATUS_CONVERTING when enter;
// Change pTask->m_nStatus to CONV_STATUS_COMPLETED, CONV_STATUS_FAILED or CONV_STATUS_CANCELED when exit;
DWORD WINAPI CTask::ConvProc(PVOID lp)
{
	CONV_PROC_PARAM *pParam = (CONV_PROC_PARAM *)lp;
	CTask *pTask = (CTask *)pParam->pTask;
	HANDLE hEvent = pParam->hEvent;

	::SetEvent(hEvent);

	pTask->Lock();
	
	CLog::Instance()->AppendLog("Enter CTask::ConvProc(): %s\r\n", pTask->m_strSourceFile);

	pTask->m_nStartTick = ::GetTickCountEx();
	pTask->m_fEncodedTime = 0.0;
	pTask->m_nStatus = CONV_STATUS_CONVERTING;
	//pTask->CLog::Instance()->AppendLog(LOG_INFORMATION, "%s.\r\n", "Start conversion.");
	pTask->m_nUpdate |= (UPDATE_STATUS | UPDATE_LOG);

	pTask->Unlock();
	BOOL EncodeResult=FALSE;

	if (::PathFileExists(pTask->m_strSourceFile))
	{
		for(int n=0;n<pTask->m_Atoms.size();n++)
		{
			taskAtom *atom=&pTask->m_Atoms[n];
			pTask->m_pCurAtom=atom;

			CString strOutputName = pTask->m_strOutputName;
			CString strFolder = COptions::Instance()->m_strTempFolder;

			// PathRemoveBackslash() will not remove the backslash of root folder such as C:\
			// Under Windows Vista, path like C:\\Temp will cause error.
			::PathAddBackslash(strFolder.GetBufferSetLength(_MAX_PATH));
			strFolder.ReleaseBuffer();

			CString strUnique;
			strUnique.Format("%s%s.%s", strFolder, strOutputName, atom->ext);
			if (::PathFileExists(strUnique))
			{
				for (int seq = 1; ; seq++)
				{
					strUnique.Format("%s%s-%d.%s", strFolder, strOutputName, seq, atom->ext);
					if (!::PathFileExists(strUnique))
						break;
				}
			}

			try
			{
				CFile file(strUnique, CFile::modeCreate);
				file.Close();
			}
			catch (CFileException *e)
			{
				e->Delete();
			}


			atom->outfile=strUnique;
			atom->cmd.Replace("$(output)", (LPCTSTR)strUnique);
			//ASSERT(atom->cmd.Find('$') < 0);
#ifdef AC3_AFTEN
			if (atom->novideo)
			{
				CString strDuration;
				if(pTask->m_fVideoStream > FLT_EPSILON)
					strDuration.Format("-t %f",pTask->m_fVideoStream);
				atom->cmd.Replace("$(time)", (LPCTSTR)strDuration);

				delete pTask->m_pEncoder;
				pTask->m_pEncoder = new CEncoderAC3();
				pTask->m_pEncoder->m_pTask = pTask;
			}
#endif		
			pTask->m_pEncoder->m_strCommandLine=atom->cmd;

			EncodeResult=pTask->m_pEncoder->Start();
			if(EncodeResult==FALSE)	break;
			
#ifdef AC3_AFTEN
			if(!atom->novideo)
			{
				CMediaInfo mi;
				if (mi.ParseMedia(strUnique))
					pTask->m_fVideoStream = mi.m_fDuration;
				else
					pTask->m_fVideoStream = 0.0f;
			}
#endif	
		}
	}
	else
		pTask->m_nStatus = CONV_STATUS_FAILED;
	
	if(EncodeResult)
		pTask->m_nStatus=CONV_STATUS_COMPLETED;
	else
		if(pTask->m_nStatus!=CONV_STATUS_CANCELED) pTask->m_nStatus=CONV_STATUS_FAILED;

	pTask->Lock();

	delete pTask->m_pEncoder;
	pTask->m_pEncoder = NULL;

	pTask->m_hConvThread = NULL;

	UINT nMessage;
	switch (pTask->m_nStatus)
	{
	case CONV_STATUS_COMPLETED:
		nMessage = WM_TASK_COMPLETED;
		break;
	case CONV_STATUS_FAILED:
		nMessage = WM_TASK_FAILED;
		break;
	case CONV_STATUS_CANCELED:
		nMessage = WM_TASK_CANCELED;
		break;
	default:
		ASSERT(FALSE);
		nMessage = 0;
		break;
	}

	if (pTask->m_nStatus != CONV_STATUS_COMPLETED)
	{
		pTask->DeleteFiles();
		pTask->ClearTasks();
	}

	CFileList::Instance()->PostMessage(nMessage, 0, (LPARAM)pTask);

	pTask->Unlock();

	CLog::Instance()->AppendLog("Exit CTask::ConvProc().\r\n");

	return 0;
}

void CTask::Pause()
{
	ASSERT(!IsPaused());

	CLog::Instance()->AppendLog("CTask::Pause()\r\n");

	::SetEvent(m_hPauseEvent);
	HANDLE handles[] = {m_hConvThread, m_hPauseEvent2};
	if (::WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)
	{
		::ResetEvent(m_hPauseEvent2);
		m_bPaused = TRUE;
		m_nPauseTick = ::GetTickCountEx();
	}
	else
	{
		::ResetEvent(m_hPauseEvent);
	}
}

void CTask::Resume()
{
	ASSERT(IsPaused());

	CLog::Instance()->AppendLog("CTask::Resume()\r\n");

	::SetEvent(m_hResumeEvent);
	HANDLE handles[] = {m_hConvThread, m_hResumeEvent2};
	if (::WaitForMultipleObjects(2, handles, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)
	{
		::ResetEvent(m_hResumeEvent2);
		m_bPaused = FALSE;
		if (m_nStartTick > 0 && m_nPauseTick > 0)
		{
			m_nStartTick += (::GetTickCountEx() - m_nPauseTick);
			ASSERT(m_nStartTick > 0);
		}
		m_nPauseTick = 0;
	}
	else
	{
		::ResetEvent(m_hResumeEvent);
	}
}

void CTask::Cancel()
{
	ASSERT(!IsCanceled());

	m_bCanceled = TRUE;
	::SetEvent(m_hCancelEvent);
	::WaitForSingleObject(m_hConvThread, INFINITE);
	::ResetEvent(m_hCancelEvent);
}

BOOL CTask::IsPaused()
{
	return m_bPaused;
}

BOOL CTask::IsCanceled()
{
	return m_bCanceled;
}

CString CTask::GetMediaInfo()
{
	CString sGenerial;
	sGenerial.Format(IDS_MI_GENERAL, 
		(LPCTSTR)m_strSourceFile, 
		m_mi.m_sFormat.c_str(), 
		GetFileSizeString(m_mi.m_i64FileSize), 
		FormatTime((UINT)(m_mi.m_fDuration + 0.5), 0, FALSE));

	CString sAudio;
	if (m_mi.m_nAudioCount > 0)
	{
		sAudio.Format(IDS_MI_AUDIO, 
			m_mi.m_sAudioFormat.c_str(), m_mi.m_nSampleRate, m_mi.m_nAudioBitrate, m_mi.m_nChannels);
	}

	CString sVideo;
	if (m_mi.m_nVideoCount > 0)
	{
		CString strDisplayRatio;
		if (fabs(m_mi.m_fDisplayRatio - 1.333) < 0.001)
			strDisplayRatio = "4/3";
		else if (fabs(m_mi.m_fDisplayRatio - 1.778) < 0.001)
			strDisplayRatio = "16/9";
		else
			strDisplayRatio.Format("%.02f/1", m_mi.m_fDisplayRatio);

		sVideo.Format(IDS_MI_VIDEO, 			
			m_mi.m_sVideoFormat.c_str(), m_mi.m_nWidth, m_mi.m_nHeight, strDisplayRatio, 
			m_mi.m_fFrameRate, m_mi.m_nVideoBitRate);
	}

	return sGenerial + sAudio + sVideo;
}

BOOL CTask::FindSubtitle(CString &strSubtitle)
{
	if(!AfxGetApp()->GetProfileInt("Debug", "Find Subtitle", TRUE))
		return FALSE;
	if (m_strSourceFileAlt.IsEmpty())
	{
		const TCHAR *SUBTITLE_EXTS[4] = 
		{
			".srt", 
			".sub", 
			".ssa", 
			".ass", 
		};

		CString strFileName = m_strSourceFile;

		if (strFileName.IsEmpty() || !::PathFileExists(strFileName))
			return FALSE;

		PathRemoveExtension(strFileName.GetBuffer());
		strFileName.ReleaseBuffer();

		for (int i = 0; i < sizeof(SUBTITLE_EXTS) / sizeof(TCHAR *); i++)
		{
			CString strFind;
			strFind.Format("%s*%s", strFileName, SUBTITLE_EXTS[i]);

			CFileFind ff;
			BOOL ret = ff.FindFile(strFind);

			while (ret)
			{
				ret = ff.FindNextFile();
				if (!ff.IsDots())
				{
					strSubtitle = ff.GetFilePath();
					TRACE("Subtitle: %s\n", strSubtitle);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


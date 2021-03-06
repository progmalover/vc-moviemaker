#include "StdAfx.h"
#include "Converter.h"
#include "Options.h"
#include "MainFrm.h"
#include "Options.h"
#include "Burner.h"
#include "DlgNoDiskSpace.h"
#include "DlgSelectTempFolder.h"
#include "ProfileManager.h"
#include "FileList.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL ReadString(int nID,CStringArray *pStrArray,TCHAR chSep)
{
	CString strExtensions,strTempExtension;
	strExtensions.LoadString(nID);
	for(int n = 0;;n++)
	{
		AfxExtractSubString(strTempExtension, strExtensions, n, chSep);
		if(strTempExtension.IsEmpty()) break;
		pStrArray->Add(strTempExtension);
	}
	return pStrArray->GetCount()>0;
}

IMPLEMENT_SINGLETON(COptions)

COptions::COptions(void)
{
	m_bLoaded = FALSE;
}

COptions::~COptions(void)
{
}

BOOL COptions::LoadState()
{
	int nNumberOfProcessors = GetNumberOfProcesseors();
	CLog::Instance()->AppendLog("GetNumberOfProcesseors(): %d\r\n", nNumberOfProcessors);

	m_nMaxConcurrentTasks = max(1, min(nNumberOfProcessors, AfxGetApp()->GetProfileInt("Config\\Encoding", "ConcurrentTasks", nNumberOfProcessors - 1)));
	m_nEncodingThreads = max(1, min(nNumberOfProcessors, AfxGetApp()->GetProfileInt("Config\\Encoding", "EncodingThreads", nNumberOfProcessors)));

	m_bPromptStopConversion = AfxGetApp()->GetProfileInt("Config\\Encoding", "StopConversionPrompt", TRUE);
	m_bFixAspectRatio = AfxGetApp()->GetProfileInt("Config\\Encoding", "Fix Aspect", TRUE);
	m_bPromptIncompatibleCodecs = AfxGetApp()->GetProfileInt("Config\\Encoding", "IncompatibleCodecsPrompt", TRUE);

	CString strModuleFilePath = GetModuleFilePath();

	m_bSoundFinished = AfxGetApp()->GetProfileInt("Config\\Encoding", "EnbaleFinishedFinished", TRUE);
	m_strSoundFinished = AfxGetApp()->GetProfileString("Config\\Encoding", "FinishedSoundFilePath", strModuleFilePath + "\\Sound\\Finished.wav");	

	m_strTempFolder = AfxGetApp()->GetProfileString("Config\\Temp Folder", "LastFolder", NULL);
	m_strTempFolder.Trim();
	if (m_strTempFolder.IsEmpty())
		m_strTempFolder = GetDefaultTempFolder();

	m_bCheckDiscStatus = AfxGetApp()->GetProfileInt("Config\\Burner", "CheckDiscStatus", TRUE);
	m_bPromptNonBlankDisc = AfxGetApp()->GetProfileInt("Config\\Burner", "Non-blankDiscPromt", TRUE);
	m_bPromptMakeAnotherDisc = AfxGetApp()->GetProfileInt("Config\\Burner", "AnotherDiscPrompt", TRUE);

	m_strUniqueID = AfxGetApp()->GetProfileString("Config\\Burner", "BurnerID", NULL);
	m_strDiscType = AfxGetApp()->GetProfileString("Config\\Burner", "DiscType", NULL);

#ifdef _BD
	m_strDimension = AfxGetApp()->GetProfileString("Options\\Video", "Dimension", NULL);
	m_strFrameRate = AfxGetApp()->GetProfileString("Options\\Video", "Frame Rate", NULL);
	m_strMenuDuration = AfxGetApp()->GetProfileString("Options\\Video", "Menu Duration", "30");
	if(_ttoi(m_strMenuDuration)<MENU_DURATION_MIN || _ttoi(m_strMenuDuration)>MENU_DURATION_MAX)
		m_strMenuDuration.Format("%d",MENU_DURATION_DEFAULT);
#else	// _BD
	m_strVideoStandard = AfxGetApp()->GetProfileString("Config\\Video", "StandardVideo", "NTSC");
	m_strVideoAspect   = AfxGetApp()->GetProfileString("Config\\Video", "AspectVideo", "16:9");
	m_nAfterCurVideoOptions = (NAVIGATION_ACTION)AfxGetApp()->GetProfileInt("Config\\Navigation","PlayCurrentVideoFinished",PLAY_NEXT_VIDEO);
#endif	// _DVD

	m_nAfterLastVideoOptions = (NAVIGATION_ACTION)AfxGetApp()->GetProfileInt("Config\\Navigation","PlayLastVideoFinished",BACK_FIRST_MENU);
	m_strZoommode = AfxGetApp()->GetProfileString("Config\\Video", "Zoom mode", "Letterbox");

	m_bShowHorzGridLines = AfxGetApp()->GetProfileInt("Config\\Interface", "HorzGridLinesEnabled", TRUE);
	m_bShowVertGridLines = FALSE;//AfxGetApp()->GetProfileInt("Options\\Interface", "Show Vert Grid Lines", TRUE);
	m_bSmoothScrolling = FALSE;//AfxGetApp()->GetProfileInt("Options\\Interface", "Smooth Scrolling", TRUE);
	m_nDoubleClickAction = DCA_DO_NOTHING;//max(DCA_DO_NOTHING, min(DCA_VIEW_PROPERTIES, AfxGetApp()->GetProfileInt("Config\\Interface", "Double Click", DCA_PREVIEW)));
	m_bShutDown = FALSE;
	m_nShutDownOptions = max(SHUTDOWN_SLEEP, min(SHUTDOWN_SHUTDOWN, AfxGetApp()->GetProfileInt("Config\\General", "ShutDownDetails", SHUTDOWN_SHUTDOWN)));

	m_bLoaded = TRUE;

	return TRUE;
}

BOOL COptions::SaveState()
{
	// SaveState() will also be called if an instance is already running.
	if (!m_bLoaded)
		return FALSE;

	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Concurrent Tasks", m_nMaxConcurrentTasks);
	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Encoding Threads", m_nEncodingThreads);
	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Prompt Stop Conversion", m_bPromptStopConversion);
	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Fix Aspect", m_bFixAspectRatio);
	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Prompt for Incompatible Codecs", m_bPromptIncompatibleCodecs);
	AfxGetApp()->WriteProfileInt("Options\\Encoding", "Play Finished Sound", m_bSoundFinished);
	AfxGetApp()->WriteProfileString("Options\\Encoding", "Finished Sound File", m_strSoundFinished);

	AfxGetApp()->WriteProfileString("Options\\Temp Folder", "Last Folder", m_strTempFolder);

	AfxGetApp()->WriteProfileInt("Options\\Burner", "Check Disc Status", m_bCheckDiscStatus);
	AfxGetApp()->WriteProfileInt("Options\\Burner", "Prompt Non-blank Disc", m_bPromptNonBlankDisc);
	AfxGetApp()->WriteProfileInt("Options\\Burner", "Prompt Make Another Disc", m_bPromptMakeAnotherDisc);

	AfxGetApp()->WriteProfileString("Options\\Burner", "Burner ID", m_strUniqueID);
	AfxGetApp()->WriteProfileString("Options\\Burner", "Disc Type", m_strDiscType);

#ifdef _BD
	AfxGetApp()->WriteProfileString("Options\\Video", "Dimension", m_strDimension);
	AfxGetApp()->WriteProfileString("Options\\Video", "Frame Rate", m_strFrameRate);
	AfxGetApp()->WriteProfileString("Options\\Video", "Menu Duration", m_strMenuDuration);
#else
	AfxGetApp()->WriteProfileString("Options\\Video", "Video Standard", m_strVideoStandard);
	AfxGetApp()->WriteProfileString("Options\\Video", "Video Aspect", m_strVideoAspect);
	AfxGetApp()->WriteProfileInt("Options\\Navigation","After play current video",m_nAfterCurVideoOptions);
#endif	// _DVD

	AfxGetApp()->WriteProfileInt("Options\\Navigation","After play last video",m_nAfterLastVideoOptions);
	AfxGetApp()->WriteProfileString("Options\\Video", "Zoom mode", m_strZoommode);

	AfxGetApp()->WriteProfileInt("Options\\Interface", "Show Horz Grid Lines", m_bShowHorzGridLines);
	AfxGetApp()->WriteProfileInt("Options\\Interface", "Show Vert Grid Lines", m_bShowVertGridLines);
	AfxGetApp()->WriteProfileInt("Options\\Interface", "Smooth Scrolling", m_bSmoothScrolling);
	AfxGetApp()->WriteProfileInt("Options\\Interface", "Double Click", m_nDoubleClickAction);
	AfxGetApp()->WriteProfileInt("Options\\General", "Shut Down Options", m_nShutDownOptions);

	return TRUE;
}

int COptions::GetNumberOfProcesseors()
{
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

BOOL COptions::CheckFreeDiskSpace(INT64 nTotalSize)
{
_retry:
	ULARGE_INTEGER nFreeBytes;
	::GetDiskFreeSpaceEx(m_strTempFolder, &nFreeBytes, NULL, NULL);
	if (nTotalSize > (INT64)nFreeBytes.QuadPart )
	{
		CString strRoot = m_strTempFolder;
		::PathStripToRoot(strRoot.GetBuffer());
		strRoot.ReleaseBuffer();
		strRoot.TrimRight('\\');

		CDlgNoDiskSpace dlg;
		dlg.m_strPrompt.Format(IDS_NO_ENOUGH_DISK_SPACE5, (LPCTSTR)m_strTempFolder, (LPCTSTR)strRoot, 
			GetFileSizeString(nTotalSize), GetFileSizeString((INT64)nFreeBytes.QuadPart), (LPCTSTR)strRoot);
		INT_PTR ret = dlg.DoModal();
		switch (ret)
		{
		case IDOK:
			{
				m_strTempFolder = dlg.m_strFolder;
				goto _retry;
			}

		case IDRETRY:
			goto _retry;

		default:
			return FALSE;
		}
	}

	return TRUE;
}

CString COptions::GetDefaultTempFolder()
{
#ifdef _BD
	const INT64 SUFFICIENT_SIZE = 100;
#elif defined _DVD
	const INT64 SUFFICIENT_SIZE = 20;
#endif

	ULARGE_INTEGER nFreeBytesMax = {0, 0};
	CString strDriveMax;

	DWORD dwDrives = ::GetLogicalDrives();
	for (int i = 0; i < 26; i++)
	{
		if ((dwDrives & ((DWORD)1 << i)) != 0)
		{
			CString strDrive;
			strDrive.Format("%C:\\", 'A' + i);
			if (::GetDriveType(strDrive) == DRIVE_FIXED)
			{
				ULARGE_INTEGER nFreeBytes;
				if (::GetDiskFreeSpaceEx(strDrive, &nFreeBytes, NULL, NULL))
				{
					if (nFreeBytes.QuadPart > nFreeBytesMax.QuadPart)
					{
						nFreeBytesMax = nFreeBytes;
						strDriveMax = strDrive;
						if ((INT64)nFreeBytesMax.QuadPart >= SUFFICIENT_SIZE * (1024 * 1024 * 1024))
							break;
					}
				}
			}
		}
	}

	ASSERT(!strDriveMax.IsEmpty());

	TCHAR szPath[_MAX_PATH];

	if (GetTempPath(_MAX_PATH, szPath))
	{
		if (::PathIsSameRoot(szPath, strDriveMax))
			return szPath;
	}

	if (::SHGetSpecialFolderPath(NULL, szPath, CSIDL_MYDOCUMENTS, FALSE))
	{
		if (::PathIsSameRoot(szPath, strDriveMax))
		{
			_tcscat(szPath, "\\");
			_tcscat(szPath, AfxGetApp()->m_pszProfileName);
			return szPath;
		}
	}

	return strDriveMax + AfxGetApp()->m_pszProfileName;
}

BOOL COptions::PrepareTempFolder(BOOL bPrompt)
{
	ASSERT(!m_strTempFolder.IsEmpty());

	if (!IsValidPath(m_strTempFolder))
	{
		if (bPrompt)
			AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_INVALID_TEMP_FOLDER1, (LPCTSTR)m_strTempFolder);
		return FALSE;
	}

	if (::PathIsDirectory(m_strTempFolder))
	{
		// This API does not work correctly for some paths, e.g. C:\Temp///
		if (!IsDirWritable(m_strTempFolder))
		{
			if (bPrompt)
				AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_CREATE_FILE_IN_TEMP_FOLDER1, (LPCTSTR)m_strTempFolder);
			return FALSE;
		}
	}
	else
	{
		::PathRemoveBackslash(m_strTempFolder.GetBuffer());
		m_strTempFolder.ReleaseBuffer();

		if (!::CreateDirectoryNest(m_strTempFolder))
		{
			if (bPrompt)
				AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
			return FALSE;
		}
	}
	
#ifdef _BD
	if(!isNTFSVolume(m_strTempFolder))
	{
		if (bPrompt)
			AfxMessageBox(IDS_E_TEMP_FOLDER_MUST_BE_NTFS , MB_ICONWARNING | MB_OK) ;
		return FALSE ;
	}
#endif
	return TRUE;
}

void COptions::UpdateULStyle()
{
	DWORD dwStyle = ULS_MULTISEL | ULS_HEADERDRAGDROP;

	if (CFileList::Instance()->IsReportView())
		dwStyle |= ULS_REPORT;

	if (m_bShowHorzGridLines)
		dwStyle |= ULS_ROWLINE;
	if (m_bShowVertGridLines)
		dwStyle |= ULS_COLLINE;
	if (m_bSmoothScrolling)
		dwStyle |= ULS_SMOOTHSCROLLING;
	
	CFileList::Instance()->ModifyULStyle(-1L, dwStyle);
}

#include "StdAfx.h"
#include "Converter.h"
#include "EncoderBlueRay.h"
#include "Task.h"
#include "Options.h"
#include "MKVMerge.h"
#include "ProfileManager.h"
#include "FileList.h"
#include "BDMenuUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _BD

CEncoderBlueRay::CEncoderBlueRay(void):m_strInputAudios(_T(""))
{
}

CEncoderBlueRay::~CEncoderBlueRay(void)
{
	if (m_pTask != NULL)
		delete m_pTask;
}

BOOL CEncoderBlueRay::Start()
{	
	BOOL bUsageMKVMerge    = AfxGetApp()->GetProfileInt("Debug", "Use mkvmerge", TRUE);
	BOOL bDeleteFiles      = AfxGetApp()->GetProfileInt("Debug", "Delete Output Files", TRUE);
	BOOL bDeleteMetaFolder = AfxGetApp()->GetProfileInt("Debug", "Delete Meta Files", TRUE);
	BOOL bDeleteMergedMKV  = AfxGetApp()->GetProfileInt("Debug", "Delete Merge MKV", TRUE);

	g_fMKVMergeRatio = 0.0;
	CString strTempFolder;
	if (!::CreateTempFolder(NULL, strTempFolder) || !::CreateTempFolder(m_strOutputRoot, m_strOutput))
	{
		AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
		return FALSE;
	}

	//(2)Merge all mkv files;
	CString strInput = m_strInputFiles;
	
	CProfileItem *pItem;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pItem);
	CString strExt  = pItem->m_strExt;
	if(strExt.Find(".")==-1)
		strExt.Insert(0,".");

	BOOL bMerge = FALSE;
	if (bUsageMKVMerge)
	{
		CMKVMerge merge;
		merge.m_strOutputFolder = m_strOutput;
		merge.m_pTask = m_pTask;
		merge.m_strInputs = m_strInputFiles+" "+m_strInputAudios;
		bMerge =  merge.Start();
		if(bMerge)
		{
			strInput.Format("\"%s\"",merge.m_strMKV);
			//Merge mkv files succeed,(3)Remove all mkv files.
			CFileList::Instance()->RemoveOutputFiles(bDeleteFiles);
		}
		else
			CLog::Instance()->AppendLog("Error: mkvmerge.exe run failed.\r\n");
	}
	else
		CLog::Instance()->AppendLog("Not use mkvmerge.exe.\r\n");

	//(4)tsMuxeR the merged mkv or all output mkv files to BD/AVCHD
	CProfileItem *pProfile;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);
	CProfileParam *pParam = pProfile->GetProfileParam("aenable");
    CString strEnableAudio = pParam->GetValue();
	if(strEnableAudio.CompareNoCase("No")==0)
	{
		int nAC3 = m_strScript.Find("A_AC3");
		if(nAC3!=-1)
			m_strScript.Delete(nAC3,m_strScript.GetLength()-nAC3-1);
	}
	else
	{
		m_strInputAudios.Trim();
		m_strScript.Replace("$(input_audio)", bMerge?strInput:(m_strInputAudios.GetLength()==0?m_strInputFiles:m_strInputAudios));
	}

	m_strScript.Replace("$(input)", strInput);
	ASSERT(m_strScript.Find('$') < 0);
	CString strBlueRayMeta;
	strBlueRayMeta.Format("%s\\tsMuxeR.meta", (LPCTSTR)strTempFolder);

	try
	{
		CStdioFile file(strBlueRayMeta, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(m_strScript, m_strScript.GetLength());
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	m_strCommandLine.Format("\"%s\" \"%s\"", strBlueRayMeta, m_strOutput);
	BOOL ret = CEncoder::Start();
	if (bDeleteMetaFolder)
	{
		if (!strTempFolder.IsEmpty() && ::PathIsDirectory(strTempFolder))
			::RemoveDirectory(strTempFolder, TRUE);
	}

	if(bMerge)
	{
		if (bDeleteMergedMKV)
		{
			//Removes quotes from the beginning and end of a path.
			PathUnquoteSpaces(strInput.GetBuffer());
			strInput.ReleaseBuffer();
			BOOL bRe = ::DeleteFile(strInput);
			ASSERT(bRe);
		}
	}
	else
		CFileList::Instance()->RemoveOutputFiles(bDeleteFiles);

	if(ret && !m_strMenuFolder.IsEmpty())
	{
		//(5)Copy menu to BD/AVCHD
		if(!::CopyBlurayMenu(m_strMenuFolder,m_strOutput))
		{
			//Copy menu to BD/AVCHD failed!
			CLog::Instance()->AppendLog("Error: Copy bluray menu folder.\r\n");
			m_strMenuFolder.Empty();
			ret = FALSE;
		}
	}

	if(!ret)
	{
		//Create Disc Structure Failed£ºremove disc structure.
		if (AfxGetApp()->GetProfileInt("Debug", "Delete Disc Structure", TRUE))
			::RemoveDirectory(m_strOutput, TRUE);
	}
	return ret;
}

CString CEncoderBlueRay::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\tsMuxeR.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderBlueRay::ParseOutput(LPCTSTR lpszLine)
{
	// 1.3% complete

	Regexx re;
	if (re.exec(lpszLine, "^[ \t]*([0-9\\.]+)% complete") > 0)
	{
		std::string m_sPercent = re.match[0].atom[0].str();

		m_pTask->Lock();

		double fEncodedPercent = max(0, min(100.0, (float)atof(m_sPercent.c_str())));
		if (fabs(fEncodedPercent - m_pTask->m_fEncodedPercent) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedPercent = g_fMKVMergeRatio*100+fEncodedPercent*(1-g_fMKVMergeRatio);
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}

		TRACE("EncoderBlueRay percent %.1f%%\n", m_pTask->m_fEncodedPercent);
		m_pTask->Unlock();
		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
#endif
	}

	return FALSE;
}

#endif
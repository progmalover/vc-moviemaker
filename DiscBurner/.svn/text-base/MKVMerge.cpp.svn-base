#include "StdAfx.h"
#include "MKVMerge.h"

#ifdef _BD
float g_fMKVMergeRatio = 0.0;

CMKVMerge::CMKVMerge(void):m_nProgress(0)
{

}

CMKVMerge::~CMKVMerge(void)
{
	//if (m_pTask != NULL)
	//	delete m_pTask;
}

BOOL CMKVMerge::Start()
{
	for (int i = 0; ; i++)
	{
		m_strMKV.Format("%s\\merge%d.mkv", m_strOutputFolder, i);
		if (!PathFileExists(m_strMKV)) break;
	}	

	int   nTrackID = 1;
	float fAspect = (float)16/9;
	m_strCommandLine.Format("-o \"%s\" %s --aspect-ratio %d:%f --priority highest --ui-language en", m_strMKV,m_strInputs,nTrackID,fAspect);
	BOOL ret = FALSE;
	g_fMKVMergeRatio = 0.3;
	if(CEncoder::Start()|| m_nProgress==100)
		ret = TRUE;
	else
		::DeleteFile(m_strMKV);
	return ret;
}

CString CMKVMerge::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\mkvmerge.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CMKVMerge::ParseOutput(LPCTSTR lpszLine)
{
	// Progress: 42%

	Regexx re;
	if (re.exec(lpszLine, "Progress: ([0-9\\.]+)%") > 0)
	{
		std::string sPercent = re.match[0].atom[0].str();
		m_nProgress = atoi(sPercent.c_str());
		m_pTask->Lock();
		if (fabs(m_nProgress - m_pTask->m_fEncodedPercent) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedPercent = m_nProgress*g_fMKVMergeRatio;
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}
		TRACE("MKVMerge percent %.1f%%\n", m_pTask->m_fEncodedPercent);
		m_pTask->Unlock();
	}
	else if(re.exec(lpszLine, "Error:") > 0)
	{
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
	}
	else
	{
#ifdef _DEBUG
		CString strMsg = lpszLine;
		strMsg.Trim();
		TRACE("MKVMerge:%s\n", strMsg);
		CLog::Instance()->AppendLog("MKVMerge:%s", strMsg);
#endif	
	}
	return TRUE;
}
#endif
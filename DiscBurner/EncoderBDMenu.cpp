#include "StdAfx.h"
#include "EncoderBDMenu.h"
#include "Task.h"

#ifdef _BD
CEncoderBDMenu::CEncoderBDMenu(void)
{
}

CEncoderBDMenu::~CEncoderBDMenu(void)
{
	if (m_pTask != NULL)
		delete m_pTask;
}

BOOL CEncoderBDMenu::Start()
{
	m_strCommandLine.Format("-a \"%s\" \"%s\"", m_strMenuXML,m_strOutputFolder);
	BOOL ret =  CEncoder::Start();
	return ret;
}

CString CEncoderBDMenu::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\IGS.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderBDMenu::ParseOutput(LPCTSTR lpszLine)
{
	// Progress: 55.5%

	Regexx re;
	if (re.exec(lpszLine, "Progress\\:[ \t]*([0-9\\.]+)%") > 0)
	{
		std::string m_sPercent = re.match[0].atom[0].str();
		double fPercent = atof(m_sPercent.c_str());

		m_pTask->Lock();

		double fEncodedPercent = max(0.0, min(100.0, fPercent));
		if (fabs(fEncodedPercent - m_pTask->m_fEncodedPercent) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedPercent = fEncodedPercent;
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}
		TRACE("Blu-ray Menu Percent %.1f%%\n", m_pTask->m_fEncodedPercent);
		m_pTask->Unlock();
		return TRUE;
	} 
	else if(re.exec(lpszLine, "Error:") > 0)
	{
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s", lpszLine);
	}
	else if(re.exec(lpszLine, "Information:") > 0)
	{
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
	}
	else
	{
		TRACE("%s\n", lpszLine);
#ifdef _DEBUG
		CString strTemp = lpszLine;
		strTemp.TrimRight("\r\n");
		CLog::Instance()->AppendLog("%s\r\n", strTemp);
#endif
	}

	return FALSE;
}
#endif
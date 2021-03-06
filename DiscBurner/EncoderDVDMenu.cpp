#include "StdAfx.h"
#include "EncoderDVDMenu.h"
#include "Task.h"
#include "Options.h"
#include "xmlutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DVD

CEncoderDVDMenu::CEncoderDVDMenu(void)
{
}

CEncoderDVDMenu::~CEncoderDVDMenu(void)
{
	if (m_pTask != NULL)
		delete m_pTask;
}

BOOL CEncoderDVDMenu::Start()
{
	m_strCommandLine.Format("\"%s\" \"%s\" \"%s\"", m_strXMLFile, m_strResourceFolder, m_strOutputFolder);

	BOOL ret = CEncoder::Start();

	return ret;
}

CString CEncoderDVDMenu::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\DVDMenu.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderDVDMenu::ParseOutput(LPCTSTR lpszLine)
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
		TRACE("DVDMenu Percent %.1f%%\n", m_pTask->m_fEncodedPercent);
		m_pTask->Unlock();

		return TRUE;
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
		TRACE("DVDMenu:%s\n", strMsg);
		CLog::Instance()->AppendLog("DVDMenu %s\r\n", strMsg);
#endif
	}

	return FALSE;
}

#endif
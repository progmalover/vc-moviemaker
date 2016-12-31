#include "StdAfx.h"
#include "EncoderDVD.h"
#include "Task.h"
#include "Options.h"
#include "xmlutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DVD

CEncoderDVD::CEncoderDVD(void)
{
	m_nTotalSize = 0;
}

CEncoderDVD::~CEncoderDVD(void)
{
	if (m_pTask != NULL)
		delete m_pTask;
}

BOOL CEncoderDVD::Start()
{
	m_strCommandLine.Format("-o \"%s\" -x \"%s\"", m_strOutputFolder, m_strXMLFile);

	BOOL ret = CEncoder::Start();

	return ret;
}

CString CEncoderDVD::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\DVDAuthor.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderDVD::ParseOutput(LPCTSTR lpszLine)
{
	// Phase 1: STAT: VOBU 16 at 2MB, 1 PGCS				// About 60
	// Phase 2: STAT: fixing VOBU at 3MB (17/6220, 0%)		// About 40

	Regexx re;
	if (re.exec(lpszLine, ".*STAT:[ \t]*VOBU[ \t]+[0-9]+[ \t]+at[ \t]+([0-9]+MB)") > 0)
	{
		std::string m_sEncodedSize = re.match[0].atom[0].str();
		INT64 nEncodedSize = (INT64)atof(m_sEncodedSize.c_str()) * (1024 * 1024);

		m_pTask->Lock();

		double fEncodedPercent = max(0.0, min(100.0, (double)nEncodedSize * 100 / m_nTotalSize * 0.6));
		if (fabs(fEncodedPercent - m_pTask->m_fEncodedPercent) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedPercent = fEncodedPercent;
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}
		TRACE("Percent %.1f%%\n", m_pTask->m_fEncodedPercent);

		m_pTask->Unlock();

		return TRUE;
	}
	else if (re.exec(lpszLine, ".*STAT:[ \t]*fixing VOBU at ([0-9]+)MB") > 0)
	{
		std::string m_sEncodedSize = re.match[0].atom[0].str();
		INT64 nEncodedSize = (INT64)atof(m_sEncodedSize.c_str()) * (1024 * 1024);

		m_pTask->Lock();

		double fEncodedPercent = max(0.0, min(100.0, 60.0 + (double)nEncodedSize * 100 / m_nTotalSize * 0.4));
		if (fabs(fEncodedPercent - m_pTask->m_fEncodedPercent) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedPercent = fEncodedPercent;
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}
		TRACE("Percent %.1f%%\n", m_pTask->m_fEncodedPercent);

		m_pTask->Unlock();

		return TRUE;
	}
	else
	{
		TRACE("%s\n", lpszLine);
	}

	return FALSE;
}

#endif
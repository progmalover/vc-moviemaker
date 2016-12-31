#include "StdAfx.h"
#include "EncoderMEncoder.h"
#include "Task.h"

CEncoderMEncoder::CEncoderMEncoder()
{
}

CEncoderMEncoder::~CEncoderMEncoder(void)
{
}

CString CEncoderMEncoder::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\mencoder.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderMEncoder::ParseOutput(LPCTSTR lpszLine)
{
	// Pos:  12.6s    191f ( 0%) 54.02fps Trem:   0min   0mb  A-V:0.010 [1028:127]

	Regexx re;
	if (re.exec(lpszLine, "^Pos\\:[ \t]*([0-9\\.]+)s[ \t]*[0-9]+f") > 0)
	{
		//TRACE("%s\n", lpszLine);

		std::string m_sTime = re.match[0].atom[0].str();

		m_pTask->Lock();

		float fEncodedTime = max(0, min(m_pTask->m_mi.m_fDuration, (float)atof(m_sTime.c_str())));
		if (fabs(fEncodedTime - m_pTask->m_fEncodedTime) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedTime = fEncodedTime;
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}
		TRACE("Time %.1f/%.1f\n", fEncodedTime, m_pTask->m_mi.m_fDuration);

		m_pTask->Unlock();

		return TRUE;
	}
	else
	{
		TRACE("%s\n", lpszLine);
	}

	return FALSE;
}

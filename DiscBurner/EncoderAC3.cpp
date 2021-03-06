#include "StdAfx.h"
#include "EncoderAC3.h"

#ifdef AC3_AFTEN
CEncoderAC3::CEncoderAC3(void)
{
}

CEncoderAC3::~CEncoderAC3(void)
{
}

CString CEncoderAC3::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\aften.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderAC3::ParseOutput(LPCTSTR lpszLine)
{
	//frames=124 time=3.936 progress=49 q=531.5 bw=59.0 bitrate=448.0 kbps 
	Regexx re;
	if (re.exec(lpszLine, "frames=[0-9]+.*time=([0-9\\.]+)") > 0)
	{
		TRACE("%s\n", lpszLine);
		std::string m_sTime = re.match[0].atom[0].str();
		m_pTask->Lock();

		float fEncodedTime = max(0, min(m_pTask->m_fDuration, (float)atof(m_sTime.c_str())));
		if (fabs(fEncodedTime - m_pTask->m_fEncodedTime) > FLT_EPSILON)
		{
			m_pTask->m_fEncodedTime = fEncodedTime*g_fEncoderAC3+m_pTask->m_fDuration*(1-g_fEncoderAC3);
			m_pTask->m_nUpdate |= UPDATE_STATUS;
			TRACE("Audio Percent Time %.1f/%.1f\n", m_pTask->m_fEncodedTime, m_pTask->m_fDuration);
		}
		m_pTask->Unlock();
	}
	else if(re.exec(lpszLine, "Error:") > 0)
	{
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
	}
	else
	{
		TRACE("%s\n", lpszLine);
#ifdef _DEBUG
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
#endif
	}
	return TRUE;
}

BOOL CEncoderAC3::Start()
{
	BOOL bRe = CEncoder::Start();
	return bRe;
}
#endif
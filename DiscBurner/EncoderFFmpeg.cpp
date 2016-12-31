#include "StdAfx.h"
#include "EncoderFFmpeg.h"
#include "Task.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEncoderFFmpeg::CEncoderFFmpeg():m_nConvertedFrames(0)
{
}

CEncoderFFmpeg::~CEncoderFFmpeg(void)
{
}

CString CEncoderFFmpeg::GetApplicationName()
{
	CString strApplication;
	strApplication.Format("%s\\encoder\\ffmpeg.exe", GetModuleFilePath());
	return strApplication;
}

BOOL CEncoderFFmpeg::Start()
{
	BOOL bRe = CEncoder::Start();
	CLog::Instance()->AppendLog("Converted Frames:%d.\r\n", m_nConvertedFrames);
	//如果转换后的视频显示为AVS错误信息，实际转换的视频就只包含2张图片
    return bRe && m_nConvertedFrames > 2;
}

BOOL CEncoderFFmpeg::ParseOutput(LPCTSTR lpszLine)
{
	// Video: frame=  191 fps=142 q=1.6 Lsize=    1089kB time=12.73 bitrate= 700.6kbits/s
	// Audio: size=     791kB time=40.49 bitrate= 160.0kbits/s

	Regexx re;
	if (re.exec(lpszLine, "^frame=[ \t]*[0-9]+.*time=[ \t]*([0-9\\.]+)") > 0 || 
		re.exec(lpszLine, "^size=[ \t]*[0-9]+.*time=[ \t]*([0-9\\.]+)") > 0)
	{
		if(m_nConvertedFrames < 30)
		{
			Regexx re2;
			if(re2.exec(lpszLine, "^frame=[ \t]*([0-9]+)") > 0)
			{
				std::string m_sFrame = re2.match[0].atom[0].str();
				m_nConvertedFrames = _ttoi(m_sFrame.c_str());
			}
		}

		std::string m_sTime = re.match[0].atom[0].str();
		//Example-1：frame=   11 fps=   0  q=5.0 size=    61KB time=10000000000.00

		//Example-2  frame=   11 fps=  0 q=5.0 size=       1kB time=0.99 bitrate=   5.1kbits/s    
		//           frame=   20 fps= 19 q=5.0 size=       1kB time=1.89 bitrate=   2.7kbits/s    
		//           frame=   29 fps= 18 q=5.0 size=       1kB time=2.78 bitrate=   1.8kbits/s
		//           frame=   38 fps= 18 q=22.0 size=      33kB time=0.54 bitrate= 493.8kbits/s
		Regexx re3;
		if(atof(m_sTime.c_str())>999999999.0)
			m_sTime = "0.0";
		else if(re3.exec(lpszLine, "^frame=[ \t]*[0-9]+.*size=[ \t]*([0-9]+)") > 0)
		{
			std::string m_sSize = re3.match[0].atom[0].str();
			if( atoi(m_sSize.c_str())<5)
				m_sTime = "0.0";
		}

		m_pTask->Lock();

		//以下代码，看上去有些“歧义”
		//float fEncodedTime = max(0, min(m_pTask->m_mi.m_fDuration, (float)(m_pTask->m_pCurAtom->starttime+atof(m_sTime.c_str()))));
		//m_pTask->m_mi.m_fDuration  修改为 m_pTask->m_duration 似乎更合理些，这样就可以避免“歧义”。

		//(1)m_pTask->m_duration 是视频需要转换的长度，其已经考虑了视频的Trim、时长限制2个影响的因素;
		//(2)m_pTask->m_pCurAtom 视频拆分考虑了视频的Trim、时长限制2个影响的因素;
		//(3)m_pTask->m_pCurAtom->starttime + atof(m_sTime.c_str()) <= m_pTask->m_duration <=  m_pTask->m_mi.m_fDuration;

		float fEncodedTime = max(0, min(m_pTask->m_fDuration, (float)(m_pTask->m_pCurAtom->starttime+atof(m_sTime.c_str()))));
		if (fabs(fEncodedTime - m_pTask->m_fEncodedTime) > FLT_EPSILON)
		{
#ifdef AC3_AFTEN 
			m_pTask->m_fEncodedTime = fEncodedTime*(1-g_fEncoderAC3);
			TRACE("Video Percent Time %.1f/%.1f\n", m_pTask->m_fEncodedTime, m_pTask->m_fDuration);
#else
			m_pTask->m_fEncodedTime = fEncodedTime;
#endif
			m_pTask->m_nUpdate |= UPDATE_STATUS;
		}

#ifndef AC3_AFTEN
		TRACE("Time %.1f/%.1f\n", fEncodedTime, m_pTask->m_fDuration);//m_pTask->m_mi.m_fDuration);
#endif
		m_pTask->Unlock();
		return TRUE;
	}
	else if(re.exec(lpszLine, "Unable") > 0)
	{
		TRACE("%s\n", lpszLine);
		CLog::Instance()->AppendLog("%s\r\n", lpszLine);
	}
	else
	{
		TRACE("%s\n", lpszLine);
	}
	return FALSE;
}

BOOL CEncoderFFmpeg::CanPause()
{
	return m_bOutput;
}

BOOL CEncoderFFmpeg::CanStop()
{
	return m_bOutput;
}

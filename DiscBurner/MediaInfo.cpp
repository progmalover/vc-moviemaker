#include "StdAfx.h"
#include "MediaInfo.h"
#include "Options.h"

#include <iostream>
#include <iomanip>

using namespace MediaInfoDLL;

CMediaInfo::CMediaInfo(void)
{
	m_nAudioCount = 0;
	m_nVideoCount = 0;

	m_i64FileSize = 0;
	m_fDuration = 0.0;

	m_nSampleRate = 0;
	m_nAudioBitrate = 0;
	m_nChannels = 0;

	m_nWidth = 0;
	m_nHeight = 0;
	m_fDisplayRatio = 0.0;
	m_fFrameRate = 0.0;
	m_nVideoBitRate = 0;

	m_nSampleRate = 0;
	m_nAudioBitrate = 0;
	m_nChannels = 0;
}

CMediaInfo::~CMediaInfo(void)
{
}

BOOL CMediaInfo::ParseMedia(LPCTSTR lpszFile)
{	
	MediaInfo MI;
	
	size_t ret = MI.Open(std::string(lpszFile));

	if (!MI.IsReady())
	{
		CLog::Instance()->AppendLog("Error: MI.IsReady() == FALSE.\r\n");
		return FALSE;
	}

	m_nAudioCount = atoi(MI.Get(Stream_General, 0, _T("AudioCount"), Info_Text, Info_Name).c_str());
	m_nVideoCount = atoi(MI.Get(Stream_General, 0, _T("VideoCount"), Info_Text, Info_Name).c_str());

	if (m_nAudioCount == 0 && m_nVideoCount == 0)
	{
		CLog::Instance()->AppendLog("Error: m_nAudioCount == 0 && m_nVideoCount == 0.\r\n");
		return FALSE;
	}

	m_sFormat = MI.Get(Stream_General, 0, _T("Format"), Info_Text, Info_Name);
	m_i64FileSize = _atoi64(MI.Get(Stream_General, 0, _T("FileSize"), Info_Text, Info_Name).c_str());
	m_fDuration = (float)(atof(MI.Get(Stream_General, 0, _T("Duration"), Info_Text, Info_Name).c_str()) / 1000.0);


	//const PCTSTR pszParams[] = 
	//{
	//	"Format/Info", 
	//	"CodecID/Info",
	//	"CodecID/Hint", 
	//	"CodecID"
	//};

	if (m_nAudioCount > 0)
	{
		// get audio info
		m_sAudioFormat = MI.Get(Stream_Audio, 0, _T("Format"), Info_Text, Info_Name);

		//for (int i =0; i < sizeof(pszParams) / sizeof(PCTSTR); i++)
		//{
		//	std::string sFormatAdd = MI.Get(Stream_Audio, 0, pszParams[i], Info_Text, Info_Name);
		//	if (!sFormatAdd.empty())
		//	{
		//		BOOL bNumeric = TRUE;
		//		for (int i = 0; i < sFormatAdd.size(); i++)
		//		{
		//			if (!_istdigit(sFormatAdd[i]))
		//			{
		//				bNumeric = FALSE;
		//				break;
		//			}
		//		}

		//		if (!bNumeric)
		//		{
		//			m_sAudioFormat += " (" + sFormatAdd + ")";
		//			break;
		//		}
		//	}
		//}

		if (m_sAudioFormat == "MPEG Audio")
		{
			std::string sVersion = MI.Get(Stream_Audio, 0, "Format_Version", Info_Text, Info_Name);
			//if (!sVersion.empty())
			//	m_sAudioFormat += " " + sVersion;

			std::string sProfile = MI.Get(Stream_Audio, 0, "Format_Profile", Info_Text, Info_Name);
			//if (!sProfile.empty())
			//	m_sAudioFormat += " " + sProfile;

			if (sProfile == "Layer 2")
			{
				//std::string sAudioFormat = "MP2";
				//sAudioFormat += " (" + m_sAudioFormat + ")";
				//m_sAudioFormat = sAudioFormat;
				m_sAudioFormat = "MP2";
			}
			else if (sProfile == "Layer 3")
			{
				//std::string sAudioFormat = "MP3";
				//sAudioFormat += " (" + m_sAudioFormat + ")";
				//m_sAudioFormat = sAudioFormat;
				m_sAudioFormat = "MP3";
			}
		}

		m_nSampleRate = atoi(MI.Get(Stream_Audio, 0, "SamplingRate", Info_Text, Info_Name).c_str());
		m_nAudioBitrate = (int)(atoi(MI.Get(Stream_Audio, 0, "BitRate", Info_Text, Info_Name).c_str()) / 1000 + 0.5);
		m_sAudioBitrateMode = MI.Get(Stream_Audio, 0, "BitRate_Mode", Info_Text, Info_Name);
		m_nChannels = atoi(MI.Get(Stream_Audio, 0, "Channel(s)", Info_Text, Info_Name).c_str());

		if (!m_sAudioBitrateMode.empty())
			m_sAudioFormat += " (" + m_sAudioBitrateMode + ")";
	}

	if (m_nVideoCount > 0)
	{
		// get video info
		m_sVideoFormat = MI.Get(Stream_Video, 0, _T("Format"), Info_Text, Info_Name);

		//for (int i =0; i < sizeof(pszParams) / sizeof(PCTSTR); i++)
		//{
		//	std::string sFormatAdd = MI.Get(Stream_Video, 0, pszParams[i], Info_Text, Info_Name);
		//	if (!sFormatAdd.empty())
		//	{
		//		BOOL bNumeric = TRUE;
		//		for (int i = 0; i < sFormatAdd.size(); i++)
		//		{
		//			if (!_istdigit(sFormatAdd[i]))
		//			{
		//				bNumeric = FALSE;
		//				break;
		//			}
		//		}

		//		if (!bNumeric)
		//		{
		//			m_sVideoFormat += " (" + sFormatAdd + ")";
		//			break;
		//		}
		//	}
		//}

		if (m_sVideoFormat == "MPEG Video")
		{
			std::string sVersion = MI.Get(Stream_Video, 0, "Format_Version", Info_Text, Info_Name);
			//if (!sVersion.empty())
			//	m_sVideoFormat += " " + sVersion;

			if (sVersion == "Version 1")
			{
				//std::string sVideoFormat = "MPEG-1";
				//sVideoFormat += " (" + m_sVideoFormat + ")";
				//m_sVideoFormat = sVideoFormat;
				m_sVideoFormat = "MPEG-1";
			}
			else if (sVersion == "Version 2")
			{
				//std::string sVideoFormat = "MPEG-2";
				//sVideoFormat += " (" + m_sVideoFormat + ")";
				//m_sVideoFormat = sVideoFormat;
				m_sVideoFormat = "MPEG-2";
			}
		}

		m_nWidth = atoi(MI.Get(Stream_Video, 0, _T("Width"), Info_Text, Info_Name).c_str());
		m_nHeight = atoi(MI.Get(Stream_Video, 0, _T("Height"), Info_Text, Info_Name).c_str());
		m_fFrameRate = (float)atof(MI.Get(Stream_Video, 0, "FrameRate", Info_Text, Info_Name).c_str());
		m_nVideoBitRate = (int)(atoi(MI.Get(Stream_Video, 0, "BitRate", Info_Text, Info_Name).c_str()) / 1000 + 0.5);
		m_sVideoBitrateMode = MI.Get(Stream_Video, 0, "BitRate_Mode", Info_Text, Info_Name);

		m_fDisplayRatio = (float)atof(MI.Get(Stream_Video, 0, "DisplayAspectRatio", Info_Text, Info_Name).c_str());
		if (COptions::Instance()->m_bFixAspectRatio)
		{
			if (m_nWidth == 1440 && m_nHeight == 1080 && fabs(m_fDisplayRatio - (float)4 / 3) < 0.01)
				m_fDisplayRatio = (float)16 / 9;
		}

		if (!m_sVideoBitrateMode.empty())
			m_sVideoFormat += " (" + m_sVideoBitrateMode + ")";
	}

	return TRUE;
}

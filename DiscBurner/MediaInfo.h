#pragma once

#include "..\MediaInfo\MediaInfoDLL.h"
#include <string>

class CMediaInfo
{
public:
	CMediaInfo(void);
	virtual ~CMediaInfo(void);

public:
	BOOL ParseMedia(LPCTSTR lpszFile);

	// General
	int m_nAudioCount;
	int m_nVideoCount;

	__int64 m_i64FileSize;
	std::string m_sFormat;
	float m_fDuration;

	// Video
	std::string m_sVideoFormat;	// Intelligently display
	int m_nWidth;
	int m_nHeight;
	float m_fDisplayRatio;
	float m_fFrameRate;
	int m_nVideoBitRate;				// Kbps
	std::string m_sVideoBitrateMode;

	// Audio
	std::string m_sAudioFormat;			// Intelligently display
	int m_nSampleRate;					// Hz
	int m_nAudioBitrate;				// Kbps
	std::string m_sAudioBitrateMode;
	int m_nChannels;
};

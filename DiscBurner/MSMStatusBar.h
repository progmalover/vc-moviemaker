#pragma once


enum
{
	ID_INDICATOR_TOTAL_DURATION = 101, 
	ID_INDICATOR_TOTAL_SIZE, 
	ID_INDICATOR_TIME_LEFT, 
	ID_INDICATOR_STATUS_ICON, 
	ID_INDICATOR_PROGRESS, 
};

enum
{
	SP_MESSAGE = 0, 
	SP_TOTAL_DURATION, 
	SP_TOTAL_SIZE, 
	SP_TIME_LEFT, 
	SP_STATUS_ICON, 
	SP_PROGRESS,
};

// CMSMStatusBar

class CMSMStatusBar : public CMFCStatusBar
{
	DECLARE_DYNAMIC(CMSMStatusBar)

public:
	CMSMStatusBar();
	virtual ~CMSMStatusBar();

	enum STATUS
	{
		STATUS_INIT, 
		STATUS_IDLE,
		STATUS_CONVERTING, 
		STATUS_PAUSED, 
	}m_nStatus;

	void Init();
	void UpdateSummary();
	void SetConvertingStatus(STATUS status);
	void UpdateTimeLeft(int nTimeLeft);

protected:
	DECLARE_MESSAGE_MAP()

	HICON m_hIconFilm;
	HICON m_hIconClock;
	HICON m_hIconDisc;
	HICON m_hIconDiscWarning;
	CBitmap m_bmpIdle;
	CImageList m_imlPaused;
	CImageList m_imlConverting;

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

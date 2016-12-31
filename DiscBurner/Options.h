#pragma once
#include "PGCAction.h"
#include "BDMenuUtils.h"

#define MENU_DURATION_MIN       15
#define MENU_DURATION_MAX       180
#define MENU_DURATION_DEFAULT   30

enum DOUBLE_CLICK_ACTION
{
	DCA_DO_NOTHING = 0, 
	DCA_PREVIEW, 
	DCA_PLAY, 
	DCA_VIEW_PROPERTIES, 
};

enum SHUTDOWN_OPTIONS
{
	SHUTDOWN_SLEEP = 0, 
	SHUTDOWN_HIBERNATE, 
	SHUTDOWN_SHUTDOWN,
};

BOOL ReadString(int nID,CStringArray *pStrArray,TCHAR chSep = ';');

class COptions
{
public:
	COptions(void);
	~COptions(void);

	DECLARE_SINGLETON(COptions)

protected:
	BOOL m_bLoaded;

public:

	CString m_strTempFolder;
	BOOL m_bCheckDiscStatus;
	BOOL m_bPromptNonBlankDisc;
	BOOL m_bPromptMakeAnotherDisc;

	CString m_strUniqueID;
	CString m_strDiscType;
#ifdef _BD
	CString m_strDimension;
	CString m_strFrameRate;
	LAST_TITLE_ACTION m_nAfterLastVideoOptions;
	CString m_strMenuDuration;
#else	// _BD
	CString m_strVideoStandard;
	CString m_strVideoAspect;
	NAVIGATION_ACTION m_nAfterCurVideoOptions; 
	NAVIGATION_ACTION m_nAfterLastVideoOptions; 
#endif	// _DVD
    CString m_strZoommode;
    
	BOOL m_bSoundFinished;
	CString m_strSoundFinished;

	int m_nMaxConcurrentTasks;
	int m_nEncodingThreads;
	BOOL m_bPromptStopConversion;
	BOOL m_bFixAspectRatio;
	BOOL m_bPromptIncompatibleCodecs;

	BOOL m_bShowHorzGridLines;
	BOOL m_bShowVertGridLines;
	BOOL m_bSmoothScrolling;
	int m_nDoubleClickAction;
	BOOL m_bShutDown;
	int m_nShutDownOptions;


public:
	BOOL LoadState();
	BOOL SaveState();

	BOOL  CheckFreeDiskSpace(INT64 nTotalSize);
	int GetNumberOfProcesseors();
	void UpdateULStyle();

	CString GetDefaultTempFolder();
	BOOL PrepareTempFolder(BOOL bPrompt);
};

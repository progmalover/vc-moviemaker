#pragma once

#include "PlayerToolBar.h"
#include "VideoPlayerWnd.h"
#include "Subjects.h"
#include "MFCToolBarLabel.h"
#include "Utils.h"
// CPlayerPane

class CPlayerPane : public CBasePane 
{
	DECLARE_DYNAMIC(CPlayerPane)
    
public:
	CPlayerPane();
	virtual ~CPlayerPane();
    DECLARE_SINGLETON(CPlayerPane)
protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

	DECLARE_OBSERVER(CPlayerPane, SUB_FILE_SEL_CHANGED)
#ifdef _MENU_TEMPLATE_VIEW
	DECLARE_OBSERVER(CPlayerPane, SUB_MENU_TEMPLATE_CHANGED)
	DECLARE_OBSERVER(CPlayerPane, SUB_CHANGE_ACTIVE_TAB_SETTINGS)
#endif

	BOOL m_bPreviewVideo;

	void PreviewVideo();

#ifdef _MENU_TEMPLATE_VIEW
	CBitmap m_bmpMenuTemplate;
	void PreviewMenuTemplate();
#endif

public:
	CVideoPlayerWnd m_wndPlayer;
	CPlayerToolbar m_wndPlayerToolBar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPlayerTrack();
	afx_msg void OnPlayerPlay();
	afx_msg void OnPlayerPause();
	afx_msg void OnPlayerStop();
	afx_msg void OnPlayerMute();
	afx_msg void OnPlayerVolume();

	afx_msg void OnUpdatePlayerTrack(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerPlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerPause(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerStop(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerMute(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerVolume(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlayerTime(CCmdUI *pCmdUI);
};

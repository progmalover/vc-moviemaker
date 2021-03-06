
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "Resource.h"
#include "ChildView.h"

#include "MSMMenuBar.h"
#include "MainToolBar.h"
#include "MSMStatusBar.h"
#include "SidePane.h"
#include "PropertyGridCtrlAdvanced.h"
#include "PropertyGridCtrlMenu.h"
#include "MFCToolBarHistoryComboBoxButton.h"
#include "MFCToolBarSlider.h"
#include "MFCCaptionBarEx.h"
#include "PlayerToolBar.h"
#include "TaskBar.h"

class CMainFrame : public CFrameWndEx
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:
	void UpdateThumbnailSlider();
	BOOL QueryExit();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CMSMMenuBar     m_wndMenuBar;
	CMainToolBar    m_wndToolBar;
	CMSMStatusBar	m_wndStatusBar;
	CSidePane	m_wndSidePane;

#ifdef _DVD
	CMFCCaptionBarEx m_wndMessageBar;
#endif

protected:  // control bar embedded members
	HWND m_hWndFocused;
	CChildView    m_wndView;
	void SetupMemoryBitmapSize (const CSize& sz);
	virtual BOOL OnShowPopupMenu (CMFCPopupMenu* pMenuPopup);

	CMenu				 m_menuFavotites;
	BOOL				 m_bIsHighColor;
	BOOL				 m_bMainToolbarMenu;
	UINT				 m_nCategorie;

// Generated message map functions
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnViewThumbnail();
	afx_msg void OnViewDetails();
	afx_msg void OnUpdateViewThumbnail(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewDetails(CCmdUI *pCmdUI);
	afx_msg void OnViewThumbnailMin();
	afx_msg void OnViewThumbnailMax();
	afx_msg void OnThumbnailSize();

	afx_msg void OnUpdateProfile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEnable(CCmdUI *pCmdUI);

	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnDWMCompositionChanged(WPARAM,LPARAM);
	afx_msg void OnViewStyleBlue();
	afx_msg void OnUpdateViewStyleBlue(CCmdUI *pCmdUI);
	afx_msg void OnViewStyleBlack();
	afx_msg void OnUpdateViewStyleBlack(CCmdUI *pCmdUI);
	afx_msg void OnViewStyleSilver();
	afx_msg void OnUpdateViewStyleSilver(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnDiscInformation();
	afx_msg void OnDiscQuickErase();
	afx_msg void OnDiscFullErase();
	afx_msg void OnDebugBurn();
	afx_msg void OnTempFolder();
	afx_msg void OnDebugMux();
	afx_msg void OnMakeFileList();
	afx_msg void OnTrimVideo();
	afx_msg void OnLoadMenuTemplate();

	afx_msg void OnViewOptions();
	afx_msg void OnHelpTopics();
	afx_msg void OnHelpRegister();
	afx_msg void OnHelpCheckForUpdates();

	afx_msg LRESULT OnTaskbarButtonCreated(WPARAM wp, LPARAM lp);
};

__inline CMFCToolBarEditBoxButton *GetDiscLabelEditBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		//CMFCToolBar &wndToolBar = pFrame->m_wndSidePane.m_wndPaneSimple.m_wndToolBar;
		//return (CMFCToolBarEditBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_DISC_LABEL));
	}
	return NULL;
}

__inline CMFCToolBarComboBoxButton *GetBurnerComboBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		CMFCToolBar &wndToolBar = CPaneSimple::Instance()->m_wndToolBar;
		return (CMFCToolBarComboBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_BURNER_LIST));
	}
	return NULL;
}

__inline CMFCToolBarComboBoxButton *GetDiscTypeComboBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		CMFCToolBar &wndToolBar = CPaneSimple::Instance()->m_wndToolBar;
		return (CMFCToolBarComboBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_DISC_TYPE_LIST));
	}
	return NULL;
}

__inline CMFCToolBarComboBoxButton *GetVideoDimensionComboBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		CMFCToolBar &wndToolBar = CPaneSimple::Instance()-> m_wndToolBar;
		return (CMFCToolBarComboBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_VIDEO_DIMENSION));
	}
	return NULL;
}

__inline CMFCToolBarComboBoxButton *GetVideoFrameRateComboBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		CMFCToolBar &wndToolBar = CPaneSimple::Instance()->m_wndToolBar;
		return (CMFCToolBarComboBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_VIDEO_FRAME_RATE));
	}
	return NULL;
}

__inline CMFCToolBarComboBoxButton *GetVideoStandardComboBox()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		CMFCToolBar &wndToolBar = CPaneSimple::Instance()->m_wndToolBar;
		return (CMFCToolBarComboBoxButton *)wndToolBar.GetButton(wndToolBar.CommandToIndex(ID_VIDEO_STANDARD));
	}
	return NULL;
}

__inline CPropertyGridCtrlAdvanced *GetPropertyGridAdvanced()
{
	 
	return &CPaneAdvanced::Instance()->m_wndPropList;
	 
}

__inline CPropertyGridCtrlMenu *GetPropertyGridMenu()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return &pFrame->m_wndSidePane.m_wndPaneMenu.m_wndPropList;
	return NULL;
}

__inline CMFCToolBarSlider *GetThumbnailSizeSlider()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return (CMFCToolBarSlider *)pFrame->m_wndToolBar.GetButton(pFrame->m_wndToolBar.CommandToIndex(ID_THUMBNAIL_SIZE));
	return NULL;
}

__inline CPlayerPane *GetPlayerPane()
{
	/*CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return &pFrame->m_wndSidePane.m_wndPlayerPane;*/
	return CPlayerPane::Instance();
}

__inline CVideoPlayerWnd *GetVideoPlayer()
{
	/*CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return &pFrame->m_wndSidePane.m_wndPlayerPane.m_wndPlayer;*/
	return &CPlayerPane::Instance()->m_wndPlayer;
}

__inline CMFCToolBarSlider *GetVideoPlayerTrack()
{
	CPlayerToolbar &playerToolBar = CPlayerPane::Instance()->m_wndPlayerToolBar;
	return (CMFCToolBarSlider*)playerToolBar.GetButton(playerToolBar.CommandToIndex(ID_PLAYER_TRACK));
	
}

__inline CMSMStatusBar *GetStatusBar()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return &pFrame->m_wndStatusBar;
	return NULL;
}

__inline void SetConversionProgress(int nPos)
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
	{
		pFrame->m_wndStatusBar.SetPaneProgress(SP_PROGRESS, nPos, TRUE);
	
		if (g_spTaskbar)
			g_spTaskbar->SetProgressValue(pFrame->m_hWnd, nPos, 100);
	}
}

__inline CSidePane *GetSidePane()
{
	CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != NULL)
		return &pFrame->m_wndSidePane;
	return NULL;
}

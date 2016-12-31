#pragma once

#include "MSMTabCtrl.h"
#include "ProfilePane.h"
#include "PaneSimple.h"
#include "PaneAdvanced.h"
#include "PaneMenu.h"
#include "PlayerPane.h"
#include "MenuViewPane.h"

#define TAB_SIMPLE		0
#define TAB_ADVANCED	1
#define TAB_MENU		2

// CSidePane
class CSidePane : public CDockablePane
{
	DECLARE_DYNAMIC(CSidePane)

public:
	CSidePane();
	virtual ~CSidePane();

protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

	virtual BOOL OnBeforeShowPaneMenu(CMenu& /*menu*/) { return FALSE; }

public:
	CMSMTabCtrl m_wndSettingsTab;
	//CPaneAdvanced m_wndPaneAdvanced;	// 注意先后顺序，先定义m_wndPaneAdvanced，保证它的Observer先注册
	//CPaneSimple m_wndPaneSimple;

	CPaneMenu m_wndPaneMenu;

	CMSMTabCtrl m_wndPlayerTab;
	//CPlayerPane m_wndPlayerPane;

#ifdef _MENU_TEMPLATE_VIEW
	CMSMTabCtrl m_wndMenuTab;
	CMenuViewPane m_wndMenuView;
#endif

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnChangeActiveTab(WPARAM wp, LPARAM lp);

#ifdef _MENU_TEMPLATE_VIEW
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
#endif
};

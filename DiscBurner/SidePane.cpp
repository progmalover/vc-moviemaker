// ProfilePane.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "SidePane.h"
#include "DeferWindowPos.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT ID_PROFILE_TAB				= 1001;
const UINT ID_SETTINGS_TAB				= 1002;
const UINT ID_PLAYER_TAB				= 1003;
const UINT ID_MENU_TAB				    = 1004;
// CSidePane

IMPLEMENT_DYNAMIC(CSidePane, CDockablePane)

CSidePane::CSidePane()
{

}

CSidePane::~CSidePane()
{
}


BEGIN_MESSAGE_MAP(CSidePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
END_MESSAGE_MAP()



// CSidePane message handlers



int CSidePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CRect rc(0, 0, 0, 0);

	// Settings Tab

	// Important : create the advanced pane first because simple pane creation procedure will notify the advanced pane of
	// property changes.
	if (!CPaneAdvanced::Instance()->Create(AfxRegisterWndClass(0), "Advanced", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, rc, this, 1002, 0))
	{
		TRACE0("Failed to create advanced pane\n");
		return -1;      // fail to create
	}

	if (!m_wndPaneMenu.Create(AfxRegisterWndClass(0), "Menu", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, rc, this, 1003, 0))
	{
		TRACE0("Failed to create simple pane\n");
		return -1;      // fail to create
	}

	if(!m_wndMenuView.Create(AfxRegisterWndClass(0), "MenuView", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, rc, this, 1005, 0))
	{
		TRACE0("Failed to create menu view  pane\n");
		return -1;      // fail to create
	}

 
	return 0;
}

void CSidePane::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO: Add your message handler code here

	AfxGetApp()->WriteProfileInt("Settings", "Active Tab", m_wndSettingsTab.GetActiveTab());
}

void CSidePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RepositionControls();

}

void CSidePane::RepositionControls()
{
	if (::IsWindow(m_hWnd))
	{

		CRect rcClient;
		GetClientRect(&rcClient);
		m_wndMenuView.MoveWindow(rcClient);
	}
}

LRESULT CSidePane::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
	if ((CWnd *)lp == &m_wndSettingsTab)
	{
#ifdef _MENU_TEMPLATE_VIEW
		if(m_wndMenuTab.GetSafeHwnd()!=NULL)
		{
			if(m_wndSettingsTab.GetActiveTab()==TAB_MENU)
			{
				m_wndMenuTab.ShowWindow(SW_SHOW);
				m_wndPlayerTab.ShowWindow(SW_HIDE);
				CSubjectManager::Instance()->GetSubject(SUB_CHANGE_ACTIVE_TAB_SETTINGS)->Notify(NULL);
			}
			else
			{
				m_wndMenuTab.ShowWindow(SW_HIDE);
				m_wndPlayerTab.ShowWindow(SW_SHOW);
			}

			//zxy
			
		}
#else
		CSubjectManager::Instance()->GetSubject(SUB_CHANGE_ACTIVE_TAB_SETTINGS)->Notify(NULL);
#endif	
	}
	return TRUE;
}


#ifdef _MENU_TEMPLATE_VIEW
BOOL CSidePane::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_wndPaneMenu.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	if (m_wndMenuView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return FALSE;
}
#endif

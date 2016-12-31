// MenuViewPane.cpp : implementation file
//
#include "stdafx.h"
#include "Converter.h"
#include "MenuViewPane.h"
#include "DeferWindowPos.h"
#include "MainFrm.h"
#include "MenuTemplateEx.h"
#include "MenuList.h"
#include "MenuPage.h"

#ifdef _MENU_TEMPLATE_VIEW
// CMenuViewPane
IMPLEMENT_DYNAMIC(CMenuViewPane, CBasePane)
CMenuViewPane::CMenuViewPane():
m_pWndMenuView(NULL)
{
	m_pWndMenuView = CMenuWnd::Instance();
}

CMenuViewPane::~CMenuViewPane()
{
	CMenuList::ReleaseInstance();
	CMenuTemplateEx::ReleaseInstance();
	CMenuWnd::ReleaseInstance();
}

BEGIN_MESSAGE_MAP(CMenuViewPane, CBasePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_MENU_PRE_PAGE, OnPrePage)
	ON_COMMAND(ID_MENU_NEXT_PAGE, OnNextPage)
	ON_UPDATE_COMMAND_UI(ID_MENU_PRE_PAGE, OnUpdatePrePage)
	ON_UPDATE_COMMAND_UI(ID_MENU_PAGE_INDEX,OnUpdatePageIndex)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT_PAGE, OnUpdateNextPage)
	ON_MESSAGE(MENU_PAGE_CHANGE,OnMenuPageChange)
END_MESSAGE_MAP()

// CMenuViewPane message handlers
int CMenuViewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBasePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pWndMenuView->CreateEx(0, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), this, ID_MENU_VIEW);

	if (!m_wndMenuToolBar.Create(this, WS_VISIBLE | WS_CHILD | CBRS_ALIGN_BOTTOM, ID_MENU_VIEW_TOOLBAR))
	{
		TRACE0("Failed to create menu toolbar\n");
		return -1;      // fail to create
	}

	m_wndMenuToolBar.SetPaneStyle(m_wndMenuToolBar.GetPaneStyle() & ~CBRS_GRIPPER);
	m_wndMenuToolBar.SetBorders(TOOLBAR_BORDER_X, TOOLBAR_BORDER_Y, TOOLBAR_BORDER_X, TOOLBAR_BORDER_Y);
    //m_wndMenuToolBar.LoadToolBar (IDR_PLAYER)---会导致MainFrame的ToolBar有问题。 
	//UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL /*bLocked*/ = FALSE, UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0, UINT uiHotResID = 0
	if (!m_wndMenuToolBar.LoadToolBar(IDR_MENU_VIEW,IDB_MENU_TOOLBAR_COLD, 0, TRUE, IDB_MENU_TOOLBAR_DIS, 0, IDB_MENU_TOOLBAR_HOT))
	{
		TRACE0("Failed to load menu toolbar\n");
		return -1;      // fail to load
	}

	m_wndMenuToolBar.SetRouteCommandsViaFrame(TRUE);
	m_wndMenuToolBar.SetOwner(this);
	m_wndMenuToolBar.SetCustomizeMode(FALSE);
	m_wndMenuToolBar.m_bInit = TRUE;

	return 0;
}

void CMenuViewPane::OnSize(UINT nType, int cx, int cy)
{
	CBasePane::OnSize(nType, cx, cy);
	RepositionControls();
}

void CMenuViewPane::RepositionControls()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		if(rcClient.Width()>0)
		{
			CSize size = m_wndMenuToolBar.GetButtonSize();
			size.cy += 2*TOOLBAR_BORDER_Y;
			size.cy *= 2;
			size.cy += 2;

			CDeferWindowPos dwp;

			dwp.BeginDeferWindowPos();

			dwp.DeferWindowPos(m_wndMenuToolBar, NULL, rcClient.left, rcClient.bottom - size.cy, rcClient.Width(), size.cy, 
				SWP_NOACTIVATE | SWP_NOZORDER);

			int x = rcClient.left;
			int w = rcClient.Width();
			int h = rcClient.Height() - size.cy;
			if (w > (int)((float)h * 4 / 3 + 0.5))
			{
				w = (int)((float)h * 4 / 3 + 0.5);
				x = rcClient.left + (rcClient.Width() - w) / 2;
			}

			dwp.DeferWindowPos(m_pWndMenuView->m_hWnd, NULL, x, rcClient.top, w, h, 
				SWP_NOACTIVATE | SWP_NOZORDER);

			dwp.EndDeferWindowPos();

			RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
		}
	}
}

void CMenuViewPane::OnPrePage()
{
	CMenuPage *pCurPage = CMenuList::Instance()->GetViewPage();
	ASSERT(pCurPage!=NULL &&  pCurPage->m_bEnablePreBtn);
	if(pCurPage!=NULL)
		pCurPage->m_iFocus = PAGE_BTN_PRE;
	CMenuList::Instance()->PreviewPage(TRUE);
	ChangePageIndex();
}

void CMenuViewPane::OnNextPage()
{
	CMenuPage *pCurPage = CMenuList::Instance()->GetViewPage();
	ASSERT(pCurPage!=NULL &&  pCurPage->m_bEnableNextBtn);
	if(pCurPage!=NULL)
		pCurPage->m_iFocus = PAGE_BTN_NEXT;
	CMenuList::Instance()->NextPage(TRUE);
	ChangePageIndex();
}

void CMenuViewPane::OnUpdatePrePage(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if(CMenuTemplateEx::Instance()->EnableMenu())
	{
		CMenuPage *pCurPage = CMenuList::Instance()->GetViewPage();
		if(pCurPage!=NULL)
			bEnable = pCurPage->m_bEnablePreBtn;
	}
	pCmdUI->Enable(bEnable);
}

void CMenuViewPane::OnUpdateNextPage(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if(CMenuTemplateEx::Instance()->EnableMenu())
	{
		CMenuPage *pCurPage = CMenuList::Instance()->GetViewPage();
		if(pCurPage!=NULL)
			bEnable = pCurPage->m_bEnableNextBtn;
	}
	pCmdUI->Enable(bEnable);
}

void CMenuViewPane::OnUpdatePageIndex(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	pCmdUI->Enable(bEnable);
}

LRESULT CMenuViewPane::OnMenuPageChange(WPARAM wParam, LPARAM lParam)
{
	ChangePageIndex();
	return 0;
}

void  CMenuViewPane::ChangePageIndex()
{
	BOOL bEnablePre=FALSE,bEnableNext=FALSE;
	CString strIndex;
	if(CMenuTemplateEx::Instance()->EnableMenu())
	{
		CMenuList *pList = CMenuList::Instance();
		CMenuPage *pCurPage = pList->GetViewPage();
		int nCur = pList->GetCurentPageIndex(),
			nAll = pList->GetPages();
		if(pCurPage!=NULL && nCur>0 && nAll>0)
			strIndex.Format(IDS_PAGE_INDEX2,nCur,nAll);
	}
	CMFCToolBarButton *pButton = m_wndMenuToolBar.GetButton(m_wndMenuToolBar.CommandToIndex(ID_MENU_PAGE_INDEX));
	pButton->m_strText = strIndex;
	//m_wndMenuToolBar.UpdateButtonEx(ID_MENU_PAGE_INDEX);
	m_wndMenuToolBar.ReDraw();
}

#endif
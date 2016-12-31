// MenuViewToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MenuViewToolBar.h"
#include "MFCToolBarLabel.h"
#include "MFCToolBarButtonEx.h"
#include "MFCToolBarButtonEx2.h"
// CMenuViewToolBar
#ifdef _MENU_TEMPLATE_VIEW
IMPLEMENT_DYNAMIC(CMenuViewToolBar, CMFCToolBarEx)
CMenuViewToolBar::CMenuViewToolBar():m_bInit(FALSE)
{

}

CMenuViewToolBar::~CMenuViewToolBar()
{
}


BEGIN_MESSAGE_MAP(CMenuViewToolBar, CMFCToolBarEx)
END_MESSAGE_MAP()



// CMenuViewToolBar message handlers
/*
Parameters:[in] bVertDock
    TRUE to specify that the toolbar is docked vertically; 
	FALSE to specify that the toolbar is docked horizontally.
Return Value:
    A CSize object that specifies the overall size of the buttons on the toolbar.
*/
CSize CMenuViewToolBar::CalcSize(BOOL bVertDock)
{
	CRect rc;
	GetWindowRect(&rc);
	return rc.Size();
}

/*
Remarks:Override this method to handle notification about a toolbar reset.The default implementation does nothing. 
        Override OnReset in a class derived from CMFCToolBar Class when the toolbar has dummy buttons that must be 
		replaced when the toolbar returns to its original state.
void CMFCToolBar::LoadToolBar()
*/
void CMenuViewToolBar::OnReset()
{
	CMFCToolBar::OnReset();
	ReplaceButton(ID_MENU_PAGE_INDEX, CMFCToolBarLabel(ID_MENU_PAGE_INDEX));
	ReplaceButton(ID_MENU_PRE_PAGE, CMFCToolBarButtonEx(ID_MENU_PRE_PAGE, 1));
	ReplaceButton(ID_MENU_NEXT_PAGE, CMFCToolBarButtonEx(ID_MENU_NEXT_PAGE, 2));

	((CMFCToolBarButtonEx *)GetButton(CommandToIndex(ID_MENU_PRE_PAGE)))->ShowBorder(TRUE);
	((CMFCToolBarButtonEx *)GetButton(CommandToIndex(ID_MENU_NEXT_PAGE)))->ShowBorder(TRUE);
}

//Loads the toolbar state information from the Windows registry.
BOOL CMenuViewToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

//void CMFCToolBar::OnSize(UINT nType, int cx, int cy)
void CMenuViewToolBar::AdjustLocations()
{
	ASSERT_VALID(this);

	if (GetSafeHwnd() == NULL || !::IsWindow(m_hWnd) || !m_bInit)
	{
		return;
	}

	CMFCToolBar::AdjustLocations();
	CRect rc;
	GetClientRect(&rc);
	if(rc.Width()>0)
	{
		CMFCToolBarButton *pButton;
		int border_x = 4,left = 0,top = 0;
		//Page Index
		pButton = GetButton(CommandToIndex(ID_MENU_PAGE_INDEX));
		CRect rcPageIndex = pButton->Rect();
		left =  rc.left + border_x-2;//(rc.Width()-rcPageIndex.Width())/2-border_x;
		top = (rc.Height()-rcPageIndex.Height())/2;
		rcPageIndex.OffsetRect(-(rcPageIndex.left - left),-(rcPageIndex.top - top));
		pButton->SetRect(rcPageIndex);

        //Next Page
		pButton = GetButton(CommandToIndex(ID_MENU_NEXT_PAGE));
		CRect rcNextPage = pButton->Rect();
		left  = rc.right - border_x - rcNextPage.Width();
		rcNextPage.OffsetRect(-(rcNextPage.left - left),-(rcNextPage.top-top));
		//rcNextPage.left -= 47;
		pButton->SetRect(rcNextPage);

		//Pre Page
		pButton = GetButton(CommandToIndex(ID_MENU_PRE_PAGE));
		CRect rcPrePage = pButton->Rect();
		left = rcNextPage.left-rcPrePage.Width()-border_x;
		rcPrePage.OffsetRect(-(rcPrePage.left - left),-(rcPrePage.top-top));
		//rcPrePage.left -= 68;
		pButton->SetRect(rcPrePage);

		/*
		CMFCToolBarButton *pButton1 = GetButton(CommandToIndex(ID_MENU_PRE_PAGE)),
		*pButton2 = GetButton(CommandToIndex(ID_MENU_PAGE_INDEX)),
		*pButton3 = GetButton(CommandToIndex(ID_MENU_NEXT_PAGE));

		CRect rcPrePage = pButton1->Rect(),
		rcPageIndex = pButton2->Rect(),
		rcNextPage = pButton3->Rect();

		int w = rcPrePage.Width()+rcPageIndex.Width()+rcNextPage.Width(),
		bargin_x = (rc.Width()-w)/4,
		left = 0,
		top = (rc.Height()-rcPrePage.Height())/2;

		//Pre  page
		left = rc.left + bargin_x;
		rcPrePage.OffsetRect(-(rcPrePage.left - left),-(rcPrePage.top-top));
		pButton1->SetRect(rcPrePage);

		//Page index
		left  = rcPrePage.right + bargin_x;
		rcPageIndex.OffsetRect(-(rcPageIndex.left - left),-(rcPageIndex.top-top));
		pButton2->SetRect(rcPageIndex);

		//Next page
		left = rcPageIndex.right+bargin_x;
		rcNextPage.OffsetRect(-(rcNextPage.left - left),-(rcNextPage.top-top));
		pButton3->SetRect(rcNextPage);
		*/
		UpdateTooltips();
	}
}

BOOL CMenuViewToolBar::ReDraw()
{
	if (GetSafeHwnd() == NULL)
		return FALSE;
	AdjustLocations();
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	return TRUE;
}

BOOL CMenuViewToolBar::UpdateButtonEx(int nID)
{
	UpdateButton(CommandToIndex(nID));
	return TRUE;
}
#endif

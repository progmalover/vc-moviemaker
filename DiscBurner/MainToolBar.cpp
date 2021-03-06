#include "stdafx.h"
#include "MainToolBar.h"
#include "resource.h"
#include "MFCToolBarSlider.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainToolBar

IMPLEMENT_DYNCREATE(CMainToolBar, CMFCToolBarEx)

CMainToolBar::CMainToolBar()
{
}


CMainToolBar::~CMainToolBar()
{
}

BEGIN_MESSAGE_MAP(CMainToolBar, CMFCToolBarEx)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainToolBar message handlers

void CMainToolBar::OnReset()
{
	CMFCToolBarEx::OnReset();

	CMFCToolBarSlider slider(ID_THUMBNAIL_SIZE, 80);
	ReplaceButton(ID_THUMBNAIL_SIZE, slider);
	
	((CMainFrame *)AfxGetMainWnd())->UpdateThumbnailSlider();

	//SetToolBarBtnText(CommandToIndex(ID_FILE_ADD), NULL, TRUE, TRUE);
	//SetToolBarBtnText(CommandToIndex(ID_FILE_CONVERT), NULL, TRUE, TRUE);

	int pButtons[] = 
	{
		CommandToIndex(ID_VIEW_DETAILS), 
		CommandToIndex(ID_VIEW_THUMBNAIL), 
		CommandToIndex(ID_THUMBNAIL_MIN) - 1, 
		CommandToIndex(ID_THUMBNAIL_MIN), 
		CommandToIndex(ID_THUMBNAIL_SIZE), 
		CommandToIndex(ID_THUMBNAIL_MAX)
	};
	
	for(int i = sizeof(pButtons) / sizeof(int) - 1; i >= 0; i--)
	          RemoveButton(pButtons[i]);

	EnableTextLabels(TRUE);

	SetHotBorder(FALSE);
}

BOOL CMainToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

void CMainToolBar::AdjustLocations()
{
	ASSERT(IsHorizontal());

	CMFCToolBarEx::AdjustLocations();

	/*int pButtons[] = 
	{
		CommandToIndex(ID_VIEW_DETAILS), 
		CommandToIndex(ID_VIEW_THUMBNAIL), 
		CommandToIndex(ID_THUMBNAIL_MIN) - 1, 
		CommandToIndex(ID_THUMBNAIL_MIN), 
		CommandToIndex(ID_THUMBNAIL_SIZE), 
		CommandToIndex(ID_THUMBNAIL_MAX)
	};
	
	for(int i = sizeof(pButtons) / sizeof(int) - 1; i >= 0; i--)
	{
		CMFCToolBarButton *pButton = GetButton(pButtons[i]);
		if (pButton == NULL)
			return;
	}

	CRect rectClient;
	GetClientRect(&rectClient);
	int right = rectClient.right;

	CRect rectLast = GetButton(CommandToIndex(ID_THUMBNAIL_MAX))->Rect();
	if (rectLast.right >= right)
		return;

	for(int i = sizeof(pButtons) / sizeof(CMFCToolBarButton *) - 1; i >= 0; i--)
	{
		CMFCToolBarButton *pButton = GetButton(pButtons[i]);
		CRect rect = pButton->Rect();
		int width = rect.Width();
		rect.right = right;
		rect.left = rect.right - width;
		pButton->SetRect(rect);
		right -= width;
	}*/

	UpdateTooltips();
}

// ProfileSimpleToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ProfileToolBar.h"
#include "MFCToolBarLabel.h"

// CProfileToolBar

IMPLEMENT_DYNAMIC(CProfileToolBar, CMFCToolBarEx)

CProfileToolBar::CProfileToolBar()
{
	m_bInit = FALSE;
}

CProfileToolBar::~CProfileToolBar()
{
}


BEGIN_MESSAGE_MAP(CProfileToolBar, CMFCToolBarEx)
END_MESSAGE_MAP()



// CProfileToolBar message handlers

CSize CProfileToolBar::CalcSize(BOOL bVertDock)
{
	CRect rc;
	GetWindowRect(&rc);
	return rc.Size();
}

void CProfileToolBar::OnReset()
{
	CMFCToolBarEx::OnReset();

	SetRouteCommandsViaFrame(FALSE);
	SetOwner(this);
	SetCustomizeMode(FALSE);
}

void CProfileToolBar::AdjustLocations()
{
	//TRACE("CProfileToolBar::AdjustLocations()\n");

	ASSERT_VALID(this);

	if (GetSafeHwnd() == NULL || !::IsWindow(m_hWnd) || !m_bInit)
	{
		return;
	}

	CMFCToolBarEx::AdjustLocations();

	UpdateTooltips();
}

BOOL CProfileToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

// ToolTipCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ToolTipCtrlEx.h"


// CToolTipCtrlEx

IMPLEMENT_DYNAMIC(CToolTipCtrlEx, CMFCToolTipCtrl)

CToolTipCtrlEx::CToolTipCtrlEx()
{
	CMFCToolTipInfo params;
	params.m_bVislManagerTheme = TRUE;
	SetParams(&params);
}

CToolTipCtrlEx::~CToolTipCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CToolTipCtrlEx, CMFCToolTipCtrl)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()



// CToolTipCtrlEx message handlers



int CToolTipCtrlEx::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	return MA_NOACTIVATE;
	//return CMFCToolTipCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

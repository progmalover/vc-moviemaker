// MFCToolBarLabel.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarLabel.h"


// CMFCToolBarLabel

IMPLEMENT_SERIAL(CMFCToolBarLabel, CMFCToolBarButton, 1)

CMFCToolBarLabel::CMFCToolBarLabel (UINT nID, LPCTSTR lpszText)
{
	if (lpszText != NULL)	
	{		
		m_strText = lpszText;	
	}	

	m_bText = TRUE;
	m_nID = nID;
	m_iImage = -1;
}

void CMFCToolBarLabel::OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
							  BOOL bHorz, BOOL /*bCustomizeMode*/, BOOL /*bHighlight*/, 
							  BOOL /*bDrawBorder*/,	BOOL /*bGrayDisabledButtons*/)
{
	UINT nStyle = m_nStyle;	
	m_nStyle &= ~TBBS_DISABLED;

	CMFCToolBarButton::OnDraw (pDC, rect, pImages, bHorz, FALSE,
								FALSE, FALSE, FALSE);

	m_nStyle = nStyle;
}

BOOL CMFCToolBarLabel::OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str)
{
	str.Empty();
	return TRUE;
}

void CMFCToolBarLabel::SetText(LPCTSTR lpszText)
{
	m_strText = lpszText;
	CMFCToolBar *pToolBar = (CMFCToolBar *)m_pWndParent;
	pToolBar->InvalidateButton(pToolBar->CommandToIndex(m_nID));
	pToolBar->UpdateWindow();
}
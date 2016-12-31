// MFCToolBarSlider.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarButtonEx.h"
#include "MSMVisualManager.h"


// CMFCToolBarButtonEx

IMPLEMENT_SERIAL(CMFCToolBarButtonEx, CMFCToolBarButton, 1)

// CMFCToolBarButtonEx message handlers

void CMFCToolBarButtonEx::CopyFrom(const CMFCToolBarButton& src)
{
	ASSERT_VALID(this);

	CMFCToolBarButton::CopyFrom(src);
	CMFCToolBarButtonEx &s = (CMFCToolBarButtonEx &)src;

}

void CMFCToolBarButtonEx::OnLButtonDown(CPoint point)
{
	ASSERT_VALID(this);
}

void CMFCToolBarButtonEx::OnLButtonUp(CPoint point)
{
	ASSERT_VALID(this);
}

void CMFCToolBarButtonEx::OnMouseMove(CPoint point)
{
	ASSERT_VALID(this);
}

BOOL CMFCToolBarButtonEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ASSERT_VALID(this);
	return FALSE;
}

void CMFCToolBarButtonEx::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz, 
								 BOOL bCustomizeMode, BOOL bHighlight, BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	CMFCToolBarButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

	if (m_bShowBorder)
	{
		// Draw button border:
		if (!bCustomizeMode)
			DrawBorder(pDC, rect,bHighlight);
	}
}

void CMFCToolBarButtonEx::DrawBorder(CDC* pDC, const CRect& rect,BOOL bHighlight)
{
	BOOL bChecked = m_nStyle & TBBS_CHECKED;
	BOOL bPressed = m_nStyle & TBBS_PRESSED;
	BOOL bDisabled = m_nStyle &TBBS_DISABLED;

	if (!bChecked && !bPressed && !bHighlight && !IsDroppedDown())
	{
		CMSMVisualManager *pManager = (CMSMVisualManager *)CMSMVisualManager::GetInstance();

		CMFCControlRenderer *pRenderer = &pManager->m_ctrlToolBarBtn;
		pRenderer->DrawFrame(pDC, rect, 0);
	}
}
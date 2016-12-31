// MFCToolBarSlider.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarMenuButtonWithBorder.h"
#include "MSMVisualManager.h"

// CMFCToolBarMenuButtonWithBorder

IMPLEMENT_SERIAL(CMFCToolBarMenuButtonWithBorder, CMFCToolBarMenuButton, 1)

// CMFCToolBarMenuButtonWithBorder message handlers
//
CMFCToolBarMenuButtonWithBorder::CMFCToolBarMenuButtonWithBorder()
{
}

CMFCToolBarMenuButtonWithBorder::CMFCToolBarMenuButtonWithBorder(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText, BOOL bUserButton)
	: CMFCToolBarMenuButton(uiID, hMenu, iImage, lpszText, bUserButton)
{
}

void CMFCToolBarMenuButtonWithBorder::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz, 
	BOOL bCustomizeMode, BOOL bHighlight, BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

	// Draw button border:
	if (!bCustomizeMode)
	{
		BOOL bChecked = m_nStyle & TBBS_CHECKED;
		BOOL bPressed = m_nStyle & TBBS_PRESSED;
		BOOL bDisabled = m_nStyle &TBBS_DISABLED;

		if (!bChecked && !bPressed && !bHighlight && !IsDroppedDown())
		{
			CMSMVisualManager *pManager = (CMSMVisualManager *)CMSMVisualManager::GetInstance();

			CMFCControlRenderer *pRenderer = &pManager->m_ctrlToolBarBtn;
			pRenderer->DrawFrame(pDC, rect, 0);

			if (m_pPopupMenu != NULL)
			{
				CRect rectSeparator = m_rectArrow;

				const int nSeparatorSize = 2;
				if (bHorz)
				{
					rectSeparator.right = rectSeparator.left + nSeparatorSize;
				}
				else
				{
					rectSeparator.bottom = rectSeparator.top + nSeparatorSize;
				}

				CPen pen(PS_SOLID, 1, pManager->m_clrSeparator2);

				CPen* pOldPen = pDC->SelectObject(&pen);
				ENSURE(pOldPen != NULL);

				if (bHorz)
				{
					pDC->MoveTo(rectSeparator.left, rectSeparator.top + 2);
					pDC->LineTo(rectSeparator.left, rectSeparator.bottom - 2);
				}
				else
				{
					pDC->MoveTo(rectSeparator.left  + 2, rectSeparator.top);
					pDC->LineTo(rectSeparator.right - 2, rectSeparator.top);
				}

				pDC->SelectObject(pOldPen);
			}
		}
	}
}

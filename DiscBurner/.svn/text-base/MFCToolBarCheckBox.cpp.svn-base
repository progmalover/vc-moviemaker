// MFCToolBarCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarCheckBox.h"

const int nTextMarginLeft = 4;
const int nTextMarginRight = 6;

// CMFCToolBarCheckBox

IMPLEMENT_SERIAL(CMFCToolBarCheckBox, CMFCToolBarButton, 1)

CMFCToolBarCheckBox::CMFCToolBarCheckBox(UINT nID, LPCTSTR lpszText)
{
	if (lpszText != NULL)	
	{		
		m_strText = lpszText;	
	}	

	m_nStyle |= TBBS_CHECKBOX;
	m_bText = TRUE;
	m_nID = nID;
	m_iImage = -1;
}

CMFCToolBarCheckBox::~CMFCToolBarCheckBox()
{
}

// CMFCToolBarCheckBox message handlers

SIZE CMFCToolBarCheckBox::OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	SIZE size = CMFCToolBarButton::OnCalculateSize(pDC, sizeDefault, bHorz);
	size.cx += 1 + 16 + nTextMarginLeft + nTextMarginRight;

	return size;
}

void CMFCToolBarCheckBox::OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
							   BOOL bHorz, BOOL bCustomizeMode, BOOL bHighlight, 
							   BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	if (m_rect.IsRectEmpty())
	{
		return;
	}

	if (!bCustomizeMode)
	{
		BOOL bChecked = (m_nStyle & TBBS_CHECKED) != 0;
		BOOL bPressed = (m_nStyle & TBBS_PRESSED) != 0;
		BOOL bDisabled = (m_nStyle & TBBS_DISABLED) != 0;

		const CSize sizeCheckBox = CSize(16, 16);

		// Draw check box:
		CRect rectCheck = m_rect;
		rectCheck.DeflateRect(CSize(2, 3));
		rectCheck.left++;
		rectCheck.right = rectCheck.left + sizeCheckBox.cx;
		rectCheck.top = rectCheck.CenterPoint().y - sizeCheckBox.cx / 2;

		rectCheck.bottom = rectCheck.top + sizeCheckBox.cy;

		CMFCVisualManager::GetInstance()->OnDrawCheckBoxEx(pDC, rectCheck, bChecked ? 1 : 0,
			bHighlight, bPressed, !bDisabled);

		// Draw text:
		COLORREF clrTextOld = (COLORREF)-1;

		if (bDisabled)
		{
			clrTextOld = pDC->SetTextColor(CMFCVisualManager::GetInstance()->GetToolbarDisabledTextColor());
		}
		else
		{
			clrTextOld = pDC->SetTextColor(CMFCVisualManager::GetInstance()->GetToolbarButtonTextColor(this, CMFCVisualManager::ButtonsIsRegular));
		}
		CRect rectText = m_rect;
		rectText.left = rectCheck.right + nTextMarginLeft;

		pDC->DrawText(m_strText, &rectText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor(clrTextOld);
		}
	}
}

int CMFCToolBarCheckBox::GetCheck()
{
	return (m_nStyle & TBBS_CHECKED);
}

void CMFCToolBarCheckBox::SetCheck(int nCheck)
{
	SetStyle(m_nStyle | nCheck);
}

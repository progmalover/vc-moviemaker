#include "StdAfx.h"
#include "MFCToolBarButtonEx2.h"
#include "MSMVisualManager.h"

IMPLEMENT_SERIAL(CMFCToolBarButtonEx2, CMFCToolBarButtonEx, 1)


void CMFCToolBarButtonEx2::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz, 
								 BOOL bCustomizeMode, BOOL bHighlight, BOOL bDrawBorder, BOOL bGrayDisabledButtons)
{
	//CMFCToolBarButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);
	ASSERT_VALID(pDC);
	ASSERT_VALID(this);

	m_bHorz = bHorz;

	// Fill button interior:
	FillInterior(pDC, rect, bHighlight);
	BOOL bHot = bHighlight;
	CSize sizeImage = (pImages == NULL) ? CSize(0, 0) : pImages->GetImageSize(TRUE);

	CString strDummyAmpSeq = _T("\001\001");
	int nTextMargin = 3;
	CRect rectInternal = rect;
	CSize sizeExtra = m_bExtraSize ? CMFCVisualManager::GetInstance()->GetButtonExtraBorder() : CSize(0, 0);
	rectInternal.DeflateRect(sizeExtra.cx / 2, sizeExtra.cy / 2);

	int x = m_bLeftText?(rectInternal.right-sizeImage.cx):(rectInternal.left);
	int y = rectInternal.top+(rectInternal.Height()-sizeImage.cy)/2;
	int iTextLen = 0;
	
	CString strWithoutAmp = m_strText;
	strWithoutAmp.Replace(_T("&&"), strDummyAmpSeq);
	strWithoutAmp.Remove(_T('&'));
	strWithoutAmp.Replace(strDummyAmpSeq, _T("&"));
	CSize sizeText = pDC->GetTextExtent(strWithoutAmp);

	BOOL bFadeImage = !bHighlight && CMFCVisualManager::GetInstance()->IsFadeInactiveImage();

	BOOL bDisabled = (bCustomizeMode && !IsEditable()) || (!bCustomizeMode &&(m_nStyle & TBBS_DISABLED));
	pImages->Draw(pDC, x, y, GetImage(), FALSE, bDisabled && bGrayDisabledButtons, FALSE, FALSE, bFadeImage);

	CMFCVisualManager::AFX_BUTTON_STATE state = CMFCVisualManager::ButtonsIsRegular;
	if (bHighlight)
	{
		state = CMFCVisualManager::ButtonsIsHighlighted;
	}
	else if (m_nStyle &(TBBS_PRESSED | TBBS_CHECKED))
	{
		// Pressed in or checked:
		state = CMFCVisualManager::ButtonsIsPressed;
	}
	COLORREF clrText = CMFCVisualManager::GetInstance()->GetToolbarButtonTextColor(this, state);
	pDC->SetTextColor(clrText);
	CRect rectText = rectInternal;
	if(!m_bLeftText) 
		rectText.left += sizeImage.cx;
	rectText.left += nTextMargin;
	UINT uiTextFormat = DT_VCENTER |DT_SINGLELINE|DT_LEFT;
	pDC->DrawText(strWithoutAmp, &rectText, uiTextFormat);

	CMFCToolBarButtonEx::DrawBorder(pDC,rect,bHighlight);
}

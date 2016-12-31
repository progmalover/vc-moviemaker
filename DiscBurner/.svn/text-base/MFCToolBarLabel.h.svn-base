#pragma once


// CMFCToolBarLabel

class CMFCToolBarLabel : public CMFCToolBarButton
{
	DECLARE_SERIAL(CMFCToolBarLabel)

public:
	CMFCToolBarLabel(UINT nID = 0, LPCTSTR lpszText = NULL);
	void  SetText(LPCTSTR lpszText);

protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
						 BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,						
						 BOOL bHighlight = FALSE,						
						 BOOL bDrawBorder = TRUE, 
						 BOOL bGrayDisabledButtons = TRUE);
	virtual BOOL OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str);
};

#pragma once


// CMFCToolBarCheckBox

class CMFCToolBarCheckBox : public CMFCToolBarButton
{
	DECLARE_SERIAL(CMFCToolBarCheckBox)

public:
	CMFCToolBarCheckBox(UINT nID = 0, LPCTSTR lpszText = NULL);
	virtual ~CMFCToolBarCheckBox();

protected:
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);

	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,						
		BOOL bHighlight = FALSE,						
		BOOL bDrawBorder = TRUE, 
		BOOL bGrayDisabledButtons = TRUE);

public:
	int GetCheck();
	void SetCheck(int nCheck);
};

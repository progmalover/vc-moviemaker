#pragma once


// CMFCToolBarMenuButtonWithBorder

class CMFCToolBarMenuButtonWithBorder : public CMFCToolBarMenuButton
{
	friend class CMFCToolBarEx;

	DECLARE_SERIAL(CMFCToolBarMenuButtonWithBorder)

public:
	CMFCToolBarMenuButtonWithBorder();
	CMFCToolBarMenuButtonWithBorder(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);

	// Attributes
public:

	// Operations
protected:

	// Overrides
public:
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);

protected:
};

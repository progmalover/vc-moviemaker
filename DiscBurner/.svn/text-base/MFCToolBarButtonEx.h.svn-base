#pragma once


// CMFCToolBarButtonEx

class CMFCToolBarButtonEx : public CMFCToolBarButton
{
	DECLARE_SERIAL(CMFCToolBarButtonEx)

public:
	CMFCToolBarButtonEx()
	{
		m_bShowBorder = FALSE;
	}

	CMFCToolBarButtonEx(UINT uiID, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE, BOOL bLocked = FALSE)
		: CMFCToolBarButton(uiID, iImage, lpszText, bUserButton, bLocked)
	{
		m_bShowBorder = FALSE;
	}

	// Attributes
public:
	BOOL IsDisabled() { return (m_nStyle & TBBS_DISABLED) != 0; }

	void ShowBorder(BOOL bShowBorder)
	{
		m_bShowBorder = bShowBorder;
		if (m_pWndParent != NULL)
		{
			CMFCToolBar *pToolBar = (CMFCToolBar *)m_pWndParent;
			pToolBar->UpdateButton(pToolBar->CommandToIndex(m_nID));
		}
	}

	// Operations
protected:
	BOOL m_bShowBorder;
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,						
		BOOL bHighlight = FALSE,						
		BOOL bDrawBorder = TRUE, 
		BOOL bGrayDisabledButtons = TRUE);
	void DrawBorder(CDC* pDC, const CRect& rect,BOOL bHighlight);

	// Overrides
public:
	virtual void CopyFrom(const CMFCToolBarButton& s);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);
	virtual void OnMouseMove(CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

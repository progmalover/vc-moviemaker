#pragma once

#include "MFCToolBarButtonEx.h"

enum{
	nNoneIndex = -1, 
	nThumbIndex, 
	nSliderIndex, 
	nZoomInIndex, 
	nZoomOutIndex,
};

// CMFCToolBarSlider

class CMFCToolBarSlider : public CMFCToolBarButtonEx
{
	DECLARE_SERIAL(CMFCToolBarSlider)

public:
	CMFCToolBarSlider();
	CMFCToolBarSlider(UINT nID, int nWidth = 0);
	virtual ~CMFCToolBarSlider();

	// Attributes
public:
	void SetRange(int nMin, int nMax);

	int GetRangeMin() const { return m_nMin; }
	int GetRangeMax() const { return m_nMax; }

	void SetPos(int nPos, BOOL bRedraw = TRUE);

	int GetPos() const { return m_nPos; }

	int GetZoomIncrement() const { return m_nZoomIncrement; }
	void SetZoomIncrement(int nZoomIncrement);

	void SetZoomButtons(BOOL bSet = TRUE);
	BOOL HasZoomButtons() const { return m_bZoomButtons; }

	void Redraw();

	BOOL IsPressed() { return (m_nStyle & TBBS_PRESSED) != 0; }

	int GetPressedIndex() { return m_nPressed; }

	// Operations
protected:
	void CommonInit();
	void SetThumbRect();
	int GetPosFromPoint(int x);
	int GetHitTest(CPoint point) const;

	void NotifyCommand();
	void RecalcLayout();

	// Overrides
public:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,						
		BOOL bHighlight = FALSE,						
		BOOL bDrawBorder = TRUE, 
		BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void CopyFrom(const CMFCToolBarButton& s);
	virtual BOOL OnClickUp() { return TRUE; }	// If OnClickUp(0 returns TRUE, CMFCToolBar will not send WM_COMMAND on WM_LBUTTONUP.

protected:
	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);
	virtual void OnMouseMove(CPoint point);
	virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	virtual BOOL IsAutoRepeatMode(int& nDelay /* ms */) const;
	virtual BOOL OnAutoRepeat();

	// Attributes
protected:
	int  m_nMin;
	int  m_nMax;
	int  m_nPos;
	int  m_nZoomIncrement;
	int  m_nWidth;
	int  m_nHighlighted;	// -1 - none, 0 - thumb, 1 -Zoom In, 2 - Zoom Out
	int  m_nPressed;		// -1 - none, 0 - thumb, 1 -Zoom In, 2 - Zoom Out
	BOOL m_bZoomButtons;

	CRect m_rectZoomOut;
	CRect m_rectZoomIn;
	CRect m_rectSlider;
	CRect m_rectThumb;
};

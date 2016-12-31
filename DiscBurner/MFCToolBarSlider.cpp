// MFCToolBarSlider.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarSlider.h"
#include "MSMVisualManager.h"

const int cySliderHeight = 18;
const int cxThumbWidth = 10;

// CMFCToolBarSlider

IMPLEMENT_SERIAL(CMFCToolBarSlider, CMFCToolBarButtonEx, 1)

CMFCToolBarSlider::CMFCToolBarSlider()
{
	CommonInit();
}

CMFCToolBarSlider::CMFCToolBarSlider(UINT nID, int nWidth)
{
	CommonInit();

	m_nID = nID;
	m_nWidth = nWidth;
}

CMFCToolBarSlider::~CMFCToolBarSlider()
{
}


// CMFCToolBarSlider message handlers

void CMFCToolBarSlider::CommonInit()
{
	m_nMin = 0;
	m_nMax = 100;
	m_nPos = 0;
	m_nZoomIncrement = 10;
	m_bZoomButtons = FALSE;
	m_nWidth = 100;

	m_rectZoomOut.SetRectEmpty();
	m_rectZoomIn.SetRectEmpty();
	m_rectSlider.SetRectEmpty();
	m_rectThumb.SetRectEmpty();

	m_nHighlighted = -1;
	m_nPressed = -1;
}

void CMFCToolBarSlider::OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
					 BOOL bHorz, BOOL bCustomizeMode,						
					 BOOL bHighlight,						
					 BOOL bDrawBorder, 
					 BOOL bGrayDisabledButtons)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	// Note: OnCalculateSize() may not have been called after the toolbar was resized.
	RecalcLayout();

	BOOL bDisabled = (m_nStyle & TBBS_DISABLED);
	BOOL bPressed = (m_nStyle & TBBS_PRESSED);

	CMSMVisualManager *pManager = (CMSMVisualManager *)CMFCVisualManager::GetInstance();

	if (m_bZoomButtons)
	{
		// Draw zoom buttons:
		pManager->OnDrawRibbonSliderZoomButton(pDC, (CMFCRibbonSlider *)this, m_rectZoomOut, TRUE,
			bHighlight && m_nHighlighted == nZoomOutIndex, bPressed && m_nPressed == nZoomOutIndex, bDisabled);
		pManager->OnDrawRibbonSliderZoomButton(pDC, (CMFCRibbonSlider *)this, m_rectZoomIn, FALSE,
			bHighlight && m_nHighlighted == nZoomInIndex, bPressed && m_nPressed == nZoomInIndex, bDisabled);
	}

	// Draw channel:
	CRect rectChannel = m_rectSlider;
	rectChannel.top = rectChannel.CenterPoint().y - 2;
	rectChannel.bottom = rectChannel.top + 2;

	//pManager->OnDrawRibbonSliderChannel(pDC, (CMFCRibbonSlider *)this, rectChannel);
	pManager->DrawSeparator(pDC, rectChannel, pManager->m_penSeparatorDark, pManager->m_penSeparator2, TRUE);

	// Draw thumb:
	pManager->OnDrawRibbonSliderThumb(pDC, (CMFCRibbonSlider *)this, m_rectThumb,
		bHighlight && (m_nHighlighted == nThumbIndex || m_nHighlighted == nSliderIndex), bPressed && m_nPressed == nThumbIndex, bDisabled);
}

SIZE CMFCToolBarSlider::OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	SIZE size = __super::OnCalculateSize(pDC, sizeDefault, bHorz);
	size.cx = m_nWidth;
	size.cy = sizeDefault.cy;

	RecalcLayout();

	return size;
}

void CMFCToolBarSlider::RecalcLayout()
{
	ASSERT_VALID(this);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	m_rectZoomOut.SetRectEmpty();
	m_rectZoomIn.SetRectEmpty();
	m_rectThumb.SetRectEmpty();

	m_rectSlider = m_rect;
	m_rectSlider.top = m_rect.CenterPoint().y - cySliderHeight / 2 + 1;
	m_rectSlider.bottom = m_rectSlider.top + cySliderHeight;
	m_rectSlider.right -= 1;

	if (m_bZoomButtons)
	{
		m_rectZoomOut = m_rectSlider;
		m_rectZoomOut.right = m_rectZoomOut.left + m_rectZoomOut.Height();

		m_rectZoomIn = m_rectSlider;
		m_rectZoomIn.left = m_rectZoomIn.right - m_rectZoomIn.Height();
	}

	int nThumbWidth = cxThumbWidth;

	m_rectSlider.DeflateRect(nThumbWidth / 2, 0);

	SetThumbRect();
}

void CMFCToolBarSlider::Redraw()
{
	CMFCToolBar *pToolBar = (CMFCToolBar *)m_pWndParent;
	pToolBar->InvalidateButton(pToolBar->CommandToIndex(m_nID));
	pToolBar->UpdateWindow();
}

void CMFCToolBarSlider::SetRange(int nMin, int nMax)
{
	ASSERT_VALID(this);

	m_nMin = nMin;
	m_nMax = nMax;
}

void CMFCToolBarSlider::SetPos(int nPos, BOOL bRedraw)
{
	ASSERT_VALID(this);

	m_nPos = min(max(m_nMin, nPos), m_nMax);
	SetThumbRect();

	if (bRedraw)
	{
		Redraw();
	}
}

void CMFCToolBarSlider::SetZoomIncrement(int nZoomIncrement)
{
	ASSERT_VALID(this);
	m_nZoomIncrement = nZoomIncrement;
}

void CMFCToolBarSlider::SetZoomButtons(BOOL bSet)
{
	ASSERT_VALID(this);
	m_bZoomButtons = bSet;
}

void CMFCToolBarSlider::SetThumbRect()
{
	ASSERT_VALID(this);

	if (m_nMax <= m_nMin || m_rect.IsRectEmpty())
	{
		m_rectThumb.SetRectEmpty();
		return;
	}

	m_rectThumb = m_rectSlider;

	double dx = ((double) m_rectSlider.Width()) /(m_nMax - m_nMin);
	int xOffset = (int)(.5 + dx *(m_nPos - m_nMin));

	int nThumbWidth = cxThumbWidth;

	m_rectThumb.left += xOffset - nThumbWidth / 2;
	m_rectThumb.right = m_rectThumb.left + nThumbWidth;
}

void CMFCToolBarSlider::OnLButtonDown(CPoint point)
{
	ASSERT_VALID(this);

	if (IsDisabled())
	{
		return;
	}

	int nHighlightedOld = m_nHighlighted;

	m_nHighlighted = GetHitTest(point);

	if (nHighlightedOld != m_nHighlighted)
	{
		Redraw();
	}

	m_nPressed = GetHitTest(point);
	
	if (m_nPressed = nSliderIndex)
	{
		int nPos = GetPosFromPoint(point.x);
		if (nPos != m_nPos)
		{
			m_nPressed = nThumbIndex;
			m_nHighlighted = nThumbIndex;
			SetPos(nPos);
			NotifyCommand();
		}
	}
}

void CMFCToolBarSlider::OnLButtonUp(CPoint point)
{
	ASSERT_VALID(this);

	if (m_nPressed == m_nHighlighted && !IsDisabled())
	{
		switch(m_nPressed)
		{
		case nZoomInIndex:
			SetPos(m_nPos + m_nZoomIncrement);
			break;

		case nZoomOutIndex:
			SetPos(m_nPos - m_nZoomIncrement);
			break;

		default:
			//SetPos(GetPosFromPoint(point.x));
			return;
		}

		NotifyCommand();
	}
}

void CMFCToolBarSlider::OnMouseMove(CPoint point)
{
	ASSERT_VALID(this);

	if (IsDisabled())
	{
		return;
	}

	int nHighlightedOld = m_nHighlighted;

	m_nHighlighted = GetHitTest(point);

	if (nHighlightedOld != m_nHighlighted)
	{
		Redraw();
	}

	if (IsPressed())
	{
		if (m_nPressed == nThumbIndex)
		{
			int nPos = GetPosFromPoint(point.x);
			if (nPos != m_nPos)
			{
				SetPos(nPos);
				NotifyCommand();
			}
		}
	}
}

BOOL CMFCToolBarSlider::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nWheelLines;
	if (::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nWheelLines, 0) == 0)
		nWheelLines = 3;

	if (nWheelLines == -1)
		nWheelLines = 3;

	int nLinesToScroll = max(1, (int)round(abs((float)zDelta / WHEEL_DELTA * nWheelLines)));

	int nPos = GetPos();
	if (zDelta > 0)
		nPos += nLinesToScroll;
	else
		nPos -= nLinesToScroll;

	int nPosNew = max(GetRangeMin(), min(GetRangeMax(), nPos));
	if (nPosNew != GetPos())
	{
		SetPos(nPosNew);
		NotifyCommand();
	}

	return TRUE;
}

int CMFCToolBarSlider::GetHitTest(CPoint point) const
{
	ASSERT_VALID(this);

	if (m_rectThumb.PtInRect(point))
	{
		return nThumbIndex;
	}

	if (m_rectSlider.PtInRect(point))
	{
		return nSliderIndex;
	}

	if (m_rectZoomOut.PtInRect(point))
	{
		return nZoomOutIndex;
	}

	if (m_rectZoomIn.PtInRect(point))
	{
		return nZoomInIndex;
	}

	return -1;
}

int CMFCToolBarSlider::GetPosFromPoint(int x)
{
	ASSERT_VALID(this);

	if (m_nMax <= m_nMin || m_rect.IsRectEmpty())
	{
		return m_nMin;
	}

	double dx = ((double) m_rectSlider.Width()) /(m_nMax - m_nMin);
	int xOffset = x - m_rectSlider.left;

	return m_nMin +(int)((double) xOffset / dx);
}

BOOL CMFCToolBarSlider::IsAutoRepeatMode(int& /*nDelay*/) const
{
	ASSERT_VALID(this);
	return m_nPressed == nZoomInIndex || m_nPressed == nZoomOutIndex;
}

BOOL CMFCToolBarSlider::OnAutoRepeat()
{
	ASSERT_VALID(this);

	if (IsDisabled())
	{
		return FALSE;
	}

	if (m_nPressed == nZoomInIndex)
	{
		SetPos(m_nPos + m_nZoomIncrement);
		NotifyCommand();
	}
	else if (m_nPressed == nZoomOutIndex)
	{
		SetPos(m_nPos - m_nZoomIncrement);
		NotifyCommand();
	}

	return TRUE;
}

void CMFCToolBarSlider::NotifyCommand()
{
	ASSERT_VALID(this);

	if (m_nID == 0 || m_nID == (UINT)-1)
	{
		return;
	}

	m_pWndParent->GetTopLevelFrame()->SendMessage(WM_COMMAND, m_nID);
}

void CMFCToolBarSlider::CopyFrom(const CMFCToolBarButton& src)
{
	ASSERT_VALID(this);

	CMFCToolBarButtonEx::CopyFrom(src);
	CMFCToolBarSlider &s = (CMFCToolBarSlider &)src;

	m_nMin = s.m_nMin;
	m_nMax = s.m_nMax;
	m_nPos = s.m_nPos;
	m_bZoomButtons = s.m_bZoomButtons;
	m_nWidth = s.m_nWidth;
}

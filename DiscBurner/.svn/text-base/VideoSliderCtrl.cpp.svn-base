// VideoSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "VideoSliderCtrl.h"
#include "Graphics.h"

// CVideoSliderCtrl

IMPLEMENT_DYNAMIC(CVideoSliderCtrl, CCustomSliderCtrl)
CVideoSliderCtrl::CVideoSliderCtrl()
{
}

CVideoSliderCtrl::~CVideoSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CVideoSliderCtrl, CCustomSliderCtrl)
END_MESSAGE_MAP()



// CVideoSliderCtrl message handlers


void CVideoSliderCtrl::GetChannelRect(LPRECT pRect) const
{
	CRect rc;
	GetClientRect(&rc);
	int nHeight = rc.Height();		

	if (m_bEnableSelection)
		rc.InflateRect(-nHeight / 3, -nHeight / 3);
	::CopyRect(pRect, &rc);
}

void CVideoSliderCtrl::GetThumbRect(LPRECT pRect) const
{
	CRect rc;
	GetClientRect(&rc);

	CSize size;
	if (m_bEnableSelection)
	{
		size.cx = rc.Height() / 3;
		if (size.cx % 2 != 0)
			size.cx++;
		size.cy = rc.Height() / 3;
	}
	else
	{
		size.cx = rc.Height();
		if (size.cx % 2 != 0)
			size.cx++;
		size.cy = rc.Height();
	}

	CPoint point = PosToPoint(m_nPos);

	CRect rcChannel;
	GetChannelRect(&rcChannel);

	if (m_bEnableSelection)
	{
		::SetRect(pRect, point.x - size.cx / 2, rc.top, point.x - size.cx / 2 + size.cx, rc.top + size.cy);
	}
	else
	{
		::SetRect(pRect, point.x - size.cx / 2, point.y - size.cy / 2, point.x - size.cx / 2 + size.cx, point.y + size.cy / 2);
	}
}

void CVideoSliderCtrl::GetSelectionMarkerRect(LPRECT pRect1, LPRECT pRect2) const
{
	ASSERT(m_bEnableSelection);
	if (!m_bEnableSelection)
		return;

	CRect rc;
	GetClientRect(&rc);

	CSize size;
	if (m_bEnableSelection)
	{
		size.cx = size.cy = rc.Height() / 3;
	}
	else
	{
		size.cx = size.cy = rc.Height();
	}

	CPoint ptStart = PosToPoint(m_nSelStart);
	CPoint ptEnd = PosToPoint(m_nSelEnd);

	CRect rcChannel;
	GetChannelRect(&rcChannel);

	::SetRect(pRect1, ptStart.x - size.cx, rc.bottom - size.cy, ptStart.x, rc.bottom);
	::SetRect(pRect2, ptEnd.x, rc.bottom - size.cy, ptEnd.x + size.cx, rc.bottom);
}

int CVideoSliderCtrl::PointToPos(POINT point) const
{
	CRect rcChannel;
	GetChannelRect(&rcChannel);

	int nRange = rcChannel.Width();

	int nPos = (int)((point.x - rcChannel.left) * ((double)max(1, m_nMax - m_nMin) /  nRange));
	//TRACE("nPos: %d\n", nPos);
	return max(m_nMin, min(m_nMax, nPos));
}

CPoint CVideoSliderCtrl::PosToPoint(int nPos) const
{
	CRect rcChannel;
	GetChannelRect(&rcChannel);

	int nRange = rcChannel.Width();

	CPoint point;
	point.x = (int)(rcChannel.left + nPos * ((double)nRange / max(1, m_nMax - m_nMin)));
	point.y = (rcChannel.top + rcChannel.bottom) / 2;

	return point;
}

void CVideoSliderCtrl::DrawChannel(CDC *pDC) const
{
	COLORREF crFace = ::GetSysColor(COLOR_3DFACE);
	COLORREF crWindow = ::GetSysColor(COLOR_WINDOW);
	COLORREF crShadow = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF crHilight = ChangeVValue(::GetSysColor(COLOR_3DSHADOW), 1.2);

	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, crFace);

	CRect rcChannel;
	GetChannelRect(&rcChannel);
	pDC->FillSolidRect(&rcChannel, crWindow);

	CPoint ptStart = PosToPoint(m_nSelStart);
	CPoint ptEnd = PosToPoint(m_nSelEnd);
	CRect rcSel(ptStart.x, rcChannel.top, ptEnd.x, rcChannel.bottom);
	pDC->FillSolidRect(&rcSel, crHilight);

	if (GetFocus() == this)
	{
		rc.InflateRect(-4, -4);
		pDC->DrawFocusRect(&rc);
	}
}

void CVideoSliderCtrl::DrawThumb(CDC *pDC) const
{
	COLORREF crFrame = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF crFill = ::GetSysColor(m_nFocusedHandle == 0 ? COLOR_3DSHADOW : COLOR_WINDOW);

	CRect rcThumb;
	GetThumbRect(&rcThumb);

	CPoint ptCenter = rcThumb.CenterPoint();
	POINT ptThumb[] = 
	{
		{rcThumb.left, rcThumb.top}, 
		{rcThumb.right, rcThumb.top}, 
		{rcThumb.right, ptCenter.y}, 
		{ptCenter.x, rcThumb.bottom}, 
		{rcThumb.left, ptCenter.y}, 
		{rcThumb.left, rcThumb.top}, 
	};

	CRgn rgnThumb;
	rgnThumb.CreatePolygonRgn(ptThumb, 5, WINDING);
	pDC->FillRgn(&rgnThumb, &CBrush(crFill));

	CPen pen(PS_SOLID, 1, crFrame);
	CPen *pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(ptThumb[0].x, ptThumb[0].y);
	for (int i = 1; i < sizeof (ptThumb) / sizeof(POINT); i++)
		pDC->LineTo(ptThumb[i]);

	CRect rcChannel;
	GetChannelRect(&rcChannel);
	pDC->MoveTo(rcThumb.CenterPoint().x, rcThumb.bottom);
	pDC->LineTo(rcThumb.CenterPoint().x, rcChannel.bottom);

	pDC->SelectObject(pOldPen);
}

void CVideoSliderCtrl::DrawSelectionMarker(CDC *pDC) const
{
	ASSERT(m_bEnableSelection);
	if (!m_bEnableSelection)
		return;

	COLORREF crFrame = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF crFill = ::GetSysColor(COLOR_WINDOW);
	COLORREF crFillFocused = ::GetSysColor(COLOR_3DSHADOW);

	CRect rcStart, rcEnd;
	GetSelectionMarkerRect(&rcStart, &rcEnd);

	CBrush brFrame(crFrame);
	CBrush brFill(crFill);
	CBrush brFillFocused(crFillFocused);

	CPen pen(PS_SOLID, 1, crFrame);
	CPen *pOldPen = pDC->SelectObject(&pen);

	POINT ptStart[] = 
	{
		{rcStart.left + 1, rcStart.bottom - 1}, 
		{rcStart.right, rcStart.top}, 
		{rcStart.right, rcStart.bottom - 1}, 
		{rcStart.left + 1, rcStart.bottom - 1}, 
	};
	CRgn rgnStart;
	rgnStart.CreatePolygonRgn(ptStart, 3, WINDING);
	pDC->FillRgn(&rgnStart, m_nFocusedHandle == 1 ? &brFillFocused : &brFill);
	pDC->MoveTo(ptStart[0].x, ptStart[0].y);
	for (int i = 1; i < sizeof (ptStart) / sizeof(POINT); i++)
		pDC->LineTo(ptStart[i]);

	POINT ptEnd[] = 
	{
		{rcEnd.left, rcEnd.bottom - 1}, 
		{rcEnd.left, rcEnd.top}, 
		{rcEnd.right - 1, rcEnd.bottom - 1}, 
		{rcEnd.left, rcEnd.bottom - 1}, 
	};
	CRgn rgnEnd;
	rgnEnd.CreatePolygonRgn(ptEnd, 3, WINDING);
	pDC->FillRgn(&rgnEnd, m_nFocusedHandle == 2 ? &brFillFocused : &brFill);
	pDC->MoveTo(ptEnd[0].x, ptEnd[0].y);
	for (int i = 1; i < sizeof (ptEnd) / sizeof(POINT); i++)
		pDC->LineTo(ptEnd[i]);

	pDC->SelectObject(pOldPen);
}

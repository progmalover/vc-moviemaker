// ScrollList.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ScrollList.h"
#include "MSMVisualManager.h"


// CScrollList

CScrollList::CScrollList()
{
	m_nItemHeight = afxGlobalData.GetTextHeight();
	m_nScrollerHeight = m_nItemHeight / 2;
	m_nTopIndex = 0;
	m_nSelIndex = -1;
	m_nHoverIndex = -1;
}

CScrollList::~CScrollList()
{
}

BEGIN_MESSAGE_MAP(CScrollList, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()



// CScrollList message handlers

int CScrollList::InsertItem(int index, LPARAM lParam)
{
	if (index <0 || index > (int)m_list.size())
		return -1;

	m_list.insert(m_list.begin() + index, lParam);

	if (::IsWindow(m_hWnd))
		Invalidate();

	return index;
}

BOOL CScrollList::DeleteItem(int index)
{
	if (index <0 || index > (int)m_list.size() - 1)
		return FALSE;

	m_list.erase(m_list.begin() + index);

	if (::IsWindow(m_hWnd))
		Invalidate();

	return TRUE;
}

void CScrollList::DeleteAllItems()
{
	if (m_list.size() == 0)
		return;

	m_list.clear();

	if (::IsWindow(m_hWnd))
		Invalidate();
}

BOOL CScrollList::SetItemData(int index, LPARAM lParam)
{
	if (index <0 || index > (int)m_list.size() - 1)
		return FALSE;

	m_list[index] = lParam;

	if (::IsWindow(m_hWnd))
		Invalidate();

	return TRUE;
}

LPARAM CScrollList::GetItemData(int index)
{
	if (index <0 || index > (int)m_list.size() - 1)
		return NULL;
	
	return m_list[index];
}

void CScrollList::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

void CScrollList::SetScrollerHeight(int nHeight)
{
	m_nScrollerHeight = nHeight;

	if (::IsWindow(m_hWnd))
		Invalidate();
}

BOOL CScrollList::GetItemRect(int index, LPRECT lpRect)
{
	if (index <0 || index > (int)m_list.size() - 1)
	{
		::SetRectEmpty(lpRect);
		return FALSE;
	}

	CRect rc;
	GetClientRect(&rc);

	int nTopIndex = GetTopIndex();
	::SetRect(lpRect, rc.left, rc.top + m_nItemHeight * (index - nTopIndex), rc.right, rc.top + m_nItemHeight * (index - nTopIndex + 1));

	return TRUE;
}

void CScrollList::SetTopIndex(int index)
{
	ASSERT(::IsWindow(m_hWnd));
	int nTopIndex = max(0, min(GetItemCount() - GetVisibleItems(), index));
	if (m_nTopIndex != nTopIndex)
	{
		m_nTopIndex = nTopIndex;
		Invalidate();
	}
}

void CScrollList::SetSelIndex(int index, BOOL bNotify)
{
	if (m_nSelIndex != index)
	{
		if (index >= 0 && index < GetCount())
		{
			m_nSelIndex = index;
		}
		else
		{
			m_nSelIndex = -1;
		}

		if (::IsWindow(m_hWnd))
			Invalidate();

		if (bNotify)
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SLN_SELCHANGE), (LPARAM)m_hWnd);
	}
}

void CScrollList::SetHoverIndex(int index, BOOL bNotify)
{
	if (m_nHoverIndex != index)
	{
		if (index >= 0 && index < GetCount())
		{
			m_nHoverIndex = index;
		}
		else
		{
			m_nHoverIndex = -1;
		}

		if (::IsWindow(m_hWnd))
			Invalidate();

		if (bNotify)
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SLN_HOVRTCHANGE), (LPARAM)m_hWnd);
	}
}

BOOL CScrollList::CanScrollUp()
{
	return GetTopIndex() > 0;
}

BOOL CScrollList::CanScrollDown()
{
	int nFirst = GetTopIndex();
	int nVisible = GetVisibleItems();
	return nFirst + nVisible < GetItemCount();
}

void CScrollList::Scroll(BOOL bScrollUp)
{
	int nTopIndex = max(0, min(GetItemCount() - GetVisibleItems(), m_nTopIndex + (bScrollUp ? -1 : 1)));
	if (nTopIndex == m_nTopIndex)
		return;

	SetTopIndex(nTopIndex);

	//CRect rc;
	//GetClientRect(&rc);

	//if (CanScrollUp())
	//	rc.top += GetScrollerHeight();

	//if (CanScrollDown())
	//	rc.bottom -= GetScrollerHeight();

	//for (int i = 0; i < GetItemHeight(); i++)
	//{
	//	ScrollWindow(0, bScrollUp ? 1 : -1, &rc, &rc);
	//	UpdateWindow();
	//}
}

int CScrollList::GetVisibleItems()
{
	CRect rc;
	GetClientRect(&rc);

	int count = rc.Height() / m_nItemHeight;
	ASSERT(rc.Height() % m_nItemHeight == 0);

	return count;
}

BOOL CScrollList::IsRectVisible(const CRect &rcClient, const CRect &rcItem)
{
	CRect rcTemp;
	rcTemp.IntersectRect(&rcClient, &rcItem);
	return !rcTemp.IsRectEmpty();
}

int CScrollList::ItemFromPoint(const CPoint &point)
{
	CRect rc;
	GetClientRect(&rc);

	if (!rc.PtInRect(point))
		return -1;

	if (CanScrollUp())
	{
		CRect rcTop(rc);
		rcTop.bottom = rcTop.top + m_nScrollerHeight;
		if (rcTop.PtInRect(point))
			return -1;
	}

	if (CanScrollDown())
	{
		CRect rcBottom(rc);
		rcBottom.top = rcBottom.bottom - m_nScrollerHeight;
		if (rcBottom.PtInRect(point))
			return -1;
	}

	int index = GetTopIndex() + (point.y - rc.top) / m_nItemHeight;

	return index >= 0 && index < GetCount() ? index : -1;
}

void CScrollList::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CListBox::OnPaint() for painting messages

	DoPaint(&dc);
}

void CScrollList::DoPaint(CDC *pDC)
{
	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

	CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);

	DrawBackground(&dcMem, rc);

	int nTopIndex = GetTopIndex();

	for (int i = nTopIndex; i < GetCount(); i++)
	{
		CRect rcItem;
		GetItemRect(i, &rcItem);

		//TRACE(_T("rcItem.left = %d, rcItem.top = %d\n"), rcItem.left, rcItem.top);

		if (IsRectVisible(rc, rcItem))
			DrawItem(&dcMem, i, rcItem);
		else
			break;
	}

	if (CanScrollUp())
	{
		CRect rcTop(rc);
		rcTop.bottom = rcTop.top + m_nScrollerHeight;
		DrawScroller(&dcMem, rcTop, TRUE);
	}

	if (CanScrollDown())
	{
		CRect rcBottom(rc);
		rcBottom.top = rcBottom.bottom - m_nScrollerHeight;
		DrawScroller(&dcMem, rcBottom, FALSE);
	}

	pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldBitmap);
}

void CScrollList::DrawBackground(CDC *pDC, const CRect &rc)
{
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_WINDOW));
}

void CScrollList::DrawScroller(CDC *pDC, const CRect &rc, BOOL bTop)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	
	BOOL bHover = rc.PtInRect(point);

	CMSMVisualManager *pManager = (CMSMVisualManager *)CMSMVisualManager::GetInstance();
	pManager->m_ctrlMenuScrollBtn[0].Draw(pDC, &rc, bHover ? 1 : 0, 204);

	CMenuImages::Draw(pDC, bTop ? CMenuImages::IdArrowUp : CMenuImages::IdArrowDown, 
		rc, CMenuImages::ImageBlack);
}

void CScrollList::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	TRACE(_T("CScrollList::OnLButtonDown(%d, %d)\n"), point.x, point.y);

	SetFocus();

	int index = ItemFromPoint(point);
	if (index != -1 && index != m_nSelIndex)
	{
		SetHoverIndex(index);
		SetSelIndex(index);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CScrollList::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	TRACE(_T("CScrollList::OnMouseMove(%d, %d)\n"), point.x, point.y);

	int index = ItemFromPoint(point);
	if (index != m_nHoverIndex)
	{
		SetHoverIndex(index);
	}

	CRect rc;
	GetClientRect(&rc);

	if (CanScrollUp())
	{
		CRect rcTop(rc);
		rcTop.bottom = rcTop.top + m_nScrollerHeight;
		if (rcTop.PtInRect(point))
			SetTimer(1000, 100, NULL);
	}
	
	if (CanScrollDown())
	{
		CRect rcBottom(rc);
		rcBottom.top = rcBottom.bottom - m_nScrollerHeight;
		if (rcBottom.PtInRect(point))
			SetTimer(1000, 100, NULL);
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CScrollList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	int nWheelLines;
	if (::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nWheelLines, 0) == 0)
		nWheelLines = 3;

	if (nWheelLines == -1)
		nWheelLines = max(1, GetVisibleItems() - 1);

	int nLinesToScroll = max(1, (int)round(abs((float)zDelta / WHEEL_DELTA * nWheelLines)));

	for (int i = 0; i < nLinesToScroll; i++)
		Scroll(zDelta > 0);

	ScreenToClient(&pt);

	return TRUE;
}

void CScrollList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1000)
	{
		CRect rc;
		GetClientRect(&rc);

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		if (CanScrollUp())
		{
			CRect rcTop(rc);
			rcTop.bottom = rcTop.top + m_nScrollerHeight;
			if (rcTop.PtInRect(point))
			{
				Scroll(TRUE);
				return;
			}
		}
		
		if (CanScrollDown())
		{
			CRect rcBottom(rc);
			rcBottom.top = rcBottom.bottom - m_nScrollerHeight;
			if (rcBottom.PtInRect(point))
			{
				Scroll(FALSE);
				return;
			}
		}

		KillTimer(1000);
	}

	CWnd::OnTimer(nIDEvent);
}

void CScrollList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	int nHover = GetSelIndex();

	switch (nChar)
	{
		case VK_UP:
		{
			if (nHover > 0)
			{
				SetHoverIndex(nHover - 1, FALSE);
				SetSelIndex(GetHoverIndex(), TRUE);
				if (GetSelIndex() < GetTopIndex())
					Scroll(TRUE);
			}
			break;
		}

		case VK_DOWN:
		{
			if (nHover < GetItemCount() - 1)
			{
				SetHoverIndex(nHover + 1, FALSE);
				SetSelIndex(GetHoverIndex(), TRUE);
				if (GetSelIndex() > GetTopIndex() + GetVisibleItems() - 1)
					Scroll(FALSE);
			}			
			break;
		}

		case VK_HOME:
			if (GetItemCount() > 0 && nHover != 0)
			{
				SetHoverIndex(0, FALSE);
				SetSelIndex(GetHoverIndex(), TRUE);
				SetTopIndex(GetSelIndex());
			}
			break;

		case VK_END:
			if (GetItemCount() > 0 && nHover != GetItemCount() - 1)
			{
				SetHoverIndex(GetItemCount() - 1, FALSE);
				SetSelIndex(GetHoverIndex(), TRUE);
				SetTopIndex(GetSelIndex());
			}
			break;

		case VK_ESCAPE:
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SLN_SELCANCEL), (LPARAM)m_hWnd);
			break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CScrollList::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	Invalidate(FALSE);
}

void CScrollList::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	Invalidate(FALSE);
}

LRESULT CScrollList::OnPrintClient(WPARAM wp, LPARAM lp)
{
	CDC *pDC = CDC::FromHandle((HDC)wp);	
	DoPaint(pDC);

	return TRUE;
}

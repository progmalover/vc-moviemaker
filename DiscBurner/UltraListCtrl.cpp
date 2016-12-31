// UltraListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "UltraListCtrl.h"
#include "Resource.h"
#include "VisualStylesXP.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT IDT_ENSUREVISIBLE	= 999;
const UINT IDT_AUTOSCROLL		= 998;
const UINT IDT_HOVER			= 997;

const UINT ID_HEADER			= 0;

#define HSB_LINE_SIZE			10
#define VSB_SCROLL_TIMES					5
#define VSB_LINE_SIZE			max(VSB_SCROLL_TIMES * 10, (GetItemSize().cy + GetItemSpace().cy) / 2 / VSB_SCROLL_TIMES * VSB_SCROLL_TIMES)

#define IsControlDown()			(GetKeyState(VK_CONTROL) & (1 << (sizeof(short int) * 8 - 1)))
#define IsShiftDown()			(GetKeyState(VK_SHIFT) & (1 << (sizeof(short int) * 8 - 1)))

// CUltraListCtrl

 
CUltraListCtrl::CUltraListCtrl()
{
	m_rcMargin.SetRect(0, 0, 0, 0);

	m_sizeIcon.cx = 32;
	m_sizeIcon.cy = 32;

	m_sizeIconSpace.cx = 0;
	m_sizeIconSpace.cy = 0;

	m_nItemHeight = afxGlobalData.GetTextHeight();
	m_nItemSpace = 0;

	m_nHover = -1;
	m_nFocus = -1;
	m_nAnchor = -1;
	m_nLastSelected = -1;

	m_dwStyle = 0;

	m_bSelecting = FALSE;
	m_rcSelection.SetRectEmpty();

	m_bAutoScroll = FALSE;
}

CUltraListCtrl::~CUltraListCtrl()
{
	ASSERT(m_items.size() == 0);
}


BEGIN_MESSAGE_MAP(CUltraListCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_NOTIFY(NM_RCLICK, 0, OnNmRclickHeader)
	ON_NOTIFY(HDN_DIVIDERDBLCLICKA, 0, OnHdnDividerDblClick)
	ON_NOTIFY(HDN_DIVIDERDBLCLICKW, 0, OnHdnDividerDblClick)
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdnItemChanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdnItemChanged)
	ON_NOTIFY(HDN_ENDDRAG, 0, OnHdnEndDrag)
	ON_NOTIFY(HDN_CHECK, 0, OnHdnCheck)
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()



// CUltraListCtrl message handlers


BOOL CUltraListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	return CUltraListCtrl::CreateEx(0, dwStyle, rect, pParentWnd, nID);
}

BOOL CUltraListCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW), ::GetSysColorBrush(COLOR_WINDOW), NULL);

	dwStyle |= WS_CLIPCHILDREN;

	return CWnd::CreateEx(dwExStyle, lpszClass, _T("ThumbnailList"), dwStyle, rect, pParentWnd, nID, NULL);
}

int CUltraListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | HDS_BUTTONS;
	if (m_dwStyle & ULS_HEADERDRAGDROP)
		dwStyle |= HDS_DRAGDROP;
	m_wndHeader.Create(dwStyle, CRect(0, 0, 0, 0), this, ID_HEADER);

	if(this->m_dwStyle & ULS_CHECKBOX)
		m_wndHeader.EnableCheckBox(TRUE);
	else
		m_wndHeader.EnableCheckBox(FALSE);

	m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_tooltip.SetMaxTipWidth(540);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 15 * 1000);

	RecalcHeaderPos();
	RecalcScrollPos();

	return 0;
}

int CUltraListCtrl::InsertItem(int index, DWORD_PTR dwData, BOOL bNotify)
{
	if (index >= 0 && index <= (int)m_items.size())
	{
		CListItem *pItem = new CListItem();
		pItem->m_dwData = dwData;
		m_items.insert(m_items.begin() + index, pItem);

		int cols = GetHeaderCtrl()->GetItemCount();
		for (int col = 0; col < cols; col++)
			pItem->m_list.push_back(NULL);

		if (m_nFocus == -1)
			m_nFocus = index;

		if (bNotify)
			OnInsertItem(index);

		m_sel.OnInsertItem(index);

		RecalcScrollPos();
		Invalidate(FALSE);

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		UpdateToolTip(point);

		return index;
	}

	return -1;
}

void CUltraListCtrl::DeleteItem(int index, BOOL bNotify)
{
	ASSERT(index >= 0 && index < (int)m_items.size());
	if (index >= 0 && index < (int)m_items.size())
	{
		ITEM_LIST::iterator it = m_items.begin() + index;

		if (IsItemSelected(index))
		{
			// send ULN_SELCHANGED message
			UnselectItem(index);
		}

		if (bNotify)
			OnDeleteItem(index);

		m_sel.OnDeleteItem(index);

		delete *it;
		m_items.erase(it);

		if (m_nFocus >= index)
			m_nFocus--;

		RecalcScrollPos();
		Invalidate(FALSE);

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		UpdateToolTip(point);
	}
}

void CUltraListCtrl::OnDeleteItem(int index)
{
}

void CUltraListCtrl::OnInsertItem(int index)
{
}

void CUltraListCtrl::DeleteAllItems()
{
	bool bSendSelChange = (m_sel.GetCount() > 0);

	if (m_items.size() > 0)
	{
		int index = 0;
		for (ITEM_LIST::iterator it = m_items.begin(); it != m_items.end(); it++, index++)
		{
			OnDeleteItem(index);
			delete *it;
		}
		m_items.clear();
		m_sel.Clear();
		m_nFocus = -1;

		RecalcScrollPos();
		Invalidate(FALSE);

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		UpdateToolTip(point);
	}
	
	if (bSendSelChange)
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
}

void CUltraListCtrl::GetWorkSpace(LPRECT pRect)
{
	CRect rc;
	GetClientRect(pRect);

	if (IsReportView())
		pRect->top += GetHeaderSize().cy;
}

// Note: return academic value, even the item does not exist
int CUltraListCtrl::GetFirstVisibleRow()
{
	int nScrollPos = GetScrollPos(SB_VERT);
	int nFirstRow = max(0, nScrollPos - m_rcMargin.top) / (GetItemSize().cy + GetItemSpace().cy);

	CRect rc;
	GetWorkSpace(&rc);

	CRect rcItem;
	if (IsReportView())
	{
		GetItemRect(nFirstRow, &rcItem);
	}
	else
	{
		int index = PosToIndex(nFirstRow, 0);
		ASSERT(index >= 0);
		GetItemRect(index, &rcItem);
	}

	// Maybe only the space below is visible
	if (rcItem.bottom > rc.top)
		return nFirstRow;
	else
		return nFirstRow + 1;
}

// Note: return academic value, even the item does not exist. may return ZERO
int CUltraListCtrl::GetVisibleRows()
{
	CRect rc;
	GetWorkSpace(&rc);

	CRect rcItem;
	int nFirstRow = GetFirstVisibleRow();
	if (IsReportView())
	{
		GetItemRect(nFirstRow, &rcItem);
	}
	else
	{
		int index = PosToIndex(nFirstRow, 0);
		ASSERT(index >= 0);
		GetItemRect(index, &rcItem);
	}

	// Calculate fully visible rows.
	rc.top = rcItem.bottom;
	int nRows = 1 + rc.Height() / (GetItemSize().cy + GetItemSpace().cy);
	
	// Is there one or two partially visible rows?
	int nRest = rc.Height() % (GetItemSize().cy + GetItemSpace().cy);
	if (nRest > GetItemSpace().cy)
		nRows++;

	return nRows;
}

CSize CUltraListCtrl::GetItemSize()
{
	return IsReportView() ? CSize(GetHeaderSize().cx, m_nItemHeight) : m_sizeIcon;
}

CSize CUltraListCtrl::GetItemSpace()
{
	if (IsReportView())
	{
		return CSize(0, m_nItemSpace);
	}
	else
	{
		CRect rc;
		GetWorkSpace(&rc);
		int nCols = GetRowCol().cx;

		if (nCols <= 1 || nCols >= GetItemCount())
		{
			return m_sizeIconSpace;
		}
		else 
		{
			// Do not make the horizontal space larger than the right margin
			int cx = (rc.Width() - (m_rcMargin.left/* + m_rcMargin.right*/) - m_sizeIcon.cx * nCols) / (nCols /*- 1*/);
			return CSize(max(0, cx), m_sizeIconSpace.cy);
		}
	}
}

void CUltraListCtrl::DoPaint(CDC &dc)
{
	CRect rc;
	GetClientRect(&rc);

	int left = rc.left;
	int top = rc.top;

	CRect rcClip;
	dc.GetClipBox(&rcClip);
	//TRACE(_T("DoPaint: GetClipBox() = (%d, %d, %d, %d)\n"), rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
	rcClip.OffsetRect(-left, -top);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);

	DrawBackground(&dcMem, &rc);

	int nFirstRow = GetFirstVisibleRow();
	int nVisibleRows = GetVisibleRows();
	int nLastRow = nFirstRow + nVisibleRows - 1;

	//TRACE(_T("DoPaint: nFirstRow = %d, nVisibleRows = %d\n"), nFirstRow, nVisibleRows);

	if (IsReportView())
	{
		for (int i = nFirstRow; i <= nLastRow && i < GetItemCount(); i++)
		{
			CRect rcItem;
			GetItemRect(i, &rcItem);

			CRect rcIntersect;
			if (rcIntersect.IntersectRect(&rcItem, &rcClip))
				DrawItem(&dcMem, i, &rcItem);
		}
	}
	else
	{
		int nCols = GetRowCol().cx;
		for (int i = nFirstRow; i <= nLastRow; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				int index = i * nCols + j;
				if (index >= (int)m_items.size())
					goto _done;

				CRect rcItem;
				GetItemRect(index, &rcItem);

				CRect rcIntersect;
				if (rcIntersect.IntersectRect(&rcItem, &rcClip))
					DrawItem(&dcMem, index, &rcItem);
			}
		}
	}

_done:
	if (m_bSelecting)
		DrawSelectionRect(&dcMem);

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
}

void CUltraListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	DoPaint(dc);
}

void CUltraListCtrl::DrawBackground(CDC *pDC, const CRect *pRect)
{
	CBrush brBack;
	brBack.CreateSysColorBrush((int)COLOR_WINDOW);
	pDC->FillRect(pRect, &brBack);

	if (IsReportView() && (m_dwStyle & (ULS_COLLINE | ULS_ROWLINE /* uncompleted */)))
	{
		CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DFACE));
		CPen *pOldPen = pDC->SelectObject(&pen);

		if (m_dwStyle & ULS_ROWLINE)
		{
			CRect rc;
			GetWorkSpace(&rc);

			int nFirstRow = GetFirstVisibleRow();

			// The content of the row before nFirstRow is invisible, but the below space may be visible.
			if (nFirstRow > 0)
				nFirstRow--;

			CRect rcItem;
			GetItemRect(nFirstRow, &rcItem);

			if (nFirstRow == 0 && m_rcMargin.top > GetItemSpace().cy / 2)
			{
				int y = rcItem.top - GetItemSpace().cy / 2;
				if (y > rc.top)
				{
					pDC->MoveTo(rc.left, y);
					pDC->LineTo(rc.right, y);
				}
			}

			int y = rcItem.bottom + GetItemSpace().cy / 2;
			for (int i = nFirstRow; ; i++)
			{
				if (y > rc.bottom)
					break;

				pDC->MoveTo(rc.left, y);
				pDC->LineTo(rc.right, y);

				y += (GetItemSize().cy + GetItemSpace().cy);
			}
		}

		if (m_dwStyle & ULS_COLLINE)
		{
			CHeaderCtrlEx *pHeader = GetHeaderCtrl();

			for (int i = 0; i < pHeader->GetItemCount(); i++)
			{
				CRect rcItem;
				pHeader->GetItemRect(i, &rcItem);
				pHeader->AdjustItemRect(rcItem);
				
				pDC->MoveTo(rcItem.right - 1, pRect->top);
				pDC->LineTo(rcItem.right - 1, pRect->bottom);
			}
		}

		pDC->SelectObject(pOldPen);
	}
}

void CUltraListCtrl::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
#ifdef _DEBUG
	if (IsItemSelected(index))
	{
		pDC->FillRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_HIGHLIGHT)));
		pDC->FrameRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));
	}
	else
	{
		pDC->FillRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_WINDOW)));
		pDC->FrameRect(pRect, CBrush::FromHandle(::GetSysColorBrush(COLOR_3DSHADOW)));
	}
#endif
}

BOOL CUltraListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE(_T("OnEraseBkgnd()\n"));

	return TRUE;
}

CSize CUltraListCtrl::GetRowCol()
{
	if (IsReportView())
	{
		return CSize(1, GetItemCount());
	}
	else
	{
		CRect rc;
		GetWorkSpace(&rc);

		int nCols = 0;
		if ((rc.Width() - m_rcMargin.left) > 0)
			nCols = 1;
		nCols += (rc.Width() - (m_rcMargin.left + m_sizeIcon.cx + m_rcMargin.right)) / (m_sizeIcon.cx + m_sizeIconSpace.cx);
		if (nCols == 0)
			nCols++;

		int nRows = (int)m_items.size() / nCols;
		if (m_items.size() % nCols > 0)
			nRows++;

		return CSize(nCols, nRows);
	}
}

CSize CUltraListCtrl::GetHeaderSize()
{
	if (IsReportView())
	{
		int cx = 0;
		for (int i = 0; i < m_wndHeader.GetItemCount(); i++)
			cx += m_wndHeader.GetItemWidth(i);

		CRect rc;
		m_wndHeader.GetWindowRect(&rc);

		return CSize(cx, rc.Height());
	}
	else
	{
		ASSERT(FALSE);
		return CSize(0, 0);
	}
}

void CUltraListCtrl::RecalcHeaderPos(BOOL bRedraw)
{
	if (IsReportView())
	{
		CRect rc;
		GetClientRect(&rc);

		CFont *pFont = GetFont();
		if (pFont == NULL)
			pFont = &afxGlobalData.fontRegular;

		m_wndHeader.SetFont(pFont);

		int left = rc.left;
		int top = rc.top;
		int bottom = rc.bottom;
		int cx = rc.Width();
		int cy = rc.Height();

		int x = 0;
		if (cx < GetHeaderSize().cx)
			x = GetScrollPos(SB_HORZ);

		// Layout() will change rc
		HDLAYOUT  hdl;
		WINDOWPOS wp;
		hdl.prc = &rc;
		hdl.pwpos = &wp;
		m_wndHeader.Layout(&hdl);

		int nHeader = wp.cy;

		UINT nFlags = SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE;
		if (!bRedraw)
			nFlags |= SWP_NOREDRAW;
		m_wndHeader.SetWindowPos(NULL, left - x, top, cx + x, nHeader, nFlags);
	}
	else
	{
		m_wndHeader.ShowWindow(SW_HIDE);
	}
}

void CUltraListCtrl::RecalcScrollPos()
{
	CRect rc;
	GetWorkSpace(&rc);

	// SB_HORZ
	{
		int nTotal = IsReportView() ? GetHeaderSize().cx : m_rcMargin.left + (GetItemSize().cx + GetItemSpace().cx) * GetRowCol().cx - GetItemSpace().cx + m_rcMargin.bottom;
		int nVisile = rc.Width();

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

		si.nMin = 0;

		if (nTotal <= nVisile)
		{
			si.nMax = 0;
			si.nPage = 0;
			si.nPos = 0;
		}
		else
		{
			si.nMax = nTotal - 1;
			si.nPage = nVisile;
			si.nPos = max(0, min(si.nMax - si.nPage + 1, GetScrollPos(SB_HORZ)));
		}

		SetScrollInfo(SB_HORZ, &si, TRUE);
	}

	// SetScrollInfo(SB_HORZ, &si, FALSE) will trigger another WM_SIZE message (reenter OnSize()) 
	// if visibility of the horizontal scroll bar was changed.
	GetWorkSpace(&rc);

	// SB_VERT
	{
		int nTotal = m_rcMargin.top + (GetItemSize().cy + GetItemSpace().cy) * (IsReportView() ? GetItemCount() : GetRowCol().cy) - GetItemSpace().cy + m_rcMargin.bottom;
		int nVisible = rc.Height();

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

		si.nMin = 0;

		if (nTotal <= nVisible)
		{
			si.nMax = 0;
			si.nPage = 0;
			si.nPos = 0;
		}
		else
		{
			si.nMax = nTotal - 1;
			si.nPage = nVisible;
			si.nPos = max(0, min(si.nMax - si.nPage + 1, GetScrollPos(SB_VERT)));
		}

		SetScrollInfo(SB_VERT, &si, TRUE);
	}
}

void CUltraListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE(_T("CUltraListCtrl::OnHScroll()\n"));

	ASSERT((GetStyle() & WS_HSCROLL) != 0);
	if ((GetStyle() & WS_HSCROLL) == 0)
		return;

	SCROLLINFO si;
	if (!GetScrollInfo(SB_HORZ, &si, SIF_ALL))	// no scroll bar
		return;

	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = HSB_LINE_SIZE;
		nPosNew = nPosOld + ds;
		break;
	case SB_LINEUP:
		ds = HSB_LINE_SIZE;
		nPosNew = nPosOld - ds;
		break;
	case SB_PAGEDOWN:
		ds = si.nPage > HSB_LINE_SIZE * 2 ? si.nPage - HSB_LINE_SIZE : si.nPage;
		nPosNew = nPosOld + ds;
		break;
	case SB_PAGEUP:
		ds = si.nPage > HSB_LINE_SIZE * 2 ? si.nPage - HSB_LINE_SIZE : si.nPage;
		nPosNew = nPosOld - ds;
		break;
	case SB_TOP:
		nPosNew = si.nMin;
		break;
	case SB_BOTTOM:
		nPosNew = si.nMax;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
		break;
	default:
		return;
	}

	nPosNew = min(si.nMax - si.nPage + 1, max(si.nMin, nPosNew));

	if (nPosNew != nPosOld)
	{
		si.fMask = SIF_POS;
		si.nPos = nPosNew;
		SetScrollInfo(SB_HORZ, &si, TRUE);
		RecalcHeaderPos(FALSE);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUltraListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	//TRACE(_T("CUltraListCtrl::OnVScroll()\n"));

	ASSERT((GetStyle() & WS_VSCROLL) != 0);
	if ((GetStyle() & WS_VSCROLL) == 0)
		return;

	SCROLLINFO si;
	if (!GetScrollInfo(SB_VERT, &si, SIF_ALL))	// no scroll bar
		return;

	int nPosOld = si.nPos;
	int nPosNew = nPosOld;

	int ds;
	switch (nSBCode)
	{
	case SB_LINEDOWN:
		ds = VSB_LINE_SIZE;
		nPosNew = nPosOld + ds;
		break;
	case SB_LINEUP:
		ds = VSB_LINE_SIZE;
		nPosNew = nPosOld - ds;
		break;
	case SB_PAGEDOWN:
		ds = si.nPage > VSB_LINE_SIZE * 2 ? si.nPage - VSB_LINE_SIZE : si.nPage;
		nPosNew = nPosOld + ds;
		break;
	case SB_PAGEUP:
		ds = si.nPage > VSB_LINE_SIZE * 2 ? si.nPage - VSB_LINE_SIZE : si.nPage;
		nPosNew = nPosOld - ds;
		break;
	case SB_TOP:
		nPosNew = si.nMin;
		break;
	case SB_BOTTOM:
		nPosNew = si.nMax;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ds = nPos - nPosOld;
		nPosNew = nPosOld + ds;
		break;
	default:
		return;
	}

	nPosNew = min(si.nMax - si.nPage + 1, max(si.nMin, nPosNew));

	if (nPosNew != nPosOld)
	{
		if ((nSBCode != SB_LINEDOWN && nSBCode != SB_LINEUP && nSBCode != SB_PAGEDOWN && nSBCode != SB_PAGEUP) || m_bSelecting)
		{
			si.fMask = SIF_POS;
			si.nPos = nPosNew;
			SetScrollInfo(SB_VERT, &si, TRUE);
			Invalidate();
		}
		else
		{
			int nScrollTimes;
			if (m_dwStyle & ULS_SMOOTHSCROLLING)
			{
				nScrollTimes = VSB_SCROLL_TIMES;
				if (abs(nPosOld - nPosNew) > VSB_LINE_SIZE * 3)
					nScrollTimes *= 2;
			}
			else
			{
				nScrollTimes = 1;
			}

			int nStep = (nPosOld - nPosNew) / nScrollTimes;
			int nLast = (nPosOld - nPosNew) % nScrollTimes;

			CRect rc;
			GetWorkSpace(&rc);

			for (int i = 0; i < nScrollTimes; i++)
			{
				si.fMask = SIF_POS;
				si.nPos = GetScrollPos(SB_VERT) - nStep;
				SetScrollInfo(SB_VERT, &si, TRUE);
				if (GetScrollPos(SB_VERT) == si.nPos)
				{
					ScrollWindow(0, nStep, &rc, NULL);
					UpdateWindow();
				}
				else
				{
					TRACE(_T("SetScrollInfo() failed (Step 1).\n"));
					Invalidate(FALSE);
				}
			}

			if (nLast != 0)
			{
				si.fMask = SIF_POS;
				si.nPos = GetScrollPos(SB_VERT) - nLast;
				SetScrollInfo(SB_VERT, &si, TRUE);
				if (GetScrollPos(SB_VERT) == si.nPos)
				{
					ScrollWindow(0, nLast, &rc, NULL);
					UpdateWindow();
				}
				else
				{
					TRACE(_T("SetScrollInfo() failed (Step 2).\n"));
					Invalidate(FALSE);
				}
			}
		}
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CUltraListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (::IsWindow(m_hWnd))
	{
		RecalcHeaderPos();
		RecalcScrollPos();
	}
}

void CUltraListCtrl::SelectItem(int index, BOOL bNotify)
{
	//TRACE(_T("SelectItem(%d)\n"), index);

	// Cancel pending delay scroll event
	CancelDelayEnsureVisible();

	if (m_dwStyle & ULS_MULTISEL)
	{
		if (m_sel.Find(index) < 0)
		{
			m_sel.Add(index);

			RedrawItem(index);

			if (bNotify)
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
		}
	}
	else
	{
		if (m_sel.GetCount() > 0)
		{
			ASSERT(m_sel.GetCount() == 1);
			int nOld = m_sel.GetAt(0);
			if (nOld != index)
			{
				m_sel.Select(index);
				SetFocusItem(index);
				RedrawItem(nOld);
				RedrawItem(index);

				if (bNotify)
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
			}
		}
		else
		{
			m_sel.Add(index);
			SetFocusItem(index);
			RedrawItem(index);
			if (bNotify)
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
		}
	}
}

void CUltraListCtrl::UnselectItem(int index)
{
	//TRACE(_T("UnselectItem(%d)\n"), index);
	if (m_sel.Find(index) >= 0)
	{
		m_sel.Remove(index);
		RedrawItem(index);

		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
	}
}

void CUltraListCtrl::SelectAllItems()
{
	m_sel.Clear();
	for (int i = 0; i < GetItemCount(); i++)
		m_sel.Add(i);

	Invalidate(FALSE);
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
}

void CUltraListCtrl::UnselectAllItems()
{
	if (m_sel.GetCount() > 0)
	{
		m_sel.Clear();
		Invalidate(FALSE);
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
	}
}

void CUltraListCtrl::SetFocusItem(int index)
{
	if (m_nFocus != index)
	{
		if (m_nFocus >= 0)
			RedrawItem(m_nFocus);

		m_nFocus = index;
		if (m_nFocus >= 0)
			RedrawItem(m_nFocus);
	}
}

void CUltraListCtrl::SetAnchorItem(int index)
{
	//TRACE(_T("SetAnchorItem(%d)\n"), index);
	m_nAnchor = index;
}

void CUltraListCtrl::GetCheckBoxRect(int nItem, RECT *pRect)
{
	ASSERT(nItem >= 0 && nItem < GetItemCount());
	::SetRectEmpty(pRect);
}

void CUltraListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();
	int index = ItemFromPoint(point);

	if (index >= 0 && (m_dwStyle & ULS_CHECKBOX))
	{
		CRect rcCheck;
		GetCheckBoxRect(index, &rcCheck);
		if (rcCheck.PtInRect(point))
		{
			BOOL bCheck = !GetCheck(index);
			
			if (!CanCheckItem(index, bCheck))
			{
				MessageBeep(0);
				return;
			}

			SetCheck(index, bCheck);
			OnCheckItem(index, bCheck);
	
			return;
		}
	}

	if (m_dwStyle & ULS_MULTISEL)
	{
		if (!IsControlDown() &&  !IsShiftDown())
		{
			if (m_sel.GetCount() == 1 && m_sel.GetAt(0) == index)
				return;

			UnselectAllItems();
		}

		if (index >= 0)
		{
			SetFocusItem(index);

			if (!IsShiftDown())
			{
				SetAnchorItem(index);

				if (!IsItemSelected(index))
				{
					SelectItem(index, TRUE);
				}
				else
				{
					if (IsControlDown())
						UnselectItem(index);
				}
			}
			else
			{
				if (!(m_nAnchor >= 0 && m_nAnchor < GetItemCount()))
				{
					ASSERT(FALSE);		// must be an error
					SetAnchorItem(index);
				}

				int nStart = min(m_nAnchor, index);
				int nEnd = max(m_nAnchor, index);

				m_sel.Clear();
				for (int i = nStart; i <= nEnd; i++)
					m_sel.Add(i);

				Invalidate(FALSE);
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
			}

			DelayEnsureVisible(index);
		}
		else
		{
			TrackSelection(point);
		}
	}
	else
	{
		if (index >= 0)
		{
			SetFocusItem(index);
			SelectItem(index, TRUE);
			DelayEnsureVisible(index);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CUltraListCtrl::TrackSelection(CPoint point)
{
	SetCapture();
	ASSERT(this == CWnd::GetCapture());
	UpdateWindow();

	m_bSelecting = TRUE;

	CDC *pDC = GetDC();
	pDC->SetBkColor(RGB(255, 255, 255));
	pDC->SetTextColor(RGB(0, 0, 0));

	BOOL bMoved = FALSE;

	CPoint ptOld = point;

	CRect rc;
	GetWorkSpace(&rc);

	CRect rcOld;
	rcOld.SetRectEmpty();

	// original selection
	CULSelection selOld = m_sel;
	// temporarily selected items
	CULSelection selNew;

	MSG msg;
	while (::GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
			{
				//TRACE(_T("%d: TraceSelection(): WM_MOUSEMOVE, msg.hwnd = %x, m_hWnd = %x\n"), GetTickCount(), msg.hwnd, m_hWnd);

				bMoved = TRUE;

				CPoint ptNew;
				ptNew.x = (int)(short)LOWORD(msg.lParam);
				ptNew.y = (int)(short)HIWORD(msg.lParam);
				ptNew.x = max(rc.left, min(rc.right, ptNew.x));
				ptNew.y = max(rc.top, min(rc.bottom, ptNew.y));

				BOOL bAutoScrolled = FALSE;

				// Auto scroll horizontally
				// HScroll will cause CHeaderCtrlEx to move and generate a WM_MOUSEMOVE message automatically.
				if ((GetStyle() & WS_HSCROLL) && (ptNew.x == rc.left || ptNew.x == rc.right))
				{
					int nPosOld = GetScrollPos(SB_HORZ);
					SendMessage(WM_HSCROLL, ptNew.x == rc.left ? SB_LINEUP : SB_LINEDOWN, 0);
					int nPosNew = GetScrollPos(SB_HORZ);
					int nDelta = nPosNew - nPosOld;
					if (nDelta != 0)
					{
						bAutoScrolled = TRUE;
						ptOld.x -= nDelta;
						rcOld.OffsetRect(-nDelta, 0);
					}
				}

				// Auto scroll vertically
				if ((GetStyle() & WS_VSCROLL) && (ptNew.y == rc.top || ptNew.y == rc.bottom))
				{
					EnableAutoScroll(TRUE);

					int nPosOld = GetScrollPos(SB_VERT);
					SendMessage(WM_VSCROLL, ptNew.y == rc.top ? SB_LINEUP : SB_LINEDOWN, 0);
					int nPosNew = GetScrollPos(SB_VERT);
					int nDelta = nPosNew - nPosOld;
					if (nDelta != 0)
					{
						bAutoScrolled = TRUE;
						ptOld.y -= nDelta;
						rcOld.OffsetRect(0, -nDelta);
					}
				}
				else
				{
					EnableAutoScroll(FALSE);
				}

				CRect rcNew(ptOld, ptNew);
				rcNew.NormalizeRect();
				if (rcNew != rcOld)
				{
					int nVScrollPos = GetScrollPos(SB_VERT);

					if (IsReportView())
					{
						int nStartRow = max(0, (rcNew.top - (GetHeaderSize().cy + m_rcMargin.top) + nVScrollPos) / 
							(GetItemSize().cy + GetItemSpace().cy));
						int nEndRow = min(GetItemCount() - 1, (rcNew.bottom - (GetHeaderSize().cy + m_rcMargin.top) + nVScrollPos) / 
							(GetItemSize().cy + GetItemSpace().cy));

						TRACE(_T("Select from row %d to %d\n"), nStartRow, nEndRow);
						for (int i = nStartRow; i <= nEndRow; i++)
						{
							CRect rcItem;
							GetItemRect(i, &rcItem);

							CRect rcTemp;
							if (rcTemp.IntersectRect(rcNew, rcItem))
								selNew.Add(i);
						}
					}
					else
					{
						int nStartRow = max(0, (rcNew.top - m_rcMargin.top + nVScrollPos) / 
							(GetItemSize().cy + GetItemSpace().cy));
						int nEndRow = min(GetItemCount() - 1, (rcNew.bottom - m_rcMargin.top + nVScrollPos) / 
							(GetItemSize().cy + GetItemSpace().cy));
						int nColCount = GetRowCol().cx;

						TRACE(_T("Select from row %d to %d\n"), nStartRow, nEndRow);
						for (int i = nStartRow; i <= nEndRow; i++)
						{
							for (int j = 0; j < nColCount; j++)
							{
								int index = i * nColCount + j;
								if (index < GetItemCount())
								{
									CRect rcItem;
									GetItemRect(index, &rcItem);

									CRect rcTemp;
									if (rcTemp.IntersectRect(rcNew, rcItem))
										selNew.Add(index);
								}
								else
								{
									break;
								}
							}
						}
					}

					if (IsControlDown() || IsShiftDown())	// compute union
					{
						CULSelection selTemp;
						selTemp.Set(selOld);

						if (!selNew.IsEmpty())
						{
							for (int i = 0; i < (int)selNew.GetCount(); i++)
							{
								int index = selNew.GetAt(i);
								if (selTemp.Find(index) >= 0)
								{
									if (IsControlDown())
										selTemp.Remove(index);
								}
								else
								{
									selTemp.Add(index);
								}
							}
						}

						selNew.Set(selTemp);
					}

					if (!m_sel.IsEqual(selNew))
					{
						TRACE(_T("Recreate the selection.\n"));
						m_sel.Set(selNew);

						Invalidate(FALSE);
						GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
					}

					selNew.Clear();

					if (!rcOld.IsRectEmpty() || !rcNew.IsRectEmpty())
						InvalidateSelectionRect(rcOld, rcNew);

					if (bAutoScrolled)
						UpdateWindow();
	
					rcOld = rcNew;
				}

				continue;
			}

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_SYSKEYDOWN:
				continue;

			case WM_LBUTTONUP:
				ReleaseCapture();
				break;

			case WM_KEYDOWN:
			{
				if (msg.wParam == VK_ESCAPE)
				{
					ReleaseCapture();
					break;
				}
				else
				{
					continue;
				}
			}

			default:
				break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
			break;
	}

	EnableAutoScroll(FALSE);

	m_bSelecting = FALSE;

	if (!rcOld.IsRectEmpty())
		InvalidateSelectionRect(rcOld, CRect(0, 0, 0, 0));

	ReleaseDC(pDC);

	if (!bMoved && !IsControlDown() && !IsShiftDown())
		UnselectAllItems();
}

void CUltraListCtrl::InvalidateSelectionRect(const CRect &rcOld, const CRect &rcNew)
{
	InvalidateRect(rcOld);
	InvalidateRect(rcNew);
	m_rcSelection = rcNew;
}

void CUltraListCtrl::DrawSelectionRect(CDC *pDC)
{
	if (!m_rcSelection.IsRectEmpty())
	{
		CRect rc;
		GetWorkSpace(&rc);

		CRect rcIntersect;
		rcIntersect.IntersectRect(&rc, &m_rcSelection);
		if (!rcIntersect.IsRectEmpty())
		{
			COLORREF clrSelction = GetSelectionRectColor();
			CBrush br(clrSelction);

			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			CBitmap bmp;
			bmp.CreateCompatibleBitmap(pDC, rcIntersect.Width(), rcIntersect.Height());
			CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);
			
			CRect rcMem = rcIntersect;
			rcMem.OffsetRect(-rcIntersect.left, -rcIntersect.top);
			dcMem.FillRect(&rcMem, &br);

			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.AlphaFormat = 0;
			bf.SourceConstantAlpha = 112;

			AlphaBlend(*pDC, rcIntersect.left, rcIntersect.top, rcIntersect.Width(), rcIntersect.Height(), 
				dcMem, 0, 0, rcMem.Width(), rcMem.Height(), bf);

			pDC->FrameRect(&m_rcSelection, &br);
		}
	}
}

COLORREF CUltraListCtrl::GetSelectionRectColor()
{
	return GetSysColor(COLOR_HIGHLIGHT);
}

int CUltraListCtrl::ItemFromPoint(CPoint point)
{
	CRect rc;
	GetWorkSpace(&rc);
	if (!rc.PtInRect(point))
		return -1;

	if (IsReportView())
	{
		int index = (point.y - (GetHeaderSize().cy + m_rcMargin.top) + GetScrollPos(SB_VERT)) / (GetItemSize().cy + GetItemSpace().cy);
		//TRACE(_T("ItemFromPoint() = %d\n"), index);
		if (index >= 0 && index < GetItemCount())
		{
			CRect rc;
			GetItemRect(index, &rc);
			if (rc.PtInRect(point))
				return index;
		}
	}
	else
	{
		int row = (point.y - m_rcMargin.top + GetScrollPos(SB_VERT)) / (GetItemSize().cy + GetItemSpace().cy);
		if (row >= 0)
		{
			int col = (point.x - m_rcMargin.left) / (GetItemSize().cx + GetItemSpace().cx);
			if (col >= 0 && col < GetRowCol().cx)
			{
				int index = row * GetRowCol().cx + col;
				//TRACE(_T("ItemFromPoint() = %d\n"), index);
				if (index >= 0 && index < GetItemCount())
				{
					CRect rc;
					GetItemRect(index, &rc);
					if (rc.PtInRect(point))
						return index;
				}
			}
		}
	}

	return -1;
}

// Note: return academic value, even the item does not exist.
void CUltraListCtrl::GetItemRect(int index, RECT *pRect)
{
	ASSERT(index >= 0);
	index = max(0, index);

	CRect rc;
	GetWorkSpace(&rc);

	int nHScrollPos = GetScrollPos(SB_HORZ);
	int nVScrollPos = GetScrollPos(SB_VERT);

	if (IsReportView())
	{
		pRect->left = rc.left + m_rcMargin.left - nHScrollPos;
		pRect->top = rc.top + m_rcMargin.top + (GetItemSize().cy + GetItemSpace().cy) * index - nVScrollPos;
		pRect->right = pRect->left + GetHeaderSize().cx;
		pRect->bottom = pRect->top + GetItemSize().cy;
	}
	else
	{
		int nCols = GetRowCol().cx;
		CPoint point = IndexToPos(index);

		pRect->left = rc.left + m_rcMargin.left + (GetItemSize().cx + GetItemSpace().cx) * point.x - nHScrollPos;
		pRect->top = rc.top + m_rcMargin.top + (GetItemSize().cy + GetItemSpace().cy) * point.y - nVScrollPos;
		pRect->right = pRect->left + GetItemSize().cx;
		pRect->bottom = pRect->top + GetItemSize().cy;
	}
}

void CUltraListCtrl::RedrawItem(int index)
{
	//TRACE(_T("RedrawItem(%d)\n"), index);

	CRect rc;
	GetItemRect(index, &rc);
	InvalidateRect(&rc, FALSE);
}

void CUltraListCtrl::GetSubItemRect(int nItem, int nSubItem, RECT *pRect)
{
	GetItemRect(nItem, pRect);

	CRect rcHeader;
	CHeaderCtrlEx *pHeader = GetHeaderCtrl();
	pHeader->GetItemRect(nSubItem, &rcHeader);
	pHeader->AdjustItemRect(rcHeader);

	pRect->left = rcHeader.left;
	pRect->right = rcHeader.right;
}

BOOL CUltraListCtrl::IsItemSelected(int index)
{
	return (m_sel.Find(index) >= 0);
}

void CUltraListCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

	DeleteAllItems();
}

int CUltraListCtrl::GetFirstSelectedItem()
{
	return m_sel.GetCount() > 0 ? m_sel.GetAt(0) : -1;
}

void CUltraListCtrl::SetItemData(int index, DWORD dwData)
{
	if (index >= 0 && index < (int)m_items.size())
		m_items[index]->m_dwData = dwData;
}

DWORD_PTR CUltraListCtrl::GetItemData(int index)
{
	if (index >= 0 && index < (int)m_items.size())
		return m_items[index]->m_dwData;
	return NULL;
}

void CUltraListCtrl::EnsureVisible(int index)
{
	ASSERT(index >= 0);

	if (GetStyle() & WS_VSCROLL)
	{
		index = max(0, index);

		CRect rc;
		GetWorkSpace(&rc);

		CRect rcItem;
		GetItemRect(index, &rcItem);

		int nDelta = 0;
		if (rcItem.top < rc.top)
		{
			nDelta = rcItem.top - rc.top;
		}
		else if (rcItem.top > rc.top)
		{
			if (rcItem.bottom > rc.bottom)
				nDelta = rcItem.bottom - rc.bottom;
		}
		else
		{
			// it must be the top item. do nothing.
		}

		if (nDelta != 0)
		{
			int nScrollPos = GetScrollPos(SB_VERT);
			nScrollPos += nDelta;
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nScrollPos), NULL);
		}
	}
}

CPoint CUltraListCtrl::IndexToPos(int index)
{
	CPoint point;
	int nCols = GetRowCol().cx;
	point.x = index % nCols;
	point.y = index / nCols;
	return point;
}

// Note: the return position may be invalid (out of range)
int CUltraListCtrl::PosToIndex(int nRow, int nCol)
{
	int nCols = GetRowCol().cx;
	int index = nRow * nCols + nCol;
	return index;
}

void CUltraListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (GetItemCount() == 0)
		return;

	int index = GetFocusItem();
	if (index < 0)
	{
		SetFocusItem(0);
		index = 0;
	}

	int nRows = GetRowCol().cy;
	int nCols = GetRowCol().cx;
	CPoint point = IndexToPos(index);

	switch (nChar)
	{
	case VK_LEFT:
		if (IsReportView())
		{
			if ((GetStyle() & WS_HSCROLL))
				SendMessage(WM_HSCROLL, SB_LINEUP, 0);
		}
		else
		{
			if (point.x > 0)
			{
				point.x--;
				index = PosToIndex(point.y, point.x);
				ASSERT(index >= 0);
				SetFocusItem(index);

				if (IsShiftDown())
				{
					if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
					{
						m_sel.SetRange(m_nAnchor, index);
						Invalidate(FALSE);
						GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
					}
				}
				else if (!IsControlDown())
				{
					UnselectAllItems();
					SetAnchorItem(index);
					SelectItem(index, TRUE);
				}

				EnsureVisible(index);
			}
		}
		break;

	case VK_RIGHT:
		if (IsReportView())
		{
			if (GetStyle() & WS_HSCROLL)
				SendMessage(WM_HSCROLL, SB_LINEDOWN, 0);
		}
		else
		{
			if (point.x < nCols - 1)
			{
				point.x++;
				index = PosToIndex(point.y, point.x);
				if (index >= 0 && index < GetItemCount())
				{
					SetFocusItem(index);

					if (IsShiftDown())
					{
						if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
						{
							m_sel.SetRange(m_nAnchor, index);
							Invalidate(FALSE);
							GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
						}
					}
					else if (!IsControlDown())
					{
						UnselectAllItems();
						SetAnchorItem(index);
						SelectItem(index, TRUE);
					}

					EnsureVisible(index);
				}
			}
		}
		break;

	case VK_UP:
		if (point.y > 0)
		{
			point.y--;
			index = PosToIndex(point.y, point.x);
			ASSERT(index >= 0);

			SetFocusItem(index);

			if (IsShiftDown())
			{
				if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
				{
					m_sel.SetRange(m_nAnchor, index);
					Invalidate(FALSE);
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
				}
			}
			else if (!IsControlDown())
			{
				UnselectAllItems();
				SetAnchorItem(index);
				SelectItem(index, TRUE);
			}

			EnsureVisible(index);
		}
		break;

	case VK_DOWN:
		if (point.y < nRows - 1)
		{
			point.y++;
			index = PosToIndex(point.y, point.x);
			if (index >= 0 && index < GetItemCount())
			{
				SetFocusItem(index);

				if (IsShiftDown())
				{
					if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
					{
						m_sel.SetRange(m_nAnchor, index);
						Invalidate(FALSE);
						GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
					}
				}
				else if (!IsControlDown())
				{
					UnselectAllItems();
					SetAnchorItem(index);
					SelectItem(index, TRUE);
				}

				EnsureVisible(index);
			}
		}
		break;

	case VK_PRIOR:
		if (GetStyle() & WS_VSCROLL)
			SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
		break;

	case VK_NEXT:
		if (GetStyle() & WS_VSCROLL)
			SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
		break;

	case VK_HOME:
		if (GetItemCount() > 0)
		{
			index = 0;
			SetFocusItem(index);

			if (IsShiftDown())
			{
				if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
				{
					m_sel.SetRange(m_nAnchor, index);
					Invalidate(FALSE);
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
				}
			}
			else if (!IsControlDown())
			{
				UnselectAllItems();
				SetAnchorItem(index);
				SelectItem(index, TRUE);
			}
			
			EnsureVisible(index);
		}
		break;

	case VK_END:
		if (GetItemCount() > 0)
		{
			index = GetItemCount() - 1;
			SetFocusItem(index);

			if (IsShiftDown())
			{
				if (m_nAnchor >= 0 && m_nAnchor < GetItemCount())
				{
					m_sel.SetRange(m_nAnchor, index);
					Invalidate(FALSE);
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
				}
			}
			else if (!IsControlDown())
			{
				UnselectAllItems();
				SetAnchorItem(index);
				SelectItem(index, TRUE);
			}

			EnsureVisible(index);
		}
		break;

	case VK_SPACE:
		index = GetFocusItem();
		if (index >= 0 && !IsItemSelected(index))
		{
			SelectItem(index, TRUE);
			SetAnchorItem(index);
			EnsureVisible(index);
			break;
		}
	
	default:
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

UINT CUltraListCtrl::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	// should allow TAB and ENTER/ESCAPE to work
	return DLGC_WANTARROWS;
	//return CWnd::OnGetDlgCode();
}

BOOL CUltraListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	UINT nCode = -1;
	DWORD dwStyle = GetStyle();
	if (dwStyle & WS_VSCROLL)
		nCode = SB_VERT;
	else if (dwStyle & WS_HSCROLL)
		nCode = SB_HORZ;

	if (nCode != -1)
	{
		UINT nWheelLines;
		if (::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nWheelLines, 0) == 0)
			nWheelLines = 3;

		if (nWheelLines == -1)
		{
			SendMessage(nCode == SB_VERT ? WM_VSCROLL : WM_HSCROLL, zDelta > 0 ? SB_PAGEUP : SB_PAGEDOWN, 0);
		}
		else
		{
			int nLinesToScroll = max(1, (int)round(abs((float)zDelta / WHEEL_DELTA * nWheelLines)));
			for (int i = 0; i < nLinesToScroll; i++)
				SendMessage(nCode == SB_VERT ? WM_VSCROLL : WM_HSCROLL, zDelta > 0 ? SB_LINEUP : SB_LINEDOWN, 0);
		}

		ScreenToClient(&pt);
		PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

		return TRUE;
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CUltraListCtrl::SetMargin(int l, int t, int r, int b)
{
	int index = 0;
	if (::IsWindow(m_hWnd))
		index = GetFirstVisibleItem();

	m_rcMargin.SetRect(l, t, r, b);

	if (::IsWindow(m_hWnd))
	{
		RecalcScrollPos();
		Invalidate(FALSE);

		SetFirstVisibleItem(index);
	}
}

void CUltraListCtrl::SetIconSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);

	int index = 0;
	if (::IsWindow(m_hWnd))
		index = GetFirstVisibleItem();

	m_sizeIcon.cx = max(0, cx);
	m_sizeIcon.cy = max(0, cy);
	if (::IsWindow(m_hWnd))
	{
		if (!IsReportView())
		{
			RecalcScrollPos();
			Invalidate(FALSE);

			SetFirstVisibleItem(index);
		}
	}
}

void CUltraListCtrl::SetIconSpace(int cx, int cy)
{
	int index = 0;
	if (::IsWindow(m_hWnd))
		index = GetFirstVisibleItem();

	m_sizeIconSpace.cx = cx;
	m_sizeIconSpace.cy = cy;
	if (::IsWindow(m_hWnd))
	{
		if (!IsReportView())
		{
			RecalcScrollPos();
			Invalidate(FALSE);

			SetFirstVisibleItem(index);
		}
	}
}

void CUltraListCtrl::SetItemHeight(int nHeight)
{
	int index = 0;
	if (::IsWindow(m_hWnd))
		index = GetFirstVisibleItem();

	m_nItemHeight = nHeight;
	if (::IsWindow(m_hWnd))
	{
		if (IsReportView())
		{
			RecalcScrollPos();
			Invalidate(FALSE);

			SetFirstVisibleItem(index);
		}
	}
}

void CUltraListCtrl::SetItemSpace(int nSpace)
{
	int index = 0;
	if (::IsWindow(m_hWnd))
		index = GetFirstVisibleItem();

	m_nItemSpace = nSpace;
	if (::IsWindow(m_hWnd))
	{
		if (IsReportView())
		{
			RecalcScrollPos();
			Invalidate(FALSE);

			SetFirstVisibleItem(index);
		}
	}
}

BOOL CUltraListCtrl::GetToolTipText(int index, CString &strText)
{
	return FALSE;
}

BOOL CUltraListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreTranslateMessage(pMsg);
}

void CUltraListCtrl::UpdateToolTip(CPoint point)
{
	if (!::IsWindow(m_tooltip.m_hWnd))
		return;

	int index = ItemFromPoint(point);

	//TRACE(_T("UpdateToolTip(): index = %d\n"), index);

	m_tooltip.Pop();
	m_tooltip.DelTool(this, 1);

	if (index >= 0)
	{
		CString strText;
		if (GetToolTipText(index, strText))
		{
			TOOLINFO ti;
			memset(&ti, 0, sizeof(ti));
			ti.cbSize = sizeof(ti);
			ti.hinst = 0;
			ti.hwnd = m_hWnd;
			ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;
			ti.lpszText = (LPTSTR)(LPCTSTR)strText;
			ti.uId = 1;

			CRect rcItem;
			GetItemRect(index, &rcItem);
			ti.rect = rcItem;

			m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
			m_tooltip.Activate(TRUE);
		}
	}
}

void CUltraListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);

	int index = ItemFromPoint(point);
	if (index != m_nHover)
	{
		if (m_nHover >= 0 && m_nHover < GetItemCount())
			RedrawItem(m_nHover);

		if (index >= 0 && index < GetItemCount())
			RedrawItem(index);

		m_nHover = index;

		if (m_nHover >= 0)
		{
			SetTimer(IDT_HOVER, 500, NULL);
		}
		else
		{
			KillTimer(IDT_HOVER);
		}

		UpdateToolTip(point);
	}
}

void CUltraListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();

	int index = ItemFromPoint(point);
	if (index >= 0)
	{
		if (!IsItemSelected(index))
		{
			UnselectAllItems();
			SelectItem(index, TRUE);
		}
		SetFocusItem(index);
		DelayEnsureVisible(index);
	}
	else
	{
		UnselectAllItems();
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CUltraListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetFocus();

	CWnd::OnMButtonDown(nFlags, point);
}

void CUltraListCtrl::DelayEnsureVisible(int index)
{
	#pragma message (__FILELINE__ "Should check if the item is already visible.")

	m_nLastSelected = index;
	SetTimer(IDT_ENSUREVISIBLE, GetDoubleClickTime() + 1, NULL);
}

void CUltraListCtrl::CancelDelayEnsureVisible()
{
	KillTimer(IDT_ENSUREVISIBLE);
}

void CUltraListCtrl::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case IDT_ENSUREVISIBLE:
		{
			KillTimer(IDT_ENSUREVISIBLE);
			if (m_nLastSelected >= 0)
				EnsureVisible(m_nLastSelected);
			break;
		}

		case IDT_AUTOSCROLL:
		{
			CPoint point;
			GetCursorPos(&point);
			ScreenToClient(&point);

			if (m_bAutoScroll)
				PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y));
			else
				KillTimer(IDT_AUTOSCROLL);

			break;
		}

		case IDT_HOVER:
		{
			CRect rc;
			GetWorkSpace(&rc);

			CPoint point;
			GetCursorPos(&point);
			BOOL bOnWindow = (WindowFromPoint(point) == this);

			ScreenToClient(&point);

			if (!rc.PtInRect(point) || !bOnWindow)
			{
				KillTimer(IDT_HOVER);
				if (m_nHover >= 0)
				{
					int nHover = m_nHover;
					m_nHover = -1;
					RedrawItem(nHover);
				}
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void CUltraListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CancelDelayEnsureVisible();

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_DBLCLK), (LPARAM)m_hWnd);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CUltraListCtrl::OnNcPaint()
{
	Default();
	g_xpStyle.DrawWindowEdge(m_hWnd);
}

int CUltraListCtrl::GetFirstVisibleItem()
{
	int nFirstItem;

	int nRow = GetFirstVisibleRow();
	if (IsReportView())
		nFirstItem = nRow;
	else
		nFirstItem = PosToIndex(nRow, 0);

	TRACE(_T("GetFirstVisibleItem() = %d\n"), nFirstItem);
	return nFirstItem;
}

void CUltraListCtrl::SetFirstVisibleItem(int index)
{
	TRACE(_T("SetFirstVisibleItem(%d)\n"), index);

	if (index >= 0 && index < GetItemCount())
	{
		if (GetStyle() & WS_VSCROLL)
		{
			CRect rc;
			GetWorkSpace(&rc);

			CRect rcItem;
			GetItemRect(index, &rcItem);

			int nDelta = rcItem.top - rc.top;
			if (nDelta != 0)
			{
				int nPos = GetScrollPos(SB_VERT) + nDelta;
				SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos), NULL);
				TRACE(_T("GetFirstVisibleItem() = %d\n"), GetFirstVisibleItem());
			}
		}
	}
}

void CUltraListCtrl::ModifyULStyle(DWORD dwRemove, DWORD dwAdd)
{
	ASSERT(::IsWindow(m_hWnd) && ::IsWindow(m_wndHeader.m_hWnd));

	BOOL bReportOld = (m_dwStyle & ULS_REPORT) != 0;
	BOOL bReportNew = (((m_dwStyle & ~dwRemove) | dwAdd) & ULS_REPORT) != 0;

	int nFirstItem = GetFirstVisibleItem();

	m_dwStyle &= ~dwRemove;
	m_dwStyle |= dwAdd;

	if ((dwAdd & ULS_HEADERDRAGDROP) == ULS_HEADERDRAGDROP)
		m_wndHeader.ModifyStyle(0, HDS_DRAGDROP);

	if ((m_dwStyle & ULS_CHECKBOX) == ULS_CHECKBOX)
		m_wndHeader.SetCheckBoxItem(0, TRUE);
	else
		m_wndHeader.SetCheckBoxItem(0, FALSE);

	RecalcHeaderPos();
	RecalcScrollPos();

	if (bReportOld != bReportNew)
		SetFirstVisibleItem(nFirstItem);

	Invalidate(FALSE);
}

void CUltraListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	Invalidate(FALSE);
}

void CUltraListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	Invalidate(FALSE);
}

int CUltraListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth)
{
	ASSERT(::IsWindow(m_wndHeader.m_hWnd));
	if (!::IsWindow(m_wndHeader.m_hWnd))
		return -1;

	CHeaderCtrlEx::CItemData *pData = new CHeaderCtrlEx::CItemData(nWidth, TRUE, TRUE);

	HDITEM hdi;
	hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH | HDI_LPARAM;
	hdi.pszText = (LPTSTR)lpszColumnHeading;
	hdi.fmt = nFormat;
	hdi.cxy = nWidth;
	hdi.lParam = (LPARAM)pData;
	int index = m_wndHeader.InsertItem(nCol, &hdi);
	if (index >= 0)
	{
		// add sub item text for existing items
		int count = GetItemCount();
		for (int i = 0; i < count; i++)
		{
			CListItem *pItem = m_items[i];
			pItem->m_list.insert(pItem->m_list.begin() + index, NULL);
		}

		RecalcHeaderPos();
		RecalcScrollPos();
	}
	return index;
}

void CUltraListCtrl::SetRegistryKey(LPCTSTR lpszKey)
{
	m_strKey = lpszKey;
}

BOOL CUltraListCtrl::SaveState()
{
	ASSERT(!m_strKey.IsEmpty());
	if (m_strKey.IsEmpty())
		return FALSE;

	int count = m_wndHeader.GetItemCount();
	AfxGetApp()->WriteProfileInt(m_strKey, _T("Column Count"), count);

	int *nColOrder = new int[count];
	m_wndHeader.GetOrderArray(nColOrder, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, _T("Column Order"), (BYTE *)nColOrder, sizeof(int) * count);
	delete[] nColOrder;

	int *nColWidth = new int[count];
	m_wndHeader.GetWidthArray(nColWidth, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, _T("Column Width"), (BYTE *)nColWidth, sizeof(int) * count);
	delete[] nColWidth;

	int *nColVisible = new int[count];
	m_wndHeader.GetVisibleArray(nColVisible, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, _T("Column Visible"), (BYTE *)nColVisible, sizeof(int) * count);
	delete[] nColVisible;

	return TRUE;
}

BOOL CUltraListCtrl::RestoreState()
{
	ASSERT(!m_strKey.IsEmpty());
	if (m_strKey.IsEmpty())
		return FALSE;

	int count = AfxGetApp()->GetProfileInt(m_strKey, _T("Column Count"), 0);
	if (count != m_wndHeader.GetItemCount())
		return FALSE;

	UINT bytes;

	int *nColOrder = NULL;
	BOOL ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, _T("Column Order"), (BYTE **)&nColOrder, &bytes) && bytes == sizeof(int) * count))
	{
		ret = VerifyOrderArray(nColOrder, count) && m_wndHeader.SetOrderArray(count, nColOrder);
		ASSERT(ret);
	}
	delete[] nColOrder;

	if (!ret)
		return FALSE;

	int *nColWidth = NULL;
	ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, _T("Column Width"), (BYTE **)&nColWidth, &bytes) && bytes == sizeof(int) * count))
		ret = m_wndHeader.SetWidthArray(count, nColWidth);
	delete[] nColWidth;
	if (!ret)
		return FALSE;

	int *nColVisible = NULL;
	ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, _T("Column Visible"), (BYTE **)&nColVisible, &bytes) && bytes == sizeof(int) * count))
		ret = m_wndHeader.SetVisibleArray(count, nColVisible);
	delete[] nColVisible;

	return ret;
}

BOOL CUltraListCtrl::VerifyOrderArray(int *piArray, int count)
{
	for (int i = 0; i < count; i++)
	{
		if (!(piArray[i] >= 0 && piArray[i] <= count - 1))
			return FALSE;

		// compare with items after current one
		for (int j = i + 1; j < count; j++)
			if (piArray[i] == piArray[j])
				return FALSE;
	}
	return TRUE;
}

BOOL CUltraListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR pszText)
{
	if (nItem >= 0 && nItem	< GetItemCount() && nSubItem >= 0 && nSubItem < GetHeaderCtrl()->GetItemCount())
	{
		TCHAR *pszTextNew = (pszText == NULL ? NULL : new TCHAR[_tcslen(pszText) + 1]);

		CListItem *pItem = m_items[nItem];
		if (pItem->m_list[nSubItem] != NULL)
		{
			delete pItem->m_list[nSubItem];
			pItem->m_list[nSubItem] = NULL;
		}

		if (pszTextNew != NULL)
			_tcscpy(pszTextNew, pszText);

		pItem->m_list[nSubItem] = pszTextNew;
		
		RedrawItem(nItem);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CString CUltraListCtrl::GetItemText(int nItem, int nSubItem)
{
	CString strText;

	if (nItem >= 0 && nItem	< GetItemCount() && nSubItem >= 0 && nSubItem < GetHeaderCtrl()->GetItemCount())
	{
		CListItem *pItem = m_items[nItem];
		if (pItem->m_list[nSubItem] != NULL)
			strText = pItem->m_list[nSubItem];
	}

	return strText;
}

int CUltraListCtrl::GetSubItemIdealWith(int nItem)
{
	CClientDC dc(this);

	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = &afxGlobalData.fontRegular;

	CFont *pOldFont = dc.SelectObject(pFont);

	int nMaxWidth = 0;
	CString str;

	int count = GetItemCount();
	for (int i = 0; i < count; i++)
	{
		str = GetItemText(i, nItem);
		int w = dc.GetTextExtent(str).cx;
		nMaxWidth = max(nMaxWidth, w + 12);
	}

	dc.SelectObject(pOldFont);

	return nMaxWidth;
}

void CUltraListCtrl::OnHdnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: Add your control notification handler code here

	if (phdr->pitem->mask & (HDI_WIDTH | HDI_FORMAT))
	{
		if (GetItemCount() > 0)
		{
			RecalcHeaderPos();
			RecalcScrollPos();
			Invalidate(FALSE);
		}
	}

	*pResult = 0;
}

void CUltraListCtrl::OnHdnCheck(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: Add your control notification handler code here

	BOOL bCheck = m_wndHeader.GetCheck();

	for (int i = 0; i < m_items.size(); i++)
	{
		if (!CanCheckItem(i, bCheck))
		{
			MessageBeep(0);
			continue;
		}

		SetCheck(i, bCheck);
		OnCheckItem(i, bCheck);
	}

	*pResult = 0;
}

void CUltraListCtrl::OnHdnEndDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: Add your control notification handler code here

	Invalidate(FALSE);

	*pResult = 0;
}

void CUltraListCtrl::OnNmRclickHeader(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: Add your control notification handler code here

	*pResult = 0;

#ifdef _DEBUG
	int count = m_wndHeader.GetItemCount();
	int *piArray = new int[count];
	m_wndHeader.GetOrderArray(piArray, count);
	for (int i = 0; i < count; i++)
	{
		TRACE(_T("%d"), piArray[i]);
		if (!m_wndHeader.GetVisible(piArray[i]))
			TRACE(_T("*"));
		TRACE(_T(" "));
	}
	TRACE(_T("\n"));
	delete[] piArray;
#endif

	*pResult = 0;

	CPoint point;
	GetCursorPos(&point);

	CMenu menu;
	if (menu.CreatePopupMenu())
	{
		const int TEXT_LEN = 64;
		const TCHAR TEXT_TAIL[] = _T("...");

		TCHAR szText[TEXT_LEN + sizeof(TEXT_TAIL) / sizeof(TCHAR) + 1];
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = szText;
		hdi.cchTextMax = TEXT_LEN;
		int count = m_wndHeader.GetItemCount();
		for (int i = 0; i < count; i++)
		{
			if ((m_dwStyle & ULS_CHECKBOX) && i == 0)
			{
				::LoadString(AfxGetResourceHandle(), IDS_COL_CHECKBOX, szText, TEXT_LEN + sizeof(TEXT_TAIL) / sizeof(TCHAR) + 1);
			}
			else
			{
				if (!m_wndHeader.GetItem(i, &hdi))
					return;
				if (hdi.cchTextMax == TEXT_LEN - 1)
					_tcscat(szText, TEXT_TAIL);
			}

			UINT nFlags = MF_STRING;
			if (!m_wndHeader.GetRemovable(i))
				nFlags |= MF_GRAYED | MF_CHECKED;			
			if (m_wndHeader.GetVisible(i))
				nFlags |= MF_CHECKED;
			if (!menu.AppendMenu(nFlags, i + 1, szText))
				return;
		}

		if (LoadString(AfxGetResourceHandle(), IDS_RESET_COLUMNS, szText, 54))
		{
			menu.AppendMenu(MF_SEPARATOR, 0, (LPCTSTR)NULL);
			menu.AppendMenu(MF_STRING, count + 1, szText);
		}


		if (point.x == -1 && point.y == -1)
		{
			point.x = 0;
			point.y = 0;
			ClientToScreen(&point);
		}
		//int index = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
		int index = ((CWinAppEx *)AfxGetApp())->GetContextMenuManager()->TrackPopupMenu(menu.GetSafeHmenu(), point.x, point.y, this, FALSE);
		if (index > 0)
		{
			if (index <= count)
			{
				index--;
				BOOL bVisible = m_wndHeader.GetVisible(index);
				m_wndHeader.SetVisible(index, !bVisible);
			}
			else
			{
				OnResetColumns();
			}
		}
	}
}

void CUltraListCtrl::OnHdnDividerDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: Add your control notification handler code here

	*pResult = 1;

	int nItem = m_wndHeader.FindVisibleItem(phdr->iItem);
	if (nItem >= 0)
	{
		int count = GetItemCount();
		if (count > 0)
		{
			int nIdealWidth = GetSubItemIdealWith(nItem);
			GetHeaderCtrl()->SetItemWidth(nItem, nIdealWidth);
		}
	}
}

void CUltraListCtrl::OnResetColumns()
{
	int count = m_wndHeader.GetItemCount();
	int *nCols = new int[count];

	for (int i = 0; i < count; i++)
		nCols[i] = i;
	m_wndHeader.SetOrderArray(count, nCols);

	for (int i = 0; i < count; i++)
		nCols[i] = 1;
	m_wndHeader.SetVisibleArray(count, nCols);

	for (int i = 0; i < count; i++)
	{
		if (m_wndHeader.GetVisible(i))
			m_wndHeader.ResetItemWidth(i);
	}

	delete[] nCols;
}

void CUltraListCtrl::EnableAutoScroll(BOOL bEnable)
{
	m_bAutoScroll = bEnable;

	if (m_bAutoScroll)
	{
		TRACE(_T("EnableAutoScroll(): SetTimer()\n"));
		SetTimer(IDT_AUTOSCROLL, 10, NULL);
	}
	else
	{
		KillTimer(IDT_AUTOSCROLL);
	}
}

BOOL CUltraListCtrl::GetCheck(int index)
{
	if(m_dwStyle & ULS_CHECKBOX); //if no check style ,is look as all checked.
	else return TRUE;

	if (index >= 0 && index < GetItemCount())
	{
		CListItem *pItem = m_items[index];
		return pItem->m_bChecked;
	}

	return FALSE;
}

void CUltraListCtrl::SetCheck(int index, BOOL bCheck)
{
	if(m_dwStyle & ULS_CHECKBOX);
	else return ;

	if (index >= 0 && index < GetItemCount())
	{
		CListItem *pItem = m_items[index];
		if (pItem->m_bChecked != bCheck)
		{
			pItem->m_bChecked = bCheck;
			RedrawItem(index);

			BOOL bCheckHeader = (GetItemCount() > 0);
			if (bCheck)
			{
				for (int i = 0;i < m_items.size(); i++)
				{
					CListItem *pItem = m_items[i];
					if (!pItem->m_bChecked)
					{
						bCheckHeader = FALSE;
						break;
					}
				}
			}
			else
			{
				bCheckHeader = FALSE;
			}

			if (bCheckHeader != m_wndHeader.GetCheck())
				m_wndHeader.SetCheck(bCheckHeader);
		}
	}
}

void CUltraListCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);

	// TODO: Add your message handler code here

	// Reset the font
	RecalcHeaderPos(TRUE);
}

void CUltraListCtrl::MoveSelection(int nNew)
{
	ITEM_LIST items;
	int nNewAdjusted = nNew;
	int count = m_sel.GetCount();
	for (int i = count - 1; i >= 0; i--)
	{
		int index = m_sel.GetAt(i);
		CListItem *pItem = m_items[index];
		items.insert(items.begin(), pItem);
		m_items.erase(m_items.begin() + index);

		if (m_sel.GetAt(i) < nNew)
			nNewAdjusted--;
	}

	for (int i = count - 1; i >= 0; i--)
	{
		CListItem *pItem = items[i];
		m_items.insert(m_items.begin() + nNewAdjusted, pItem);
	}

	m_sel.Clear();
	for (int i = 0; i < count; i++)
		m_sel.Add(nNewAdjusted + i);

	Invalidate(FALSE);
	
	EnsureVisible(m_sel.GetAt(0));

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	UpdateToolTip(point);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), ULN_SELCHANGED), (LPARAM)m_hWnd);
}

// CustomSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include ".\CustomSliderCtrl.h"


// CCustomSliderCtrl

IMPLEMENT_DYNAMIC(CCustomSliderCtrl, CStatic)
CCustomSliderCtrl::CCustomSliderCtrl()
{
	m_bEnableSelection = FALSE;

	m_nMin = 0;
	m_nMax = 100;
	m_nPos = 0;

	m_nSelStart = 0;
	m_nSelEnd = 100;
	m_nMinSelRange = 0;

	m_nFocusedHandle = 0;
	m_nLineSize = 1;
	m_nPageSize = 10;
}

CCustomSliderCtrl::~CCustomSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomSliderCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CCustomSliderCtrl message handlers

void CCustomSliderCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_NOTIFY);
}

void CCustomSliderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pBmpOld = dcMem.SelectObject(&bmp);

	DrawChannel(&dcMem);
	DrawThumb(&dcMem);

	if (m_bEnableSelection)
		DrawSelectionMarker(&dcMem);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBmpOld);
}

void CCustomSliderCtrl::GetRange(int &nMin, int &nMax) const
{
	nMin = m_nMin;
	nMax = m_nMax;
}

BOOL CCustomSliderCtrl::SetRange(int nMin, int nMax)
{
	ASSERT(nMin <= nMax);
	if (nMin > nMax)
		return FALSE;

	m_nMin = nMin;
	m_nMax = nMax;
	
	if (m_bEnableSelection)
	{
		m_nSelStart = max(m_nMin, m_nSelStart);
		m_nSelEnd = min(m_nMax, m_nSelEnd);
	}
	else
	{
		m_nSelStart = nMin;
		m_nSelEnd = nMax;
	}

	if (m_nPos < m_nMin || m_nPos > m_nMax)
	{
		m_nPos = min(m_nMax, max(m_nMin, m_nPos));
		if (::IsWindow(m_hWnd))
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_POSCHANGED), (LPARAM)m_hWnd);
	}

	if (::IsWindow(m_hWnd))
	{
		Invalidate(FALSE);
		UpdateWindow();
	}

	return TRUE;
}

void CCustomSliderCtrl::GetSelection(int &nStart, int &nEnd) const
{
	nStart = m_nSelStart;
	nEnd = m_nSelEnd;
}

BOOL CCustomSliderCtrl::SetSelection(int nStart, int nEnd, BOOL bNofify)
{
	ASSERT(nStart <= nEnd);
	if (nStart > nEnd)
		return FALSE;

	m_nSelStart = max(m_nMin, nStart);
	m_nSelEnd = min(m_nMax, nEnd);

	if (bNofify && ::IsWindow(m_hWnd))
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_SELCHANGED), (LPARAM)m_hWnd);

	if (m_nPos < m_nSelStart || m_nPos > m_nSelEnd)
	{
		m_nPos = min(m_nSelEnd, max(m_nSelStart, m_nPos));
		if (::IsWindow(m_hWnd))
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_POSCHANGED), (LPARAM)m_hWnd);
	}

	if (::IsWindow(m_hWnd))
	{
		Invalidate(FALSE);
		UpdateWindow();
	}

	return TRUE;
}

void CCustomSliderCtrl::EnableSelection(BOOL bEnable)
{
	if (m_bEnableSelection != bEnable)
	{
		m_bEnableSelection = bEnable;
		Invalidate();
	}
}

void CCustomSliderCtrl::SetMinSelRange(int nRange)
{
	ASSERT(nRange <= m_nMax - m_nMin);
	m_nMinSelRange = min(nRange, m_nMax - m_nMin);

	// move selection end point
	if (m_nSelEnd - m_nSelStart < m_nMinSelRange)
	{
		m_nSelEnd = min(m_nMax, m_nSelStart + m_nMinSelRange);
		// move selection start point
		if (m_nSelEnd - m_nSelStart < m_nMinSelRange)
		{
			m_nSelStart = max(m_nMin, m_nSelEnd - m_nMinSelRange);
		}
	}

	// adjust current pos
	if (m_nPos < m_nSelStart || m_nPos > m_nSelEnd)
	{
		m_nPos = min(m_nSelEnd, max(m_nSelStart, m_nPos));
		if (::IsWindow(m_hWnd))
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_POSCHANGED), (LPARAM)m_hWnd);
	}

	if (::IsWindow(m_hWnd))
	{
		Invalidate(FALSE);
		UpdateWindow();
	}
}

int CCustomSliderCtrl::GetPos() const
{
	return m_nPos;
}

void CCustomSliderCtrl::SetPos(int nPos, BOOL bNofify)
{
	if (m_bEnableSelection)
		m_nPos = min(m_nSelEnd, max(m_nSelStart, nPos));
	else
		m_nPos = min(m_nMax, max(m_nMin, nPos));

	if (::IsWindow(m_hWnd))
	{
		Invalidate(FALSE);
		UpdateWindow();

		if (bNofify)
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_POSCHANGED), (LPARAM)m_hWnd);
	}
}

void CCustomSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CStatic::OnLButtonDown(nFlags, point);

	SetFocus();

	CRect rc;

	GetThumbRect(&rc);
	if (rc.PtInRect(point))
	{
		SetFocusedHandle(0);
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_BEGINTRACKING), (LPARAM)m_hWnd);
		Track(point, 0);
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_ENDTRACKING), (LPARAM)m_hWnd);
		return;
	}

	if (m_bEnableSelection)
	{
		CRect rcStart, rcEnd;
		GetSelectionMarkerRect(&rcStart, &rcEnd);
		if (rcStart.PtInRect(point))
		{
			SetFocusedHandle(1);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_SELBEGINTRACKING), (LPARAM)m_hWnd);
			Track(point, 1);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_SELENDTRACKING), (LPARAM)m_hWnd);
			return;
		}
		if (rcEnd.PtInRect(point))
		{
			SetFocusedHandle(2);			
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_SELBEGINTRACKING), (LPARAM)m_hWnd);
			Track(point, 2);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), SC_SELENDTRACKING), (LPARAM)m_hWnd);
			return;
		}
	}

	GetChannelRect(&rc);
	if (rc.PtInRect(point))
	{
		SetFocusedHandle(0);
		int nPos = PointToPos(point);
		SetPos(nPos, TRUE);
		return;
	}
}

void CCustomSliderCtrl::Track(CPoint point, int nHandle)
{
	SetCapture();

	CRect rc;
	GetChannelRect(&rc);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
			case WM_MOUSEMOVE:
			{
				int x = (int)(short)LOWORD(msg.lParam);
				int y = (int)(short)LOWORD(msg.wParam);
				int nPos = PointToPos(CPoint(x, y));
				//TRACE("%d, %d\n", x, nPos);

				if (nHandle == 0)
				{
					if (nPos != m_nPos)
					{
						SetPos(max(m_nSelStart, min(m_nSelEnd, nPos)), TRUE);
					}
				}
				else if (nHandle == 1)
				{
					if (m_nSelEnd - nPos < m_nMinSelRange)
						nPos = m_nSelEnd - m_nMinSelRange;

					if (nPos != m_nSelStart)
					{
						SetSelection(min(nPos, m_nSelEnd), m_nSelEnd, TRUE);
						SetPos(nPos, TRUE);							
					}
				}
				else
				{
					if (nPos - m_nSelStart < m_nMinSelRange)
						nPos = m_nSelStart + m_nMinSelRange;

					if (nPos != m_nSelEnd)
					{
						SetSelection(m_nSelStart, max(nPos, m_nSelStart), TRUE);
						SetPos(nPos, TRUE);
					}
				}

				continue;
			}

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				continue;

			case WM_LBUTTONUP:
				ReleaseCapture();
				return;

			default:
				break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (GetCapture() != this)
			break;
	}
}

void CCustomSliderCtrl::SetFocusedHandle(int nFocusedHandle)
{
	if (m_nFocusedHandle != nFocusedHandle)
	{
		m_nFocusedHandle = nFocusedHandle;
		Invalidate();
	}
}

void CCustomSliderCtrl::OnSetFocus(CWnd *pOldWnd)
{
	CStatic::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here

	Invalidate(FALSE);
}

void CCustomSliderCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CStatic::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	Invalidate(FALSE);
}

UINT CCustomSliderCtrl::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default

	return DLGC_WANTARROWS;

	//return CStatic::OnGetDlgCode();
}

void CCustomSliderCtrl::SetLineSize(int nLine)
{
	m_nLineSize = nLine;
}

void CCustomSliderCtrl::SetPageSize(int nPage)
{
	m_nPageSize = nPage;
}

void CCustomSliderCtrl::MoveHandle(int nHandle, int nSize)
{
	if (m_nFocusedHandle == 0)
	{
		int nPos = m_nPos + nSize;
		int nStart = (m_bEnableSelection ? m_nSelStart : m_nMin);
		int nEnd = (m_bEnableSelection ? m_nSelEnd : m_nMax);
		nPos = max(nStart, min(nEnd, nPos));
		if (nPos != m_nPos)
			SetPos(nPos, TRUE);
		return;
	}

	if (m_nFocusedHandle == 1)
	{
		int nPos = m_nSelStart + nSize;
		int nStart = m_nMin;
		int nEnd = m_nSelEnd - m_nMinSelRange;
		nPos = max(nStart, min(nEnd, nPos));
		if (nPos != m_nSelStart)
		{
			SetSelection(nPos, m_nSelEnd, TRUE);
			SetPos(nPos, TRUE);
		}
		return;
	}

	if (m_nFocusedHandle == 2)
	{
		int nPos = m_nSelEnd + nSize;
		int nStart = m_nSelStart + m_nMinSelRange;
		int nEnd = m_nMax;
		nPos = max(nStart, min(nEnd, nPos));
		if (nPos != m_nSelEnd)
		{
			SetSelection(m_nSelStart, nPos, TRUE);
			SetPos(nPos, TRUE);
		}
		return;
	}
}

void CCustomSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);

	switch (nChar)
	{
		case VK_LEFT:
			MoveHandle(m_nFocusedHandle, -m_nLineSize);		
			break;
		case VK_RIGHT:
			MoveHandle(m_nFocusedHandle, m_nLineSize);
			break;
		case VK_PRIOR:
			MoveHandle(m_nFocusedHandle, -m_nPageSize);
			break;
		case VK_NEXT:
			MoveHandle(m_nFocusedHandle, m_nPageSize);
			break;
		case VK_UP:
		{
			int nFocusedHandle = m_nFocusedHandle - 1;
			if (nFocusedHandle < 0)
				nFocusedHandle = 2;
			SetFocusedHandle(nFocusedHandle);
			break;
		}
		case VK_DOWN:
		{
			int nFocusedHandle = m_nFocusedHandle + 1;
			if (nFocusedHandle > 2)
				nFocusedHandle = 0;
			SetFocusedHandle(nFocusedHandle);
			break;
		}
		default:
			break;
	}
}

void CCustomSliderCtrl::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);
	
	// TODO: Add your message handler code here
	
	Invalidate(TRUE);
}

//*************************************************************************
// HeaderCtrlEx.cpp : header file
// Version : 1.0
// Date : June 2005
// Author : Chen Hao
// Email :  ch@sothink.com
// Website : http://www.sothink.com
// 
// You are free to use/modify this code but leave this header intact. 
// This class is public domain so you are free to use it any of 
// your applications (Freeware,Shareware,Commercial). All I ask is 
// that you let me know that some of my code is in your app.
//*************************************************************************

#include "stdafx.h"
#include "Resource.h"
#include ".\HeaderCtrlEx.h"
#include "MSMVisualManager.h"
#include <atlbase.h>

// CHeaderCtrlEx

CHeaderCtrlEx::CHeaderCtrlEx()
{
	m_nDraggingItem = -1;
	m_nItemWidth = 0;
	m_bDragFullWindow = FALSE;
	m_bAllowDragFullWindow = TRUE;

	m_nCheckBox = -1;
	m_bChecked = FALSE;
	m_bEnableCheckBox = TRUE;
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CHeaderCtrlEx::OnNMCustomdraw)
END_MESSAGE_MAP()



// CHeaderCtrlEx message handlers

BOOL CHeaderCtrlEx::SetItemWidth(int index, int nWidth)
{
	HDITEM hdi;
	memset(&hdi, 0, sizeof(hdi));
	hdi.mask = HDI_WIDTH;
	hdi.cxy = nWidth;
	return SetItem(index, &hdi);
}

int CHeaderCtrlEx::GetItemWidth(int index)
{
	HDITEM hdi;
	memset(&hdi, 0, sizeof(hdi));
	hdi.mask = HDI_WIDTH;
	if (GetItem(index, &hdi))
		return hdi.cxy;
	return 0;
}

BOOL CHeaderCtrlEx::GetWidthArray(int *piArray, int iCount)
{
	HDITEM hdi;
	memset(&hdi, 0, sizeof(hdi));
	hdi.mask = HDI_WIDTH;
	for (int i = 0; i < iCount; i++)
	{
		if (!GetItem(i, &hdi))
			return FALSE;
		piArray[i] = hdi.cxy;
	}

	return TRUE;
}

BOOL CHeaderCtrlEx::SetWidthArray(int iCount, int *piArray)
{
	HDITEM hdi;
	memset(&hdi, 0, sizeof(hdi));
	hdi.mask = HDI_WIDTH;
	for (int i = 0; i < iCount; i++)
	{
		hdi.cxy = piArray[i];
		if (!SetItem(i, &hdi))
			return FALSE;
	}

	return TRUE;
}

BOOL CHeaderCtrlEx::GetVisibleArray(int *piArray, int iCount)
{
	for (int i = 0; i < iCount; i++)
		piArray[i] = GetVisible(i);

	return TRUE;
}

BOOL CHeaderCtrlEx::SetVisibleArray(int iCount, int *piArray)
{
	for (int i = 0; i < iCount; i++)
		SetVisible(i, piArray[i]);
	return TRUE;
}

BOOL CHeaderCtrlEx::GetVisible(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	return pData->m_bVisible;
}

int CHeaderCtrlEx::GetVisibleItemCount()
{
	int count = GetItemCount();
	int visible = 0;
	for (int i = 0; i < count; i++)
		if (GetVisible(i))
			visible++;
	return visible;
}

void CHeaderCtrlEx::SetVisible(int index, BOOL bVisible)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	ASSERT(pData);
	if (pData->m_bVisible != bVisible)
	{
		pData->m_bVisible = bVisible;

		/*
		// get total items
		int count = GetItemCount();
		// get current item's order
		int nOrder = IndexToOrder(index);

		CAutoPtr<int> pCols(new int[count]);
		GetOrderArray(pCols, count);
		*/

		if (bVisible)
		{
			// restore item width
			ResetItemWidth(index);

			/*
			// move the item to the original position
			int nVisible = GetVisibleItemCount();
			int nTarget;
			if (index > nVisible - 1)
				nTarget = nVisible - 1;
			else
				nTarget = index;

			ASSERT(nTarget <= nOrder);
	
			for (int i = nOrder; i > nTarget; i--)
				pCols[i] = pCols[i - 1];
			pCols[nTarget] = index;
			*/
		}
		else
		{
			// hide item
			SetItemWidth(index, 0);
		}

		/*
		SetOrderArray(count, pCols);
		*/
	}
}

BOOL CHeaderCtrlEx::SetOrderArray(int iCount, LPINT piArray)
{
	/*
	// move all hidden items to the end
	int *piArrayNew = new int[iCount];
	int *p1 = piArrayNew;
	int *p2 = piArrayNew + (iCount - 1);
	for (int i = 0, j = 0; i < iCount; i++)
	{
		int index = piArray[i];
		if (GetVisible(index))
			*p1++ = index;
		else
			*p2-- = index;
	}

	BOOL ret = CHeaderCtrl::SetOrderArray(iCount, piArrayNew);
	delete[] piArrayNew;
	*/

	BOOL ret = CHeaderCtrl::SetOrderArray(iCount, piArray);

	GetParent()->Invalidate();

	return ret;
}

BOOL CHeaderCtrlEx::GetRemovable(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	return pData->m_bRemovable;
}

void CHeaderCtrlEx::SetRemovable(int index, BOOL bRemovable)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	pData->m_bRemovable = bRemovable;
}

void CHeaderCtrlEx::EnableCheckBox(BOOL bEnable)
{
	m_bEnableCheckBox = bEnable;

	CRect rcCheck;
	GetItemRect(m_nCheckBox, rcCheck);
	InvalidateRect(&rcCheck, FALSE);
}

void CHeaderCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	HDHITTESTINFO ht;
	ht.pt = point;
	int index = (int)SendMessage(HDM_HITTEST, 0, (LPARAM)&ht);
	if (index >= 0)
	{
		if (m_nCheckBox >= 0)
		{
			if (ht.flags == HHT_ONHEADER && ht.iItem == m_nCheckBox)
			{
				if (m_bEnableCheckBox)
				{
					SetCheck(!GetCheck());

					NMHEADER nm;
					nm.hdr.code = HDN_CHECK;
					nm.hdr.hwndFrom = m_hWnd;
					nm.hdr.idFrom = GetDlgCtrlID();
					nm.iButton = 0;
					nm.iItem = ht.iItem;
					nm.pitem = NULL;
					GetParent()->SendMessage(WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm);
				}
				else
				{
					MessageBeep(0);
				}
				return;
			}
		}
		
		if (ht.flags == HHT_ONDIVIDER || ht.flags == HHT_ONDIVOPEN)
		{
			ReleaseCapture();
			m_nDraggingItem = FindVisibleItem(index);
		}

		if (m_nDraggingItem >= 0)
		{
			m_ptDragStart = point;
			BeginDragDivider();

			return;
		}
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void CHeaderCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_nDraggingItem >= 0)
	{
		int dx = point.x - m_ptDragStart.x;
		int cxy = max(0, m_nItemWidth + dx);

		CItemData *pData = (CItemData *)GetItemData(m_nDraggingItem);			
		if (cxy < pData->m_nMinWidth)
			cxy = pData->m_nMinWidth;
		if (pData->m_nMaxWidth >0 && cxy > pData->m_nMaxWidth)
			cxy = pData->m_nMaxWidth;
		
		if (m_bDragFullWindow)
		{
			SetItemWidth(m_nDraggingItem, cxy);
		}
		else
		{
			DrawDragDivider();
			m_ptDragMove = point;
			m_ptDragMove.x = m_ptDragStart.x + (cxy - m_nItemWidth);
			DrawDragDivider();
		}
		return;
	}

	CHeaderCtrl::OnMouseMove(nFlags, point);
}

void CHeaderCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_nDraggingItem >= 0)
	{
		EndDragDivider(TRUE);
		return;
	}

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

void CHeaderCtrlEx::BeginDragDivider()
{
	SetCapture();

	m_nItemWidth = GetItemWidth(m_nDraggingItem);

	// check if dragging full window is enabled
	if (m_bAllowDragFullWindow)
		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bDragFullWindow, 0);
	else
		m_bDragFullWindow = FALSE;	// force off

	m_ptDragMove = m_ptDragStart;
	if (!m_bDragFullWindow)
		DrawDragDivider();
}

void CHeaderCtrlEx::EndDragDivider(BOOL bSubmit)
{
	if (!m_bDragFullWindow)
		DrawDragDivider();

	if (bSubmit)
	{
		if (!m_bDragFullWindow)
		{
			int dx = m_ptDragMove.x - m_ptDragStart.x;
			if (dx != 0)
			{
				int cxy = max(0, m_nItemWidth + dx);
				SetItemWidth(m_nDraggingItem, cxy);
			}
		}
	}
	else
	{
		// cancel and restore the original width
		if (m_bDragFullWindow)
			SetItemWidth(m_nDraggingItem, m_nItemWidth);
	}

	m_nDraggingItem = -1;
	ReleaseCapture();
}

void CHeaderCtrlEx::DrawDragDivider()
{
	CWnd *pParent = GetParent();

	CDC *pDC = pParent->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
	int nROP2 = pDC->SetROP2(R2_NOT);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen *pPenOld = pDC->SelectObject(&pen);

	CRect rc;
	pParent->GetWindowRect(&rc);
	ScreenToClient(&rc);

	CRect rcItem;
	GetItemRect(m_nDraggingItem, &rcItem);
	AdjustItemRect(rcItem);

	int dx = m_ptDragMove.x - m_ptDragStart.x;
	int x = max(rcItem.left, rcItem.right + dx);
	pDC->MoveTo(x, rc.top);
	pDC->LineTo(x, rc.bottom);

	pDC->SelectObject(pPenOld);
	pDC->SetROP2(nROP2);
	pParent->ReleaseDC(pDC);
}

void CHeaderCtrlEx::AdjustItemRect(CRect &rc)
{
	CWnd *pParent = GetParent();

	CRect rcParent;
	pParent->GetClientRect(&rcParent);

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	pParent->ScreenToClient(&rcWindow);

	int offset = pParent->GetScrollPos(SB_HORZ);
	rc.OffsetRect(-offset, 0);
}

void CHeaderCtrlEx::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	TRACE("OnCaptureChanged(0x%08x), m_nDraggingItem = %d\n", pWnd, m_nDraggingItem);
	if (pWnd != this && m_nDraggingItem >= 0)
		EndDragDivider(FALSE);

	CHeaderCtrl::OnCaptureChanged(pWnd);
}

int CHeaderCtrlEx::ItemFromPoint(CPoint point)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		CRect rc;
		GetItemRect(i, &rc);
		AdjustItemRect(rc);
		if (rc.PtInRect(point))
			return i;
	}
	return -1;
}

DWORD_PTR CHeaderCtrlEx::GetItemData(int index)
{
	HDITEM hdi;
	hdi.mask = HDI_LPARAM;
	if (GetItem(index, &hdi))
		return (DWORD_PTR)hdi.lParam;
	return NULL;
}

BOOL CHeaderCtrlEx::SetItemData(int index, DWORD_PTR dwData)
{
	HDITEM hdi;
	hdi.mask = HDI_LPARAM;
	hdi.lParam = (LPARAM)dwData;
	return SetItem(index, &hdi);
}

int CHeaderCtrlEx::IndexToOrder(int index)
{
	int nOrder = -1;
	int count = GetItemCount();
	if (count > 0)
	{
		CAutoPtr <int> pnColOrder(new int[count]);
		GetOrderArray(pnColOrder, count);
		for (int i = 0; i < count; i++)
		{
			if (pnColOrder[i] == index)
			{
				nOrder = i;
				break;
			}
		}
	}
	return nOrder;
}

int CHeaderCtrlEx::FindVisibleItem(int index)
{
	if (GetVisible(index))
		return index;

	int nOrder = IndexToOrder(index);
	while (nOrder > 0)
	{
		index = OrderToIndex(--nOrder);
		if (GetVisible(index))
			return index;
	}
	return -1;
}

void CHeaderCtrlEx::OnDestroy()
{

	for (int i = 0; i < GetItemCount(); i++)
	{
		delete (CItemData *)GetItemData(i);
		SetItemData(i, NULL);
	}

	CHeaderCtrl::OnDestroy();

	// TODO: Add your message handler code here
}

void CHeaderCtrlEx::ResetItemWidth(int index)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	SetItemWidth(index, pData->m_nInitWidth);
}

BOOL CHeaderCtrlEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	//return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);

	HCURSOR hCursor = NULL;

	HDHITTESTINFO ht;
	::GetCursorPos(&ht.pt);
	ScreenToClient(&ht.pt);
	int index = (int)SendMessage(HDM_HITTEST, 0, (LPARAM)&ht);
	if (index >= 0)
	{
		if (ht.flags == HHT_ONDIVIDER)
		{
			hCursor = AfxGetApp()->LoadCursor(IDC_HEADER_SIZE);
			ASSERT(hCursor != NULL);
		}
		else if (ht.flags == HHT_ONDIVOPEN)
		{
			int nItem = FindVisibleItem(index);
			if (nItem >= 0)
			{
				if (GetItemWidth(nItem) > 0)
					hCursor = AfxGetApp()->LoadCursor(IDC_HEADER_SIZE);
				else
					hCursor = AfxGetApp()->LoadCursor(IDC_HEADER_OPEN);
			}
		}
	}

	if (hCursor == NULL)
		hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	::SetCursor(hCursor);

	return TRUE;
}

void CHeaderCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	HDHITTESTINFO ht;
	::GetCursorPos(&ht.pt);
	ScreenToClient(&ht.pt);
	int index = (int)SendMessage(HDM_HITTEST, 0, (LPARAM)&ht);
	if (index >= 0)
	{
		TRACE("ht.flags = %d\n", ht.flags);
		if (ht.flags == HHT_ONDIVOPEN)
		{
			int nItem = FindVisibleItem(index);
			if (nItem >= 0)
			{
				if (GetItemWidth(nItem) > 0)
				{
					NMHEADER nm;
					nm.hdr.code = HDN_DIVIDERDBLCLICK;
					nm.hdr.hwndFrom = m_hWnd;
					nm.hdr.idFrom = GetDlgCtrlID();
					nm.iButton = 0;
					nm.iItem = nItem;
					nm.pitem = NULL;	// not used for HDN_DIVIDERDBLCLICK
					GetParent()->SendMessage(WM_NOTIFY, (WPARAM)nm.hdr.idFrom, (LPARAM)&nm);
				}
			}
		}
	}

	CHeaderCtrl::OnLButtonDblClk(nFlags, point);
}

void CHeaderCtrlEx::LimitItemWidth(int index, int nMin, int nMax)
{
	CItemData *pData = (CItemData *)GetItemData(index);
	pData->m_nMinWidth = nMin;
	pData->m_nMaxWidth = nMax;
}

void CHeaderCtrlEx::AllowDragFullWindow(BOOL bAllow)
{
	m_bAllowDragFullWindow = bAllow;
}

void CHeaderCtrlEx::SetCheckBoxItem(int index, BOOL bCheckBox)
{
	m_nCheckBox = index >= 0 ? index : -1;
}

BOOL CHeaderCtrlEx::GetCheck()
{
	return m_bChecked;
}

void CHeaderCtrlEx::SetCheck(BOOL bCheck)
{
	if (m_nCheckBox >= 0 && m_bChecked != bCheck)
	{
		m_bChecked = bCheck;

		CRect rcCheck;
		GetItemRect(m_nCheckBox, rcCheck);
		InvalidateRect(&rcCheck, FALSE);
	}
}

void CHeaderCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	// TODO: Add your control notification handler code here

	if (m_nCheckBox >= 0)
	{
		if (pNMCD->dwDrawStage == CDDS_PREPAINT)
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
		else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
		{
			if (pNMCD->dwItemSpec == m_nCheckBox)
			{
				*pResult = CDRF_NOTIFYPOSTPAINT;
				return; 
			}
		}
		else if (pNMCD->dwDrawStage == CDDS_ITEMPOSTPAINT)
		{
			int nItem = (int)pNMCD->dwItemSpec;
			ASSERT(nItem == m_nCheckBox);

			CRect rcCheck;
			GetItemRect(nItem, &rcCheck);
			if(this->m_bEnableCheckBox)
			{
				CMFCVisualManager *pManager = CMFCVisualManager::GetInstance();
				pManager->OnDrawCheckBox(CDC::FromHandle(pNMCD->hdc), rcCheck, FALSE, GetCheck() ? 1 : 0, m_bEnableCheckBox);
			}
		}
	}

	*pResult = CDRF_DODEFAULT;
}

//*************************************************************************
// ListCtrlEx.cpp : header file
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
#include ".\ListCtrlEx.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)
CListCtrlEx::CListCtrlEx()
{
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_NOTIFY(NM_RCLICK, 0, OnNmRclickHeader)	// this map is added manually.
	ON_NOTIFY(HDN_DIVIDERDBLCLICKA, 0, OnHdnDividerdblclick)
	ON_NOTIFY(HDN_DIVIDERDBLCLICKW, 0, OnHdnDividerdblclick)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CListCtrlEx message handlers


BOOL CListCtrlEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	if (CListCtrl::Create(dwStyle, rect, pParentWnd, nID))
		return SubClassHeaderCtrl();
	else
		return FALSE;
}

BOOL CListCtrlEx::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	if (CListCtrl::CreateEx(dwExStyle, dwStyle, rect, pParentWnd, nID))
		return SubClassHeaderCtrl();
	else
		return FALSE;
}

void CListCtrlEx::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	SubClassHeaderCtrl();

	CListCtrl::PreSubclassWindow();
}

BOOL CListCtrlEx::SubClassHeaderCtrl()
{
	// check if already subclassed
	if (m_wndHeader.m_hWnd != NULL)
		return TRUE;

	HWND hWnd = ::GetWindow(m_hWnd, GW_CHILD);
	if (hWnd != NULL)
		return m_wndHeader.SubclassWindow(hWnd);

	return FALSE;
}

LRESULT CListCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	switch (message)
	{
		case LVM_INSERTCOLUMN:
		{
			LRESULT ret = CListCtrl::WindowProc(message, wParam, lParam);
			if ((int)ret >= 0)
			{
				LVCOLUMN *pCol = (LVCOLUMN *)lParam;				
				CHeaderCtrlEx::CItemData *pData = new CHeaderCtrlEx::CItemData(pCol->cx, TRUE, TRUE);
				m_wndHeader.SetItemData((int)ret, (DWORD_PTR)pData);
			}
			return ret;
		}

		case LVM_DELETECOLUMN:
		{
			CHeaderCtrlEx::CItemData *pData = (CHeaderCtrlEx::CItemData *)m_wndHeader.GetItemData((int)wParam);
			ASSERT(pData);
			LRESULT ret = CListCtrl::WindowProc(message, wParam, lParam);
			if ((BOOL)ret && pData)
				delete pData;
			return ret;
		}
	}

	return CListCtrl::WindowProc(message, wParam, lParam);
}

void CListCtrlEx::OnNmRclickHeader(NMHDR *pNMHDR, LRESULT *pResult)
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
		TRACE("%d", piArray[i]);
		if (!m_wndHeader.GetVisible(piArray[i]))
			TRACE("*");
		TRACE(" ");
	}
	TRACE("\n");
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

		TCHAR szText[TEXT_LEN + sizeof(TEXT_TAIL) - 1];
		HDITEM hdi;
		hdi.mask = HDI_TEXT;
		hdi.pszText = szText;
		hdi.cchTextMax = TEXT_LEN;
		int count = m_wndHeader.GetItemCount();
		for (int i = 0; i < count; i++)
		{
			if (!m_wndHeader.GetItem(i, &hdi))
				return;

			if (hdi.cchTextMax == TEXT_LEN - 1)
				_tcscat(szText, TEXT_TAIL);

			UINT nFlags = MF_STRING;
			if (!m_wndHeader.GetRemovable(i))
				nFlags |= MF_GRAYED | MF_CHECKED;			
			if (m_wndHeader.GetVisible(i))
				nFlags |= MF_CHECKED;
			if (!menu.AppendMenu(nFlags, i + 1, szText))
				return;
		}

		if (LoadString(AfxGetInstanceHandle(), IDS_RESET_COLUMNS, szText, 54))
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
		int index = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this);
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

void CListCtrlEx::OnResetColumns()
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

int CListCtrlEx::GetSubItemIdealWith(int nItem)
{
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(GetFont());

	int nMaxWidth = 0;
	CString str;

	int count = GetItemCount();
	for (int i = 0; i < count; i++)
	{
		str = GetItemText(i, nItem);
		int w = GetStringWidth(str);
		nMaxWidth = max(nMaxWidth, w + 12);
	}

	if (nItem == 0)
	{
		CImageList *pImgList = GetImageList(LVSIL_SMALL);
		if (pImgList != NULL)
		{
			int cx, cy;
			ImageList_GetIconSize(pImgList->m_hImageList, &cx, &cy);
			nMaxWidth += cx;
		}
	}

	dc.SelectObject(pOldFont);

	return nMaxWidth;
}

void CListCtrlEx::OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult)
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
			SetColumnWidth(nItem, nIdealWidth);

		}
	}
}

void CListCtrlEx::SetRegistryKey(LPCTSTR lpszKey)
{
	m_strKey = lpszKey;
}

BOOL CListCtrlEx::SaveState()
{
	ASSERT(!m_strKey.IsEmpty());

	int count = m_wndHeader.GetItemCount();
	AfxGetApp()->WriteProfileInt(m_strKey, "Column Count", count);

	int *nColOrder = new int[count];
	m_wndHeader.GetOrderArray(nColOrder, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, "Column Order", (BYTE *)nColOrder, sizeof(int) * count);
	delete[] nColOrder;

	int *nColWidth = new int[count];
	m_wndHeader.GetWidthArray(nColWidth, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, "Column Width", (BYTE *)nColWidth, sizeof(int) * count);
	delete[] nColWidth;

	int *nColVisible = new int[count];
	m_wndHeader.GetVisibleArray(nColVisible, count);
	AfxGetApp()->WriteProfileBinary(m_strKey, "Column Visible", (BYTE *)nColVisible, sizeof(int) * count);
	delete[] nColVisible;

	return TRUE;
}

BOOL CListCtrlEx::RestoreState()
{
	ASSERT(!m_strKey.IsEmpty());

	int count = AfxGetApp()->GetProfileInt(m_strKey, "Column Count", 0);
	if (count != m_wndHeader.GetItemCount())
		return FALSE;

	UINT bytes;

	int *nColOrder = NULL;
	BOOL ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, "Column Order", (BYTE **)&nColOrder, &bytes) && bytes == sizeof(int) * count))
	{
		ret = VerifyOrderArray(nColOrder, count) && m_wndHeader.SetOrderArray(count, nColOrder);
		ASSERT(ret);
	}
	delete[] nColOrder;

	if (!ret)
		return FALSE;

	int *nColWidth = NULL;
	ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, "Column Width", (BYTE **)&nColWidth, &bytes) && bytes == sizeof(int) * count))
		ret = m_wndHeader.SetWidthArray(count, nColWidth);
	delete[] nColWidth;
	if (!ret)
		return FALSE;

	int *nColVisible = NULL;
	ret = FALSE;
	if ((AfxGetApp()->GetProfileBinary(m_strKey, "Column Visible", (BYTE **)&nColVisible, &bytes) && bytes == sizeof(int) * count))
		ret = m_wndHeader.SetVisibleArray(count, nColVisible);
	delete[] nColVisible;

	return ret;
}

BOOL CListCtrlEx::VerifyOrderArray(int *piArray, int count)
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

void CListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	// Ctrl + '+' will auto resize all columns
	if (nChar == VK_ADD && (GetKeyState(VK_CONTROL) & 0x8000))
		return;

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

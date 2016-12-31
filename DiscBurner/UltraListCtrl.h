#pragma once

#include "HeaderCtrlEx.h"
#include "ToolTipCtrlEx.h"
#include <vector>
#include <algorithm>

#define ULS_REPORT				(1L << 0)
#define ULS_MULTISEL			(1L << 1)
#define ULS_HEADERDRAGDROP		(1L << 2)
#define ULS_HOTTRACK			(1L << 3)
#define ULS_ROWLINE				(1L << 4)
#define ULS_COLLINE				(1L << 5)
#define ULS_SMOOTHSCROLLING		(1L << 6)
#define ULS_CHECKBOX			(1L << 7)

#define ULN_SELCHANGED			1
#define ULN_DBLCLK				2

class CULSelection : protected std::vector<int>
{
public:
	CULSelection()
	{
	}

	void Add(int index)
	{
		for (int i = size() - 1; i >= 0; i--)
		{
			if (index > (*this)[i])
			{
				insert(begin() + i + 1, index);
				return;
			}
		}
		
		insert(begin(), index);
	}

	void Remove(int index)
	{
		iterator it = std::find(begin(), end(), index);
		ASSERT(it != end());
		if (it != end())
			erase(it);
	}

	size_t GetCount()
	{
		return size();
	}

	void Clear()
	{
		return clear();
	}

	bool IsEmpty()
	{
		return empty();
	}

	bool IsEqual(const CULSelection &sel)
	{
		return *this == sel;
	}

	void Set(const CULSelection &sel)
	{
		clear();
		*this = sel;
	}

	void SetRange(int index1, int index2)
	{
		int nStart = min(index1, index2);
		int nEnd = max(index1, index2);

		Clear();
		for (int i = nStart; i <= nEnd; i++)
			Add(i);
	}

	int Find(int index)
	{
		iterator it = std::find(begin(), end(), index);
		if (it != end())
			return it - begin();
		else
			return -1;
	}

	void OnDeleteItem(int index)
	{
		for (iterator it = begin(); it != end(); it++)
		{
			if (*it > index)
				(*it)--;
		}
	}

	void OnInsertItem(int index)
	{
		for (iterator it = begin(); it != end(); it++, index++)
		{
			if (*it >= index)
				(*it)++;
		}
	}

	int GetAt(int index)
	{
		return (*this)[index];
	}

	// Note: Use this function only in single selection mode because it does not sort the selection.
	void Select(int value)
	{
		clear();
		push_back(value);
	}
};

// CUltraListCtrl

class CUltraListCtrl : public CWnd
{
protected:
	class CListItem
	{
		typedef std::vector<TCHAR *> ITEM_TEXT_LIST;
	public:
		CListItem()
		{
			ASSERT(m_list.size() == 0);
			m_bChecked = FALSE;
			m_dwData = 0;	// user data
		};

		~CListItem()
		{
			for (ITEM_TEXT_LIST::iterator it = m_list.begin(); it != m_list.end(); ++it)
			{
				delete *it;
			}
			m_list.clear();
		};

		ITEM_TEXT_LIST m_list;
		BOOL m_bChecked;
		DWORD_PTR m_dwData;	// User data
	};
	typedef std::vector<CUltraListCtrl::CListItem *> ITEM_LIST;

	DWORD m_dwStyle;

	ITEM_LIST m_items;
	CULSelection m_sel;

	CRect m_rcMargin;
	CSize m_sizeIcon;
	CSize m_sizeIconSpace;
	int m_nItemHeight;
	int m_nItemSpace;

	int m_nHover;
	int m_nFocus;
	int m_nAnchor;
	int m_nLastSelected;

	CToolTipCtrlEx m_tooltip;

	BOOL m_bSelecting;

	CHeaderCtrlEx m_wndHeader;

	CString m_strKey;

	CRect m_rcSelection;
	BOOL m_bAutoScroll;


public:
	CUltraListCtrl();
	virtual ~CUltraListCtrl();

public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	DWORD GetULStyle() { return m_dwStyle; }
	void ModifyULStyle(DWORD dwRemove, DWORD dwAdd);
	int InsertItem(int index, DWORD_PTR dwData, BOOL bNotify = TRUE);
	void DeleteItem(int index, BOOL bNotify = TRUE);
	void DeleteAllItems();
	int ItemFromPoint(CPoint point);
	void MoveSelection(int nNew);

	int GetFirstVisibleRow();
	int GetVisibleRows();

	int GetFirstVisibleItem();
	void SetFirstVisibleItem(int index);

	void SelectItem(int index, BOOL bNotify);
	void SelectAllItems();
	void UnselectItem(int index);
	void UnselectAllItems();
	int GetSelectedCount() {return m_sel.GetCount();}
	int GetFirstSelectedItem();

	void SetFocusItem(int index);
	void SetAnchorItem(int index);

	void GetItemRect(int index, RECT *pRect);
	void RedrawItem(int index);

	void GetSubItemRect(int nItem, int nSubItem, RECT *pRect);
	virtual void GetCheckBoxRect(int nItem, RECT *pRect);

	BOOL IsItemSelected(int index);
	int GetFocusItem() {return m_nFocus;}
	int GetHoverItem() {return m_nHover;}

	BOOL GetCheck(int index);
	void SetCheck(int index, BOOL bCheck);

	void SetItemData(int index, DWORD dwData);
	DWORD_PTR GetItemData(int index);
	int GetItemCount() {return (int)m_items.size();}

	void SetMargin(int l, int t, int r, int b);
	void SetIconSpace(int cx, int cy);
	void SetIconSize(int cx, int cy);
	void SetItemHeight(int nHeight);
	void SetItemSpace(int nSpace);

	CPoint IndexToPos(int index);
	int PosToIndex(int nRow, int nCol);
	void EnsureVisible(int index);

	CHeaderCtrlEx *GetHeaderCtrl() {return &m_wndHeader;}
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1);
	void SetRegistryKey(LPCTSTR lpszKey);
	BOOL SaveState();
	BOOL RestoreState();

	CString GetItemText(int nItem, int nSubItem);
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	virtual int GetSubItemIdealWith(int nItem);
	BOOL IsReportView() {return (m_dwStyle & ULS_REPORT) == ULS_REPORT;}

	void GetWorkSpace(LPRECT pRect);
	CSize GetItemSize();
	CSize GetItemSpace();

protected:
	void RecalcHeaderPos(BOOL bRedraw = TRUE);
	void RecalcScrollPos();
	virtual void DrawBackground(CDC *pDC, const CRect *pRect);
	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual COLORREF GetSelectionRectColor();
	virtual void OnInsertItem(int index);
	virtual void OnDeleteItem(int index);
	
	virtual BOOL CanCheckItem(int index, BOOL bCheck) { return TRUE; }
	virtual void OnCheckItem(int index, BOOL bCheck) {}

	virtual BOOL GetToolTipText(int index, CString &strText);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void TrackSelection(CPoint point);
	void DelayEnsureVisible(int index);
	void CancelDelayEnsureVisible();
	CSize GetHeaderSize();
	BOOL VerifyOrderArray(int *piArray, int count);
	virtual void OnResetColumns();
	void EnableAutoScroll(BOOL bEnable);

	void InvalidateSelectionRect(const CRect &rcOld, const CRect &rcNew);
	void DrawSelectionRect(CDC *pDC);
	CSize GetRowCol();
	void DoPaint(CDC &dc);

	void UpdateToolTip(CPoint point);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNmRclickHeader(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEndDrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnDividerDblClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnCheck(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
};

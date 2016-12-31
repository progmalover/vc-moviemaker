#pragma once

#include <vector>

#define SLN_SELCHANGE		1
#define SLN_HOVRTCHANGE		2
#define SLN_SELCANCEL		3

// CScrollList

class CScrollList : public CWnd
{
	typedef std::vector<LPARAM> ITEM_LIST;

public:
	CScrollList();
	virtual ~CScrollList();
	int InsertItem(int index, LPARAM lParam);
	BOOL DeleteItem(int index);
	BOOL SetItemData(int index, LPARAM lParam);
	LPARAM GetItemData(int index);
	void SetItemHeight(int nHeight);
	int GetItemHeight() {return m_nItemHeight;}
	void SetScrollerHeight(int nHeight);
	int GetScrollerHeight() {return m_nScrollerHeight;}
	int GetCount() {return m_list.size();}
	BOOL GetItemRect(int index, LPRECT lpRect);
	void DeleteAllItems();
	void SetTopIndex(int index);
	int GetTopIndex() {return m_nTopIndex;}
	void SetSelIndex(int index, BOOL bNotify = TRUE);
	int GetSelIndex() {return m_nSelIndex;}
	void SetHoverIndex(int index, BOOL bNotify = TRUE);
	int GetHoverIndex() {return m_nHoverIndex;}
	int ItemFromPoint(const CPoint &point);
	int GetItemCount() {return m_list.size();}
	int GetVisibleItems();
	BOOL CanScrollUp();
	BOOL CanScrollDown();
	void Scroll(BOOL bScrollUp);

protected:
	int m_nItemHeight;
	int m_nScrollerHeight;
	ITEM_LIST m_list;

	int m_nTopIndex;
	int m_nSelIndex;
	int m_nHoverIndex;

	virtual void DoPaint(CDC *pDC);
	virtual void DrawBackground(CDC *pDC, const CRect &rc);
	virtual void DrawItem(CDC *pDC, int index, const CRect &rc) = 0;
	virtual void DrawScroller(CDC *pDC, const CRect &rc, BOOL bTop);
	BOOL IsRectVisible(const CRect &rcClient, const CRect &rcItem);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPrintClient(WPARAM wp, LPARAM lp);
};

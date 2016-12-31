//*************************************************************************
// HeaderCtrlEx.h : header file
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

#pragma once

#define HDN_CHECK (HDN_LAST + 1)

// CHeaderCtrlEx

class CHeaderCtrlEx : public CHeaderCtrl
{
public:
	class CItemData
	{
	public:
		CItemData(int nInitWidth, BOOL bRemovable, BOOL bVisible)
		{
			m_nInitWidth = nInitWidth;
			m_nMinWidth = 0;
			m_nMaxWidth = 0;
			m_bRemovable = bRemovable;
			m_bVisible = bVisible;
		}
		int m_nInitWidth;
		int m_nMinWidth;
		int m_nMaxWidth;
		BOOL m_bRemovable;
		BOOL m_bVisible;
	};

public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();

	BOOL SetItemWidth(int index, int nWidth);
	int GetItemWidth(int index);
	BOOL GetWidthArray(int *piArray, int iCount);
	BOOL SetWidthArray(int iCount, int *piArray);
	DWORD_PTR GetItemData(int index);
	BOOL SetItemData(int index, DWORD_PTR dwData);
	int IndexToOrder(int index);
	int FindVisibleItem(int index);
	BOOL GetVisibleArray(int *piArray, int iCount);
	BOOL SetVisibleArray(int iCount, int *piArray);
	BOOL GetVisible(int index);
	void SetVisible(int index, BOOL bVisible);
	BOOL GetRemovable(int index);
	void SetRemovable(int index, BOOL bRemovable);
	void ResetItemWidth(int index);
	void LimitItemWidth(int index, int nMin, int nMax);
	BOOL SetOrderArray(int iCount, LPINT piArray);
	int GetVisibleItemCount();
	void AllowDragFullWindow(BOOL bAllow);
	void AdjustItemRect(CRect &rc);

	void SetCheckBoxItem(int index, BOOL bCheckBox);
	BOOL GetCheck();
	void SetCheck(BOOL bCheck);
	void EnableCheckBox(BOOL bEnable);


protected:
	DECLARE_MESSAGE_MAP()
	int m_nDraggingItem;
	CPoint m_ptDragStart;
	CPoint m_ptDragMove;
	BOOL m_nItemWidth;
	BOOL m_bDragFullWindow;
	BOOL m_bAllowDragFullWindow;
	
	int m_nCheckBox;
	BOOL m_bChecked;
	BOOL m_bEnableCheckBox;

	int ItemFromPoint(CPoint point);
	void BeginDragDivider();
	void EndDragDivider(BOOL bSubmit);
	void DrawDragDivider();

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

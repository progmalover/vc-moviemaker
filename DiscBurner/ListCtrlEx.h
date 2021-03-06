//*************************************************************************
// ListCtrlEx.h : header file
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

#include "HeaderCtrlEx.h"

// CListCtrlEx

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

	CHeaderCtrlEx *GetHeaderCtrl() {return &m_wndHeader;}

protected:
	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual int GetSubItemIdealWith(int nItem);
	void SetRegistryKey(LPCTSTR lpszKey);
	BOOL SaveState();
	BOOL RestoreState();

protected:
	CString m_strKey;
	CHeaderCtrlEx m_wndHeader;
	BOOL VerifyOrderArray(int *piArray, int count);
	BOOL SubClassHeaderCtrl();
	virtual void OnResetColumns();

public:
	afx_msg void OnNmRclickHeader(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

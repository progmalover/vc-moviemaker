#pragma once

#include "MFCToolBarButtonEx.h"

// MFCToolBarEx.h : header file
//
#pragma once

class CMFCToolBarButtonEx;

/////////////////////////////////////////////////////////////////////////////
// CMFCToolBarEx window

class CMFCToolBarEx : public CMFCToolBar
{
	DECLARE_DYNCREATE(CMFCToolBarEx)

// Construction
public:
	CMFCToolBarEx();

	virtual ~CMFCToolBarEx ();

// Attributes
public:

// Operations
public:

// Implementation
public:

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

protected:
	CMFCToolBarButtonEx *m_pButtonTracking;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

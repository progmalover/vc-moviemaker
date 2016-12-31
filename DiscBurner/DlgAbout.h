#pragma once

#include "StaticLink.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog

class CDlgAbout : public CDialog
{
// Construction
public:
	CDlgAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAbout)
#if defined AHS
	enum { IDD = IDD_ABOUTBOX_AHS };
#else
	enum { IDD = IDD_ABOUTBOX };
#endif

	CStaticText		m_stcProduct;
	CStaticText		m_stcBuild;
	CStaticLink		m_stcWebsite;
	CStaticLink		m_stcSupport;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAbout)
	virtual BOOL OnInitDialog();
	afx_msg void OnSupport();
	afx_msg void OnWebsite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

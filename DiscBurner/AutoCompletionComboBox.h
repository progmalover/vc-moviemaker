#if !defined(AFX_AUTOCOMPLETIONCOMBOBOX_H__A2F5170E_7F18_11D2_9B7D_006097F7F4CE__INCLUDED_)
#define AFX_AUTOCOMPLETIONCOMBOBOX_H__A2F5170E_7F18_11D2_9B7D_006097F7F4CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AutoCompletionComboBox.h : header file
//

#include "AutoCompletionComboBoxEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoCompletionComboBox window

class CAutoCompletionComboBox : public CComboBox
{
// Construction
public:
	CAutoCompletionComboBox();

protected:
  virtual void HandleCompletion();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoCompletionComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAutoCompletionComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoCompletionComboBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	CAutoCompletionComboBoxEdit m_edit;
	CString m_strText;
	BOOL m_bSelCanceled;
	BOOL m_bEnableAutoCompletion;

public:
	afx_msg void OnSelEndCancel();
	afx_msg void OnDropDown();
	afx_msg void OnCloseUp();

public:
	void EnableAutoCompletion(BOOL bEnable);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMPLETIONCOMBOBOX_H__A2F5170E_7F18_11D2_9B7D_006097F7F4CE__INCLUDED_)

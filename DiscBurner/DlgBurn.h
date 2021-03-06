#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CDlgBurn dialog

class CDlgBurn : public CDialog
{
	DECLARE_DYNAMIC(CDlgBurn)

public:
	CDlgBurn(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBurn();

// Dialog Data
	enum { IDD = IDD_DIALOG_BURN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	void StartBurning(BOOL bCheckDiscStatus);
	static DWORD BurnThread(LPVOID lpParameter);
	HANDLE m_hThread;

public:
	CString m_strUniqueID;
	CString m_strFolder;
	BOOL m_bSucceeded;

public:
	CComboBox m_cmbBurners;
	CStatic m_wndStatus;
	CProgressCtrl m_wndProgress;

	afx_msg void OnOK();
	afx_msg void OnBnClickedButtonOpenFolder();
	afx_msg void OnCancel();
	afx_msg LRESULT OnBurnMessage(WPARAM wp, LPARAM lp);
	afx_msg void OnCbnSelchangeComboBurners();
	afx_msg void OnDestroy();
};

#pragma once

// CDlgSelectTempFolder dialog

class CDlgSelectTempFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectTempFolder)

public:
	CDlgSelectTempFolder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectTempFolder();

// Dialog Data
	enum { IDD = IDD_SELECT_TEMP_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void UpdateFreeSpace();

public:
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonDiskCost();

protected:
	virtual void OnOK();

public:
	CString m_strTempFolder;
	virtual BOOL OnInitDialog();
};

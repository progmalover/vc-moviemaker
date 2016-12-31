#pragma once


// CDlgNoDiskSpace dialog

class CDlgNoDiskSpace : public CDialog
{
	DECLARE_DYNAMIC(CDlgNoDiskSpace)

public:
	CDlgNoDiskSpace(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNoDiskSpace();

// Dialog Data
	enum { IDD = IDD_NO_DISK_SPACE };

	CString m_strPrompt;
	CString m_strFolder;
	CStatic m_stcIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	afx_msg void OnBnClickedRetry();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDiskCost();
};

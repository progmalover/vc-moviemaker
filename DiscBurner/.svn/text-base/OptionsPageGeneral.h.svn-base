#pragma once

// COptionsPageGeneral dialog

class COptionsPageGeneral : public CPropertyPage
{
public:
	COptionsPageGeneral();
	virtual ~COptionsPageGeneral();

// Dialog Data
	enum { IDD = IDD_OPTIONS_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	DECLARE_MESSAGE_MAP()

	void UpdateFreeSpace();
public:
	CString m_strTempFolder;

	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonDiskCost();
	afx_msg void OnBnClickedButtonUpdateSettings();
	afx_msg void OnBnClickedButtonCheckUpdates();
};

#pragma once

// COptionsPageBurner dialog

class COptionsPageBurner : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageBurner)

public:
	COptionsPageBurner();
	virtual ~COptionsPageBurner();

// Dialog Data
	enum { IDD = IDD_OPTIONS_BURNER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	BOOL m_bCheckDiscStatus;
	BOOL m_bPromptNonBlankDisc;
	BOOL m_bPromptMakeAnotherDisc;

	afx_msg void OnCheckCheckDiscStatus();
};

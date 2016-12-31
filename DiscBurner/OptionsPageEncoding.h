#pragma once
#include "afxwin.h"


// COptionsPageEncoding dialog

class COptionsPageEncoding : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageEncoding)

public:
	COptionsPageEncoding();
	virtual ~COptionsPageEncoding();

// Dialog Data
	enum { IDD = IDD_OPTIONS_ENCODING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	CComboBox m_cmbConcurrentTasks;
	CComboBox m_cmbEncodingThreads;
	int m_nConcurrentTasks;
	int m_nEncodingThreads;
	BOOL m_bPromptStopConversion;
	BOOL m_bFixAspectRatio;
	BOOL m_bSoundFinished;
	CString m_strSoundFinished;

public:
	afx_msg void OnBnClickedCheckSoundFinished();
	afx_msg void OnBnClickedButtonSoundFinished();
};

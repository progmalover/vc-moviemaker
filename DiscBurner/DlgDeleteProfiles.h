#pragma once


// CDlgDeleteProfiles dialog

class CDlgDeleteProfiles : public CDialog
{
	DECLARE_DYNAMIC(CDlgDeleteProfiles)

public:
	CDlgDeleteProfiles(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeleteProfiles();

// Dialog Data
	enum { IDD = IDD_DELETE_PROFILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CCheckListBox m_wndList;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	afx_msg void OnClbnChkChange();
};

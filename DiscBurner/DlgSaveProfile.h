#pragma once

class CProfileItem;

// CDlgSaveProfile dialog

class CDlgSaveProfile : public CDialog
{
	DECLARE_DYNAMIC(CDlgSaveProfile)

public:
	CDlgSaveProfile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSaveProfile();

// Dialog Data
	enum { IDD = IDD_SAVE_PROFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CProfileItem *m_pCurrentProfile;
public:
	CString m_strName;
	CString m_strDesc;
	BOOL m_bRename;
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};

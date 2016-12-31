// DlgSaveProfile.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgSaveProfile.h"
#include "ProfileManager.h"

// CDlgSaveProfile dialog

IMPLEMENT_DYNAMIC(CDlgSaveProfile, CDialog)

CDlgSaveProfile::CDlgSaveProfile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveProfile::IDD, pParent)
	, m_strName(_T(""))
	, m_strDesc(_T(""))
{
	CProfileManager::Instance()->GetCurrentProfile(NULL, &m_pCurrentProfile);

	m_bRename = FALSE;
	m_strName = m_pCurrentProfile->m_strName;
	m_strDesc = m_pCurrentProfile->m_strDesc;
}

CDlgSaveProfile::~CDlgSaveProfile()
{
}

void CDlgSaveProfile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 64);
	DDX_Text(pDX, IDC_EDIT_DESC, m_strDesc);
	DDV_MaxChars(pDX, m_strDesc, 128);
}


BEGIN_MESSAGE_MAP(CDlgSaveProfile, CDialog)
END_MESSAGE_MAP()


// CDlgSaveProfile message handlers

BOOL CDlgSaveProfile::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	if (m_bRename)
	{
		CString str;
		str.LoadString(IDS_RENAME_PROFILE);
		SetWindowText(str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSaveProfile::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!UpdateData(TRUE))
		return;

	m_strName.Trim();
	m_strDesc.Trim();

	if (m_strName.IsEmpty())
	{
		AfxMessageBox(IDS_ENTER_PROFILE_NAME);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
		return;
	}

	CProfileItem *pExistingProfile = CProfileManager::Instance()->FindCustomizedProfile(m_strName);
	if (!m_bRename)
	{
		if (pExistingProfile != NULL)
		{
			if (AfxMessageBoxEx(MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2, IDS_CONFIR_OVERWRITE_PROFILE1, m_strName) != IDYES)
			{
				GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
				return;
			}
		}
	}
	else
	{
		if (pExistingProfile != NULL)
		{
			if (pExistingProfile != m_pCurrentProfile)
			{
				AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_EXISTING_PROFILE_NAME, m_strName);
				GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
				return;
			}
		}
	}

	// Do not call UpdateData() again.
	EndDialog(IDOK);
	//CDialog::OnOK();
}

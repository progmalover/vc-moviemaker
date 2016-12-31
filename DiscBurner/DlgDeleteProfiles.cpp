// DlgDeleteProfiles.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgDeleteProfiles.h"
#include "ProfileManager.h"

// CDlgDeleteProfiles dialog

IMPLEMENT_DYNAMIC(CDlgDeleteProfiles, CDialog)

CDlgDeleteProfiles::CDlgDeleteProfiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDeleteProfiles::IDD, pParent)
{

}

CDlgDeleteProfiles::~CDlgDeleteProfiles()
{
}

void CDlgDeleteProfiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndList);
}


BEGIN_MESSAGE_MAP(CDlgDeleteProfiles, CDialog)
	ON_CLBN_CHKCHANGE(IDC_LIST, OnClbnChkChange)
END_MESSAGE_MAP()


// CDlgDeleteProfiles message handlers

BOOL CDlgDeleteProfiles::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CProfileGroup *pGroup = CProfileManager::Instance()->GetCustomizedGroup();
	ASSERT(pGroup->m_items.size() > 0);

	for (int i = 0; i < pGroup->m_items.size(); i++)
	{
		m_wndList.AddString(pGroup->m_items[i]->m_strName);
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDeleteProfiles::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (AfxMessageBox(IDS_CONFIRM_DELETE_PROFILES, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) != IDYES)
		return;

	for (int i = m_wndList.GetCount() - 1; i >= 0 ; i--)
	{
		if (m_wndList.GetCheck(i) != 0)
		{
			CString strName;
			m_wndList.GetText(i, strName);
			if (CProfileManager::Instance()->DeleteCustomizedProfile(strName))
				m_wndList.DeleteString(i);
		}
	}

	CDialog::OnOK();
}

void CDlgDeleteProfiles::OnClbnChkChange()
{
	for (int i = 0; i < m_wndList.GetCount(); i++)
	{
		if (m_wndList.GetCheck(i) != 0)
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			return;
		}
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);
}

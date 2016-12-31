// DlgNoDisckSpace.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgNoDiskSpace.h"
#include "DlgDiskCost.h"


// CDlgNoDiskSpace dialog

IMPLEMENT_DYNAMIC(CDlgNoDiskSpace, CDialog)

CDlgNoDiskSpace::CDlgNoDiskSpace(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNoDiskSpace::IDD, pParent)
	, m_strPrompt(_T(""))
{

}

CDlgNoDiskSpace::~CDlgNoDiskSpace()
{
}

void CDlgNoDiskSpace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ICON, m_stcIcon);
	DDX_Text(pDX, IDC_STATIC_PROMPT, m_strPrompt);
}


BEGIN_MESSAGE_MAP(CDlgNoDiskSpace, CDialog)
	ON_BN_CLICKED(IDRETRY, &CDlgNoDiskSpace::OnBnClickedRetry)
	ON_BN_CLICKED(IDC_BUTTON_DISK_COST, &CDlgNoDiskSpace::OnBnClickedButtonDiskCost)
END_MESSAGE_MAP()


// CDlgNoDiskSpace message handlers

BOOL CDlgNoDiskSpace::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_stcIcon.SetIcon(AfxGetApp()->LoadStandardIcon(MAKEINTRESOURCE(IDI_WARNING)));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNoDiskSpace::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (ShellBrowseForFolder(m_hWnd, LoadStringInline(IDS_SELECT_A_FOLDER),  "", m_strFolder))
		CDialog::OnOK();
}

void CDlgNoDiskSpace::OnBnClickedRetry()
{
	// TODO: Add your control notification handler code here

	EndDialog(IDRETRY);
}

void CDlgNoDiskSpace::OnBnClickedButtonDiskCost()
{
	// TODO: Add your control notification handler code here

	CDlgDiskCost dlg;
	dlg.DoModal();
}

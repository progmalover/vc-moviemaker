// DlgSelectTempFolder.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgSelectTempFolder.h"
#include "Options.h"
#include "DlgDiskCost.h"

// CDlgSelectTempFolder dialog

IMPLEMENT_DYNAMIC(CDlgSelectTempFolder, CDialog)

CDlgSelectTempFolder::CDlgSelectTempFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectTempFolder::IDD, pParent)
	, m_strTempFolder(_T(""))
{

}

CDlgSelectTempFolder::~CDlgSelectTempFolder()
{
}

void CDlgSelectTempFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_strTempFolder);
}


BEGIN_MESSAGE_MAP(CDlgSelectTempFolder, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDlgSelectTempFolder::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DISK_COST, &CDlgSelectTempFolder::OnBnClickedButtonDiskCost)
END_MESSAGE_MAP()


// CDlgSelectTempFolder message handlers

void CDlgSelectTempFolder::UpdateFreeSpace()
{
	ULARGE_INTEGER nFreeBytes;
	if (::GetDiskFreeSpaceEx(m_strTempFolder, &nFreeBytes, NULL, NULL))
	{
		CString str;
		str.Format(IDS_FREE_SPACE1, GetFileSizeString((INT64)nFreeBytes.QuadPart));
		SetDlgItemText(IDC_STATIC_FREE_SPACE, str);
	}
}

void CDlgSelectTempFolder::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here

	CString strNewFolder = m_strTempFolder;
	if (ShellBrowseForFolder(m_hWnd, LoadStringInline(IDS_SELECT_A_FOLDER), m_strTempFolder, strNewFolder))
	{
		m_strTempFolder = strNewFolder;
		UpdateData(FALSE);
		UpdateFreeSpace();
	}
}

void CDlgSelectTempFolder::OnBnClickedButtonDiskCost()
{
	// TODO: Add your control notification handler code here

	CDlgDiskCost dlg;
	dlg.DoModal();
}

void CDlgSelectTempFolder::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!UpdateData(TRUE))
		return;
	
	CString strTempFolderOld = COptions::Instance()->m_strTempFolder;
	COptions::Instance()->m_strTempFolder = m_strTempFolder;
	if (!COptions::Instance()->PrepareTempFolder(TRUE))
	{
		COptions::Instance()->m_strTempFolder = strTempFolderOld;	// restore
		return;
	}

	CDialog::OnOK();
}

BOOL CDlgSelectTempFolder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	UpdateFreeSpace();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

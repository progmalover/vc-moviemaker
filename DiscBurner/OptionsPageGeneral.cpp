// OptionsPageGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "OptionsPageGeneral.h"
#include "Options.h"
#include "DlgDiskCost.h"
#include "FileList.h"

#include "..\Activation\Activation.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsPageGeneral dialog

COptionsPageGeneral::COptionsPageGeneral()
	: CPropertyPage(COptionsPageGeneral::IDD)
{
	m_strTempFolder = COptions::Instance()->m_strTempFolder;
}

COptionsPageGeneral::~COptionsPageGeneral()
{
}

void COptionsPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_strTempFolder);
}


BEGIN_MESSAGE_MAP(COptionsPageGeneral, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &COptionsPageGeneral::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_DISK_COST, &COptionsPageGeneral::OnBnClickedButtonDiskCost)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SETTINGS, &COptionsPageGeneral::OnBnClickedButtonUpdateSettings)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_UPDATES, &COptionsPageGeneral::OnBnClickedButtonCheckUpdates)
END_MESSAGE_MAP()


// COptionsPageGeneral message handlers


BOOL COptionsPageGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	UpdateFreeSpace();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COptionsPageGeneral::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::OnApply();
}

void COptionsPageGeneral::UpdateFreeSpace()
{
	CString strTempFolder;
	GetDlgItemText(IDC_EDIT_FOLDER, strTempFolder);
	ULARGE_INTEGER nFreeBytes;
	if (::GetDiskFreeSpaceEx(strTempFolder, &nFreeBytes, NULL, NULL))
	{
		CString str;
		str.Format(IDS_DISK_SPACE1, GetFileSizeString((INT64)nFreeBytes.QuadPart));
		SetDlgItemText(IDC_STATIC_FREE_SPACE, str);
	}
}

void COptionsPageGeneral::OnBnClickedButtonDiskCost()
{
	// TODO: Add your control notification handler code here

	CDlgDiskCost dlg;
	dlg.DoModal();
}

void COptionsPageGeneral::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here

	if (CFileList::Instance()->IsConverting())
	{
		AfxMessageBox(IDS_E_CANNOT_CHANGE_TEMP_FOLDER_WHILE_CONVERING);
		return;
	}

	CString strTempFolder;
	GetDlgItemText(IDC_EDIT_FOLDER, strTempFolder);

	CString strNewFolder;
	if (ShellBrowseForFolder(m_hWnd, LoadStringInline(IDS_SELECT_A_FOLDER), strTempFolder, strNewFolder))
	{
		CString strTempFolderOld = COptions::Instance()->m_strTempFolder;
		COptions::Instance()->m_strTempFolder = strNewFolder;
		BOOL ret = COptions::Instance()->PrepareTempFolder(TRUE);
		COptions::Instance()->m_strTempFolder = strTempFolderOld;	// restore

		if (!ret)
			return;

		SetDlgItemText(IDC_EDIT_FOLDER, strNewFolder);
		UpdateFreeSpace();
	}
}

void COptionsPageGeneral::OnBnClickedButtonUpdateSettings()
{
	// TODO: Add your control notification handler code here

	CActivate::Instance()->ChangeCheckForUpdatesSettings();
}

void COptionsPageGeneral::OnBnClickedButtonCheckUpdates()
{
	// TODO: Add your control notification handler code here

	CActivate::Instance()->CheckForUpdates(FALSE);
}

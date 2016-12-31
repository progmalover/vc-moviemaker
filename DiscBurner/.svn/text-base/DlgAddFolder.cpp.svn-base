// DlgAddFolder.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgAddFolder.h"
#include <vector>

BOOL FindSpecialFilesInFolder(LPCTSTR lpszFolder, LPCTSTR lpszExts, BOOL bIncludeSubFolders, std::vector<std::tstring> &aFiles, BOOL *pbCanceled)
{
	// Use a local list to make the files in the current folder inserted 
	// in front of the files in the sub-folder.
	std::vector<std::tstring> aFilesCurrentFolder;

	CString strFind = lpszFolder;
	::PathAddBackslash(strFind.GetBufferSetLength(_MAX_PATH));
	strFind.ReleaseBuffer();
	strFind += _T("*.*");

	CFileFind ff;
	BOOL ret = ff.FindFile(strFind);

	int i = 0;
	while (ret)
	{
		ret = ff.FindNextFile();
		if (!ff.IsDots())
		{
			CString strFile = ff.GetFilePath();

			if (::PathIsDirectory(strFile))
			{
				if (bIncludeSubFolders)
				{
					if (!FindSpecialFilesInFolder(strFile, lpszExts, bIncludeSubFolders, aFiles, pbCanceled))
						return FALSE;
				}
			}
			else
			{
				if (PathMatchSpec(strFile, lpszExts))
					aFilesCurrentFolder.push_back((LPCTSTR)strFile);
			}
		}
	}

	if (aFilesCurrentFolder.size() > 0)
		aFiles.insert(aFiles.begin(), aFilesCurrentFolder.begin(), aFilesCurrentFolder.end());

	DoEvents();

	return !*pbCanceled;
}

// CDlgAddFolder dialog

IMPLEMENT_DYNAMIC(CDlgAddFolder, CDialog)

CDlgAddFolder::CDlgAddFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddFolder::IDD, pParent)
	, m_bIncludeSubFolders(FALSE)
{
	m_nFileTypeIndex = AfxGetApp()->GetProfileInt(_T("Add Folder"), _T("File Type"), 0);
	m_bIncludeSubFolders = AfxGetApp()->GetProfileInt(_T("Add Folder"), _T("Include Sub-Folders"), FALSE);
	m_bCanceled = FALSE;
}

CDlgAddFolder::~CDlgAddFolder()
{
}

void CDlgAddFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FOLDER, m_cmbFolders);
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_cmbFileTypes);
	DDX_Check(pDX, IDC_CHECK_INCLUDE_SUB_FOLDERS, m_bIncludeSubFolders);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
}


BEGIN_MESSAGE_MAP(CDlgAddFolder, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDlgAddFolder::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CDlgAddFolder message handlers

BOOL CDlgAddFolder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_wndProgress.ModifyStyle(0, 0x08 /*PBS_MARQUEE*/, SWP_FRAMECHANGED);
	m_wndProgress.SendMessage(WM_USER + 10 /*PBM_SETMARQUEE*/, TRUE, 10);

	CString strFilter;
	CString strFilterAllSupported;
	CString strFilterAllFiles;
	CString strExtAllSupported;

	strFilterAllSupported.LoadString(IDS_FILTER_ALL_SUPPORTED);
	strFilterAllFiles.LoadString(IDS_FILTER_ALL_FILES);

	UINT nIDs[] = {IDS_FILTER_VIDEO, IDS_FILTER_AUDIO};

	for (int nID = 0;  nID < sizeof(nIDs) / sizeof (UINT); nID++)
	{
		CString strFilter;
		strFilter.LoadString(nIDs[nID]);
		for (int i = 0; ;i += 2)
		{
			CString strName;
			if (!AfxExtractSubString(strName, strFilter, i, '|'))
				break;

			CString strExt;
			if (!AfxExtractSubString(strExt, strFilter, i + 1, '|'))
				break;

			m_aFileTypes.push_back(CFileType(strName, strExt));

			if (!strExtAllSupported.IsEmpty())
				strExtAllSupported += _T(";");
			strExtAllSupported += strExt;
		}
	}

	m_aFileTypes.insert(m_aFileTypes.begin(), CFileType(strFilterAllSupported, strExtAllSupported));
	m_aFileTypes.push_back(CFileType(strFilterAllFiles, _T("*.*")));

	//strFilter = strFilterAllSupported + _T("|") + strExtAllSupported + _T("|") + strFilterVideo + _T("|") + strFilterAudio  + _T("|") + strFilterAllFiles;

	for (UINT i = 0; i < m_aFileTypes.size(); i++)
		m_cmbFileTypes.AddString(m_aFileTypes[i].m_strName);
	ASSERT(m_cmbFileTypes.GetCount() == m_aFileTypes.size());

	if (m_nFileTypeIndex < 0 || m_nFileTypeIndex >= m_aFileTypes.size())
		m_nFileTypeIndex = 0;
		
	m_cmbFileTypes.SetCurSel(m_nFileTypeIndex);

	m_cmbFolders.Init(_T("Add Folder"));
	m_cmbFolders.Load();

	if (m_cmbFolders.GetCount() > 0)
		m_cmbFolders.SetCurSel(0);
	else
		OnBnClickedButtonBrowse();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddFolder::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here

	CString strFolder;
	m_cmbFolders.GetWindowText(strFolder);

	if (ShellBrowseForFolder(m_hWnd, LoadStringInline(IDS_LABEL_SELECT_FOLTER),  strFolder, strFolder))
	{
		m_cmbFolders.SetWindowText(strFolder);
		m_cmbFolders.SetFocus();
	}
}

void CDlgAddFolder::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (!UpdateData(TRUE))
		return;

	CString strFolder;
	m_cmbFolders.GetWindowText(strFolder);

	strFolder.Trim();

	if (strFolder.IsEmpty())
	{
		MessageBeep(MB_ICONWARNING);
		return;
	}

	if (!::PathIsDirectory(strFolder))
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_FOLDER_NOT_EXIST1, strFolder);
		GotoDlgCtrl(GetDlgItem(IDC_COMBO_FOLDER));
		return;
	}

	m_cmbFolders.AddCurrentText(FALSE);
	m_cmbFolders.Save();

	m_nFileTypeIndex = m_cmbFileTypes.GetCurSel();
	ASSERT(m_nFileTypeIndex >=0 && m_nFileTypeIndex < m_aFileTypes.size());
	
	AfxGetApp()->WriteProfileInt(_T("Add Folder"), _T("File Type"), m_nFileTypeIndex);
	AfxGetApp()->WriteProfileInt(_T("Add Folder"), _T("Include Sub-Folders"), m_bIncludeSubFolders);

	m_bCanceled = FALSE;

	GetDlgItem(IDC_COMBO_FILE_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_FOLDER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_INCLUDE_SUB_FOLDERS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_COMBO_FOLDER)->ShowWindow(SW_HIDE);
	m_wndProgress.ShowWindow(SW_SHOW);

	if (!FindSpecialFilesInFolder(strFolder, m_aFileTypes[m_nFileTypeIndex].m_strExts, m_bIncludeSubFolders, m_aFiles, &m_bCanceled))
		return;

	GetDlgItem(IDC_COMBO_FILE_TYPE)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_FOLDER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_INCLUDE_SUB_FOLDERS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_BROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	GetDlgItem(IDC_COMBO_FOLDER)->ShowWindow(SW_SHOW);
	m_wndProgress.ShowWindow(SW_HIDE);

	if (m_aFiles.size() == 0)
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_PROMPT_NO_MATCHED_FILES_IN_FOLDER1, (LPCTSTR)strFolder);
		return;
	}

	CDialog::OnOK();
}

void CDlgAddFolder::OnCancel()
{
	m_bCanceled = TRUE;
	CDialog::OnCancel();
}

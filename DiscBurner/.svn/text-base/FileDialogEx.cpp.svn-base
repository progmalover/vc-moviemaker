// FileDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "FileDialogEx.h"
#include ".\filedialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogEx, CFileDialog)
CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

CFileDialogEx::CFileDialogEx(LPCTSTR lpszSection, BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszFileName, CWnd *pParentWnd) : 
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, bOpenFileDialog ? OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | 0x02000000 /*OFN_DONTADDTORECENT*/ : OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT, lpszFilter, pParentWnd)
{
	m_strSection.Format("File Dialogs\\%s", lpszSection);

	int nFilterIndex = AfxGetApp()->GetProfileInt(m_strSection, "Filter Index", 0);
	m_pOFN->nFilterIndex = nFilterIndex;

	m_strPath = AfxGetApp()->GetProfileString(m_strSection, "Last Path", NULL);
	if (!m_strPath.IsEmpty() && ::PathIsDirectory(m_strPath))
		m_pOFN->lpstrInitialDir = (LPCTSTR)m_strPath;
}

CFileDialogEx::~CFileDialogEx()
{
}


BEGIN_MESSAGE_MAP(CFileDialogEx, CFileDialog)
END_MESSAGE_MAP()



// CFileDialogEx message handlers

BOOL CFileDialogEx::OnFileNameOK()
{
	// TODO: Add your specialized code here and/or call the base class

	AfxGetApp()->WriteProfileInt(m_strSection, "Filter Index", m_pOFN->nFilterIndex);
	ASSERT (m_pOFN->lpstrFile != NULL);	// see MFC source

	if (_tcslen(m_pOFN->lpstrFile) > 0)
	{
		TCHAR szPath[_MAX_PATH];
		_tcsncpy(szPath, m_pOFN->lpstrFile, _MAX_PATH);
		szPath[_MAX_PATH - 1] = '\0';

		/*
		If the OFN_ALLOWMULTISELECT flag is set and the user selects multiple files, the buffer contains the current 
		directory followed by the file names of the selected files. For Explorer-style dialog boxes, the directory 
		and file name strings are NULL separated, with an extra NULL character after the last file name. For old-style 
		dialog boxes, the strings are space separated and the function uses short file names for file names with spaces. 
		You can use the FindFirstFile function to convert between long and short file names. If the user selects only 
		one file, the lpstrFile string does not have a separator between the path and file name.
		*/

		if (!PathIsDirectory(szPath)) 
			PathRemoveFileSpec(szPath);

		ASSERT(::PathIsDirectory(szPath));
		AfxGetApp()->WriteProfileString(m_strSection, "Last Path", szPath);
	}

	return CFileDialog::OnFileNameOK();
}

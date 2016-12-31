#pragma once
#include "historycombobox.h"
#include <vector>
#include <string>
#include "afxcmn.h"


// CDlgAddFolder dialog

class CDlgAddFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddFolder)

	class CFileType
	{
	public:
		CFileType()
		{
		}
		CFileType(LPCTSTR lpszName, LPCTSTR lpszExts)
		{
			m_strName = lpszName;
			m_strExts = lpszExts;
		}
		CString m_strName;
		CString m_strExts;
	};

	typedef std::vector<CFileType> FILE_TYPE_LIST;

public:
	CDlgAddFolder(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAddFolder();

// Dialog Data
	enum { IDD = IDD_ADD_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

	FILE_TYPE_LIST m_aFileTypes;
	BOOL m_bCanceled;

public:
	CHistoryComboBox m_cmbFolders;
	CComboBox m_cmbFileTypes;
	BOOL m_bIncludeSubFolders;
	int m_nFileTypeIndex;

	std::vector<std::tstring> m_aFiles;

	afx_msg void OnBnClickedButtonBrowse();
	CProgressCtrl m_wndProgress;
};

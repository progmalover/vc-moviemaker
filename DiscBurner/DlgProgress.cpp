// DlgProgress.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DEFINE_REGISTERED_MESSAGE(WM_DLGPROGRESS)

// CDlgProgress dialog

IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)
CDlgProgress::CDlgProgress(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt)
	: CDialog(CDlgProgress::IDD, pParent), 
	m_hEvent(NULL), 
	m_bCanceled(FALSE),
	m_bAllowCancel(TRUE), 
	m_strCaption(lpszCaption), 
	m_strPrompt(lpszPrompt)
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CDlgProgress::~CDlgProgress()
{
	if (m_hEvent)
		CloseHandle(m_hEvent);
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_stcPrompt);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	ON_REGISTERED_MESSAGE(WM_DLGPROGRESS, OnProgressMessage)
END_MESSAGE_MAP()


// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	GetDlgItem(IDCANCEL)->EnableWindow(m_bAllowCancel);

	::SetEvent(m_hEvent);

	CMenu *pMenu = GetSystemMenu(FALSE);
	if (pMenu)
		pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);

	SetWindowText(m_strCaption);
	SetPrompt(m_strPrompt);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProgress::WaitForInitialize()
{
	ASSERT(m_hEvent != NULL);
	WaitForSingleObject(m_hEvent, INFINITE);
}

void CDlgProgress::AllowCancel(BOOL bAllow)
{
	m_bAllowCancel = bAllow;
	if (::IsWindow(m_hWnd))
	{
		CWnd *pButton = GetDlgItem(IDCANCEL);
		if (pButton != NULL)
			pButton->EnableWindow(bAllow);
	}
}

void CDlgProgress::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	ASSERT(m_bAllowCancel);
	if (m_bAllowCancel)
		m_bCanceled = TRUE;
}

void CDlgProgress::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CDlgProgress::End(UINT nIDResult)
{
	PostMessage(WM_DLGPROGRESS, 0, (LPARAM)nIDResult);
}

void CDlgProgress::SetPrompt(LPCTSTR lpszPrompt)
{
	SetDlgItemText(IDC_STATIC_PROMPT, lpszPrompt);
}

LRESULT CDlgProgress::OnProgressMessage(WPARAM wp, LPARAM lp)
{
	EndDialog((UINT)lp);
	return 0;
}

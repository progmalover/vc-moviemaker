// DlgShutDown.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgShutDown.h"


// CDlgShutDown dialog

IMPLEMENT_DYNAMIC(CDlgShutDown, CDialog)

CDlgShutDown::CDlgShutDown(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgShutDown::IDD, pParent)
{

}

CDlgShutDown::~CDlgShutDown()
{
}

void CDlgShutDown::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShutDown, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgShutDown message handlers

BOOL CDlgShutDown::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CString strApp;
	strApp.LoadString(AFX_IDS_APP_TITLE);
	
	SetWindowText(strApp);

	CString strPrompt;
	GetDlgItemText(IDC_STATIC_PROMPT, strPrompt);
	CString strPromptNew;
	strPromptNew.Format(strPrompt, strApp);
	SetDlgItemText(IDC_STATIC_PROMPT, strPromptNew);


	GetDlgItemText(IDOK, m_strButtonText);

	m_nCounter = 30;
	SetTimer(1000, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgShutDown::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1000)
	{
		if (m_nCounter-- == 0)
		{
			KillTimer(1000);
			EndDialog(IDOK);
		}
		else
		{
			CString strButtonText;
			strButtonText.Format("%s (%d)", m_strButtonText, m_nCounter);
			SetDlgItemText(IDOK, strButtonText);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

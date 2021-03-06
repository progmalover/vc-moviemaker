// DlgBurn.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgBurn.h"
#include "Burner.h"
#include "Options.h"
#include "MainFrm.h"
#include "ProfileManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CDlgBurn dialog

IMPLEMENT_DYNAMIC(CDlgBurn, CDialog)

CDlgBurn::CDlgBurn(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBurn::IDD, pParent)
{
	m_hThread = NULL;
	m_bSucceeded = FALSE;
}

CDlgBurn::~CDlgBurn()
{
}

void CDlgBurn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_wndStatus);
	DDX_Control(pDX, IDC_COMBO_BURNERS, m_cmbBurners);
}


BEGIN_MESSAGE_MAP(CDlgBurn, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_FOLDER, &CDlgBurn::OnBnClickedButtonOpenFolder)
	ON_REGISTERED_MESSAGE(WM_BURN, OnBurnMessage)
	ON_CBN_SELCHANGE(IDC_COMBO_BURNERS, &CDlgBurn::OnCbnSelchangeComboBurners)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDlgBurn message handlers

void CDlgBurn::OnOK()
{
	// TODO: Add your control notification handler code here

	StartBurning(TRUE);
}

void CDlgBurn::OnBnClickedButtonOpenFolder()
{
	// TODO: Add your control notification handler code here

	::ShellOpenFolder(NULL, m_strFolder, TRUE);
}

void CDlgBurn::OnCancel()
{
	// TODO: Add your control notification handler code here

	if (AfxMessageBox(IDS_CONFIRM_CANCEL, MB_YESNO) == IDNO)
		return;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	CString strStatus;
	strStatus.LoadString(IDS_CANCELING);
	m_wndStatus.SetWindowText(strStatus);

	if (m_hThread != NULL && ::IsThreadActive(m_hThread))
	{
		CxBurner::Instance()->Cancel();
		::MsgWaitForSingleObject(m_hThread, INFINITE);
	}

	CDialog::OnCancel();
}

BOOL CDlgBurn::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CBurner *pBurner = CxBurner::Instance();
	if (pBurner->IsOpen())
	{
		DEVICE_LIST devices;
		pBurner->GetBurnerList(devices);

		if (devices.size() > 0)
		{
			for (DEVICE_LIST::iterator it = devices.begin(); it != devices.end(); ++it)
			{
				CString strItem;

				DEVICE &device = *it;

				int index = m_cmbBurners.AddString(device.strName);
				m_cmbBurners.SetItemData(index, (DWORD_PTR)_tcsdup(device.strUniqueID));

				if (device.strUniqueID == m_strUniqueID)
					m_cmbBurners.SetCurSel(index);
			}
		}
	}

	PostMessage(WM_BURN, BURN_START, 0L);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgBurn::OnBurnMessage(WPARAM wp, LPARAM lp)
{
	if (wp == BURN_START)
	{
		StartBurning(lp != 0);
		return 0;
	}

	if (CxBurner::Instance()->IsCanceled())
		return 1;

	switch (wp)
	{
		case BURN_ERASE_BEGIN_EVENT:
		{
			CString strStatus;
			strStatus.LoadString(IDS_ERASING_DISC);
			m_wndStatus.SetWindowText(strStatus);

			GetDlgItem(IDCANCEL)->EnableWindow(FALSE);			
			break;
		}

		case BURN_ERASE_END_EVENT:
		{
			GetDlgItem(IDCANCEL)->EnableWindow(TRUE);			
			break;
		}

		case BURN_ERASE_EVENT:
		{
			ERASE_PROGRESS *pProgress = (ERASE_PROGRESS *)lp;
			
			if (pProgress->lElapsedTime < pProgress->lTotalTime)
			{
				m_wndProgress.SetRange32(0, pProgress->lTotalTime);
				m_wndProgress.SetPos(pProgress->lElapsedTime);
			}

			break;
		}

		case BURN_IMAGE_EVENT:
		{
			IMAGE_PROGRESS *pProgress = (IMAGE_PROGRESS *)lp;
			
			m_wndProgress.SetRange32(0, pProgress->lTotalSectors);
			m_wndProgress.SetPos(pProgress->lCopiedSectors);
			
			CString strText;
			strText.Format(IDS_PREPARING_MEDIA1, pProgress->lpszFile);
			m_wndStatus.SetWindowText(strText);
			break;
		}

		case BURN_DATA_EVENT:
		{
			CString strStatus;
			DATA_PROGRESS *pProgress = (DATA_PROGRESS *)lp;
			
			switch (pProgress->action)
			{
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_INITIALIZING_HARDWARE:
					strStatus.LoadString(IDS_BURNER_INITIALIZING);
					m_wndProgress.SetPos(0);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_CALIBRATING_POWER:
					strStatus.LoadString(IDS_BURNER_CALIBRATING_POWER);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_VALIDATING_MEDIA:
					strStatus.LoadString(IDS_BURNER_VALIDATING_MEDIA);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_FORMATTING_MEDIA:
					strStatus.LoadString(IDS_BURNER_FORMATING_MEDIA);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA:
					strStatus.LoadString(IDS_BURNER_WRITING_DATA);
					m_wndProgress.SetRange32(0, pProgress->lTotalTime);
					m_wndProgress.SetPos(pProgress->lElapsedTime);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_FINALIZATION:
					strStatus.LoadString(IDS_BURNER_FINALIZING);
					m_wndProgress.SetRange32(0, pProgress->lTotalTime);
					m_wndProgress.SetPos(pProgress->lElapsedTime);
					break;
				case IMAPI_FORMAT2_DATA_WRITE_ACTION_COMPLETED:
					strStatus.LoadString(IDS_BURNER_COMPLETED);
					int nMin, nMax;
					m_wndProgress.GetRange(nMin, nMax);
					m_wndProgress.SetPos(nMax);
					break;
				default:
					break;
			}

			m_wndStatus.SetWindowText(strStatus);

			break;
		}

		case BURN_SUCCEEDED:
		{
			m_bSucceeded = TRUE;	// At least successfully burned one copy.

			if (COptions::Instance()->m_bPromptMakeAnotherDisc && AfxMessageBox(IDS_BURNER_MAKE_ANOTHER_COPY, MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
				PostMessage(WM_BURN, BURN_START, 1L);
			else
				CDialog::OnOK();

			break;
		}
		case BURN_FAILED:
		{
			CString strError = CxBurner::Instance()->GetErrorMessage();
			m_wndStatus.SetWindowText(strError);
			AfxMessageBox(strError, MB_ICONERROR);
			m_cmbBurners.EnableWindow(TRUE);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_OPEN_FOLDER)->EnableWindow(TRUE);
			break;
		}

		case BURN_WRITE_PORGRESS:
			{
				m_wndProgress.SetRange32(0, 100);
				m_wndProgress.SetPos((int)lp);
				CString strStatus;
				strStatus.LoadString(IDS_WRITTING);
				m_wndStatus.SetWindowText(strStatus);
				break;
			}

		case BURN_PAD_PROCESS:
			{
				m_wndProgress.SetRange32(0, 100);
				m_wndProgress.SetPos((int)lp);
				CString strStatus;
				strStatus.LoadString(IDS_PADDING);
				m_wndStatus.SetWindowText(strStatus);
				break;
			}

		case BURN_CLOSE_PROCESS:
			{
				m_wndProgress.SetRange32(0, 100);
				m_wndProgress.SetPos((int)lp);
				CString strStatus;
				strStatus.LoadString(IDS_CLOSESESSION);
				m_wndStatus.SetWindowText(strStatus);
				break;
			}

		case BURN_ERASE_PROGRESS:
			{
				m_wndProgress.SetRange32(0, 100);
				m_wndProgress.SetPos((int)lp);
				CString strStatus;
				strStatus.LoadString(IDS_ERASING);
				m_wndStatus.SetWindowText(strStatus);
				break;
			}

		default:
			ASSERT(FALSE);
	}

	return 0;
}

void CDlgBurn::StartBurning(BOOL bCheckDiscStatus)
{
	CString strStatus;

	strStatus.LoadString(IDS_INITIALIZING);
	m_wndStatus.SetWindowText(strStatus);
	m_cmbBurners.EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OPEN_FOLDER)->EnableWindow(FALSE);
	m_wndProgress.SetPos(0);

	if (bCheckDiscStatus)
	{
		BOOL bEraseNeeded;
		if (!CxBurner::Instance()->CheckDiscStatus(m_strUniqueID, bEraseNeeded))
		{
			strStatus.LoadString(IDS_BURNER_INSERT_BLANK_DISC);
			m_wndStatus.SetWindowText(strStatus);
			m_cmbBurners.EnableWindow(TRUE);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_OPEN_FOLDER)->EnableWindow(TRUE);
			m_wndProgress.SetPos(0);

			return;
		}

		if (COptions::Instance()->m_bPromptNonBlankDisc && bEraseNeeded)
		{
			if (AfxMessageBox(IDS_BURNER_CONFIRM_OVERWRITE, MB_YESNO | MB_ICONWARNING) == IDNO)
			{
				strStatus.LoadString(IDS_BURNER_INSERT_BLANK_DISC);
				m_wndStatus.SetWindowText(strStatus);
				m_cmbBurners.EnableWindow(TRUE);
				GetDlgItem(IDOK)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_OPEN_FOLDER)->EnableWindow(TRUE);
				m_wndProgress.SetPos(0);

				return;
			}
		}
	}

	DWORD dwId = 0;
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BurnThread, (LPVOID)this, 0, &dwId);
}

DWORD CDlgBurn::BurnThread(LPVOID lpParameter)
{
	CDlgBurn *pDlg = (CDlgBurn *)lpParameter;

	CString strDiscLabel;
	CProfileParam *pParam = CProfileManager::Instance()->FindProfileParam("disclabel");
	if (pParam != NULL)
		pParam->GetValue(NULL, strDiscLabel);

	BOOL ret = CxBurner::Instance()->Burn(pDlg->m_strUniqueID, pDlg->m_strFolder, COptions::Instance()->m_strTempFolder, strDiscLabel, pDlg->m_hWnd);
	
	::CloseHandle(pDlg->m_hThread);
	pDlg->m_hThread = NULL;

	if (ret)
	{
		pDlg->PostMessage(WM_BURN, BURN_SUCCEEDED, 0L);
	}
	else
	{
		if (!CxBurner::Instance()->IsCanceled())
			pDlg->PostMessage(WM_BURN, BURN_FAILED, 0L);
	}
	
	return 0;
}

void CDlgBurn::OnCbnSelchangeComboBurners()
{
	// TODO: Add your control notification handler code here

	int index = m_cmbBurners.GetCurSel();
	if (index >= 0)
	{
		m_strUniqueID = (LPCTSTR)m_cmbBurners.GetItemData(index);

		/*
		CMFCToolBarComboBoxButton *pBurnerCombo = GetBurnerComboBox();

		for (int i = 0; i < pBurnerCombo->GetCount(); i++)
		{
			TCHAR *pszUniqueID = (TCHAR *)pBurnerCombo->GetItemData(i);
			if (m_strUniqueID.Compare(pszUniqueID) == 0)
			{
				pBurnerCombo->SelectItem(i);
				break;
			}
		}
		*/
	}
}

void CDlgBurn::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	for (int i = 0; i < m_cmbBurners.GetCount(); i++)
	{
		TCHAR *pszItem = (LPTSTR)m_cmbBurners.GetItemData(i);
		free(pszItem);
	}
}

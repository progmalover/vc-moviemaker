// ProfilePane.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ProfilePane.h"
#include "DeferWindowPos.h"
#include "ProfileManager.h"
#include "DlgSaveProfile.h"
#include "DlgDeleteProfiles.h"
#include "FileList.h"


// CProfilePane

const UINT ID_PROFILE_TOOLBAR = 1001;

IMPLEMENT_DYNAMIC(CProfilePane, CBasePane)

CProfilePane::CProfilePane()
{
}

CProfilePane::~CProfilePane()
{
}


BEGIN_MESSAGE_MAP(CProfilePane, CBasePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_PROFILE_SAVE_AS, OnProfileSaveAs)
	ON_COMMAND(ID_PROFILE_RENAME, OnProfileRename)
	ON_COMMAND(ID_PROFILE_DELETE, OnProfileDelete)
	ON_COMMAND(ID_PROFILE_DELETE_MULTIPLE, OnProfileDeleteMultiple)

	ON_UPDATE_COMMAND_UI(ID_PROFILE, OnUpdateProfile)
	ON_UPDATE_COMMAND_UI(ID_PROFILE_SAVE_AS, OnUpdateProfile)
	ON_UPDATE_COMMAND_UI(ID_PROFILE_RENAME, OnUpdateProfile)
	ON_UPDATE_COMMAND_UI(ID_PROFILE_DELETE, OnUpdateProfile)
	ON_UPDATE_COMMAND_UI(ID_PROFILE_DELETE_MULTIPLE, OnUpdateProfile)
END_MESSAGE_MAP()



// CProfilePane message handlers



int CProfilePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBasePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CRect rc(0, 0, 0, 0);

	if (!m_wndToolBar.Create(this, WS_VISIBLE | WS_CHILD | CBRS_ALIGN_TOP, ID_PROFILE_TOOLBAR))
	{
		TRACE0("Failed to create profile toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBar.LoadToolBar(IDR_PROFILE, IDB_PROFILE, 0, TRUE, 0, 0, IDB_PROFILE))
	{
		TRACE0("Failed to create profiletoolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetCustomizeMode(FALSE);

	m_wndToolBar.m_bInit = TRUE;

	return 0;
}

void CProfilePane::OnSize(UINT nType, int cx, int cy)
{
	CBasePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RepositionControls();
}

void CProfilePane::RepositionControls()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		m_wndToolBar.SetRedraw(FALSE);

		m_wndToolBar.SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 
			SWP_NOACTIVATE | SWP_NOZORDER);

		m_wndToolBar.SetRedraw(TRUE);

		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
	}
}

void CProfilePane::OnProfileSaveAs()
{
	CDlgSaveProfile dlg;
	dlg.m_bRename = FALSE;
	if (dlg.DoModal() == IDOK)
	{
		CProfileManager *pManager = CProfileManager::Instance();
		if (pManager->SaveCustomizedProfile(NULL, dlg.m_strName, dlg.m_strDesc, FALSE))
		{
			pManager->SetCurrentProfile(pManager->GetCustomizedGroup(), pManager->FindCustomizedProfile(dlg.m_strName));
		}
	}
}

void CProfilePane::OnProfileRename()
{
	CProfileManager *pManager = CProfileManager::Instance();

	CProfileGroup *pGroup;
	CProfileItem *pItem;
	pManager->GetCurrentProfile(&pGroup, &pItem);
	if (pGroup != CProfileManager::Instance()->GetCustomizedGroup())
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_RENAME_BUILT_IN_PROFILE1, pItem->m_strName);
		return;
	}

	CDlgSaveProfile dlg;
	dlg.m_bRename = TRUE;
	if (dlg.DoModal() == IDOK)
	{
		if (pManager->SaveCustomizedProfile(pItem->m_strName, dlg.m_strName, dlg.m_strDesc, TRUE))
		{
			pManager->SetCurrentProfile(pManager->GetCustomizedGroup(), pManager->FindCustomizedProfile(dlg.m_strName));
		}
	}
}

void CProfilePane::OnProfileDelete()
{
	CProfileGroup *pGroup;
	CProfileItem *pItem;
	CProfileManager::Instance()->GetCurrentProfile(&pGroup, &pItem);
	if (pGroup != CProfileManager::Instance()->GetCustomizedGroup())
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_DELETE_BUILT_IN_PROFILE1, pItem->m_strName);
		return;
	}

	if (AfxMessageBoxEx(MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2, IDS_CONFIRM_DELETE_PROFILE1, pItem->m_strName) == IDYES)
	{
		CProfileManager::Instance()->DeleteCustomizedProfile(pItem->m_strName);
	}
}

void CProfilePane::OnProfileDeleteMultiple()
{
	if (CProfileManager::Instance()->GetCustomizedGroup()->m_items.size() == 0)
	{
		AfxMessageBox(IDS_NO_CUSTOMIZED_PROFILES);
		return;
	}

	CDlgDeleteProfiles dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
}

void CProfilePane::OnUpdateProfile(CCmdUI *pCmd)
{
	// TODO: Add your command handler code here

	pCmd->Enable(!CFileList::Instance()->IsConverting());
}

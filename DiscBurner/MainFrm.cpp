
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Converter.h"

#include "MainFrm.h"
#include "MSMVisualManager.h"
#include "FileList.h"
#include "MFCToolBarLabel.h"
#include "ProfileManager.h"
#include "MFCToolBarSlider.h"
#include "Options.h"
#include "Burner.h"
#include "DlgBurn.h"
#include "DlgSelectTempFolder.h"
#include "OptionsSheet.h"

#include "../Activation/Activation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_VIEW_CUSTOMIZE	5000

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_COMMAND(ID_VIEW_THUMBNAIL, OnViewThumbnail)
	ON_COMMAND(ID_VIEW_DETAILS, OnViewDetails)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THUMBNAIL, OnUpdateViewThumbnail)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAILS, OnUpdateViewDetails)
	ON_COMMAND(ID_THUMBNAIL_MIN, OnViewThumbnailMin)
	ON_COMMAND(ID_THUMBNAIL_MAX, OnViewThumbnailMax)
	ON_COMMAND(ID_THUMBNAIL_SIZE, OnThumbnailSize)

	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DWMCOMPOSITIONCHANGED, OnDWMCompositionChanged)

	ON_COMMAND(ID_VIEW_STYLE_BLUE, &CMainFrame::OnViewStyleBlue)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STYLE_BLUE, &CMainFrame::OnUpdateViewStyleBlue)
	ON_COMMAND(ID_VIEW_STYLE_BLACK, &CMainFrame::OnViewStyleBlack)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STYLE_BLACK, &CMainFrame::OnUpdateViewStyleBlack)
	ON_COMMAND(ID_VIEW_STYLE_SILVER, &CMainFrame::OnViewStyleSilver)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STYLE_SILVER, &CMainFrame::OnUpdateViewStyleSilver)
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_SYSCOMMAND()
	ON_WM_DROPFILES()
	ON_REGISTERED_MESSAGE(WM_TASKBARBUTTONCREATED, OnTaskbarButtonCreated)

	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOTAL_DURATION, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TIME_LEFT, OnUpdateEnable)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOTAL_SIZE, OnUpdateEnable)

	ON_WM_NCRBUTTONDOWN()
	ON_COMMAND(ID_DISC_INFORMATION, &CMainFrame::OnDiscInformation)
	ON_COMMAND(ID_DISC_QUICK_ERASE, &CMainFrame::OnDiscQuickErase)
	ON_COMMAND(ID_DISC_FULL_ERASE, &CMainFrame::OnDiscFullErase)
	ON_COMMAND(ID_DEBUG_BURN, &CMainFrame::OnDebugBurn)
	ON_COMMAND(ID_DEBUG_TEMP_FOLDER, OnTempFolder)
	ON_COMMAND(ID_DEBUG_MUX, OnDebugMux)
	ON_COMMAND(ID_MAKE_FILE_LIST, OnMakeFileList)
	ON_COMMAND(ID_TRIM_VIDEO, &CMainFrame::OnTrimVideo)
	ON_COMMAND(ID_LOAD_MENU_TEMPLATES, &CMainFrame::OnLoadMenuTemplate)

	ON_COMMAND(ID_VIEW_OPTIONS, &CMainFrame::OnViewOptions)

	ON_COMMAND(ID_HELP_TOPICS, &CMainFrame::OnHelpTopics)
	ON_COMMAND(ID_HELP_REGISTER, &CMainFrame::OnHelpRegister)
	ON_COMMAND(ID_HELP_CHECK_FOR_UPDATES, &CMainFrame::OnHelpCheckForUpdates)

END_MESSAGE_MAP()


// CMainFrame construction/destruction

/*
enum Style
{
	Office2007_LunaBlue,
	Office2007_ObsidianBlack,
	Office2007_Aqua,
	Office2007_Silver
}
*/

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_hWndFocused = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(FWS_ADDTOTITLE, WS_CLIPCHILDREN);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
#ifdef _BD
	CRect reSidePaneDefault(0, 0, 290, 300);
#else
	CRect reSidePaneDefault(0, 0, 250, 300);
#endif
	

	
	//---------------------------------
	// Set toolbar and menu image size:
	//---------------------------------
	CMFCToolBarButton::m_bWrapText = FALSE;
	CMFCToolBar::SetMenuSizes (CSize (22, 22), CSize (16, 16));

	CMFCToolBarComboBoxButton::SetFlatMode ();
	CMFCToolBarComboBoxButton::SetCenterVert ();


	//if (!m_wndCaptionBar.Create(this, IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create caption bar\n");
	//	return -1;
	//}

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndMenuBar.Create (this, AFX_DEFAULT_TOOLBAR_STYLE))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetBorders(0, 2, 0, 2);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);

	UINT uiToolbarHotID = IDB_TOOLBAR_HOT;
	UINT uiToolbarDisID = IDB_TOOLBAR_DISABLED;
	UINT uiToolbarColdID = IDB_TOOLBAR_COLD;
	UINT uiMenuHotID = IDB_MENU_HOTIMAGES;
	UINT uiMenuDisID = 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, uiToolbarColdID, uiMenuHotID, 
		FALSE /* Not locked */, uiToolbarDisID, uiMenuDisID, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~CBRS_GRIPPER);
	m_wndToolBar.SetBorders(1, 2, 1, 2);
	DockPane (&m_wndToolBar);
	m_wndToolBar.SetWindowText("Tool Bar");	

	if (!m_wndStatusBar.Create(this))
	{
		AfxMessageBox("Failed to create Status Bar", MB_ICONERROR);
		return -1; // failed to create
	}
	m_wndStatusBar.Init();

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMSMVisualManager));

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT;
	if (!m_wndSidePane.Create(_T("Side"), this, reSidePaneDefault, FALSE, ID_VIEW_SIDE_PANE, 
		dwStyle, AFX_CBRS_REGULAR_TABS, AFX_CBRS_RESIZE))
	{
		AfxMessageBox("Failed to create Side Pane", MB_ICONERROR);
		return -1; // failed to create
	}

	m_wndSidePane.SetMinSize(CSize(200, 100));
	m_wndSidePane.EnableDocking(CBRS_ALIGN_RIGHT);

	DockPane(&m_wndSidePane);
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	//GetPropertyGridAdvanced()->SetFont(&afxGlobalData.fontRegular);

#ifdef _DEBUG
	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, "Customize...", ID_VIEW_TOOLBAR);
#endif

#ifdef _DVD
	if (!CActivate::Instance()->IsActivated())
	{
		m_wndMessageBar.SetFlatBorder(TRUE);
		m_wndMessageBar.SetMargin(5);
		m_wndMessageBar.EnableButton(FALSE);

		if (!m_wndMessageBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, 0, -1, FALSE))
		{
			TRACE0("Failed to create caption bar\n");
			return -1;
		}

		m_wndMessageBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
		CString strGoPro;
		strGoPro.LoadString(IDS_FREE_VERSION_NOTICE);
		m_wndMessageBar.SetText(strGoPro, CMFCCaptionBar::ALIGN_LEFT);
	}
#endif

	m_dockManager.EnableDockSiteMenu(FALSE);

	CFileList::Instance()->UpdateSummary();
	CFileList::Instance()->SetFocus();

	DragAcceptFiles(TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	WNDCLASS wndclass;
	wndclass.style = CS_DBLCLKS;
	wndclass.lpfnWndProc = ::DefWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = AfxGetInstanceHandle();
	wndclass.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = CONVERTER_CLASS;

	if (!AfxRegisterClass(&wndclass))
		return FALSE;

	cs.lpszClass = CONVERTER_CLASS;
	cs.style |= WS_CLIPCHILDREN;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (CFileList::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	if (CPaneSimple::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

#ifdef _MENU_TEMPLATE_VIEW
	if (m_wndSidePane.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
#endif

	if (CPlayerPane::Instance()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::SetupMemoryBitmapSize (const CSize& sz)
{
	CMSMVisualManager* pManager = DYNAMIC_DOWNCAST (CMSMVisualManager,
		CMFCVisualManager::GetInstance ());

	if (pManager != NULL)
	{
		CRect rtRes (0, 0, sz.cx, 0);

		CRect rt;

		CPane* bars[2] = {&m_wndMenuBar, &m_wndToolBar};

		for (long i = 0; i < 2; i++)
		{
			CPane* pBar = bars [i];

			if (pBar != NULL && pBar->GetSafeHwnd () != NULL)
			{
				pBar->GetWindowRect (rt);
				rtRes.bottom += rt.Height ();
			}
		}

		CDC* pDC = GetDC ();

		pManager->SetupMemoryBitmapSize (pDC, rtRes.Size ());

		ReleaseDC (pDC);
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	SetupMemoryBitmapSize (CSize (cx, cy));

	CFrameWndEx::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndToolBar.m_hWnd))
		m_wndToolBar.Invalidate();
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default

	CFrameWndEx::OnGetMinMaxInfo(lpMMI);

	if(m_wndSidePane.GetSafeHwnd () != NULL)
	{
		CSize szProfile;
		m_wndSidePane.GetMinSize(szProfile);

		CRect rtRect (0, 0, szProfile.cx + 300, 460);

		CalcWindowRect (rtRect, CWnd::adjustBorder);

		lpMMI->ptMinTrackSize.x = rtRect.Width ();
		lpMMI->ptMinTrackSize.y = rtRect.Height ();
	}
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CFrameWndEx::OnEraseBkgnd(pDC);
}

void CMainFrame::OnUpdateEnable(CCmdUI *pCmd)
{
	// TODO: Add your command handler code here

	pCmd->Enable(TRUE);
}

void CMainFrame::OnUpdateProfile(CCmdUI *pCmd)
{
	// TODO: Add your command handler code here

	pCmd->Enable(!CFileList::Instance()->IsConverting());
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMSMVisualManager* pManager = DYNAMIC_DOWNCAST (CMSMVisualManager,
		CMFCVisualManager::GetInstance ());

	if (pManager != NULL)
	{
		pManager->SetParentActive(nState != WA_INACTIVE);
	}

	// Save current focused window.
	if (m_hWndFocused == NULL)
	{
		if (nState == WA_INACTIVE)
		{
			m_hWndFocused = ::GetFocus();
			//#ifdef _DEBUG
			//	if (::IsWindow(m_hWndFocused))
			//	{
			//		TCHAR szClass[256], szCaption[256];
			//		::GetClassName(m_hWndFocused, szClass, 256);
			//		::GetWindowText(m_hWndFocused, szCaption, 256);
			//		TRACE("Save Focus: Class: %s, Caption: %s\n", szClass, szCaption);
			//	}
			//#endif
		}
	}

	CFrameWndEx::OnActivate(nState, pWndOther, bMinimized);

	// Restore focus
	if (!IsIconic())
	{
		if (nState != WA_INACTIVE)
		{
			if (m_hWndFocused != NULL)
			{
				if (::IsWindow(m_hWndFocused) && ::IsChild(m_hWnd, m_hWndFocused))
				{
					::SetFocus(m_hWndFocused);
					//#ifdef _DEBUG
					//	if (::IsWindow(m_hWndFocused))
					//	{
					//		TCHAR szClass[256], szCaption[256];
					//		::GetClassName(m_hWndFocused, szClass, 256);
					//		::GetWindowText(m_hWndFocused, szCaption, 256);
					//		TRACE("Restore Focus: Class: %s, Caption: %s\n", szClass, szCaption);
					//	}
					//#endif
				}

				m_hWndFocused = NULL;
			}
		}
	}
}

void CMainFrame::OnViewThumbnail()
{
	// TODO: Add your command handler code here

	CFileList::Instance()->SetReportView(FALSE);
}

void CMainFrame::OnViewDetails()
{
	// TODO: Add your command handler code here

	CFileList::Instance()->SetReportView(TRUE);
}

void CMainFrame::OnUpdateViewThumbnail(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(!CFileList::Instance()->IsReportView());
}

void CMainFrame::OnUpdateViewDetails(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(CFileList::Instance()->IsReportView());
}

void CMainFrame::OnViewThumbnailMin()
{
	CFileList::Instance()->SetThumbnailSize(MIN_THUMBNAIL_WIDTH, MIN_THUMBNAIL_HEIGHT);

	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndToolBar.GetButton(m_wndToolBar.CommandToIndex(ID_THUMBNAIL_SIZE));
	CFileList::Instance()->UpdateThumbnailColumnWidth();
	UpdateThumbnailSlider();
}

void CMainFrame::OnViewThumbnailMax()
{
	CFileList::Instance()->SetThumbnailSize(MAX_THUMBNAIL_WIDTH, MAX_THUMBNAIL_HEIGHT);
	CFileList::Instance()->UpdateThumbnailColumnWidth();
	UpdateThumbnailSlider();
}

void CMainFrame::UpdateThumbnailSlider()
{
	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndToolBar.GetButton(m_wndToolBar.CommandToIndex(ID_THUMBNAIL_SIZE));
	if (pSlider != NULL)
	{
		pSlider->SetRange(0, MAX_THUMBNAIL_WIDTH - MIN_THUMBNAIL_WIDTH);
		pSlider->SetPos(CFileList::Instance()->GetThumbnailSize().cx - MIN_THUMBNAIL_WIDTH);	
	}
}

void CMainFrame::OnThumbnailSize()
{
	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndToolBar.GetButton(m_wndToolBar.CommandToIndex(ID_THUMBNAIL_SIZE));
	if (pSlider != NULL)
	{
		int width = pSlider->GetPos() + MIN_THUMBNAIL_WIDTH;
		int height = width * 3 / 4;
		CFileList::Instance()->SetThumbnailSize(width, height);
		CFileList::Instance()->UpdateThumbnailColumnWidth();
		UpdateThumbnailSlider();
	}
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);

	// TODO: Add your message handler code here

	TRACE("OnSettingChange(): uFlags=%x\n", uFlags);

	GetPropertyGridAdvanced()->SetFont(&afxGlobalData.fontRegular);

	if (uFlags == SPI_SETNONCLIENTMETRICS || uFlags == SPI_SETWORKAREA || uFlags == SPI_SETICONTITLELOGFONT)
	{
		m_Impl.OnChangeVisualManager();
	}
}

LRESULT CMainFrame::OnDWMCompositionChanged(WPARAM wp, LPARAM lp )
{
	CFrameWndEx::OnDWMCompositionChanged(wp, lp);
	m_Impl.OnChangeVisualManager();
	return 0;
}

void CMainFrame::OnViewStyleBlue()
{
	// TODO: Add your command handler code here

	if (CMFCVisualManagerOffice2007::GetStyle() != CMFCVisualManagerOffice2007::Office2007_LunaBlue)
	{
		((CMSMVisualManager *)CMFCVisualManager::GetInstance())->SetStyleFixed(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
		m_Impl.OnChangeVisualManager();
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_FRAME);
	}
}

void CMainFrame::OnUpdateViewStyleBlue(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(CMFCVisualManagerOffice2007::GetStyle() == CMFCVisualManagerOffice2007::Office2007_LunaBlue);
}

void CMainFrame::OnViewStyleBlack()
{
	// TODO: Add your command handler code here

	if (CMFCVisualManagerOffice2007::GetStyle() != CMFCVisualManagerOffice2007::Office2007_ObsidianBlack)
	{
		((CMSMVisualManager *)CMFCVisualManager::GetInstance())->SetStyleFixed(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
		m_Impl.OnChangeVisualManager();
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_FRAME);
	}
}

void CMainFrame::OnUpdateViewStyleBlack(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(CMFCVisualManagerOffice2007::GetStyle() == CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
}

void CMainFrame::OnViewStyleSilver()
{
	// TODO: Add your command handler code here

	if (CMFCVisualManagerOffice2007::GetStyle() != CMFCVisualManagerOffice2007::Office2007_Silver)
	{
		((CMSMVisualManager *)CMFCVisualManager::GetInstance())->SetStyleFixed(CMFCVisualManagerOffice2007::Office2007_Silver);
		m_Impl.OnChangeVisualManager();
		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_FRAME);
	}
}

void CMainFrame::OnUpdateViewStyleSilver(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(CMFCVisualManagerOffice2007::GetStyle() == CMFCVisualManagerOffice2007::Office2007_Silver);
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	GetPropertyGridAdvanced()->SaveState();
	GetPropertyGridMenu()->SaveState();
	CFrameWndEx::OnClose();
}

BOOL CMainFrame::OnQueryEndSession()
{
	if (!CFrameWndEx::OnQueryEndSession())
		return FALSE;

	// TODO: Add your message handler code here

	if (!QueryExit())
		return FALSE;

	return TRUE;
}

BOOL CMainFrame::QueryExit()
{
	if (CFileList::Instance()->IsConverting())
	{
		if (AfxMessageBox(IDS_CONFIRM_QUIT_WHILE_CONVERTOING, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) != IDYES)
			return FALSE;
	}

	return TRUE;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	if (nID == SC_MINIMIZE)
	{
		if (m_hWndFocused == NULL)
			m_hWndFocused = ::GetFocus();
	}
	else if (nID == SC_CLOSE)
	{
		if (!QueryExit())
			return;
	}

	CFrameWndEx::OnSysCommand(nID, lParam);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	SetActiveWindow();      // activate us first !

	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	std::vector<std::string> list;

	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		list.push_back(szFileName);
	}

	::DragFinish(hDropInfo);

	if (list.size() > 0)
	{
		CFileList::Instance()->AddFiles(list);
	}
	
	// must NOT let the base class handle this message
	// CFrameWndEx::OnDropFiles(hDropInfo);
}

void CMainFrame::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (nHitTest == HTCAPTION)
	{
		int  nDefault = 0;
#ifdef _DEBUG
		nDefault = 1;
#endif
		if (AfxGetApp()->GetProfileInt(_T("Debug"), _T("Enable Test Menu"), nDefault) == 1 && !CFileList::Instance()->IsConverting())
		{
			CMenu menu;
			menu.LoadMenu(IDR_DEBUG);

			CMFCPopupMenu *pMenu = new CMFCPopupMenu();
			pMenu->Create(this, point.x, point.y, menu.Detach());
			CMFCPopupMenu::ActivatePopupMenu(this, pMenu);
			return ;
		}
	}
	CFrameWndEx::OnNcRButtonDown(nHitTest, point);
}

void CMainFrame::OnDebugBurn()
{
	// TODO: Add your command handler code here

	CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
	if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0)
	{
		CString strFolder;
		if (ShellBrowseForFolder(m_hWnd, "Please select the folder to burn:",  COptions::Instance()->m_strTempFolder, strFolder))
		{
			CDlgBurn dlg;
			dlg.m_strUniqueID = (LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel());
			dlg.m_strFolder = strFolder;
			dlg.DoModal();
		}
	}
}

void CMainFrame::OnDiscInformation()
{
	// TODO: Add your command handler code here

	CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
	if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0)
	{
		MediaInfo info;

		if (CxBurner::Instance()->GetMediaInfo((LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel()),info))
		{
			CString strYes, strNo;
			strYes.LoadString(IDS_YES);
			strNo.LoadString(IDS_NO);
			AfxMessageBoxEx
			(
				MB_ICONINFORMATION, 
				IDS_DISC_INFO3,
				CxBurner::Instance()->GetMediaTypeString(info.MediaType), 
				info.isBlank? strYes : strNo
			);

			return;
		}
	}

	AfxMessageBox(IDS_E_GET_DISC_INFO);
}

void CMainFrame::OnDiscQuickErase()
{
	// TODO: Add your command handler code here

	CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
	if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0)
	{
		LPCTSTR lpszUniqueID = (LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel());
		BOOL ret = CxBurner::Instance()->Erase(lpszUniqueID, FALSE, NULL);
		AfxMessageBox(ret ? IDS_QUICK_ERASE_SUCCEEDED : IDS_E_ERASE_DISC);
	}
}

void CMainFrame::OnDiscFullErase()
{
	// TODO: Add your command handler code here

	CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
	if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0)
	{
		LPCTSTR lpszUniqueID = (LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel());
		BOOL ret = CxBurner::Instance()->Erase(lpszUniqueID, TRUE, NULL);
		AfxMessageBox(ret ? IDS_FULLY_ERASE_SUCCEEDED : IDS_E_ERASE_DISC);
	}
}

void CMainFrame::OnTempFolder()
{
	CDlgSelectTempFolder dlg;
	dlg.m_strTempFolder = COptions::Instance()->m_strTempFolder;
	if (dlg.DoModal() == IDOK)
		COptions::Instance()->m_strTempFolder = dlg.m_strTempFolder;
}

void CMainFrame::OnDebugMux()
{
	COptions::Instance()->m_strDiscType = GetDiscTypeComboBox()->GetText();
	CFileList::Instance()->OnTaskCompletedAll();
}

void CMainFrame::OnMakeFileList()
{
	CFileList::Instance()->MakeFileList();
}

void CMainFrame::OnTrimVideo()
{
	CFileList::Instance()->FastTrim();
}


void CMainFrame::OnViewOptions()
{
	// TODO: Add your command handler code here

	COptionsSheet dlg(IDS_OPTIONS, this, COptionsSheet::m_nSelectedPage);
	if (dlg.DoModal() == IDOK)
	{
		CFileList::Instance()->Invalidate(FALSE);
		if (CFileList::Instance()->IsConverting())
			CFileList::Instance()->CheckQueue();
	}
}

void CMainFrame::OnHelpTopics()
{
	CString strURL;
	strURL.LoadString(IDS_HELP_URL);
	ShellExecute(m_hWnd, NULL, strURL, NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnHelpRegister()
{
	if (!CActivate::Instance()->IsActivated())
	{
		CDlgEnterKey dlg;
		if (dlg.DoModal() == IDOK)
		{
		#ifdef _DVD
			if (::IsWindow(m_wndMessageBar.m_hWnd))
				m_wndMessageBar.ShowPane(FALSE, FALSE, FALSE);
		#endif
		}
	}
	else
	{
		CString str;
		GetUserName(str);
	
	#ifdef _DVD
		AfxMessageBoxEx(MB_ICONINFORMATION | MB_OK, IDS_PRO_REGISTERED1, (LPCTSTR)str);
	#else
		AfxMessageBoxEx(MB_ICONINFORMATION | MB_OK, IDS_SOFTWARE_REGISTERED1, (LPCTSTR)str);
	#endif
	}
}

void CMainFrame::OnHelpCheckForUpdates()
{
	CActivate::Instance()->CheckForUpdates(FALSE);
}

LRESULT CMainFrame::OnTaskbarButtonCreated(WPARAM wp, LPARAM lp)
{
	if (!g_spTaskbar)
		g_spTaskbar.CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER);
	return 0;
}

BOOL CMainFrame::OnShowPopupMenu (CMFCPopupMenu* pMenuPopup)
{
#if (defined RETAIL || defined AHS)
	CFrameWndEx::OnShowPopupMenu(pMenuPopup);

	if (pMenuPopup == NULL)
	{
		return TRUE;
	}

	int nIndex = pMenuPopup->GetMenuBar()->CommandToIndex(ID_HELP_CHECK_FOR_UPDATES);
	if (nIndex >= 0)
	{
		pMenuPopup->RemoveItem(nIndex);
	}

	return TRUE;
#else
	return CFrameWndEx::OnShowPopupMenu(pMenuPopup);
#endif
}

void CMainFrame::OnLoadMenuTemplate()
{
#ifdef _RELOAD_MENU_TEMPLATES
	if (!CMenuTemplateManager::Instance()->ReloadMenuTemplates())
		AfxMessageBox(IDS_E_LOAD_MENU_TEMPLATES, MB_ICONERROR | MB_OK);
	else
		GetPropertyGridMenu()->LoadMenuTemplates();
#endif
}

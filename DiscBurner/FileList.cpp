// FileList.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "FileList.h"
#include "Graphics.h"
#include "ProfileManager.h"
#include "MSMVisualManager.h"
#include "FileDialogEx.h"
#include "DlgProgress.h"
#include "MainFrm.h"
#include "Options.h"
#include "EncoderFFmpeg.h"
#include "EncoderBlueRay.h"
#include "EncoderDVDMenu.h"
#include "EncoderDVD.h"
#include "EncoderBDMenu.h"
#include "Burner.h"
#include "DlgBurn.h"
#include "CheckMessageBox.h"
#include "DlgSelectTempFolder.h"
#include "DlgTrim.h"
#include "xmlutils.h"
#include "DlgShutDown.h"
#include "VideoList.h"
#include "DlgAddFolder.h"

#include <sys/stat.h>
#include <PowrProf.h>
#include "PGCAction.h"
#include "BDMenuUtils.h"

#include "..\Activation/Activation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int THUMBNAIL_MARGIN_HORZ = 4;
const int THUMBNAIL_MARGIN_VERT = 4;
const int THUMBNAIL_MARGIN_TEXT	= 3;

enum
{
	//COL_CHECKBOX = 0, 
	COL_THUMBNAIL,
	COL_NAME, 
	COL_DIMENSION, 
	COL_DURATION, 
	COL_STATUS,
	//COL_TIME_ESTIMATED, 
	//COL_TIME_ELAPSED, 
	//COL_TIME_LEFT, 
	COL_TOTALCOLS
};

enum
{
	IMAGE_WAITING = 0,
	IMAGE_CONVERTING,
	IMAGE_COMPLETED,
	IMAGE_FAILED,
	IMAGE_CANCELED,
};

#define IDT_UPDATE		1000
#define UPDATE_INTERVAL	1000

BOOL bSettingThumbnailSize = FALSE;

// CFileList

IMPLEMENT_SINGLETON(CFileList)

CFileList::CFileList()
{
	m_bInit = FALSE;
	m_hAccel = NULL;
	m_bAddingFiles = FALSE;
	m_bConverting = FALSE;
	m_nStartTick = 0;
	m_nPauseTick = 0;
	m_bPaused = FALSE;
	m_bDestroy = FALSE;
	m_bUpdateStatusTimerEnabled = FALSE;
	m_pCurTask = NULL;
	m_bDrawFocusRect = FALSE;
	m_nRunningTasks = 0;
	//TestGetTitleID();
}

CFileList::~CFileList()
{
	CVideoList::ReleaseInstance();
}


BEGIN_MESSAGE_MAP(CFileList, CUltraListCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(ULN_SELCHANGED, OnUlnSelChanged)
	ON_CONTROL_REFLECT(ULN_DBLCLK, OnUlnDblClk)
	ON_REGISTERED_MESSAGE(WM_TASK_ADD, OnTaskAdd)
	ON_REGISTERED_MESSAGE(WM_TASK_COMPLETED, OnTaskCompleted)
	ON_REGISTERED_MESSAGE(WM_TASK_FAILED, OnTaskFailed)
	ON_REGISTERED_MESSAGE(WM_TASK_CANCELED, OnTaskCanceled)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdnItemChanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdnItemChanged)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()

	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_COMMAND(ID_FILE_REMOVE, OnFileRemove)
	ON_COMMAND(ID_FILE_REMOVE_ALL, OnFileRemoveAll)
	ON_COMMAND(ID_FILE_MOVE_UP, OnFileMoveUp)
	ON_COMMAND(ID_FILE_MOVE_DOWN, OnFileMoveDown)
	ON_COMMAND(ID_FILE_CONVERT, OnFileConvert)
	ON_COMMAND(ID_FILE_PAUSE, OnFilePause)
	ON_COMMAND(ID_FILE_STOP, OnFileStop)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_FILE_PREVIEW, OnFilePreview)
	ON_COMMAND(ID_FILE_PLAY, OnFilePlay)
	ON_COMMAND(ID_FILE_TRIM, OnFileTrim)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	ON_UPDATE_COMMAND_UI(ID_FILE_REMOVE, OnUpdateFileRemove)
	ON_UPDATE_COMMAND_UI(ID_FILE_REMOVE_ALL, OnUpdateFileRemoveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVE_UP, OnUpdateFileMoveUp)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVE_DOWN, OnUpdateFileMoveDown)
	ON_UPDATE_COMMAND_UI(ID_FILE_CONVERT, OnUpdateFileConvert)
	ON_UPDATE_COMMAND_UI(ID_FILE_PAUSE, OnUpdateFilePause)
	ON_UPDATE_COMMAND_UI(ID_FILE_STOP, OnUpdateFileStop)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperties)
	ON_UPDATE_COMMAND_UI(ID_FILE_PREVIEW, OnUpdateFilePreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PLAY, OnUpdateFilePlay)
	ON_UPDATE_COMMAND_UI(ID_FILE_TRIM, OnUpdateFileTrim)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_FILE_OPEN_SOURCE_FOLDER, OnFileOpenSourceFolder)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_SOURCE_FOLDER, OnUpdateFileOpenSourceFolder)
	ON_COMMAND(ID_FILE_ADDFOLDER, &CFileList::OnFileAddFolder)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADDFOLDER, &CFileList::OnUpdateFileAddfolder)
END_MESSAGE_MAP()



// CFileList message handlers

void CFileList::Init()
{
	SetItemSpace(5);
	SetIconSpace(15, 15);

	SetThumbnailSize(DEF_THUMBNAIL_WIDTH, DEF_THUMBNAIL_HEIGHT);

	SetReportView(AfxGetApp()->GetProfileInt("Options\\Interface", "View Details", TRUE));

	((CMainFrame *)AfxGetMainWnd())->UpdateThumbnailSlider();

#define INSERT_COLUME(idx, fmt, w)			\
	{											\
	CString str##idx;						\
	str##idx.LoadString(IDS_TASK_##idx);	\
	InsertColumn(idx, str##idx, fmt, w);	\
	}

	//INSERT_COLUME(COL_CHECKBOX, LVCFMT_CENTER, GetSubItemIdealWith(COL_CHECKBOX));
	INSERT_COLUME(COL_THUMBNAIL, LVCFMT_LEFT, GetSubItemIdealWith(COL_THUMBNAIL));
	INSERT_COLUME(COL_NAME, LVCFMT_LEFT, 180);
	INSERT_COLUME(COL_DIMENSION, LVCFMT_CENTER, 80);
	INSERT_COLUME(COL_DURATION, LVCFMT_CENTER, 70);
	INSERT_COLUME(COL_STATUS, LVCFMT_CENTER, 90);
	//INSERT_COLUME(COL_TIME_ESTIMATED, LVCFMT_CENTER, 80);
	//INSERT_COLUME(COL_TIME_ELAPSED, LVCFMT_CENTER, 80);
//	INSERT_COLUME(COL_TIME_LEFT, LVCFMT_CENTER, 80);

	//m_wndHeader.SetRemovable(COL_CHECKBOX, FALSE);
	m_wndHeader.SetRemovable(COL_THUMBNAIL, FALSE);

//	m_wndHeader.LimitItemWidth(COL_CHECKBOX, GetSubItemIdealWith(COL_CHECKBOX), GetSubItemIdealWith(COL_CHECKBOX));
	m_wndHeader.LimitItemWidth(COL_THUMBNAIL, MIN_THUMBNAIL_WIDTH + THUMBNAIL_MARGIN_HORZ * 2, MAX_THUMBNAIL_WIDTH + THUMBNAIL_MARGIN_HORZ * 2);

	SetRegistryKey("Task List");

	RestoreColumnState();

	SetTimer(IDT_UPDATE, UPDATE_INTERVAL, NULL);

	m_hAccel = ::LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_FILE_LIST));

	m_bInit = TRUE;
}

void CFileList::SetThumbnailSize(int width, int height)
{
	ASSERT(width <= MAX_THUMBNAIL_WIDTH && height <= MAX_THUMBNAIL_HEIGHT);

	m_sizeThumbnail.SetSize(width, height);

	SetIconSize(width * 2 + THUMBNAIL_MARGIN_HORZ * 2, height * 2 + THUMBNAIL_MARGIN_VERT * 2 + 
		afxGlobalData.GetTextHeight() + THUMBNAIL_MARGIN_TEXT * 2);

	SetItemHeight(height + THUMBNAIL_MARGIN_VERT * 2);
}

void CFileList::UpdateThumbnailColumnWidth()
{
	GetHeaderCtrl()->SetItemWidth(COL_THUMBNAIL, GetThumbnailSize().cx + THUMBNAIL_MARGIN_HORZ * 2);
}

void CFileList::OnResetColumns()
{
	const int nColAll[] = 
	{	
		//COL_CHECKBOX, 
		COL_THUMBNAIL, 
		COL_NAME, 
		COL_DIMENSION, 
		COL_DURATION, 
		COL_STATUS,
		//COL_TIME_ELAPSED, 
		//COL_TIME_LEFT
	};

	int nCols = sizeof (nColAll) / sizeof (int);

	int va[COL_TOTALCOLS];

	for (int i = 0; i < COL_TOTALCOLS; i++)
		va[i] = i;
	m_wndHeader.SetOrderArray(sizeof (va) / sizeof (int), va);

	memset(va, 0, sizeof (va));
	for (int i = 0; i < nCols; i++)
		va[nColAll[i]] = 1;
	m_wndHeader.SetVisibleArray(sizeof (va) / sizeof (int), va);

	for (int i = 0; i < COL_TOTALCOLS; i++)
	{
		if (m_wndHeader.GetVisible(i))
			m_wndHeader.ResetItemWidth(i);
	}
}

void CFileList::SaveColumnState()
{
	VERIFY(SaveState());
}

void CFileList::RestoreColumnState()
{
	if (!RestoreState())
		OnResetColumns();
}

CTask *CFileList::GetCurTask()
{
	return m_pCurTask;
}

CTask *CFileList::ParseFile(LPCTSTR lpszFile)
{
	if (!::PathFileExists(lpszFile) || ::PathIsDirectory(lpszFile))
		return NULL;

	CLog::Instance()->AppendLog("Add file %s.\r\n",lpszFile);
	CTask *pTask = new CTask(lpszFile);

	if (!pTask->CreateTempFolder())
	{
		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_TEMP_FILE);
		delete pTask;
		return NULL;
	}

	if (!pTask->m_mi.ParseMedia(lpszFile))
	{
		CLog::Instance()->AppendLog("Parse file \"%s\" failed.\r\n", lpszFile);
		delete pTask;
		return NULL;
	}

	CString strMedia = pTask->GetMediaInfo();
	CLog::Instance()->AppendLogNoTime("%s\r\n",strMedia);

	if( !pTask->PrepareThumbnail())
	{
		if(AfxGetApp()->GetProfileInt("Debug", "Failed MessageBox", FALSE))
			AfxMessageBoxEx(MB_ICONERROR | MB_OK, "Get thumbnail failed!");
		CLog::Instance()->AppendLog("Get thumbnail \"%s\" failed.\r\n",lpszFile);
		delete pTask;
		return NULL;
	}

	if(pTask->m_mi.m_fDuration<0.1)
	{
		CLog::Instance()->AppendLog("The duration(%f) of \"%s\" can not be determined.\r\n", pTask->m_mi.m_fDuration,lpszFile);
		delete pTask;
		return NULL;
	}

#ifdef _DEBUG
	pTask->m_strButtonText = pTask->m_strSourceFile;
	::PathStripPath(pTask->m_strButtonText.GetBuffer());
	pTask->m_strButtonText.ReleaseBuffer();
	::PathRemoveExtension(pTask->m_strButtonText.GetBuffer());
	pTask->m_strButtonText.ReleaseBuffer();

	if(AfxGetApp()->GetProfileInt("Debug", "Mux From Local", FALSE))
	{
		taskAtom atom;
		CProfileItem *pItem;
		CProfileManager::Instance()->GetCurrentProfile(NULL, &pItem);
		atom.ext = pItem->m_strExt;
		if(atom.ext.Find(".")==-1)
			atom.ext.Insert(0,".");

		CString strOutputName = pTask->m_strSourceFile;
		::PathStripPath(strOutputName.GetBuffer());
		strOutputName.ReleaseBuffer();
		::PathRemoveExtension(strOutputName.GetBuffer());
		strOutputName.ReleaseBuffer();

		CString strFolder = COptions::Instance()->m_strTempFolder;
		CString strUnique;
		strUnique.Format("%s\\%s%s", strFolder, strOutputName, atom.ext);
		atom.outfile =  strUnique;
		atom.novideo = FALSE;
		pTask->m_Atoms.push_back(atom);

#ifdef _BD
		taskAtom atom2;
		strUnique.Format("%s\\%s%s", strFolder, strOutputName, ".ac3");
		atom2.outfile =  strUnique;
		atom2.novideo = TRUE;
		pTask->m_Atoms.push_back(atom2);
#endif
	}
#endif

	return pTask;
}

int CFileList::AddTask(CTask *pTask)
{
	int index = InsertItem(GetItemCount(), (DWORD_PTR)pTask);
	if (index >= 0)
	{
		SetCheck(index, TRUE);
		SetFocusItem(index);
		SetAnchorItem(index);

		pTask->Lock();
		pTask->m_nStatus = (m_bConverting ? CONV_STATUS_WAITING : CONV_STATUS_NONE);
#ifdef _DEBUG
		if(AfxGetApp()->GetProfileInt("Debug", "Mux From Local", FALSE))
			pTask->m_nStatus = CONV_STATUS_COMPLETED;
#endif
		pTask->m_nUpdate = UPDATE_ALL;
		pTask->Unlock();

		UpdateStatus();

		SelectItem(index, TRUE);

		EnsureVisible(index);

		UpdateSummary();

		if (m_bConverting)
			CheckQueue();
	}
	CSubjectManager::Instance()->GetSubject(SUB_FILE_LIST_CHANGED)->Notify(NULL);
	return index;
}

void CFileList::DeleteSelected()
{
	if (GetSelectedCount() > 0)
	{
		BOOL bConverting = FALSE;

		SetRedraw(FALSE);

		for (int i = GetItemCount() - 1; i >= 0; i--)
		{
			if (IsItemSelected(i))
			{
				CTask *pTask = (CTask *)GetItemData(i);
				Delete(i);
			}
		}

		SetRedraw(TRUE);
		Invalidate(FALSE);

		if (m_bConverting)
			CheckQueue();
	}
}

void CFileList::Start(CTask *pTask)
{
	BOOL bCanStart = FALSE;
	pTask->Lock();
	bCanStart = (pTask->m_nStatus == CONV_STATUS_WAITING);
	pTask->Unlock();

	CLog::Instance()->AppendLog("StartTask: %s. bCanStart = %d\r\n", pTask->m_strSourceFile, bCanStart);

	if (bCanStart)
	{
		pTask->m_nStartTick = 0;
		pTask->m_fEncodedTime = 0.0;
		pTask->m_nStatus = CONV_STATUS_CONVERTING;

		//pTask->ClearOldLog();
		//pTask->ClearNewLog();
		pTask->m_nUpdate |= (UPDATE_STATUS | UPDATE_LOG);

		m_nRunningTasks++;

		if (!pTask->PrepareConversion())
		{
			CLog::Instance()->AppendLog("Error: PrepareConversion() failed.\r\n");
			pTask->m_nStatus = CONV_STATUS_FAILED;
			PostMessage(WM_TASK_FAILED, 0, (LPARAM)pTask);
			return;
		}

		pTask->m_bCanceled = FALSE;
		pTask->m_bPaused = FALSE;

		UpdateStatus();

		CONV_PROC_PARAM param;
		param.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		param.pTask = pTask;

		CLog::Instance()->AppendLog("CreateThread: CTask::ConvThread()...\r\n");

		DWORD dwId;
		pTask->m_hConvThread = ::CreateThread(NULL, 0, pTask->m_lpfnConvProc, &param, 0, &dwId);
		if (pTask->m_hConvThread != NULL)
			::WaitForSingleObject(param.hEvent, INFINITE);

		CLog::Instance()->AppendLog("CreateThread: CTask::ConvThread() created.\r\n");

		::CloseHandle(param.hEvent);
	}
}

void CFileList::Start(int index)
{
	CTask *pTask = (CTask *)GetItemData(index);
	Start(pTask);
}

void CFileList::CheckQueue()
{
	ASSERT(m_bConverting);
	if (!m_bConverting)
		return;

	CLog::Instance()->AppendLog("ChechQueue(): m_nRunningTasks = %d\r\n", m_nRunningTasks);

	if (m_nRunningTasks >= COptions::Instance()->m_nMaxConcurrentTasks)
		return;

	int nWaitingTasks = 0;

	for (int index = 0; index < GetItemCount(); index++)
	{
		CTask *pTask = (CTask *)GetItemData(index);
		ASSERT(pTask != NULL);
		if (pTask != NULL)
		{
			if (GetCheck(index))
			{
				BOOL bWaiting = FALSE;

				pTask->Lock();
				if (pTask->m_nStatus == CONV_STATUS_WAITING)
				{
					bWaiting = TRUE;
					nWaitingTasks++;
				}
				pTask->Unlock();

				if (!m_bPaused)
				{
					if (bWaiting)
					{
						Start(pTask);
						if (m_nRunningTasks >= COptions::Instance()->m_nMaxConcurrentTasks)
							return;
					}
				}
			}
		}
	}

	if (m_nRunningTasks == 0 && nWaitingTasks == 0)
	{
		m_bConverting = FALSE;
		m_bPaused = FALSE;
		m_nStartTick = 0;
		m_nPauseTick = 0;

		GetHeaderCtrl()->EnableCheckBox(TRUE);
		GetStatusBar()->UpdateTimeLeft(-1);
		GetStatusBar()->SetConvertingStatus(CMSMStatusBar::STATUS_IDLE);

		if (g_spTaskbar)
			g_spTaskbar->SetProgressState(AfxGetMainWnd()->GetSafeHwnd(), TBPF_NOPROGRESS);

		OnTaskCompletedAll();
	}
}

void CFileList::QueueAll()
{
	int nQueued = 0;

	for (int i = 0; i < GetItemCount(); i++)
	{
		CTask *pTask = (CTask *)GetItemData(i);
		if (GetCheck(i))
		{
			pTask->Lock();
			BOOL bCanQueue = (pTask->m_nStatus != CONV_STATUS_WAITING && pTask->m_nStatus != CONV_STATUS_CONVERTING);
			pTask->Unlock();

			if  (bCanQueue)
			{
				pTask->m_nStatus = CONV_STATUS_WAITING;
				pTask->m_nUpdate |= UPDATE_STATUS;
				nQueued++;
			}
		}
		else
		{
#if (defined _BD || defined _DVD)
			pTask->m_nStatus = CONV_STATUS_NONE;
			pTask->m_nUpdate |= UPDATE_STATUS;
#endif
		}
	}

	if (nQueued > 0)
	{
		m_bConverting = TRUE;
		m_bPaused = FALSE;
		m_nStartTick = ::GetTickCountEx();
		m_nPauseTick = 0;

		GetHeaderCtrl()->EnableCheckBox(FALSE);
		GetStatusBar()->SetConvertingStatus(CMSMStatusBar::STATUS_CONVERTING);

		if (g_spTaskbar)
			g_spTaskbar->SetProgressState(AfxGetMainWnd()->GetSafeHwnd(), TBPF_NORMAL);

		UpdateStatus();

		CheckQueue();
	}
}

void CFileList::PauseAll()
{
	if (GetItemCount() > 0)
	{
		m_bPaused = TRUE;

		m_nPauseTick = ::GetTickCountEx();

		GetStatusBar()->SetConvertingStatus(CMSMStatusBar::STATUS_PAUSED);

		if (g_spTaskbar)
			g_spTaskbar->SetProgressState(AfxGetMainWnd()->GetSafeHwnd(), TBPF_PAUSED);

		for (int i = 0; i < GetItemCount(); i++)
		{
			CTask *pTask = (CTask *)GetItemData(i);
			pTask->Lock();
			BOOL bCanPause = (pTask->m_nStatus == CONV_STATUS_CONVERTING && !pTask->m_bPaused);
			pTask->Unlock();

			if (bCanPause)
				pTask->Pause();
		}
	}
}

void CFileList::ResumeAll()
{
	if (GetItemCount() > 0)
	{
		for (int i = 0; i < GetItemCount(); i++)
		{
			CTask *pTask = (CTask *)GetItemData(i);
			pTask->Lock();
			BOOL bCanResume = (pTask->m_nStatus == CONV_STATUS_CONVERTING && pTask->m_bPaused);
			pTask->Unlock();

			if (bCanResume)
				pTask->Resume();
		}

		m_bPaused = FALSE;

		if (m_nStartTick > 0 && m_nPauseTick > 0)
		{
			m_nStartTick += (::GetTickCountEx() - m_nPauseTick);
			ASSERT(m_nStartTick > 0);
		}
		m_nPauseTick = 0;

		CheckQueue();

		GetStatusBar()->SetConvertingStatus(CMSMStatusBar::STATUS_CONVERTING);

		if (g_spTaskbar)
			g_spTaskbar->SetProgressState(AfxGetMainWnd()->GetSafeHwnd(), TBPF_NORMAL);
	}
}

void CFileList::StopAll()
{
	for (int i = 0; i < GetItemCount(); i++)
		Stop(i);

	m_bConverting = FALSE;
	m_bPaused = FALSE;

	// The status bar may be destroyed already.
	if (!m_bDestroy)
	{
		GetHeaderCtrl()->EnableCheckBox(TRUE);
		GetStatusBar()->UpdateTimeLeft(-1);
		GetStatusBar()->SetConvertingStatus(CMSMStatusBar::STATUS_IDLE);

		if (g_spTaskbar)
			g_spTaskbar->SetProgressState(AfxGetMainWnd()->GetSafeHwnd(), TBPF_NOPROGRESS);
	}
}

void CFileList::Stop(int index)
{
	CTask *pTask = (CTask *)GetItemData(index);

	pTask->Lock();
	if (pTask->m_nStatus == CONV_STATUS_WAITING)
	{
		pTask->m_nStatus = CONV_STATUS_NONE;
		pTask->m_nUpdate |= UPDATE_STATUS;
		pTask->Unlock();
		return;
	}

	BOOL bCanResume = (pTask->m_nStatus == CONV_STATUS_CONVERTING && pTask->m_bPaused);
	BOOL bCanStop = (pTask->m_nStatus == CONV_STATUS_CONVERTING);

	pTask->Unlock();

	if (bCanStop)
	{
		if (m_bPaused && bCanResume)
			pTask->Resume();

		pTask->Cancel();
	}
}

void CFileList::Delete(int index,BOOL bNotify)
{
	Stop(index);
	CTask *pTask = (CTask *)GetItemData(index);

	// Must process these messages first before delete CTask object:
	// WM_TASK_COMPLETED
	// WM_TASK_FAILED
	// WM_TASK_CANCELED

	DoEvents();

	DeleteItem(index);

	// Note: CTask::ConvProc() will PostMessage(WM_TASK_CANCELED), but when this message is received, 
	// the CTask object has been deleted already.
	delete pTask;
	if(bNotify)
		CSubjectManager::Instance()->GetSubject(SUB_FILE_LIST_CHANGED)->Notify(NULL);

	UpdateSummary();
}

void CFileList::OpenSelected()
{
}

void CFileList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

	if (pWnd != this)
		return;

	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
		pMainWnd->IsWindowVisible())
	{
		AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
			WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
		pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
			(WPARAM)TRUE, 0, TRUE, TRUE);
	}

	UINT nMenu = IDR_FILE_LIST;

	CMenu menu;
	if (menu.LoadMenu(nMenu))
	{
		CMenu *pPopup = menu.GetSubMenu(0);
		if (pPopup != NULL)
		{
			//if (GetSelectedCount() == 1)
			//{
			//	int index = GetFirstSelectedItem();
			//	if (index >= 0)
			//	{
			//		CTask *pTask = (CTask *)GetItemData(index);

			//		switch (pTask->m_nStatus)
			//		{
			//		default:
			//			ASSERT(FALSE);
			//		}
			//	}
			//}

			if (point.x == -1 && point.y == -1)
			{
				point.x = 0;
				point.y = 0;
				ClientToScreen(&point);
			}

			CMFCPopupMenu *pMenu = new CMFCPopupMenu();
			pMenu->Create(this, point.x, point.y, pPopup->Detach());
			CMFCPopupMenu::ActivatePopupMenu(NULL, pMenu);
		}
	}
}

void CFileList::UpdateStatus()
{
	LONGLONG nCurTick = ::GetTickCountEx();

	double fTotalDuration = 0.0;
	double fTotalEncodedTime = 0.0;

#ifdef _DVD
	BOOL bFunctionLimited = FALSE;
#else
	BOOL bFunctionLimited = !g_bIsActivated;
#endif

	for (int index = 0; index < GetItemCount(); index++)
	{
		CTask *pTask = (CTask *)GetItemData(index);
		ASSERT(pTask != NULL);
		if (pTask != NULL)
		{
			BOOL bChecked = GetCheck(index);

			pTask->Lock();

			double fDurationFull = (pTask->m_bTrimmed ? pTask->m_fTrimEnd - pTask->m_fTrimStart : pTask->m_mi.m_fDuration);
			double fDurationLimited = min(fDurationFull, TRIAL_TIME_LIMIT);

			if (m_bConverting && !m_bPaused)
			{
				// always update time elapsed/left and speed.
				if (pTask->m_nStatus == CONV_STATUS_CONVERTING)
				{
					CString strEstimated;
					CString strElasped;
					CString strLeft;

					if (pTask->m_nStartTick != 0)
					{
						fTotalEncodedTime += pTask->m_fEncodedTime;

						int nElasped = (int)((nCurTick - pTask->m_nStartTick) / 1000);
						ASSERT(nElasped >= 0);

						// elapsed
						int h = int(nElasped / 3600);
						int m = int(nElasped / 60 % 60);
						int s = int(nElasped % 60);
						strElasped.Format("%02d:%02d:%02d", h, m ,s);

						if (nElasped >= 5 && pTask->m_fEncodedTime > 5.0)
						{
							//estimated
							int nEstimated = (int)(nElasped * (bFunctionLimited ? fDurationLimited : fDurationFull) / pTask->m_fEncodedTime);
							h = int(nEstimated / 3600);
							m = int(nEstimated / 60 % 60);
							s = int(nEstimated % 60);
							strEstimated.Format("%02d:%02d:%02d", h, m ,s);

							// left
							int nLeft = nEstimated - nElasped;
							h = int(nLeft / 3600);
							m = int(nLeft / 60 % 60);
							s = int(nLeft % 60);
							strLeft.Format("%02d:%02d:%02d", h, m ,s);
						}
						else
						{
							strEstimated.LoadString(IDS_ESTIMATING);
							strLeft.LoadString(IDS_ESTIMATING);
						}
					}

//					SetItemText(index, COL_TIME_ESTIMATED, strEstimated);
//					SetItemText(index, COL_TIME_ELAPSED, strElasped);
//					SetItemText(index, COL_TIME_LEFT, strLeft);
				}
				else if (pTask->m_nStatus == CONV_STATUS_COMPLETED)
				{
					fTotalEncodedTime += (bFunctionLimited ? fDurationLimited : fDurationFull);
				}

				// Total duration includes checked, converting and completed task.
				if (bChecked || pTask->m_nStatus == CONV_STATUS_CONVERTING || pTask->m_nStatus == CONV_STATUS_COMPLETED)
					fTotalDuration += (bFunctionLimited ? fDurationLimited : fDurationFull);
			}

			if (pTask->m_nUpdate & UPDATE_SOURCE_FILE)
			{
				SetItemText(index, COL_NAME, FileStripPath(pTask->m_strSourceFile));
				pTask->m_nUpdate &= ~UPDATE_SOURCE_FILE;
			}

			if (pTask->m_nUpdate & UPDATE_STATUS)
			{
				CString strStatus;
				BOOL bChangeStatus = TRUE;
				switch (pTask->m_nStatus)
				{
				case CONV_STATUS_WAITING:
					strStatus.LoadString(IDS_WAITING);
					break;

				case CONV_STATUS_CONVERTING:
					strStatus.Format("%d%%", pTask->m_mi.m_fDuration > 1.0 ? (int)round((float)pTask->m_fEncodedTime / (bFunctionLimited ? fDurationLimited : fDurationFull) * 100.0) : 0);
					bChangeStatus = m_bConverting && !m_bPaused;
					break;

				case CONV_STATUS_COMPLETED:
					strStatus.LoadString(IDS_COMPLETED);;
					break;

				case CONV_STATUS_CANCELED:
					strStatus.LoadString(IDS_CANCELED);
					break;

				case CONV_STATUS_FAILED:
					strStatus.LoadString(IDS_FAILED);
					break;

				default:
					strStatus.Empty();
					break;
				};
                //修改的主要目的是：暂停转换后，缓冲区中的数据信息读取后不在界面上显示。
				if (bChangeStatus)
					SetItemText(index, COL_STATUS, strStatus);

				if (pTask->m_nStatus != CONV_STATUS_CONVERTING) 
				{
//					SetItemText(index, COL_TIME_ESTIMATED, "");
//					SetItemText(index, COL_TIME_LEFT, "");
				}

				pTask->m_nUpdate &= ~UPDATE_STATUS;
			}

			if (pTask->m_nUpdate & UPDATE_DIMENSION)
			{
				CString strDimension;
				if (pTask->m_mi.m_nWidth > 0 && pTask->m_mi.m_nHeight > 0)
					strDimension.Format("%d x %d", pTask->m_mi.m_nWidth, pTask->m_mi.m_nHeight);
				else
					strDimension.Format("%s", "-");
				SetItemText(index, COL_DIMENSION, strDimension);
				pTask->m_nUpdate &= ~UPDATE_DIMENSION;
			}

			if (pTask->m_nUpdate & UPDATE_DURATION)
			{
				CString strDuration;
				strDuration = FormatTime(pTask->m_mi.m_fDuration + 0.5, 0, FALSE);
				SetItemText(index, COL_DURATION, strDuration);
				pTask->m_nUpdate &= ~UPDATE_DURATION;
			}

			pTask->Unlock();
		}
	}

	if (m_bConverting && !m_bPaused)
	{
		if (fTotalDuration > FLT_EPSILON)
		{
			double fProgress = fTotalEncodedTime * 100 / fTotalDuration;
			SetConversionProgress((int)(fProgress + 0.5));

			int nTotalLeft = -1;

			int nTotalElasped = (int)((nCurTick - m_nStartTick) / 1000);
			ASSERT(nTotalElasped >= 0);

			// elapsed
			if (nTotalElasped >= 5 && fTotalEncodedTime > 5.0)
			{
				//estimated
				int nTotalEstimated = (int)(nTotalElasped * fTotalDuration / fTotalEncodedTime);
				// left
				nTotalLeft = nTotalEstimated - nTotalElasped;
			}

			CMainFrame *pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			pFrame->m_wndStatusBar.UpdateTimeLeft(nTotalLeft);
		}
	}

	CMFCPropertyGridCtrl *pGridCtrl[] = 
	{
		GetPropertyGridAdvanced(), 
		GetPropertyGridMenu(),
	};

	for (int i = 0; i < sizeof(pGridCtrl) / sizeof(CMFCPropertyGridCtrl *); i++)
	{
		BOOL bChangeEnabled = FALSE;
		if (pGridCtrl[i]->IsWindowEnabled() && m_bConverting)
		{
			bChangeEnabled = TRUE;
		}
		else if (!pGridCtrl[i]->IsWindowEnabled() && !m_bConverting)
		{
			bChangeEnabled = TRUE;
		}

		if (bChangeEnabled)
		{
			COLORREF clrBackground;
			COLORREF clrText;
			COLORREF clrGroupBackground;
			COLORREF clrGroupText;
			COLORREF clrDescriptionBackground;
			COLORREF clrDescriptionText;
			COLORREF clrLine;

			pGridCtrl[i]->GetCustomColors(
				clrBackground,
				clrText,
				clrGroupBackground,
				clrGroupText,
				clrDescriptionBackground,
				clrDescriptionText,
				clrLine);

			pGridCtrl[i]->SetCustomColors(
				clrBackground,
				m_bConverting ? afxGlobalData.clrGrayedText : afxGlobalData.clrBtnText,
				clrGroupBackground,
				clrGroupText,
				clrDescriptionBackground,
				clrDescriptionText,
				clrLine);

			pGridCtrl[i]->EnableWindow(!m_bConverting);
			pGridCtrl[i]->Invalidate();
		}
	}
}

double CFileList::GetTotalDuration()
{
	double fTotalDuration = 0.0;

	for (int i = 0; i < GetItemCount(); i++)
	{
		CTask *pTask = (CTask *)GetItemData(i);

		if (GetCheck(i))
		{
			if (pTask->m_bTrimmed)
				fTotalDuration += (pTask->m_fTrimEnd - pTask->m_fTrimStart);
			else
				fTotalDuration += pTask->m_mi.m_fDuration;
		}
	}

	return fTotalDuration;
}

void CFileList::UpdateSummary()
{
	if (!m_bDestroy)
		GetStatusBar()->UpdateSummary();
}

void CFileList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	static BOOL bOnTimer = FALSE;

	if (nIDEvent == IDT_UPDATE && !bOnTimer)
	{
		//TRACE("Enter CFileList::OnTimer()\n");

		//// Display power off will cause Matroska Splitter stopping working.
		//if (m_bAddingFiles || m_bConverting)
		//{
		//	::SetThreadExecutionState(ES_DISPLAY_REQUIRED);
		//	CLog::Instance()->AppendLog("SetThreadExecutionState(ES_DISPLAY_REQUIRED);\r\n");
		//}

		bOnTimer = TRUE;

		if (m_bUpdateStatusTimerEnabled)
		{
			UpdateStatus();
		}

		bOnTimer = FALSE;

		//TRACE("Exit CFileList::OnTimer()\n");

		return;
	}

	CUltraListCtrl::OnTimer(nIDEvent);
}

BOOL CFileList::CanAddFiles()
{
	return m_bInit && !m_bDestroy;
}

void CFileList::OnDestroy()
{
	// TODO: Add your message handler code here

	TRACE("CFileList::OnDestroy()\n\n");

	// prevent adding files
	m_bDestroy = TRUE;

	KillTimer(IDT_UPDATE);

	// stop all running tasks, but remember the running state.
	StopAll();

	while (GetItemCount() > 0)
		Delete(0,FALSE);

	AfxGetApp()->WriteProfileInt("Options\\Interface", "View Details", IsReportView());

	SaveColumnState();

	CUltraListCtrl::OnDestroy();
}

void CFileList::OnUlnSelChanged()
{
	if (!m_bDestroy)
	{
		int index = GetFirstSelectedItem();
		if (index >= 0)
			m_pCurTask = (CTask *)GetItemData(index);
		else
			m_pCurTask = NULL;

		CSubjectManager::Instance()->GetSubject(SUB_FILE_SEL_CHANGED)->Notify(NULL);

	}
}

void CFileList::OnUlnDblClk()
{
	// TODO: Add your control notification handler code here

	switch (COptions::Instance()->m_nDoubleClickAction)
	{
	case DCA_DO_NOTHING:
		break;
	case DCA_PREVIEW:
		OnFilePreview();
		break;
	case DCA_PLAY:
		OnFilePlay();
		break;
	case DCA_VIEW_PROPERTIES:
		OnFileProperties();
		break;
	}
}

LRESULT CFileList::OnTaskAdd(WPARAM wParam, LPARAM lParam)
{
	return AddTask((CTask *)lParam);
}

LRESULT CFileList::OnTaskCompleted(WPARAM wParam, LPARAM lParam)
{
	CTask *pTask = (CTask *)lParam;

	CLog::Instance()->AppendLog("OnTaskCompleted: %s\r\n", pTask->m_strSourceFile);

	m_nRunningTasks--;

	pTask->m_nUpdate |= UPDATE_STATUS;
	UpdateStatus();

	if (m_bConverting)
		CheckQueue();

	return 0;
}

LRESULT CFileList::OnTaskFailed(WPARAM wParam, LPARAM lParam)
{
	CTask *pTask = (CTask *)lParam;

	CLog::Instance()->AppendLog("OnTaskFailed: %s\r\n", pTask->m_strSourceFile);

	m_nRunningTasks--;

	if (m_bConverting)
		CheckQueue();

	return 0;
}

LRESULT CFileList::OnTaskCanceled(WPARAM wParam, LPARAM lParam)
{
	// Note: When deleting a task, CTask::ConvProc() will PostMessage(WM_TASK_CANCELED), but when 
	// this message is received, the CTask object has been deleted already. So do not use the CTask 
	// object here.

	//CTask *pTask = (CTask *)lParam;
	//CLog::Instance()->AppendLog("OnTaskCanceled: %s\r\n", pTask->m_strSourceFile);

	CLog::Instance()->AppendLog("OnTaskCanceled()\r\n");

	m_nRunningTasks--;

	if (m_bConverting)
		CheckQueue();

	return 0;
}

void CFileList::DrawThumbnail(CDC *pDC, int nItem, const LPRECT pRect)
{
	CTask *pTask = (CTask *)GetItemData(nItem);

	CBitmap *pBitmap = NULL;
	if (pTask->m_bmpNonBlankFrame.m_hObject != NULL)
	{
		pBitmap = &pTask->m_bmpNonBlankFrame;
	}
	else
	{
		pBitmap = &CTask::m_bmpAudio;
	}
	CRect rc = *pRect;

	rc.InflateRect(-THUMBNAIL_MARGIN_HORZ, -THUMBNAIL_MARGIN_VERT);

	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = dcMem.SelectObject(pBitmap);

	BITMAP bm;
	pBitmap->GetBitmap(&bm);

	pDC->SetStretchBltMode(STRETCH_HALFTONE);

	if (IsReportView())
	{
		pDC->StretchBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	else
	{
		CRect rcThumb = rc;
		rcThumb.bottom -= (afxGlobalData.GetTextHeight() + THUMBNAIL_MARGIN_TEXT * 2);
		if (pTask->m_bmpNonBlankFrame.m_hObject != NULL || 
			bm.bmWidth >= rcThumb.Width() || bm.bmHeight >= rcThumb.Height())
		{
			pDC->StretchBlt(rcThumb.left, rcThumb.top, rcThumb.Width(), rcThumb.Height(), 
				&dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else
		{
			pDC->FillRect(&rcThumb, CBrush::FromHandle(GetStockBrush(WHITE_BRUSH)));
			pDC->BitBlt(rcThumb.left + (rcThumb.Width() - bm.bmWidth) / 2, rcThumb.top + (rcThumb.Height() - bm.bmHeight) / 2, bm.bmWidth, bm.bmHeight, 
				&dcMem, 0, 0, SRCCOPY);
		}

		if (pTask->m_nStatus == CONV_STATUS_CONVERTING)
		{
			CRect rcProgress = rc;
			rcProgress.top = rcProgress.bottom - (afxGlobalData.GetTextHeight() + THUMBNAIL_MARGIN_TEXT * 2) + THUMBNAIL_MARGIN_VERT / 2;
			rcProgress.InflateRect(0, -THUMBNAIL_MARGIN_TEXT);
			DrawProgress(pDC, nItem, &rcProgress);
		}
		else
		{
			CString strText = GetItemText(nItem, COL_NAME);
			if (strText.GetLength() > 0)
			{
				CRect rcText = rc;
				rcText.top = rcText.bottom - (afxGlobalData.GetTextHeight() + THUMBNAIL_MARGIN_TEXT * 2) + THUMBNAIL_MARGIN_VERT / 2;
				rcText.left += THUMBNAIL_MARGIN_TEXT;
				pDC->DrawText(strText, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
		}
	}

	dcMem.SelectObject(pOldBmp);
}

void CFileList::DrawProgress(CDC *pDC, int nItem, const LPRECT pRect)
{
	CRect rc = *pRect;
	BOOL bSelected = IsItemSelected(nItem);
	BOOL bFocusWindow = (::GetFocus() == m_hWnd);

	rc.InflateRect(-2, -(rc.Height() - (int)MIN_THUMBNAIL_HEIGHT) / 2);
	if (rc.Width() >= 2)
	{
		int nProgress = -1;
		CString str = GetItemText(nItem, COL_STATUS);
		int nLength = str.GetLength();
		if (nLength > 0 && str[nLength - 1] == '%')
			nProgress = _ttoi(str);

		CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = pDC->SelectObject(&pen);
		pDC->RoundRect(&rc, CPoint(3, 3));
		pDC->SelectObject(pOldPen);

		rc.InflateRect(-2, -2);

		int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		int clrTextColor = pDC->GetTextColor();

		CRect rc1 = rc;
		if (nProgress >= 0)
			rc1.right = (int)(rc.left + (float)rc.Width() * nProgress / 100);

		if (nProgress >= 0)
		{
			CTask *pTask = (CTask *)GetItemData(nItem);

			if (pTask->m_nStatus == CONV_STATUS_CONVERTING)
			{
				if (rc1.Width() > 0)
				{
					CMSMVisualManager *pManager = (CMSMVisualManager *)CMFCVisualManager::GetInstance();
					pManager->m_ctrlMenuHighlighted[0].Draw(pDC, rc1);
				}

				pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				pDC->DrawText(str, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}

		//if (nProgress >= 0)
		//{
		//	CRect rc2 = rc;
		//	rc2.left = rc1.right;
		//	if (rc2.Width() > 0)
		//	{
		//		CRgn rgn2;
		//		rgn2.CreateRectRgnIndirect(&rc2);
		//		pDC->SelectClipRgn(&rgn2, RGN_COPY);
		//		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		//		pDC->DrawText(str, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		//	}
		//}

		//pDC->SelectClipRgn(NULL);

		pDC->SetBkMode(nOldBkMode);
		pDC->SetTextColor(clrTextColor);
	}
}

void CFileList::DrawItem(CDC *pDC, int index, const CRect *pRect)
{
	//TRACE("DrawItem(%d)\n", index);

	BOOL bSelected = IsItemSelected(index);
	BOOL bFocused = (GetFocusItem() == index);
	BOOL bHover = (GetHoverItem() == index);
	BOOL bFocusWindow = (::GetFocus() == m_hWnd);

	CRect rcItem = *pRect;

	COLORREF clrBack = ::GetSysColor(COLOR_WINDOW);

	if (bSelected || bHover)
	{
		CDC dcMem;
		dcMem.CreateCompatibleDC(pDC);

		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC, rcItem.Width(), rcItem.Height());
		CBitmap *pOldBitmap = dcMem.SelectObject(&bitmap);

		CRect rcFill = *pRect;
		rcFill.OffsetRect(-pRect->left, -pRect->top);

		dcMem.FillRect(&rcFill, &CBrush(clrBack));

		rcFill.InflateRect(-1, -1);
		CBrush brFill(::GetSysColor(bFocusWindow || bHover ? COLOR_HIGHLIGHT : COLOR_3DFACE));
		dcMem.FillRect(&rcFill, &brFill);

		rcFill.InflateRect(1, 1);
		CRgn rgn;
		rgn.CreateRoundRectRgn(rcFill.left, rcFill.top, rcFill.right + 1, rcFill.bottom + 1, 2, 2);

		CBrush brFrame(ChangeVValue(GetSysColor(COLOR_HIGHLIGHT), 0.7));
		dcMem.FrameRgn(&rgn, &brFrame, 1, 1);

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = 0;

		if (bHover && bSelected)
			bf.SourceConstantAlpha = 144;
		else if (bSelected)
			bf.SourceConstantAlpha = 80;
		else
			bf.SourceConstantAlpha = 16;

		AlphaBlend(*pDC, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), 
			dcMem, 0, 0, rcFill.Width(), rcFill.Height(), bf);

		dcMem.SelectObject(pOldBitmap);
	}

	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = &afxGlobalData.fontRegular;

	CFont *pOldFont = pDC->SelectObject(pFont);
	COLORREF clrTextOld = pDC->SetTextColor(::GetSysColor(GetCheck(index) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
	pDC->SetBkMode(TRANSPARENT);

	CHeaderCtrlEx *pHeader = GetHeaderCtrl();
	for (int i = 0; i < pHeader->GetItemCount(); i++)
	{
	/*	if (i == COL_CHECKBOX)
		{
			if (IsReportView())
			{
				CRect rcCheck;
				GetCheckBoxRect(index, &rcCheck);
				CMFCVisualManager *pManager = CMFCVisualManager::GetInstance();
				pManager->OnDrawCheckBox(pDC, rcCheck, FALSE, GetCheck(index) ? 1 : 0, !m_bConverting);
			}
		}
		else*/ if (i == COL_THUMBNAIL)
		{
			CRect rcThumb;
			if (IsReportView())
			{
				GetSubItemRect(index, COL_THUMBNAIL, &rcThumb);
				DrawThumbnail(pDC, index, &rcThumb);
			}
			else
			{
				GetItemRect(index, &rcThumb);
				DrawThumbnail(pDC, index, &rcThumb);

				CRect rcCheck;
				GetCheckBoxRect(index, &rcCheck);
				CMFCVisualManager *pManager = CMFCVisualManager::GetInstance();
				pManager->OnDrawCheckBox(pDC, rcCheck, FALSE, GetCheck(index) ? 1 : 0, TRUE);
			}
		}
		else if (IsReportView())
		{
			CTask *pTask = (CTask *)GetItemData(index);

			if (i == COL_STATUS && pTask->m_nStatus == CONV_STATUS_CONVERTING)
			{
				CRect rcLabel;
				GetSubItemRect(index, COL_STATUS, rcLabel);
				DrawProgress(pDC, index, &rcLabel);
			}
			else
			{
				if (pHeader->GetVisible(i))
				{
					CString strText = GetItemText(index, i);
					if (strText.GetLength() > 0)
					{
						CRect rcLabel;
						GetSubItemRect(index, i, rcLabel);
						rcLabel.InflateRect(-6, -1);

						UINT nFlags = DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS | DT_NOFULLWIDTHCHARBREAK;

						CSize sizeText = pDC->GetTextExtent(strText);
						if (sizeText.cx < rcLabel.Width() || sizeText.cy * 2 > rcItem.Height())
							nFlags |= DT_SINGLELINE;
						else
							nFlags |= DT_WORDBREAK;

						HDITEM col;
						col.mask = HDI_FORMAT;
						GetHeaderCtrl()->GetItem(i, &col);

						switch (col.fmt & HDF_JUSTIFYMASK)
						{
						case HDF_LEFT:
							nFlags |= DT_LEFT;
							break;
						case HDF_CENTER:
							nFlags |= DT_CENTER;
							break;
						case HDF_RIGHT:
							nFlags |= DT_RIGHT;
							break;
						default:
							nFlags |= DT_LEFT;
							break;
						}

						if ((nFlags & DT_SINGLELINE) == 0)
						{
							CRect rcCalc = rcLabel;
							pDC->DrawText(strText, &rcCalc, nFlags | DT_CALCRECT);

							if (rcCalc.right > rcLabel.right)	// the largest word is wider than the rectangle
							{
								nFlags |= DT_SINGLELINE;
								nFlags &= ~DT_WORDBREAK;
							}
							else
							{
								if (rcCalc.Height() < rcLabel.Height())
								{
									int dy = rcLabel.Height() - rcCalc.Height();
									rcLabel.InflateRect(0, -dy / 2);
								}
								else if (rcCalc.Height() > rcLabel.Height())
								{
									rcCalc.bottom = rcCalc.top + sizeText.cy * (rcLabel.Height() / sizeText.cy);
									int dy = rcLabel.Height() - rcCalc.Height();
									rcLabel.InflateRect(0, -dy / 2);
								}
							}
						}

						pDC->DrawText(strText, &rcLabel, nFlags);
					}
				}
			}
		}
	}

	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(clrTextOld);

	if (bFocused && m_bDrawFocusRect)
	{
		rcItem.InflateRect(-2, -2);
		pDC->DrawFocusRect(&rcItem);
	}
}

BOOL CFileList::CanCheckItem(int index, BOOL bCheck)
{
	return !m_bConverting;
}

void CFileList::OnCheckItem(int index, BOOL bCheck)
{
	ASSERT(!m_bConverting);

	if (m_bConverting)
	{
		BOOL bUpdateStatus = FALSE;

		CTask *pTask = (CTask *)GetItemData(index);
		pTask->Lock();

		if (bCheck)
		{
			if (pTask->m_nStatus == CONV_STATUS_NONE)
			{
				pTask->m_nStatus = CONV_STATUS_WAITING;
				pTask->m_nUpdate |= UPDATE_STATUS;

				bUpdateStatus = TRUE;
			}
		}
		else
		{
			if (pTask->m_nStatus == CONV_STATUS_WAITING)
			{
				pTask->m_nStatus = CONV_STATUS_NONE;
				pTask->m_nUpdate |= UPDATE_STATUS;

				bUpdateStatus = TRUE;
			}
		}

		pTask->Unlock();

		if (bUpdateStatus)
			UpdateStatus();
	}

	UpdateSummary();
	CSubjectManager::Instance()->GetSubject(SUB_FILE_LIST_CHANGED)->Notify(NULL);
}

void CFileList::OnHdnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = 0;

	if (phdr->iItem == COL_THUMBNAIL)
	{
		if (phdr->pitem->mask & HDI_WIDTH)
		{
			int width = min(MAX_THUMBNAIL_WIDTH, phdr->pitem->cxy - THUMBNAIL_MARGIN_HORZ * 2);
			int height = min(MAX_THUMBNAIL_HEIGHT, width * 3 / 4);
			SetThumbnailSize(width, height);
			RecalcHeaderPos();
			RecalcScrollPos();

			((CMainFrame *)AfxGetMainWnd())->UpdateThumbnailSlider();

			return;	// avoid duplicated redrawing
		}
	}

	if (phdr->pitem->mask & (HDI_WIDTH | HDI_FORMAT))
	{
		RecalcScrollPos();
		RecalcHeaderPos();
		Invalidate();
	}
}

int CFileList::GetSubItemIdealWith(int nItem)
{
/*	if (nItem == COL_CHECKBOX)
	{
		CMSMVisualManager *pManager = (CMSMVisualManager *)CMFCVisualManager::GetInstance();
		const CMFCControlRendererInfo &params = pManager->m_ctrlRibbonBtnCheck.GetParams();
		return params.m_rectInter.Width() * 2;
	}
	else*/ if (nItem == COL_THUMBNAIL)
	{
		return DEF_THUMBNAIL_WIDTH + THUMBNAIL_MARGIN_HORZ * 2;
	}
	else
	{
		return CUltraListCtrl::GetSubItemIdealWith(nItem);
	}
}

void CFileList::GetCheckBoxRect(int nItem, RECT *pRect)
{
	ASSERT(nItem >= 0 && nItem < GetItemCount());

	CMSMVisualManager *pManager = (CMSMVisualManager *)CMFCVisualManager::GetInstance();
	const CMFCControlRendererInfo &params = pManager->m_ctrlRibbonBtnCheck.GetParams();
	CSize size = params.m_rectInter.Size();

	if (IsReportView())
	{
		GetSubItemRect(nItem, 0, pRect);
		::InflateRect(pRect, -(pRect->right - pRect->left - size.cx) / 2, -(pRect->bottom - pRect->top - size.cy) / 2);
	}
	else
	{
		GetItemRect(nItem, pRect);
		::InflateRect(pRect, -THUMBNAIL_MARGIN_HORZ * 2, -THUMBNAIL_MARGIN_VERT * 2);

		pRect->right = pRect->left + size.cx;
		pRect->bottom = pRect->top + size.cx;
		/*CRect rc;
		GetItemRect(nItem, &rc);
		rc.InflateRect(-THUMBNAIL_MARGIN_HORZ, -THUMBNAIL_MARGIN_VERT);

		CRect rcText = rc;
		rcText.top = rcText.bottom - (afxGlobalData.GetTextHeight() + THUMBNAIL_MARGIN_TEXT * 2) + THUMBNAIL_MARGIN_VERT / 2;

		rcText.InflateRect(-(rcText.Width() - size.cx) / 2, -(rcText.Height() - size.cy) / 2);
		rcText.OffsetRect(rc.left - rcText.left, 0);

		::CopyRect(pRect, &rcText);*/
	}
}

void CFileList::SetReportView(BOOL bReport)
{
	if (bReport)
	{
		SetMargin(0, 5, 0, 5);
		CFileList::Instance()->ModifyULStyle(0, ULS_REPORT);
	}
	else
	{
		SetMargin(15, 15, 15, 15);
		CFileList::Instance()->ModifyULStyle(ULS_REPORT, 0);
	}
}

COLORREF CFileList::GetSelectionRectColor()
{
	//CMSMVisualManager *pManager = (CMSMVisualManager *)CMSMVisualManager::GetInstance();
	//return pManager->m_clrHighlightGradientDark;
	return ::GetSysColor(COLOR_HIGHLIGHT);
}

struct ADD_FILE_THREAD_PARAMETER
{
	std::vector<std::string> list_all;
	std::vector<std::string> list_failed;
	CDlgProgress *pDlg;
};

DWORD CFileList::AddFileThread(LPVOID lpParameter)
{
	UINT nIDResult = IDOK;

	ADD_FILE_THREAD_PARAMETER *pParam = (ADD_FILE_THREAD_PARAMETER *)lpParameter;

	CDlgProgress *pDlg = pParam->pDlg;
	pDlg->WaitForInitialize();

	pDlg->GetProgressCtrl().SetRange(0, pParam->list_all.size());
	pDlg->GetProgressCtrl().SetStep(1);

	for (size_t i = 0; i < pParam->list_all.size(); i++)
	{
		pDlg->SetPrompt(::FileStripPath(pParam->list_all[i].c_str()));

		pDlg->GetProgressCtrl().StepIt();

		CTask *pTask = CFileList::Instance()->ParseFile(pParam->list_all[i].c_str());

		if (pDlg->IsCanceled())
		{
			if (pTask != NULL)
				delete pTask;
			nIDResult = IDCANCEL;
			break;
		}

		if (pTask != NULL)
		{
			pTask->m_strOutputName = pTask->m_strSourceFile;
			::PathStripPath(pTask->m_strOutputName.GetBuffer());
			pTask->m_strOutputName.ReleaseBuffer();
			::PathRemoveExtension(pTask->m_strOutputName.GetBuffer());
			pTask->m_strOutputName.ReleaseBuffer();

			CFileList::Instance()->SendMessage(WM_TASK_ADD, 0, (LPARAM)pTask);
		}
		else
		{
			pParam->list_failed.push_back(pParam->list_all[i]);
		}
	}

	Sleep(1);

	pDlg->End(nIDResult);

	return 0;
}

void CFileList::OnFileAdd()
{
	// TODO: Add your command handler code here

	CString strFilter;
	CString strFilterAllSupported;
	CString strFilterAllFiles;
	CString strExtAllSupported;

	strFilterAllSupported.LoadString(IDS_FILTER_ALL_SUPPORTED);
	strFilterAllFiles.LoadString(IDS_FILTER_ALL_FILES);

	CString strExt;

	CString strFilterVideo;
	strFilterVideo.LoadString(IDS_FILTER_VIDEO);
	for (int i = 1; ;i += 2)
	{
		if (!AfxExtractSubString(strExt, strFilterVideo, i, '|'))
			break;

		if (!strExtAllSupported.IsEmpty())
			strExtAllSupported += _T(";");
		strExtAllSupported += strExt;
	}

	CString strFilterAudio;
	strFilterAudio.LoadString(IDS_FILTER_AUDIO);
	for (int i = 1; ;i += 2)
	{
		if (!AfxExtractSubString(strExt, strFilterAudio, i, '|'))
			break;

		if (!strExtAllSupported.IsEmpty())
			strExtAllSupported += _T(";");
		strExtAllSupported += strExt;
	}

	strFilter = strFilterAllSupported + _T("|") + strExtAllSupported + _T("|") + 
		strFilterVideo + _T("|") + 
		strFilterAudio  + _T("|") + 
		strFilterAllFiles + _T("|*.*||");

	CFileDialogEx dlg(LoadStringInline(IDS_ADD_FILES), TRUE, NULL, strFilter, NULL, this);

	const int nMaxFiles = 100;
	const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
	CString strFile;
	dlg.m_ofn.lpstrFile = strFile.GetBuffer(nBuffSize);
	dlg.m_ofn.nMaxFile = nBuffSize;
	dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;

	if (dlg.DoModal() == IDOK)
	{
		UnselectAllItems();

		UpdateWindow();

		std::vector<std::string> list;
		POSITION pos = dlg.GetStartPosition();
		while (pos)
		{
			CString strPath = dlg.GetNextPathName(pos);
			list.push_back((LPCTSTR)strPath);
		}

		if (list.size() > 0)
			AddFiles(list);

		strFile.ReleaseBuffer();
	}
}

void CFileList::AddFiles(std::vector<std::string> &list)
{
	if (m_bConverting)
	{
		AfxMessageBox(IDS_E_ADD_FILES_WHILE_CONVERTING);
		return;
	}

	ASSERT(list.size() > 0);
	if (list.size() > 0)
	{
		CWaitCursor wc;

		CDlgProgress dlgProgress(this, LoadStringInline(IDS_ADDING_FILES), "");

		ADD_FILE_THREAD_PARAMETER param;
		param.list_all = list;

		std::vector<std::string> temp_list;
		string strVideoList="";
		BOOL bVideoList = FALSE;
		CVideoList *pVideoList = CVideoList::Instance();
		for(int i=0;i<param.list_all.size();i++)
		{
			bVideoList = FALSE;
			if(pVideoList->Check(param.list_all[i].c_str()))
				bVideoList = pVideoList->Read(param.list_all[i].c_str(),temp_list,param.list_failed);
			if(bVideoList)
				param.list_all[i]=strVideoList;
		}
		CVideoList::Remove(param.list_all,strVideoList);
		CVideoList::Splice(param.list_all,temp_list);
		ASSERT(temp_list.size()==0);

		for (int i = 0; i < param.list_all.size(); i++)
		{
			if (param.list_all[i].find('?') != std::string::npos)
			{
				if (AfxMessageBox(IDS_E_FILE_NAME_UNUASUAL_CHARS, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
					return;
				break;
			}
		}

		CFileList::Instance()->EnableUpdateStatusTimer(FALSE);

		param.pDlg = &dlgProgress;

		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AddFileThread, (LPVOID)&param, 0, &dwThreadId);

		m_bAddingFiles = TRUE;
		::SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED|ES_CONTINUOUS);
		//CLog::Instance()->AppendLog("AddFiles--::SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED|ES_CONTINUOUS);\r\n");

		dlgProgress.DoModal();

		::SetThreadExecutionState(ES_CONTINUOUS);
		//CLog::Instance()->AppendLog("AddFiles--::SetThreadExecutionState(ES_CONTINUOUS);\r\n");
		m_bAddingFiles = FALSE;
	

		CFileList::Instance()->EnableUpdateStatusTimer(TRUE);
		CFileList::Instance()->UpdateStatus();

		if (param.list_failed.size() > 0)
		{
			std::string str;
			for (size_t i = 0; i < param.list_failed.size(); i++)
				str += param.list_failed[i] + "\r\n";

			AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_ADD_FILES1, str.c_str());
		}
	}
}

void CFileList::OnFileRemove()
{
	// TODO: Add your command handler code here

	for (int i = 0; i < GetItemCount(); i++)
	{
		if (IsItemSelected(i))
		{
			CTask *pTask = (CTask *)GetItemData(i);

			pTask->Lock();
			BOOL bConverting = (pTask->m_nStatus == CONV_STATUS_CONVERTING);
			pTask->Unlock();

			if (bConverting)
			{
				if (AfxMessageBox(IDS_CONFIRM_REMOVE_CONVERTING_FILES, MB_ICONQUESTION | MB_YESNO) == IDNO)
					return;
				break;
			}
		}
	}

	CWaitCursor wc;
	DeleteSelected();
}

void CFileList::OnFileRemoveAll()
{
	// TODO: Add your command handler code here

	if (AfxMessageBox(IDS_CONFIRM_REMOVE_ALL_FILES, MB_ICONWARNING | MB_YESNO) != IDYES)
		return;

	CWaitCursor wc;
	SelectAllItems();
	DeleteSelected();
}

void CFileList::OnFileConvert()
{
	// TODO: Add your command handler code here
	CMFCPropertyGridCtrl *pPropGrid = GetPropertyGridAdvanced();
	if (!pPropGrid->EndEditItem(TRUE))
		return;

	CWaitCursor wc;

	if (m_bConverting && m_bPaused)
	{
		ResumeAll();
		return;
	}

	int count = GetItemCount();
	if (count == 0)
	{
		AfxMessageBox(IDS_NO_FILES_TO_CONVERT);
		return;
	}

	int nChencked = 0;
	for (int i = 0; i < count; i++)
	{
		if (GetCheck(i))
		{
			nChencked++;
			CTask *pTask = (CTask *)GetItemData(i);
			pTask->ClearTasks();
		}
	}

	if (nChencked<1)
	{
		AfxMessageBox(IDS_NO_CHECKED_FILES_TO_CONVERT);
		return;
	}

	if(!isNTFSVolume(COptions::Instance()->m_strTempFolder))
	{
		CString strError;
		strError.Format(IDS_E_TEMP_FOLDER_NOT_NTFS,COptions::Instance()->m_strTempFolder);
		if(AfxMessageBox(strError , MB_OKCANCEL | MB_ICONQUESTION)  != IDOK)
			return ;

		CDlgSelectTempFolder dlg;
		dlg.m_strTempFolder = COptions::Instance()->m_strTempFolder;
		if (dlg.DoModal() != IDOK)
			return;

		COptions::Instance()->m_strTempFolder = dlg.m_strTempFolder;
	}

	BOOL bExceeded = FALSE;
	CProfileManager *pManager = CProfileManager::Instance();
	if (!pManager->CalclateBitrate(bExceeded, TRUE) || bExceeded)
		return;

	m_bBurnDisc = TRUE;

	if (COptions::Instance()->m_bCheckDiscStatus)
	{
		CBurner *pBurner = CxBurner::Instance();

		if (pBurner->IsOpen())
		{
			CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
			if (pCombo->GetCurSel() >= 0)
			{
_retry:
				TCHAR *lpszUniqueID = (TCHAR *)pCombo->GetItemData(pCombo->GetCurSel());
				CString strError;

				MediaInfo info;
				if(!pBurner->GetMediaInfo(lpszUniqueID,info))
				{
					strError.LoadString(IDS_BURNER_NO_DISC);
				}
				else
				{
					switch (info.MediaType)
					{
					case IMAPI_MEDIA_TYPE_DVDPLUSR:
					case IMAPI_MEDIA_TYPE_DVDDASHR:
					case IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER:
					case IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER:
					case IMAPI_MEDIA_TYPE_BDR:
						if (!info.isBlank)
							strError.LoadString(IDS_BURNER_NOT_BLANK_DISC);
						break;					

					case IMAPI_MEDIA_TYPE_DVDPLUSRW:
					case IMAPI_MEDIA_TYPE_DVDDASHRW:
					case IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER:
					case IMAPI_MEDIA_TYPE_BDRE:
						if ((!info.isBlank))
						{
							if (COptions::Instance()->m_bPromptNonBlankDisc)
							{
								if (AfxMessageBox(IDS_BURNER_CONFIRM_OVERWRITE, MB_YESNO | MB_ICONWARNING) == IDNO)
									return;
							}
						}
						break;

					default:
						strError.LoadString(IDS_BURNER_NOT_WRITABLE_DISC);
						break;
					}
				}

				if (strError.IsEmpty())
				{
					// Check if the target disc size matches the capacity of the disc inserted.
					UINT nDiscSize = 0;
					switch (pBurner->GetDiscCapaticy(info.MediaType))
					{
					case DISC_CAP_DVD5:
						nDiscSize = 4700000;
						break;
					case DISC_CAP_DVD9:
						nDiscSize = 8500000;
						break;
				#ifdef _BD
					case DISC_CAP_BD25:
						nDiscSize = 25000000;
						break;
					case DISC_CAP_BD50:
						nDiscSize = 50000000;
						break;
				#endif						
					}

					if (nDiscSize > 0)
					{
						CMFCPropertyGridProperty *pPropDisc = GetPropertyGridAdvanced()->FindProperty(CAT_GENERAL, "disctype");
						if (pPropDisc == NULL)
							return;

						CProfileParam *pParam = (CProfileParam *)pPropDisc->GetData();
						CString strValue;
						pParam->GetValue(NULL, strValue);

						UINT nTargetDiscSize = _ttoi(strValue) ;

						if (nTargetDiscSize != nDiscSize)
						{
							AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_DISC_CAP_NOT_MATCH2, (float)nDiscSize / 1000 / 1000, (float)nTargetDiscSize / 1000 / 1000);
							return;
						}
					}
				}
				else
				{
					CString strPrompt;
#ifdef _BD
					strPrompt.LoadString(IDS_CONFIRM_CREATE_BD_FOLDER_ONLY);
#else
					strPrompt.LoadString(IDS_CONFIRM_CREATE_DVD_FOLDER_ONLY);
#endif
					UINT ret = AfxMessageBox(strError + strPrompt, MB_ICONWARNING | MB_ABORTRETRYIGNORE);
					switch (ret)
					{
					case IDRETRY:
						goto _retry;

					case IDIGNORE:
						m_bBurnDisc = FALSE;
						break;

					default:
						return;
					}
				}
			}
		}
	}

	COptions::Instance()->m_strDiscType = GetDiscTypeComboBox()->GetText();

#ifdef _BD
	COptions::Instance()->m_strDimension = GetVideoDimensionComboBox()->GetText();
	COptions::Instance()->m_strFrameRate = GetVideoFrameRateComboBox()->GetText();
#else
	COptions::Instance()->m_strVideoStandard = GetVideoStandardComboBox()->GetText();
	CMFCPropertyGridProperty *pPropAspect = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vaspect");
	if(pPropAspect!=NULL)
	{
		USES_CONVERSION;
		COptions::Instance()->m_strVideoAspect = OLE2A(pPropAspect->GetValue().bstrVal);
	}

	//OLE2A
#endif

	double dTotalDuration = CFileList::Instance()->GetTotalDuration()+CFileList::Instance()->MenusDuration();
	CLog::Instance()->AppendLog("Video bitrate: %d(K bps), Audio bitrate: %d(K bps).\r\n",pManager->m_nVBitrate,pManager->m_nABitrate);
	CLog::Instance()->AppendLog("Total Duration(Include menu):%f.\r\n",dTotalDuration);
	//unsigned long lEstSize = (pManager->m_nVBitrate+pManager->m_nABitrate)*125*dTotalDuration;
	CLog::Instance()->AppendLog("Estimated size:%I64d(byte).\r\n",CProfileManager::Instance()->CalclateSize());
   

	if (!COptions::Instance()->PrepareTempFolder(FALSE))
	{
#ifdef _BD
		if(!isNTFSVolume(COptions::Instance()->m_strTempFolder))
		{
			CString strError;
			strError.Format(IDS_E_TEMP_FOLDER_NOT_NTFS,COptions::Instance()->m_strTempFolder);
			if(AfxMessageBox(strError , MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
				return ;
		}
#endif
		CDlgSelectTempFolder dlg;
		dlg.m_strTempFolder = COptions::Instance()->m_strTempFolder;
		if (dlg.DoModal() != IDOK)
			return;

		COptions::Instance()->m_strTempFolder = dlg.m_strTempFolder;
	}

#ifdef _BD
	#ifndef  _DEBUG
		g_bIsActivated = CActivate::Instance()->IsActivated();
	#endif

	if (!g_bIsActivated)
	{
		AfxMessageBox(IDS_TRIAL_PROMPT, MB_ICONINFORMATION | MB_OK);
	}

#endif

#ifdef AC3_AFTEN
	CProfileItem *pItem;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pItem);
	CString strEnableAudio="";
	pItem->GetProfileParam("aenable")->GetValue(NULL,strEnableAudio);
	if(strEnableAudio.Compare("-an")!=0)
		g_fEncoderAC3 = 0.08f;
	else
		g_fEncoderAC3 = 0.0f;
#endif

	//Informs the system that the state being set should remain in effect until the next call that uses ES_CONTINUOUS and one of the other state flags is cleared.
   ::SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED|ES_CONTINUOUS);
   CLog::Instance()->AppendLog("OnFileConvert--::SetThreadExecutionState(ES_DISPLAY_REQUIRED|ES_SYSTEM_REQUIRED|ES_CONTINUOUS);\r\n");
   QueueAll();
}

void CFileList::OnFilePause()
{
	// TODO: Add your command handler code here

	if (m_bConverting && !m_bPaused)
	{
		CWaitCursor wc;
		PauseAll();
	}
}

void CFileList::OnFileStop()
{
	// TODO: Add your command handler code here

	if (COptions::Instance()->m_bPromptStopConversion)
	{
		BOOL bCheck = FALSE;
		if (CheckMessageBox(IDS_CONFIRM_CANCEL_CONVERSION, IDS_DO_NOT_ASK_AGAIN, bCheck, MB_ICONWARNING | MB_YESNO) == IDNO)
			return;

		if (bCheck)
			COptions::Instance()->m_bPromptStopConversion = FALSE;
	}

	CWaitCursor wc;
	StopAll();

	// Remove successfully encoded files after stopped. (Failure files have been 
	// deleted in CTask::ConvProc() already)
	RemoveOutputFiles(TRUE);

	::SetThreadExecutionState(ES_CONTINUOUS);
	CLog::Instance()->AppendLog("OnFileStop--::SetThreadExecutionState(ES_CONTINUOUS);\r\n");
}

void CFileList::OnFileProperties()
{
	// TODO: Add your command handler code here

	int index = GetFirstSelectedItem();
	if (index >= 0)
	{		
		CTask *pTask = (CTask *)GetItemData(index);
		AfxMessageBox(pTask->GetMediaInfo(), MB_OK | MB_ICONINFORMATION);
	}
}

void CFileList::OnFilePreview()
{
	// TODO: Add your command handler code here

#ifdef _MENU_TEMPLATE_VIEW
	if (GetSidePane()->m_wndSettingsTab.GetActiveTab() == TAB_MENU)
		return;
#endif

	int index = GetFirstSelectedItem();
	if (index >= 0)
	{		
		CTask *pTask = (CTask *)GetItemData(index);
		GetPlayerPane()->OnPlayerPlay();
	}
}

void CFileList::OnFilePlay()
{
	// TODO: Add your command handler code here

	int index = GetFirstSelectedItem();
	if (index >= 0)
	{		
		CTask *pTask = (CTask *)GetItemData(index);
		ShellOpenFile(NULL, pTask->m_strSourceFile);
	}
}

BOOL CFileList::GetToolTipText(int index, CString &strText)
{
	CTask *pTask = (CTask *)GetItemData(index);
	strText = pTask->GetMediaInfo();
	return TRUE;
}

void CFileList::OnKillFocus(CWnd* pNewWnd)
{
	CUltraListCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	m_bDrawFocusRect = FALSE;
}

void CFileList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	m_bDrawFocusRect = TRUE;
	int nFocus = GetFocusItem();
	if (nFocus >= 0 && nFocus < GetItemCount())
		RedrawItem(nFocus);

	CUltraListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFileList::OnEditSelectAll()
{
	// TODO: Add your command handler code here

	SelectAllItems();
}

struct ENCODING_THREAD_PARAMETER
{
	CDlgProgress *pDlg;
	CEncoder *pEncoder;
};

DWORD CFileList::EncodingThread(LPVOID lpParameter)
{
	UINT nIDResult = IDOK;

	ENCODING_THREAD_PARAMETER *pParam = (ENCODING_THREAD_PARAMETER *)lpParameter;

	CDlgProgress *pDlg = pParam->pDlg;
	pDlg->AllowCancel(FALSE);	// canceling is not allowed

	pDlg->WaitForInitialize();

	pDlg->GetProgressCtrl().SetRange(0, 100);
	pDlg->GetProgressCtrl().SetStep(1);

	CTask *pTask = pParam->pEncoder->m_pTask;
	ASSERT(pTask != NULL);

	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncodingThreadImpl, (LPVOID)pParam->pEncoder, 0, &dwThreadId);

	DWORD dwExitCode;
	while (true)
	{
		pDlg->GetProgressCtrl().SetPos(min(100, (int)pTask->m_fEncodedPercent));

		Sleep(500);

		if (::GetExitCodeThread(hThread, &dwExitCode))
		{
			if (dwExitCode != STILL_ACTIVE)
				break;
		}
	}

	pDlg->GetProgressCtrl().SetPos(min(100, (int)pTask->m_fEncodedPercent));

	Sleep(1);

	pDlg->End(dwExitCode == 0 ? IDOK : IDCANCEL);

	return 0;
}

DWORD CFileList::EncodingThreadImpl(LPVOID lpParameter)
{
	CEncoder *pEncoder = (CEncoder *)lpParameter;
	return pEncoder->Start() ? 0 : -1;
}

//可能或保留输出的文件，但是输出文件的记录必须删除
void CFileList::RemoveOutputFiles(BOOL bDeleteFile)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetCheck(i))
		{
			CTask *pTask = (CTask *)GetItemData(i);
			if (pTask->m_nStatus == CONV_STATUS_COMPLETED && bDeleteFile)
				pTask->DeleteFiles();
		}
	}
}

void CFileList::OnTaskCompletedAll()
{
	CLog::Instance()->AppendLog("CFileList::OnTaskCompletedAll().\r\n");
#ifdef _BD

	PostProcess();

#elif defined _DVD

	CString strTempFolder;
	if (!::CreateTempFolder(COptions::Instance()->m_strTempFolder, strTempFolder))
		AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
	else
	{
		CLog::Instance()->AppendLog("CFileList::PostProcess().\r\n");
		PostProcess(strTempFolder);

		if (AfxGetApp()->GetProfileInt("Debug", "Delete Temp Folder", TRUE))
			RemoveDirectory(strTempFolder, TRUE);

	}
	BOOL bDeleteFiles = AfxGetApp()->GetProfileInt("Debug", "Delete Output Files", TRUE);
	RemoveOutputFiles(bDeleteFiles);
#endif
	::SetThreadExecutionState(ES_CONTINUOUS);
    CLog::Instance()->AppendLog("OnTaskCompletedAll--::SetThreadExecutionState(ES_CONTINUOUS);\r\n");
}

#ifdef _BD

void CFileList::PostProcess()
{
	int count = GetItemCount();
	if (count == 0)
		return;

	int nSucceeded = 0;
	int nFailed = 0;

	INT64 nTotalSize = 0;

	CString strOutputFiles,strAudioFiles,strOutputFile;
	BOOL bCreateChapters = TRUE;
	float fTotalDuration = 0.0;
	std::vector<float> fDurations;

	for (int i = 0; i < count; i++)
	{
		if (GetCheck(i))
		{
			CTask *pTask = (CTask *)GetItemData(i);
			if (pTask->m_nStatus == CONV_STATUS_COMPLETED)
			{
				nSucceeded++;

				struct _stat64 fs;
				for(int n=0;n<(int)pTask->m_Atoms.size();n++)
				{
					strOutputFile=pTask->m_Atoms[n].outfile; 
					if (_stati64(strOutputFile, &fs) == 0)
						nTotalSize += fs.st_size;

					if(pTask->m_Atoms[n].novideo)
					{
						if(!strAudioFiles.IsEmpty())
							strAudioFiles += "  +";

						strAudioFiles += "\"" + strOutputFile + "\"";
					}
					else
					{
						if (!strOutputFiles.IsEmpty())
							strOutputFiles += "  +";

						strOutputFiles += "\"" + strOutputFile + "\"";
						if (bCreateChapters)
						{
							CMediaInfo mi;
							if (mi.ParseMedia(strOutputFile))
							{
								fTotalDuration += mi.m_fDuration;
							}
							else
							{
								bCreateChapters = FALSE;
							}
						}			
					}
				}

				fDurations.push_back(fTotalDuration);	
			}
			else if (pTask->m_nStatus == CONV_STATUS_FAILED)
			{
				nFailed++;
			}
		}
	}

	if (nSucceeded == 0)
	{
		AfxMessageBox(IDS_E_CONVERT_FILES);
		return;
	}

	if (nFailed > 0)
	{
		if (AfxMessageBoxEx(MB_ICONWARNING | MB_YESNO, IDS_CONFIRM_SOME_FILES_FAILED) == IDNO)
			return;
	}

	if (!COptions::Instance()->CheckFreeDiskSpace((INT64)(nTotalSize * 1.1)))	// BD structure. Temporary MKV files will be deleted after executing tsMuxeR.
		return;

	BOOL bDeleteFiles = AfxGetApp()->GetProfileInt("Debug", "Delete Output Files", TRUE);
	//(1)Create bluray menu,and save to "strMenuTempFolder"
	CString strMenuTempFolder;
	CMenuTemplate *pTemplate = CMenuTemplateManager::Instance()->GetCurrentTemplate();
	if (pTemplate->m_bTemplate)
	{
		UINT_PTR retMenu = IDCANCEL;
		CString strMenuXML = GetTempFile(NULL,"blu-ray menu","xml");
		if(pTemplate->SaveMenuTemplate(strMenuXML))
		{
			if (!::CreateTempFolder(COptions::Instance()->m_strTempFolder, strMenuTempFolder)||!CreateBDFolderStructure(strMenuTempFolder))
			{
				::DeleteFile(strMenuXML);
				AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
				return ;
			}
			CEncoderBDMenu menu;
			menu.m_strMenuXML = strMenuXML;
			menu.m_strOutputFolder = strMenuTempFolder;
			menu.m_pTask = new CTask("");

			CDlgProgress dlgProgress(this, LoadStringInline(IDS_CREATING_BD_MENU), LoadStringInline(IDS_CREATING_BD_MENU_PROMPT));

			ENCODING_THREAD_PARAMETER param;
			param.pEncoder = &menu;
			param.pDlg = &dlgProgress;

			DWORD dwThreadId;
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncodingThread, (LPVOID)&param, 0, &dwThreadId);

			retMenu = dlgProgress.DoModal();
		};
		if (bDeleteFiles)
			::DeleteFile(strMenuXML);

		if (retMenu != IDOK)
		{
			//Create Menu Failed：Remove mkv files
			if (bDeleteFiles)
				::RemoveDirectory(strMenuTempFolder, TRUE);
			strMenuTempFolder.Empty();
			if (AfxMessageBox(IDS_CONFIRM_BD_MENU_ERROR, MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				RemoveOutputFiles(bDeleteFiles);
				return;
			}
		}	
	}

	//(2)Merge all mkv files;
	//(3)Remove all mkv files.
	//(4)tsMuxeR the merged mkv file to BD/AVCHD
	//(5)Copy menu to BD/AVCHD
	CString strScript;

	try
	{
		CString strTemplate;

		if (COptions::Instance()->m_strDiscType.Left(4).CompareNoCase("DVD-") == 0)
			strTemplate.Format("%s\\Scripts\\avchd.meta", GetModuleFilePath());
		else
			strTemplate.Format("%s\\Scripts\\blu-ray.meta", GetModuleFilePath());

		CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
		ULONGLONG len = file.GetLength();
		file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
		strScript.ReleaseBuffer();

		//strScript.Replace("$(input)", strOutputFiles);
		CString strFirstMPLS;
		strFirstMPLS.Format("  --mplsOffset=%d --m2tsOffset=%d ",FIRST_MPLS,FIRST_MPLS);
		if (bCreateChapters && fDurations.size() > 1)
		{
			CString strChapters = "--custom-chapters=00:00:00.000";
			for (int i = 0; i < fDurations.size() - 1; i++)
				strChapters += ";" + FormatTime(fDurations[i], 3, FALSE);
			strChapters += strFirstMPLS;
			strScript.Replace("$(chapters)", (LPCTSTR)strChapters);
		}
		else
		{
			strScript.Replace("$(chapters)", strFirstMPLS);
		}

		//ASSERT(strScript.Find('$') < 0);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return;
	}

	CEncoderBlueRay encoder;
	encoder.m_strOutputRoot = COptions::Instance()->m_strTempFolder;
	encoder.m_pTask = new CTask("");
	encoder.m_strScript = strScript;
	encoder.m_strInputFiles  = strOutputFiles ;
	encoder.m_strInputAudios = strAudioFiles ;
	encoder.m_strMenuFolder = strMenuTempFolder;

	CWaitCursor wc;
	CDlgProgress dlgProgress(this, LoadStringInline(IDS_CREATING_DISC_STRUCTURE), LoadStringInline(IDS_CREATING_DISC_STRUCTURE_PROMPT));

	ENCODING_THREAD_PARAMETER param;
	param.pEncoder = &encoder;
	param.pDlg = &dlgProgress;

	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncodingThread, (LPVOID)&param, 0, &dwThreadId);
	UINT_PTR ret = dlgProgress.DoModal();

	//(6)Remove menu folder
	if (!strMenuTempFolder.IsEmpty() && ::PathIsDirectory(strMenuTempFolder))
		::RemoveDirectory(strMenuTempFolder, TRUE);
	CLog::Instance()->AppendLog("Output size:%I64d(byte).\r\n",GetFolderSize(encoder.m_strOutput));
	if (ret == IDOK)
	{
#ifdef _DEBUG
		if (AfxMessageBox("Blu-ray disc structure has been created successfully. Would you like to continue?", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			::ShellOpenFolder(NULL, encoder.m_strOutput, TRUE);
			return;
		}
#endif
		
		CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
		if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0 && m_bBurnDisc)
		{
			CDlgBurn dlg;
			dlg.m_strUniqueID = (LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel());
			dlg.m_strFolder = encoder.m_strOutput;
			dlg.DoModal();

			if (dlg.m_bSucceeded)
			{
				if (AfxGetApp()->GetProfileInt("Debug", "Delete Disc Structure", TRUE))
					::RemoveDirectory(encoder.m_strOutput, TRUE);

				if (COptions::Instance()->m_bSoundFinished)
				{
					PlaySound(NULL, NULL, 0);
					PlaySound(COptions::Instance()->m_strSoundFinished, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);
				}
			}
			else
			{
				AfxMessageBox(IDS_MANUALLY_BURN_PROMPT, MB_ICONINFORMATION | MB_OK);
				::ShellOpenFolder(NULL, encoder.m_strOutput, TRUE);
			}
		}
		else
		{
			if (COptions::Instance()->m_bSoundFinished)
			{
				PlaySound(NULL, NULL, 0);
				PlaySound(COptions::Instance()->m_strSoundFinished, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);
			}

			AfxMessageBox(IDS_MANUALLY_BURN_PROMPT, MB_ICONINFORMATION | MB_OK);
			::ShellOpenFolder(NULL, encoder.m_strOutput, TRUE);
		}
	}
	else
	{
		AfxMessageBox(IDS_CREATE_DISC_STRUCTURE_ERROR);
	}
}

#elif defined _DVD

void CFileList::PostProcess(CString &strTempFolder)
{
	if (GetItemCount() == 0)
		return;

	int nSucceeded = 0;
	int nFailed = 0;

	INT64 nTotalSize = 0;

	CString strMenuPgcs;
	CString strTitlePgcs;
	CMenuTemplate *pTemplate = CMenuTemplateManager::Instance()->GetCurrentTemplate();

	std::vector<CTask *> taskList;

	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetCheck(i))
		{
			CTask *pTask = (CTask *)GetItemData(i);
			if (pTask->m_nStatus == CONV_STATUS_COMPLETED)
			{
				nSucceeded++;

				taskList.push_back(pTask);

				struct _stat64 fs;
				for(int n=0;n<(int)pTask->m_Atoms.size();n++)
				{
					CString strOutputFile=pTask->m_Atoms[n].outfile; 
					if (_stati64(strOutputFile, &fs) == 0)
						nTotalSize += fs.st_size;
				}
			}
			else if (pTask->m_nStatus == CONV_STATUS_FAILED)
			{
				nFailed++;
			}
		}
	}

	if (nSucceeded == 0)
	{
		AfxMessageBox(IDS_E_CONVERT_FILES);
		return;
	}

	if (nFailed > 0)
	{
		if (AfxMessageBoxEx(MB_ICONWARNING | MB_YESNO, IDS_CONFIRM_SOME_FILES_FAILED) == IDNO)
			return;
	}

	CString strOldTempFolder = COptions::Instance()->m_strTempFolder;
	//CheckFreeDiskSpace()中可能会改变路径，需要重新创建文件
	if(!COptions::Instance()->CheckFreeDiskSpace((INT64)(nTotalSize * 1.1)))
		return ;

	if(strOldTempFolder!=COptions::Instance()->m_strTempFolder)
	{
		RemoveDirectory(strTempFolder, TRUE);
		// Temporary MPG files will be deleted after creating the DVD structure.
		if (!::CreateTempFolder(COptions::Instance()->m_strTempFolder, strTempFolder))
		{
			AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
			return;
		}
	}

	LPCTSTR lpszTempFolder = strTempFolder;
	BOOL bEnableDVDMenu = pTemplate->m_bTemplate;

	if (bEnableDVDMenu)
	{
		CString strTemplate=GetAbsolutePath("dvdmenu.xml", lpszTempFolder);
		if (!pTemplate->SaveMenuTemplate(strTemplate))
		{
			AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_DVD_MENU_TEMPLATE1, strTemplate);
			CString strMsg;
			strMsg.Format(IDS_E_CREATE_DVD_MENU_TEMPLATE1, strTemplate);
			CLog::Instance()->AppendLog("Error: %s\r\n",strMsg);
			return;
		}
		else
		{	
			CLog::Instance()->AppendLog("Start DVDMenu Encoder.\r\n");
			CEncoderDVDMenu encoder;
			encoder.m_strResourceFolder = pTemplate->m_strFolder;
			encoder.m_strOutputFolder = lpszTempFolder;
			encoder.m_pTask = new CTask("");

			encoder.m_strXMLFile = strTemplate;

			CDlgProgress dlgProgress(this, LoadStringInline(IDS_CREATING_DVD_MENU), LoadStringInline(IDS_CREATING_DVD_MENU_PROMPT));

			ENCODING_THREAD_PARAMETER param;
			param.pEncoder = &encoder;
			param.pDlg = &dlgProgress;

			DWORD dwThreadId;
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncodingThread, (LPVOID)&param, 0, &dwThreadId);

			UINT_PTR ret = dlgProgress.DoModal();

			if (ret != IDOK)
			{
				bEnableDVDMenu = FALSE;
				if (AfxMessageBox(IDS_CONFIRM_DVD_MENU_ERROR, MB_ICONQUESTION | MB_YESNO) == IDNO)
				{
					CLog::Instance()->AppendLog("Error: %s\r\n",LoadStringInline(IDS_CONFIRM_DVD_MENU_ERROR));
					return;
				}
			}
		}
	}

	if (!CActivate::Instance()->IsActivated())
	{
		if (!CreateNoticeVideo(lpszTempFolder))
		{
			CLog::Instance()->AppendLog("Error: CreateNoticeVideo() Failed\r\n");
			return;
		}
	}

	if (!CreateDVDAuthorXML(taskList, pTemplate->m_listButtons, bEnableDVDMenu, lpszTempFolder))
	{
		CLog::Instance()->AppendLog("Error: CreateDVDAuthorXML() Failed\r\n");
		return;
	}
	else
	{
		CLog::Instance()->AppendLog("Start DVD Encoder.\r\n");
		CEncoderDVD encoder;
		if (!::CreateTempFolder(COptions::Instance()->m_strTempFolder, encoder.m_strOutputFolder))
		{
			AfxMessageBox(IDS_E_CREATE_TEMP_FOLDER);
			CLog::Instance()->AppendLog("Error: %s\r\n",LoadStringInline(IDS_E_CREATE_TEMP_FOLDER));
			return;
		}

		encoder.m_pTask = new CTask("");
		encoder.m_strXMLFile.Format("%s\\dvdauthor.xml", lpszTempFolder);
		encoder.m_nTotalSize = nTotalSize;

		CWaitCursor wc;

		CDlgProgress dlgProgress(this, LoadStringInline(IDS_CREATING_DISC_STRUCTURE), LoadStringInline(IDS_CREATING_DISC_STRUCTURE_PROMPT));

		ENCODING_THREAD_PARAMETER param;
		param.pEncoder = &encoder;
		param.pDlg = &dlgProgress;

		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncodingThread, (LPVOID)&param, 0, &dwThreadId);

		UINT_PTR ret = dlgProgress.DoModal();

		if (ret != IDOK)
		{
			AfxMessageBox(IDS_CREATE_DISC_STRUCTURE_ERROR);
			CLog::Instance()->AppendLog("Error: %s\r\n",LoadStringInline(IDS_CREATE_DISC_STRUCTURE_ERROR));
			return;
		}

		CLog::Instance()->AppendLog("Output size:%I64d(byte).\r\n",GetFolderSize(encoder.m_strOutputFolder));
#ifdef _DEBUG
		if (AfxMessageBox("DVD disc structure has been created successfully. Would you like to continue?", MB_ICONQUESTION | MB_YESNO) != IDYES)
		{
			::ShellOpenFolder(NULL, encoder.m_strOutputFolder, TRUE);
			return;
		}
#endif

		CMFCToolBarComboBoxButton *pCombo = GetBurnerComboBox();
		if (CxBurner::Instance()->IsOpen() && pCombo->GetCurSel() >= 0 && m_bBurnDisc)
		{
			CDlgBurn dlg;
			dlg.m_strUniqueID = (LPCTSTR)pCombo->GetItemData(pCombo->GetCurSel());
			dlg.m_strFolder = encoder.m_strOutputFolder;
			dlg.DoModal();

			if (dlg.m_bSucceeded)
			{
				if (AfxGetApp()->GetProfileInt("Debug", "Delete Disc Structure", TRUE))
					::RemoveDirectory(encoder.m_strOutputFolder, TRUE);

				if (COptions::Instance()->m_bSoundFinished)
				{
					PlaySound(NULL, NULL, 0);
					PlaySound(COptions::Instance()->m_strSoundFinished, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);
				}
			}
			else
			{
				AfxMessageBox(IDS_MANUALLY_BURN_PROMPT, MB_ICONINFORMATION | MB_OK);
				::ShellOpenFolder(NULL, encoder.m_strOutputFolder, TRUE);
			}
		}
		else
		{
			if (COptions::Instance()->m_bSoundFinished)
			{
				PlaySound(NULL, NULL, 0);
				PlaySound(COptions::Instance()->m_strSoundFinished, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);
			}

			AfxMessageBox(IDS_MANUALLY_BURN_PROMPT, MB_ICONINFORMATION | MB_OK);
			::ShellOpenFolder(NULL, encoder.m_strOutputFolder, TRUE);
		}
	}
}
#endif


void CFileList::ShutDownWindows()
{
	ASSERT(COptions::Instance()->m_bShutDown);

	CDlgShutDown dlg;
	if (dlg.DoModal() != IDOK)
		return;

	if (COptions::Instance()->m_nShutDownOptions == SHUTDOWN_SLEEP)
	{
		SetSuspendState(FALSE, FALSE, FALSE);
	}
	else if (COptions::Instance()->m_nShutDownOptions == SHUTDOWN_HIBERNATE)
	{
		SetSuspendState(TRUE, FALSE, FALSE);
	}
	else
	{
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 

		// Get a token for this process. 
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		{
			// Get the LUID for the shutdown privilege. 
			LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 

			tkp.PrivilegeCount = 1;  // one privilege to set    
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

			// Get the shutdown privilege for this process. 
			AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 

			if (GetLastError() == ERROR_SUCCESS) 
			{
				// Shut down the system and force all applications to close. 
				ExitWindowsEx(EWX_SHUTDOWN, 
					SHTDN_REASON_MAJOR_APPLICATION |
					SHTDN_REASON_MINOR_NONE |
					SHTDN_REASON_FLAG_PLANNED);
			}
		}
	}
}

void CFileList::OnFileMoveUp()
{
	// TODO: Add your command handler code here

	if (GetSelectedCount() > 0 && m_sel.GetAt(0) > 0)
	{
		MoveSelection(m_sel.GetAt(0) - 1);
		CSubjectManager::Instance()->GetSubject(SUB_FILE_LIST_CHANGED)->Notify(NULL);
	}
}

void CFileList::OnFileMoveDown()
{
	// TODO: Add your command handler code here

	if (GetSelectedCount() > 0 && m_sel.GetAt(m_sel.GetCount() - 1) < GetItemCount() - 1)
	{
		MoveSelection(m_sel.GetAt(m_sel.GetCount() - 1) + 2);
		CSubjectManager::Instance()->GetSubject(SUB_FILE_LIST_CHANGED)->Notify(NULL);
	}
}

void CFileList::OnFileTrim()
{	
	CTask *pTask = GetCurTask();

	if (pTask != NULL)
	{

		BOOL bCanTrim = !m_bConverting;

		if (bCanTrim)
		{
			CDlgTrim dlg;
			dlg.m_pTask = pTask;
			dlg.DoModal();
		}
	}
}

void CFileList::OnUpdateFileAdd(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bConverting);
}

void CFileList::OnUpdateFileRemove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bConverting && GetSelectedCount() > 0);
}

void CFileList::OnUpdateFileRemoveAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bConverting && GetItemCount() > 0);
}

void CFileList::OnUpdateFileMoveUp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() > 0);
}

void CFileList::OnUpdateFileMoveDown(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() > 0);
}

void CFileList::OnUpdateFileConvert(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bConverting || (m_bConverting && m_bPaused));
}

void CFileList::OnUpdateFileStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bConverting);
}

void CFileList::OnUpdateFilePreview(CCmdUI *pCmdUI)
{
#ifdef _DVD
	if (GetSidePane()->m_wndSettingsTab.GetActiveTab() == TAB_MENU)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
#endif

	pCmdUI->Enable(GetSelectedCount() == 1);
}

void CFileList::OnUpdateFilePlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() == 1);
}

void CFileList::OnUpdateFileProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetSelectedCount() == 1);
}

void CFileList::OnUpdateFilePause(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bConverting && !m_bPaused);
}

void CFileList::OnUpdateFileTrim(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	CTask *pTask = GetCurTask();
	if (pTask != NULL)
	{
		if (!m_bConverting && pTask->m_mi.m_fDuration>0.99999)
			bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}

void CFileList::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetItemCount() > 0);
}

IMPLEMENT_OBSERVER(CFileList, SUB_DISC_TYPE_CHANGED)
{
	UpdateSummary();
}

IMPLEMENT_OBSERVER(CFileList, SUB_VIDEO_DIMENSION_CHANGED)
{
	UpdateSummary();
}

IMPLEMENT_OBSERVER(CFileList, SUB_VIDEO_FRAME_RATE_CHANGED)
{
	UpdateSummary();
}

IMPLEMENT_OBSERVER(CFileList, SUB_VIDEO_TRIMMED)
{
	UpdateSummary();
}

IMPLEMENT_OBSERVER(CFileList, SUB_PROPERTY_GRID_CHANGED)
{
	UpdateSummary();

	CMFCPropertyGridColorProperty *pProp = (CMFCPropertyGridColorProperty *)pData;

	CProfileParam *pParam = (CProfileParam *)pProp->GetData();
	if (pParam == NULL)
		return;

	CTask *pTask = GetCurTask();
	if (pTask != NULL)
	{
		USES_CONVERSION;

		if (pParam->m_cat == CAT_GENERAL && pParam->m_strName.CompareNoCase("outputname") == 0)
		{
			pTask->m_strOutputName = OLE2A(pProp->GetValue().bstrVal);
			return;
		}
	}
}

IMPLEMENT_OBSERVER(CFileList, SUB_MENU_TEMPLATE_CHANGED)
{
	UpdateSummary();
}

BOOL CFileList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_hAccel != NULL)
	{
		if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		{
			if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
				return TRUE;
		}
	}

	return CUltraListCtrl::PreTranslateMessage(pMsg);
}

void CFileList::OnFileOpenSourceFolder()
{
	// TODO: Add your command handler code here

	if (GetSelectedCount() == 1)
	{
		CTask *pTask = GetCurTask();
		if (pTask != NULL)
			ShellOpenFolder(m_hWnd, pTask->m_strSourceFile, FALSE);
	}
}

void CFileList::OnUpdateFileOpenSourceFolder(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->Enable(GetCurTask() != NULL);
}

UINT CFileList::MenusDuration()
{
	UINT uMenusDuration = 0;

	int nChenckedFiles = 0;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetCheck(i))
			nChenckedFiles++;
	}

	if(nChenckedFiles>0)
	{
		CMenuTemplate *pTemplate = CMenuTemplateManager::Instance()->GetCurrentTemplate();
		if(pTemplate->m_bTemplate)
		{
			int nVideosPerPage = pTemplate->m_listButtons .size() - 2;
			int nPages = nChenckedFiles/nVideosPerPage;
			int nDurarion = _ttoi(pTemplate->m_strDurationNew);
			if(nChenckedFiles%nVideosPerPage != 0)
				nPages++;
			uMenusDuration  += nDurarion*nPages;
		}
#ifdef _DVD
		if(!CActivate::Instance()->IsActivated())
			uMenusDuration  += NOTICE_DURATION;
#endif
	}
	return uMenusDuration;
}

BOOL CFileList::MakeFileList()
{
	CString strFileList = GetTempFile(COptions::Instance()->m_strTempFolder,"File list","vfl");
	return CVideoList::Instance()->MakeFileList(strFileList);
}

#define TRIM_VIDEO_LENGTH   1*60
BOOL CFileList::FastTrim()
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (GetCheck(i))
		{
			CTask *pTask = (CTask *)GetItemData(i);
			if(pTask->m_mi.m_fDuration > TRIM_VIDEO_LENGTH)
			{
				pTask->m_bTrimmed  = TRUE;
				pTask->m_fTrimStart = 0.0;
				pTask->m_fTrimEnd = TRIM_VIDEO_LENGTH;
			}
		}
	}

	UpdateStatus();
	UpdateSummary();
	return TRUE;
}


void CFileList::OnFileAddFolder()
{
	CDlgAddFolder dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_aFiles.size() > 0)
			AddFiles(dlg.m_aFiles);
	}
}

void CFileList::OnUpdateFileAddfolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bConverting);
}

// ProfilePane.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "PlayerPane.h"
#include "DeferWindowPos.h"
#include "FileList.h"
#include "MFCToolBarSlider.h"
#include "MainFrm.h"

// CPlayerPane

const UINT ID_PLAYER			= 1001;
const UINT ID_PLAYER_TOOLBAR	= 1002;

IMPLEMENT_DYNAMIC(CPlayerPane, CBasePane)
IMPLEMENT_SINGLETON(CPlayerPane)
CPlayerPane::CPlayerPane()
{
	m_bPreviewVideo = FALSE;
	int nVolume = max(0, min(100, AfxGetApp()->GetProfileInt("Preview", "Volume", 100)));
	m_wndPlayer.SetVolume(nVolume);
}

CPlayerPane::~CPlayerPane()
{
	AfxGetApp()->WriteProfileInt("Preview", "Volume", m_wndPlayer.GetVolume());
}


BEGIN_MESSAGE_MAP(CPlayerPane, CBasePane)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_COMMAND(ID_PLAYER_TRACK, OnPlayerTrack)
	ON_COMMAND(ID_PLAYER_PLAY, OnPlayerPlay)
	ON_COMMAND(ID_PLAYER_PAUSE, OnPlayerPause)
	ON_COMMAND(ID_PLAYER_STOP, OnPlayerStop)
	ON_COMMAND(ID_PLAYER_MUTE, OnPlayerMute)
	ON_COMMAND(ID_PLAYER_VOLUME, OnPlayerVolume)

	ON_UPDATE_COMMAND_UI(ID_PLAYER_TRACK, OnUpdatePlayerTrack)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_PLAY, OnUpdatePlayerPlay)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_PAUSE, OnUpdatePlayerPause)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_STOP, OnUpdatePlayerStop)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_MUTE, OnUpdatePlayerMute)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_VOLUME, OnUpdatePlayerVolume)
	ON_UPDATE_COMMAND_UI(ID_PLAYER_TIME, OnUpdatePlayerTime)
END_MESSAGE_MAP()

// CPlayerPane message handlers
int CPlayerPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBasePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CRect rc(0, 0, 0, 0);

	m_wndPlayer.Create(AfxRegisterWndClass(0, NULL, NULL, NULL), "Preview", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rc, this, ID_PLAYER, NULL);

	if (!m_wndPlayerToolBar.Create(this, WS_VISIBLE | WS_CHILD | CBRS_ALIGN_BOTTOM, ID_PLAYER_TOOLBAR))
	{
		TRACE0("Failed to create player toolbar\n");
		return -1;      // fail to create
	}

	m_wndPlayerToolBar.SetPaneStyle(m_wndPlayerToolBar.GetPaneStyle() & ~CBRS_GRIPPER);
	m_wndPlayerToolBar.SetBorders(TOOLBAR_BORDER_X, TOOLBAR_BORDER_Y, TOOLBAR_BORDER_X, TOOLBAR_BORDER_Y);

	if (!m_wndPlayerToolBar.LoadToolBar (IDR_PLAYER, IDB_PLAYER_COLD, 0, TRUE, IDB_PLAYER_DISABLED, 0, IDB_PLAYER_HOT))
	{
		TRACE0("Failed to create player toolbar\n");
		return -1;      // fail to create
	}

	m_wndPlayerToolBar.SetRouteCommandsViaFrame(TRUE);
	m_wndPlayerToolBar.SetOwner(this);
	m_wndPlayerToolBar.SetCustomizeMode(FALSE);

	m_wndPlayerToolBar.m_bInit = TRUE;

	return 0;
}

void CPlayerPane::OnSize(UINT nType, int cx, int cy)
{
	CBasePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RepositionControls();
}

void CPlayerPane::RepositionControls()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		if(rcClient.Width()>0)
		{
			//rcClient.InflateRect(-2, -2, -2, -2);

			CSize size = m_wndPlayerToolBar.GetButtonSize();
			size.cy += 2*TOOLBAR_BORDER_Y;
			size.cy *= 2;

			CDeferWindowPos dwp;

			dwp.BeginDeferWindowPos();

			dwp.DeferWindowPos(m_wndPlayerToolBar, NULL, rcClient.left, rcClient.bottom - size.cy, rcClient.Width(), size.cy, 
				SWP_NOACTIVATE | SWP_NOZORDER);

			int x = rcClient.left;
			int w = rcClient.Width();
			int h = rcClient.Height() - size.cy;
			if (w > (int)((float)h * 4 / 3 + 0.5))
			{
				w = (int)((float)h * 4 / 3 + 0.5);
				x = rcClient.left + (rcClient.Width() - w) / 2;
			}
			dwp.DeferWindowPos(m_wndPlayer, NULL, x, rcClient.top, w, h, 
				SWP_NOACTIVATE | SWP_NOZORDER);

			dwp.EndDeferWindowPos();

			RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
		}
	}
}

void CPlayerPane::OnPlayerTrack()
{
	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_TRACK));
	if (pSlider != NULL)
	{
		pSlider->Redraw();
		if (m_wndPlayer.GetState() == State_None)
		{
			CWaitCursor wc;

			CTask *pTask = CFileList::Instance()->GetCurTask();
			if (pTask == NULL)
				return;

			if (!m_wndPlayer.OpenFile(pTask->m_strPreview))
				return;

			m_wndPlayer.Play();
			m_wndPlayer.StopWhenReady();
		}

		LONGLONG lDuration = 0;
		if (m_wndPlayer.GetState() != State_None)
		{
			lDuration = m_wndPlayer.GetDuration();
			if (lDuration > 0)
			{
				int nPos = pSlider->GetPos();
				LONGLONG lPosition = (LONGLONG)(lDuration * ((float)nPos / VIDEO_TRACK_MAX));
				m_wndPlayer.SeekTo(lPosition, TRUE);
			}
		}
	}
}

void CPlayerPane::OnPlayerPlay()
{
	if (m_wndPlayer.GetState() == State_None)
	{
		CWaitCursor wc;

		CTask *pTask = CFileList::Instance()->GetCurTask();
		if (pTask == NULL)
			return;

		if (!m_wndPlayer.OpenFile(pTask->m_strPreview))
			return;
	}

	m_wndPlayer.Play();

	m_bPreviewVideo = TRUE;
}

void CPlayerPane::OnPlayerPause()
{
	if (m_wndPlayer.GetState() != State_None)
		m_wndPlayer.Pause();
}

void CPlayerPane::OnPlayerStop()
{
	if (m_wndPlayer.GetState() != State_None)
		m_wndPlayer.Stop();
}

void CPlayerPane::OnPlayerMute()
{
	m_wndPlayer.Mute(!m_wndPlayer.IsMute());
	m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_MUTE))->SetImage(m_wndPlayer.IsMute() ? 7 : 5);
}

void CPlayerPane::OnPlayerVolume()
{
	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_VOLUME));
	if (pSlider != NULL)
	{
		pSlider->Redraw();
		int nVolume = pSlider->GetPos();
		m_wndPlayer.SetVolume(nVolume);

		if (m_wndPlayer.IsMute())
		{
			m_wndPlayer.Mute(FALSE);
			m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_MUTE))->SetImage(5);
		}
	}
}

void CPlayerPane::OnUpdatePlayerTrack(CCmdUI *pCmdUI)
{
	BOOL bEnabled = (CFileList::Instance()->GetCurTask() != NULL && m_bPreviewVideo);
	pCmdUI->Enable(bEnabled);

	if (bEnabled)
	{
		LONGLONG lDuration = 0;
		if (m_wndPlayer.GetState() != State_None)
		{
			lDuration = m_wndPlayer.GetDuration();
			if (lDuration > 0)
			{
				CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_TRACK));
				if (pSlider != NULL && !pSlider->IsPressed())
				{
					LONGLONG lPosition = m_wndPlayer.GetPosition();
					pSlider->SetPos((int)((double)lPosition / lDuration * VIDEO_TRACK_MAX));
				}
			}
		}
	}
}

void CPlayerPane::OnUpdatePlayerTime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);

	CMFCToolBarLabel *pTime = (CMFCToolBarLabel *)m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_TIME));
	CString strTime;

	CTask *pTask = CFileList::Instance()->GetCurTask();
	if (pTask != NULL)
	{
		LONGLONG lPosition = 0;
		if (m_wndPlayer.GetState() != State_None)
			lPosition = m_wndPlayer.GetPosition();

		strTime.Format("%s", FormatTime(lPosition / UNITS, 0, FALSE));
	}

	if (pTime->m_strText != strTime)
	{
		BOOL bReposition = (pTime->m_strText.GetLength() != strTime.GetLength());
		pTime->SetText(strTime);
		if (bReposition)
			m_wndPlayerToolBar.AdjustLayout();
	}
}

void CPlayerPane::OnUpdatePlayerPlay(CCmdUI *pCmdUI)
{
	BOOL bEnabled = (CFileList::Instance()->GetCurTask() != NULL && m_bPreviewVideo);
	pCmdUI->Enable(bEnabled);

	BOOL bChecked = (m_wndPlayer.GetState() == State_Running);
	pCmdUI->SetCheck(bChecked);
}

void CPlayerPane::OnUpdatePlayerPause(CCmdUI *pCmdUI)
{
	BOOL bEnabled = (CFileList::Instance()->GetCurTask() != NULL && m_bPreviewVideo);
	pCmdUI->Enable(bEnabled);

	BOOL bChecked = ((m_wndPlayer.GetState() == State_Paused || m_wndPlayer.GetState() == State_Stopped) && m_wndPlayer.GetPosition() > 0);
	pCmdUI->SetCheck(bChecked);
}

void CPlayerPane::OnUpdatePlayerStop(CCmdUI *pCmdUI)
{
	BOOL bEnabled = (CFileList::Instance()->GetCurTask() != NULL && m_bPreviewVideo);
	pCmdUI->Enable(bEnabled);

	BOOL bChecked = (CFileList::Instance()->GetCurTask() != NULL && 
		(m_wndPlayer.GetState() == State_None || ((m_wndPlayer.GetState() == State_Paused || m_wndPlayer.GetState() == State_Stopped) && m_wndPlayer.GetPosition() == 0)));
	pCmdUI->SetCheck(bChecked);
}

void CPlayerPane::OnUpdatePlayerMute(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_wndPlayer.IsMute());
}

void CPlayerPane::OnUpdatePlayerVolume(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

IMPLEMENT_OBSERVER(CPlayerPane, SUB_FILE_SEL_CHANGED)
{
//#ifdef _MENU_TEMPLATE_VIEW
//	if (GetSidePane()->m_wndSettingsTab.GetActiveTab() == TAB_MENU)
//		return;
//#endif

	PreviewVideo();
}

void CPlayerPane::PreviewVideo()
{
	m_wndPlayer.CloseFile();

	CTask *pTask = CFileList::Instance()->GetCurTask();
	if (pTask != NULL)
	{
		BOOL bHasVideo = (pTask->m_bmpFirstFrame.m_hObject != NULL);
		m_wndPlayer.SetBackgroundColor(bHasVideo ? RGB(0, 0, 0) : RGB(255, 255, 255));
		m_wndPlayer.SetBitmap(bHasVideo? pTask->m_bmpFirstFrame : CTask::m_bmpAudio, bHasVideo);
		m_bPreviewVideo = TRUE;
	}
	else
	{
		m_wndPlayer.SetBackgroundColor(RGB(0, 0, 0));
		m_wndPlayer.SetBitmap(NULL, TRUE);
		m_bPreviewVideo = FALSE;
	}

	CMFCToolBarSlider *pSlider = (CMFCToolBarSlider *)m_wndPlayerToolBar.GetButton(m_wndPlayerToolBar.CommandToIndex(ID_PLAYER_TRACK));
	if (pSlider != NULL)
		pSlider->SetPos(0);
}

#ifdef _MENU_TEMPLATE_VIEW
IMPLEMENT_OBSERVER(CPlayerPane, SUB_MENU_TEMPLATE_CHANGED)
{
	PreviewMenuTemplate();
}

IMPLEMENT_OBSERVER(CPlayerPane, SUB_CHANGE_ACTIVE_TAB_SETTINGS)
{
	/*if (GetSidePane()->m_wndSettingsTab.GetActiveTab() == TAB_MENU)
	{
		PreviewMenuTemplate();
	}
	else*/
	{
		// Exclude the condition that from Simple to Advanced, or vice versa
		if (!m_bPreviewVideo)
			PreviewVideo();
	}
}

void CPlayerPane::PreviewMenuTemplate()
{
	if (GetSidePane()->m_wndSettingsTab.GetActiveTab() == TAB_MENU)
	{
		if (m_bmpMenuTemplate.m_hObject != NULL)
			m_bmpMenuTemplate.DeleteObject();

		CMenuTemplate *pTemplate = CMenuTemplateManager::Instance()->GetCurrentTemplate();
		if (pTemplate != NULL)
		{
			m_wndPlayer.CloseFile();

			m_bPreviewVideo = FALSE;

			if (pTemplate->m_bTemplate)
			{
				CImage img;
				HRESULT hr = img.Load(pTemplate->m_strFolder + "\\Preview.jpg");
				if (FAILED(hr))
					hr = img.Load(pTemplate->m_strFolder + "\\" + pTemplate->m_strBackgroundImage);
				if (SUCCEEDED(hr))
				{
					m_bmpMenuTemplate.Attach(img.Detach());
					m_wndPlayer.SetBitmap(m_bmpMenuTemplate,TRUE);
				}
			}
			else
			{
				m_wndPlayer.SetBitmap(NULL,TRUE);
			}
		}
	}
}
#endif

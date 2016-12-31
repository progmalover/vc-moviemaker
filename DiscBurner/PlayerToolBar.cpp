// ProfileSimpleToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "PlayerToolBar.h"
#include "MFCToolBarLabel.h"
#include "MFCToolBarSlider.h"
#include "Options.h"
#include "MainFrm.h"

// CPlayerToolbar

IMPLEMENT_DYNAMIC(CPlayerToolbar, CMFCToolBarEx)

CPlayerToolbar::CPlayerToolbar()
{
	m_bInit = FALSE;
}

CPlayerToolbar::~CPlayerToolbar()
{
}


BEGIN_MESSAGE_MAP(CPlayerToolbar, CMFCToolBarEx)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// CPlayerToolbar message handlers

CSize CPlayerToolbar::CalcSize(BOOL bVertDock)
{
	CRect rc;
	GetWindowRect(&rc);
	return rc.Size();
}

void CPlayerToolbar::OnReset()
{
	CMFCToolBarEx::OnReset();

	ReplaceButton(ID_PLAYER_TRACK, CMFCToolBarSlider(ID_PLAYER_TRACK, 100));
	ReplaceButton(ID_PLAYER_VOLUME, CMFCToolBarSlider(ID_PLAYER_VOLUME, 60));

	CMFCToolBarSlider *pTrack = (CMFCToolBarSlider *)GetButton(CommandToIndex(ID_PLAYER_TRACK));
	pTrack->SetRange(0, VIDEO_TRACK_MAX);

	CMFCToolBarSlider *pVolume = (CMFCToolBarSlider *)GetButton(CommandToIndex(ID_PLAYER_VOLUME));
	pVolume->SetRange(0, 100);
	pVolume->SetPos(GetVideoPlayer()->GetVolume());

	ReplaceButton(ID_PLAYER_TIME, CMFCToolBarLabel(ID_PLAYER_TIME, ""));
}

void CPlayerToolbar::AdjustLocations()
{
	//TRACE("CPlayerToolbar::AdjustLocations()\n");

	ASSERT_VALID(this);

	if (GetSafeHwnd() == NULL || !::IsWindow(m_hWnd) || !m_bInit)
	{
		return;
	}

	CMFCToolBarEx::AdjustLocations();

	CRect rc;
	GetClientRect(&rc);
	if(rc.right<=0 || rc.left<0)
		return ;

	CMFCToolBarButton *pButton;
	CRect rcButton(0, 0, 0, 0);

	// Track
	pButton = GetButton(CommandToIndex(ID_PLAYER_TRACK));
	CRect rcTrack = pButton->Rect();
	rcTrack.right = rc.right;
	rcTrack.OffsetRect(-(rcTrack.left - rc.left), -(rcTrack.top - rc.top));
	pButton->SetRect(rcTrack);

	// Play
	pButton = GetButton(CommandToIndex(ID_PLAYER_PLAY));
	CRect rcPlay = pButton->Rect();
	rcPlay.OffsetRect(-(rcPlay.left - rc.left), -(rcPlay.top - rcTrack.bottom));
	pButton->SetRect(rcPlay);

	// Pause
	pButton = GetButton(CommandToIndex(ID_PLAYER_PAUSE));
	CRect rcPause = pButton->Rect();
	rcPause.OffsetRect(-(rcPause.left - rcPlay.right), -(rcPause.top - rcTrack.bottom));
	pButton->SetRect(rcPause);

	// Stop
	pButton = GetButton(CommandToIndex(ID_PLAYER_STOP));
	CRect rcStop = pButton->Rect();
	rcStop.OffsetRect(-(rcStop.left - rcPause.right), -(rcStop.top - rcTrack.bottom));
	pButton->SetRect(rcStop);

	// Volume
	pButton = GetButton(CommandToIndex(ID_PLAYER_VOLUME));
	CRect rcVolume = pButton->Rect();
	rcVolume.OffsetRect(-(rcVolume.left - (rc.right - rcVolume.Width()) + 1), -(rcVolume.top - rcTrack.bottom));
	pButton->SetRect(rcVolume);

	// Mute
	pButton = GetButton(CommandToIndex(ID_PLAYER_MUTE));
	CRect rcMute = pButton->Rect();
	rcMute.OffsetRect(-(rcMute.left - (rcVolume.left - rcMute.Width()) + 1), -(rcMute.top - rcTrack.bottom));
	pButton->SetRect(rcMute);

	//PlayTime
	pButton = GetButton(CommandToIndex(ID_PLAYER_TIME));
	CRect rcTime = pButton->Rect();
	rcTime.OffsetRect(-(rcTime.left - (rcStop.right + (rcMute.left - rcStop.right - rcTime.Width()) / 2)), -(rcTime.top - rcTrack.bottom));
	pButton->SetRect(rcTime);

	UpdateTooltips();
}

BOOL CPlayerToolbar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

BOOL CPlayerToolbar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pButtonTracking == NULL)
	{
		CMFCToolBarButtonEx *pButton = (CMFCToolBarButtonEx *)GetButton(CommandToIndex(ID_PLAYER_VOLUME));
		if (pButton != NULL)
			if (pButton->OnMouseWheel(nFlags, zDelta, pt))
				return TRUE;
	}

	return CMFCToolBarEx::OnMouseWheel(nFlags, zDelta, pt);
}

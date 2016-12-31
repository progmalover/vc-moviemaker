// DlgNotify.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ResizableDynamicDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define ANIMATED_RESIZING

// CResizableDynamicDialog dialog

CResizableDynamicDialog::CResizableDynamicDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CResizableDialog(nIDTemplate, pParent)
{
	m_bDefFullMode = TRUE;
	m_bFullMode = TRUE;
	m_bEnableSaveRestore = FALSE;
	m_nMinTrackSizeDelta = 0;
}

CResizableDynamicDialog::~CResizableDynamicDialog()
{
}

void CResizableDynamicDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResizableDynamicDialog, CResizableDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CResizableDynamicDialog message handlers

BOOL CResizableDynamicDialog::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	AddDynamicControls();
	AddAnchors();

	for (CONTROL_ID_MAP::iterator it = m_mapID.begin(); it != m_mapID.end(); ++it)
	{
		UINT nID = (*it).first;
		UINT nIDNext = (*it).second;
		if (nIDNext != (UINT)-1)
		{
			CWnd *pControl = GetDlgItem(nID);
			CRect rcControl;
			pControl->GetWindowRect(&rcControl);

			CWnd *pControlNext = GetDlgItem(nIDNext);
			CRect rcControlNext;
			pControlNext->GetWindowRect(&rcControlNext);

			int nHeight = rcControlNext.top - rcControl.top;

			m_nMinTrackSizeDelta += nHeight;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResizableDynamicDialog::AddAnchors()
{
}

void CResizableDynamicDialog::RemoveAnchors()
{
}

void CResizableDynamicDialog::AddDynamicControls()
{
}

void CResizableDynamicDialog::OnModeChanged()
{
}

void CResizableDynamicDialog::EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly)
{
	m_bEnableSaveRestore = TRUE;
	m_sSection = pszSection;

	BOOL bFullMode = AfxGetApp()->GetProfileInt(m_sSection, "Full Mode", m_bDefFullMode);
	if (!bFullMode)
		SetMode(bFullMode);

	CResizableDialog::EnableSaveRestore(pszSection, bRectOnly);
}

BOOL CResizableDynamicDialog::GetMode()
{
	return m_bFullMode;
}

void CResizableDynamicDialog::SetMode(BOOL bFull)
{
	if (m_bFullMode == bFull)
		return;

	if (m_mapID.size() == 0)
		return;

	m_bFullMode = bFull;

	RemoveAnchors();

	if (!m_bFullMode)
	{
		for (CONTROL_ID_MAP::iterator it = m_mapID.begin(); it != m_mapID.end(); ++it)
		{
			UINT nID = (*it).first;
			UINT nIDNext = (*it).second;
			if (nIDNext != (UINT)-1)
			{
				CWnd *pControl = GetDlgItem(nID);
				CRect rcControl;
				pControl->GetWindowRect(&rcControl);

				CWnd *pControlNext = GetDlgItem(nIDNext);
				CRect rcControlNext;
				pControlNext->GetWindowRect(&rcControlNext);

				int nHeight = rcControlNext.top - rcControl.top;

				while (pControl = pControl->GetNextWindow(GW_HWNDNEXT))
				{
					m_mapOffset[pControl] += nHeight;
				}
			}

			GetDlgItem(nID)->ShowWindow(SW_HIDE);
		}
	}

	// save before calling ResetMinTrackSize()
	MINMAXINFO mmi;
	memset(&mmi, 0, sizeof(mmi));
	SendMessage(WM_GETMINMAXINFO, 0, (LPARAM)&mmi);

	// prevent CResizableDialog from limiting the min track size
	ResetMinTrackSize();
	ResetMaxTrackSize();

	int nMaxOffset = 0;
	CWnd *pControl = GetWindow(GW_CHILD);
	while (pControl)
	{
		nMaxOffset = max(nMaxOffset, m_mapOffset[pControl]);
		pControl = pControl->GetNextWindow(GW_HWNDNEXT);
	}

#ifdef ANIMATED_RESIZING
	const int nTimes = 5;
	for (int i = 0; i < nTimes; i++)
	{
		CWnd *pControl = GetWindow(GW_CHILD);
		while (pControl)
		{
			if (m_mapOffset[pControl] > 0)
			{
				CRect rcControl;
				pControl->GetWindowRect(&rcControl);
				ScreenToClient(&rcControl);

				rcControl.top += (m_bFullMode ? m_mapOffset[pControl] : -m_mapOffset[pControl]) / nTimes;
				if (i == nTimes - 1)
					rcControl.top += (m_bFullMode ? m_mapOffset[pControl] : -m_mapOffset[pControl]) % nTimes;
				pControl->SetWindowPos(NULL, rcControl.left, rcControl.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				UpdateWindow();
			}

			pControl = pControl->GetNextWindow(GW_HWNDNEXT);
		}

		CRect rcWin;
		GetWindowRect(&rcWin);
		rcWin.bottom += (m_bFullMode ? nMaxOffset : -nMaxOffset) / nTimes;
		if (i == nTimes - 1)
			rcWin.bottom += (m_bFullMode ? nMaxOffset : -nMaxOffset) % nTimes;
		SetWindowPos(NULL, 0, 0, rcWin.Width(), rcWin.Height(), SWP_NOZORDER | SWP_NOMOVE);
		UpdateWindow();

	#ifdef _DEBUG
		CRect rcWin2;
		GetWindowRect(&rcWin2);
		ASSERT(rcWin2.Height() == rcWin.Height());
	#endif

		Sleep(10 / (i + 1));
	}
#else
	pControl = GetWindow(GW_CHILD);
	while (pControl)
	{
		if (m_mapOffset[pControl] > 0)
		{
			CRect rcControl;
			pControl->GetWindowRect(&rcControl);
			ScreenToClient(&rcControl);

			rcControl.top += (m_bFullMode ? m_mapOffset[pControl] : -m_mapOffset[pControl]);
			pControl->SetWindowPos(NULL, rcControl.left, rcControl.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			UpdateWindow();
		}

		pControl = pControl->GetNextWindow(GW_HWNDNEXT);
	}

	CRect rcWin;
	GetWindowRect(&rcWin);
	rcWin.bottom += (m_bFullMode ? nMaxOffset : -nMaxOffset);
	SetWindowPos(NULL, 0, 0, rcWin.Width(), rcWin.Height(), SWP_NOZORDER | SWP_NOMOVE);
	UpdateWindow();

#ifdef _DEBUG
	CRect rcWin2;
	GetWindowRect(&rcWin2);
	ASSERT(rcWin2.Height() == rcWin.Height());
#endif
#endif

	if(m_bFullMode)
	{
		for (CONTROL_ID_MAP::iterator it = m_mapID.begin(); it != m_mapID.end(); ++it)
		{
			UINT nID = (*it).first;
			GetDlgItem(nID)->ShowWindow(SW_SHOW);
		}

		m_mapOffset.clear();
	}

	CSize size(mmi.ptMinTrackSize.x, mmi.ptMinTrackSize.y);
	size.cy += m_bFullMode ? m_nMinTrackSizeDelta : -m_nMinTrackSizeDelta;
	SetMinTrackSize(size);

	if (!m_bFullMode)
	{
		int nMaxWidth = GetSystemMetrics(SM_CXSCREEN);
		int nMaxHeight = GetSystemMetrics(SM_CYSCREEN);
		SetMaxTrackSize(CSize(nMaxWidth, nMaxHeight - nMaxOffset));
	}

	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);

	AddAnchors();

	OnModeChanged();
}

void CResizableDynamicDialog::ToggleMode()
{
	SetMode(!m_bFullMode);
}

void CResizableDynamicDialog::OnDestroy()
{
	CResizableDialog::OnDestroy();

	// TODO: Add your message handler code here

	if (m_bEnableSaveRestore)
		AfxGetApp()->WriteProfileInt(m_sSection, "Full Mode", m_bFullMode);
}

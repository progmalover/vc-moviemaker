// MSMStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MSMStatusBar.h"
#include "FileList.h"
#include "ProfileManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static UINT indicators[] =
{
	ID_SEPARATOR,				// status line indicator
	ID_INDICATOR_TOTAL_DURATION, 
	ID_INDICATOR_TOTAL_SIZE, 
	ID_INDICATOR_TIME_LEFT, 
	ID_INDICATOR_STATUS_ICON, 
	ID_INDICATOR_PROGRESS, 
};

// CMSMStatusBar

IMPLEMENT_DYNAMIC(CMSMStatusBar, CMFCStatusBar)

CMSMStatusBar::CMSMStatusBar()
{
	m_nStatus = STATUS_INIT;
}

CMSMStatusBar::~CMSMStatusBar()
{
}


BEGIN_MESSAGE_MAP(CMSMStatusBar, CMFCStatusBar)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CMSMStatusBar message handlers

void CMSMStatusBar::Init()
{
	SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	SetPaneStyle(SP_MESSAGE, SBPS_STRETCH | SBPS_NOBORDERS);

	SetPaneStyle(SP_TOTAL_DURATION, SBPS_NORMAL);
	SetPaneWidth(SP_TOTAL_DURATION, 150);
	SetPaneText(SP_TOTAL_DURATION, "");
	SetTipText(SP_TOTAL_DURATION, LoadStringInline(IDS_TOTAL_DURATION));

	SetPaneStyle(SP_TOTAL_SIZE, SBPS_NORMAL);
	SetPaneWidth(SP_TOTAL_SIZE, 150);
	SetPaneText(SP_TOTAL_SIZE, "");
	SetTipText(SP_TOTAL_SIZE, LoadStringInline(IDS_TOTAL_SIZE));

	SetPaneStyle(SP_TIME_LEFT, SBPS_NORMAL);
	SetPaneWidth(SP_TIME_LEFT, 150);
	SetPaneText(SP_TIME_LEFT, "");
	SetTipText(SP_TIME_LEFT, LoadStringInline(IDS_OVERALL_TIME_LEFT));

	SetPaneStyle(SP_STATUS_ICON, SBPS_NOBORDERS); // cause icon redrawing problem if SBPS_NOBORDERS is not specified
	SetPaneText(SP_STATUS_ICON, ""); 
	SetPaneWidth(SP_STATUS_ICON, 0);
	SetTipText(SP_STATUS_ICON, LoadStringInline(IDS_STATUS));

	SetPaneStyle(SP_PROGRESS, SBPS_NORMAL);
	SetPaneWidth(SP_PROGRESS, 120);
	SetPaneText(SP_PROGRESS, "");
	SetTipText(SP_PROGRESS, LoadStringInline(IDS_OVERALL_PROGRESS));

	m_hIconFilm = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FILM), IMAGE_ICON, 16, 16, 0); // ask for specific icon size
	SetPaneIcon(SP_TOTAL_DURATION, m_hIconFilm);

	m_hIconClock = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CLOCK), IMAGE_ICON, 16, 16, 0); // ask for specific icon size

	m_hIconDisc = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISC), IMAGE_ICON, 16, 16, 0); // ask for specific icon size
	m_hIconDiscWarning = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DISC_WARNING), IMAGE_ICON, 16, 16, 0); // ask for specific icon size

	m_bmpIdle.LoadBitmap(IDB_STATUS_IDLE);

	if (m_imlConverting.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_STATUS_CONVERTING);
		m_imlConverting.Add(&bmp, RGB(255, 0, 255));
	}

	if (m_imlPaused.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_STATUS_PAUSED);
		m_imlPaused.Add(&bmp, RGB(255, 0, 255));
	}

	SetConvertingStatus(STATUS_IDLE);

	RecalcLayout();
}

void CMSMStatusBar::UpdateSummary()
{
	double fTotalDuration = CFileList::Instance()->GetTotalDuration();
	CString strDuration;
	strDuration.Format(IDS_TOTAL_DURATION1, FormatTime(fTotalDuration + 0.5, 0, FALSE));
	SetPaneText(SP_TOTAL_DURATION, strDuration);

	INT64 nSize = 0;
	CString strSize;
	BOOL bExceeded = FALSE;

	if (fTotalDuration > FLT_EPSILON)
	{
		CProfileManager *pManager = CProfileManager::Instance();
		if (pManager->CalclateBitrate(bExceeded, FALSE))
			nSize = pManager->CalclateSize();
	}

	strSize.Format(IDS_ESTIMATED_SIZE1, GetFileSizeString(nSize));
	SetPaneText(SP_TOTAL_SIZE, strSize);
	SetPaneIcon(SP_TOTAL_SIZE, bExceeded ? m_hIconDiscWarning : m_hIconDisc);
	SetTipText(SP_TOTAL_SIZE, LoadStringInline(bExceeded ? IDS_DISC_CAPACITY_EXCEEDED  : IDS_ESTIMATED_SIZE));
}

void CMSMStatusBar::UpdateTimeLeft(int nTimeLeft /* seconds*/)
{
	CString strLeft;

	CFileList *pFileList = CFileList::Instance();

	if (pFileList->IsConverting())
	{
		if (nTimeLeft >= 0)
		{
			strLeft.Format(IDS_OVERALL_TIME_LEFT1, FormatTime(nTimeLeft, 0, FALSE));
		}
		else
		{
			strLeft.LoadString(IDS_OVERALL_TIME_LEFT_ESTIMATING);
		}
	}

	SetPaneText(SP_TIME_LEFT, strLeft);
}

void CMSMStatusBar::SetConvertingStatus(STATUS status)
{
	if (m_nStatus == status)
		return;

	m_nStatus = status;

	switch (m_nStatus)
	{
		case STATUS_IDLE:
			SetPaneIcon(SP_TIME_LEFT, m_bmpIdle);
			SetPaneIcon(SP_STATUS_ICON, m_bmpIdle);
			EnablePaneProgressBar(SP_PROGRESS, -1, FALSE);
			break;
		case STATUS_PAUSED:
			SetPaneIcon(SP_TIME_LEFT, m_hIconClock);
			SetPaneAnimation(SP_STATUS_ICON, m_imlPaused, 500, TRUE);
			break;
		case STATUS_CONVERTING:
		{
			SetPaneIcon(SP_TIME_LEFT, m_hIconClock);
			SetPaneAnimation(SP_STATUS_ICON, m_imlConverting, 500, TRUE);
			CMFCStatusBarPaneInfo* pPane = _GetPanePtr(SP_PROGRESS);
			if (pPane->nProgressTotal == -1)
				EnablePaneProgressBar(SP_PROGRESS, 100, TRUE);
			break;
		}
		default:
			ASSERT(FALSE);
	}
}


void CMSMStatusBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPane::OnShowWindow(bShow, nStatus);

	// Chen Hao:
	// Default implementation (in CMFCStatusBar) posts AFX_WM_CHANGEVISUALMANAGER to 
	// the main frame and causes flickers when the main window is displaying for the 
	// first time on Windows 7. Windows 7 uses alpha blending to display the top-level 
	// window, which will causes many redrawing when displaying a window. (Spent a whole 
	// night to find this)
	//if (GetParentFrame () != NULL)
	//{
	//	GetParentFrame ()->PostMessage (AFX_WM_CHANGEVISUALMANAGER);
	//}
}
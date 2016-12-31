// DlgTrim.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgTrim.h"
#include "Task.h"
#include "ProfileManager.h"
#include "Subjects.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern const TCHAR *SUBTITLE_EXTS[4];

// CDlgTrim dialog

CDlgTrim::CDlgTrim(CWnd* pParent /*=NULL*/)
: CResizableDialog(CDlgTrim::IDD, pParent)
{
	m_hBmpPreview = NULL;
	m_lSample = -1;
}

CDlgTrim::~CDlgTrim()
{
	m_aviReader.Destroy();

	if (!m_strAVSFile.IsEmpty() && ::PathFileExists(m_strAVSFile))
		::DeleteFile(m_strAVSFile);

	if (m_hBmpPreview != NULL)
		::DeleteObject(m_hBmpPreview);
}

void CDlgTrim::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_stcPreview);
	DDX_Control(pDX, IDC_STATIC_SLIDER, m_wndSlider);
}


BEGIN_MESSAGE_MAP(CDlgTrim, CResizableDialog)
	ON_CONTROL(SC_POSCHANGED, IDC_STATIC_SLIDER, OnSliderPosChanged)
	ON_CONTROL(SC_SELCHANGED, IDC_STATIC_SLIDER, OnSliderSelChanged)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgTrim message handlers

BOOL CDlgTrim::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CWaitCursor wc;

	AddAnchor(IDC_STATIC_PREVIEW, TOP_LEFT, BOTTOM_RIGHT);

	AddAnchor(IDC_STATIC_DURATION_LABEL, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_DURATION, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_CURRENT_LABEL, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_CURRENT, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_START_LABEL, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_START, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_END_LABEL, TOP_RIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_END, TOP_RIGHT, TOP_RIGHT);

	AddAnchor(IDC_STATIC_SLIDER, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT, BOTTOM_RIGHT);

	CString strText;
	GetWindowText(strText);
	strText.Replace(_T("%s"), FileStripPath(m_pTask->m_strSourceFile));
	SetWindowText(strText);

	CRect rcPlayer;
	m_stcPreview.ModifyStyle(0, WS_CLIPSIBLINGS);
	m_stcPreview.GetWindowRect(&rcPlayer);
	ScreenToClient(&rcPlayer);
	m_wndPlayer.Create(AfxRegisterWndClass(0, NULL, NULL, NULL), _T("Preview"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rcPlayer, this, 1000, NULL);

	if (!InitVideoPlayer())
	{
		AfxMessageBox(IDS_E_TRIM);
		PostMessage(WM_CLOSE);
	}

	UINT nDuration = (UINT)(m_pTask->m_mi.m_fDuration * 1000);
	m_wndSlider.SetRange(0, nDuration);
	m_wndSlider.SetPos(0);

	m_wndSlider.EnableSelection(TRUE);

	if (m_pTask->m_bTrimmed)
		m_wndSlider.SetSelection((int)(m_pTask->m_fTrimStart * 1000), (int)(m_pTask->m_fTrimEnd * 1000));
	else
		m_wndSlider.SetSelection(0, nDuration);

	m_wndSlider.SetMinSelRange(1000);
	m_wndSlider.SetLineSize(10);
	m_wndSlider.SetPageSize(1000);

	OnSliderPosChanged();
	OnSliderSelChanged();

	EnableSaveRestore(_T("Windows\\Trim"), TRUE);

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTrim::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	UINT nDuration = (UINT)(m_pTask->m_mi.m_fDuration * 1000);

	int nStart, nEnd;
	m_wndSlider.GetSelection(nStart, nEnd);

	m_pTask->m_bTrimmed = (nStart > 0 || nEnd < nDuration);

	if (m_pTask->m_bTrimmed)
	{
		m_pTask->m_fTrimStart = (double)nStart / 1000;
		m_pTask->m_fTrimEnd = (double)nEnd / 1000;
	}
	else
	{
		m_pTask->m_fTrimStart = 0.0;
		m_pTask->m_fTrimEnd = m_pTask->m_mi.m_fDuration;
	}

	CSubjectManager::Instance()->GetSubject(SUB_VIDEO_TRIMMED)->Notify((void *)m_pTask);

	CResizableDialog::OnOK();
}

void CDlgTrim::OnSliderPosChanged()
{
	float fTime = (float)m_wndSlider.GetPos() / 1000;
	TRACE(_T("OnSliderPosChanged(): %.3f\n"), fTime);

	CString strCurrent;
	strCurrent.Format(_T(" %s"), FormatTime(fTime, 3, FALSE));
	SetDlgItemText(IDC_STATIC_CURRENT, strCurrent);

	m_wndPlayer.SeekTo((LONGLONG)(fTime * UNITS), TRUE);
}

void CDlgTrim::OnSliderSelChanged()
{
	int nStart, nEnd;
	m_wndSlider.GetSelection(nStart, nEnd);

	float fStart = (float)nStart / 1000;
	CString strStart = _T(" ") + FormatTime(fStart, 3, FALSE);
	SetDlgItemText(IDC_STATIC_START, strStart);

	float fEnd = (float)nEnd / 1000;
	CString strEnd = _T(" ") + FormatTime(fEnd, 3, FALSE);
	SetDlgItemText(IDC_STATIC_END, strEnd);
}

BOOL CDlgTrim::InitVideoPlayer()
{
	m_strAVSFile.Format(_T("%s\\Trim.avs"), m_pTask->GetTempFolder());
	ASSERT(!PathFileExists(m_strAVSFile));

	BOOL bUnknownFps = (fabs(m_pTask->m_mi.m_fFrameRate - 0.0) < FLT_EPSILON);
	float fFramerate = min(MAX_FRAMERATE, max(MIN_FRAMERATE, (bUnknownFps ? DEF_FRAMERATE : m_pTask->m_mi.m_fFrameRate)));

	CStringA strScript;
	try
	{
		CString strTemplate;
		strTemplate.Format(_T("%s\\Scripts\\Trim.avs"), GetModuleFilePath());

		CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
		ULONGLONG len = file.GetLength();
		file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
		strScript.ReleaseBuffer();

		strScript.Replace("$(modulepath)", CT2A(GetModuleFilePath()));
		strScript.Replace("$(input)", CT2A(m_pTask->m_strSourceFileAlt.IsEmpty() ? m_pTask->m_strSourceFile : m_pTask->m_strSourceFileAlt));

		CString strSubtitle;
		m_pTask->FindSubtitle(strSubtitle);

		strScript.Replace("$(subtitle)", CT2A(strSubtitle));

		CStringA strFramerate;
		strFramerate.Format("%.3f", fFramerate);
		strScript.Replace("$(vframerate)", strFramerate);

		strScript.Replace("$(seekzero)", bUnknownFps ? "true" : "false");

		ASSERT(strScript.Find('$') < 0);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	try
	{
		CStdioFile file(m_strAVSFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(strScript, strlen(strScript));
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	if (m_wndPlayer.OpenFile(m_strAVSFile))
	{
		m_wndPlayer.Play();
		m_wndPlayer.StopWhenReady();

		if (m_pTask->m_bmpFirstFrame.m_hObject == NULL)
		{
			// No video stream. Display a audio icon.
			m_stcPreview.ModifyStyle(SS_GRAYRECT, SS_WHITERECT);
			m_wndPlayer.SetBackgroundColor(RGB(255, 255, 255));
			m_wndPlayer.SetBitmap(CTask::m_bmpAudio, FALSE);
		}

		CString strDuration;
		strDuration.Format(_T(" %s"), FormatTime(m_pTask->m_mi.m_fDuration, 3, FALSE));
		SetDlgItemText(IDC_STATIC_DURATION, strDuration);

		RepositionControls();

		return TRUE;
	}

	return FALSE;
}

void CDlgTrim::RepositionControls()
{
	CRect rcPlacehold;
	m_stcPreview.ModifyStyle(0, WS_CLIPSIBLINGS);
	m_stcPreview.GetWindowRect(&rcPlacehold);
	ScreenToClient(&rcPlacehold);
	rcPlacehold.InflateRect(-5, -5);

	CRect rcPlayer;
	int nWidth, nHeight;
	if (m_pTask->m_bmpFirstFrame.m_hObject != NULL)
	{
		nHeight = m_pTask->m_mi.m_nHeight;
		nWidth = (int)(m_pTask->m_mi.m_nHeight * m_pTask->m_mi.m_fDisplayRatio + 0.5);
	}
	else
	{
		BITMAP bm;
		CTask::m_bmpAudio.GetBitmap(&bm);
		nHeight = bm.bmHeight;
		nWidth = bm.bmWidth;
	}

	if (nWidth < rcPlacehold.Width() && nHeight < rcPlacehold.Height())
	{
		rcPlayer.left = rcPlacehold.left + (rcPlacehold.Width() - nWidth) / 2;
		rcPlayer.right = rcPlayer.left + nWidth;
		rcPlayer.top = rcPlacehold.top + (rcPlacehold.Height() - nHeight) / 2;
		rcPlayer.bottom = rcPlayer.top + nHeight;
	}
	else
	{
		if (m_pTask->m_mi.m_fDisplayRatio > (float)rcPlacehold.Width() / rcPlacehold.Height())
		{
			int nWidthNew = rcPlacehold.Width();
			int nHeightNew = (int)(nWidthNew / m_pTask->m_mi.m_fDisplayRatio + 0.5);

			rcPlayer.left = rcPlacehold.left;
			rcPlayer.right = rcPlayer.left + nWidthNew;
			rcPlayer.top = rcPlacehold.top + (rcPlacehold.Height() - nHeightNew) / 2;
			rcPlayer.bottom = rcPlayer.top + nHeightNew;
		}
		else
		{
			int nHeightNew = rcPlacehold.Height();
			int nWidthNew = (int)(nHeightNew * m_pTask->m_mi.m_fDisplayRatio + 0.5);

			rcPlayer.left = rcPlacehold.left + (rcPlacehold.Width() - nWidthNew) / 2;
			rcPlayer.right = rcPlayer.left + nWidthNew;
			rcPlayer.top = rcPlacehold.top;
			rcPlayer.bottom = rcPlayer.top + nHeightNew;
		}
	}

	m_wndPlayer.SetWindowPos(&wndTop, rcPlayer.left, rcPlayer.top, rcPlayer.Width(), rcPlayer.Height(), 0);
}

void CDlgTrim::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (::IsWindow(m_wndPlayer.m_hWnd))
		RepositionControls();
}


// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Converter.h"
#include "ChildView.h"
#include "FileList.h"
#include "Options.h"
#include "PlayerPane.h"
#include "PaneAdvanced.h"
#include "PaneSimple.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT ID_FILE_LIST = 1001;

// CChildView

CChildView::CChildView()
{
	m_bInit = FALSE;
}

CChildView::~CChildView()
{
	CFileList::ReleaseInstance();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages

#ifdef _DEBUG
	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(&rc, CBrush::FromHandle((HBRUSH)::GetStockObject(WHITE_BRUSH)));
#endif
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CFileList::Instance()->CreateEx(0, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 
		CRect(0, 0, 0, 0), this, ID_FILE_LIST);

	COptions::Instance()->UpdateULStyle();

	CFileList::Instance()->Init();

	if(!CPlayerPane::Instance()->Create(AfxRegisterWndClass(0), "Player", 
				WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect(0,0,0,0), this, 1004, 0))
				return -1;



	if (!CPaneSimple::Instance()->Create(AfxRegisterWndClass(0), "Simple", 
				WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect(0,0,0,0), this, 1001, 0))
	{
		TRACE0("Failed to create simple pane\n");
		return -1;      // fail to create
	}

	//CPaneSimple
	m_bInit = TRUE;

	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (m_bInit)
		RecalcLayout();
}

void CChildView::RecalcLayout()
{
	CRect rc;
	GetClientRect(&rc);
	
	CRect rcCtrl = rc;
	rcCtrl.bottom -= 200;
	CFileList::Instance()->MoveWindow(&rcCtrl);

	rc.top = rc.bottom - 200;
	int right = rc.right;
	//if(rc.right >= 200)
	rc.right = 200;
	CPlayerPane::Instance()->MoveWindow(rc);

	rc.left = 200;
	rc.right = right;
	CPaneSimple::Instance()->MoveWindow(rc);
}

void CChildView::OnSetFocus(CWnd* pOldWnd)
{
	CFileList::Instance()->SetFocus();
}

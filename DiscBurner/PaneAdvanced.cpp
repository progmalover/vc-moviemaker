// ProfilePane.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "PaneAdvanced.h"
#include "FileList.h"

const UINT ID_PROP_LIST = 1000;

// CPaneAdvanced

IMPLEMENT_DYNAMIC(CPaneAdvanced, CBasePane)
IMPLEMENT_SINGLETON(CPaneAdvanced)
CPaneAdvanced::CPaneAdvanced()
{

}

CPaneAdvanced::~CPaneAdvanced()
{
}


BEGIN_MESSAGE_MAP(CPaneAdvanced, CBasePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CPaneAdvanced message handlers



int CPaneAdvanced::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBasePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CRect rc(0, 0, 0, 0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rc, this, ID_PROP_LIST))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	//m_wndPropList.LoadProfile();

	return 0;
}

void CPaneAdvanced::OnSize(UINT nType, int cx, int cy)
{
	CBasePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	RepositionControls();
}

void CPaneAdvanced::RepositionControls()
{
	if (::IsWindow(m_hWnd))
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		//rcClient.InflateRect(-2, -2, -2, -2);
		m_wndPropList.SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BOOL CPaneAdvanced::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (CFileList::Instance()->IsConverting())
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		return TRUE;
	}
	return CBasePane::OnSetCursor(pWnd, nHitTest, message);
}

// MFCToolBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "MFCToolBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCToolBarEx

IMPLEMENT_DYNCREATE(CMFCToolBarEx, CMFCToolBar)

CMFCToolBarEx::CMFCToolBarEx()
{
	m_pButtonTracking = NULL;
}


CMFCToolBarEx::~CMFCToolBarEx()
{
}

BEGIN_MESSAGE_MAP(CMFCToolBarEx, CMFCToolBar)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCToolBarEx message handlers

int CMFCToolBarEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMFCToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CMFCToolBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFCToolBar::OnLButtonDown(nFlags, point);

	// TODO: Add your message handler code here and/or call default

	int index = HitTest(point);
	if (index >= 0)
	{
		CMFCToolBarButtonEx *pButton = (CMFCToolBarButtonEx *)GetButton(index);
		if (pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarButtonEx)) && !pButton->IsDisabled())
		{
			m_pButtonTracking = pButton;
			pButton->OnLButtonDown(point);
		}
	}
}

void CMFCToolBarEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMFCToolBar::OnLButtonUp(nFlags, point);

	// TODO: Add your message handler code here and/or call default

	m_pButtonTracking = NULL;

	int index = HitTest(point);
	if (index >= 0)
	{
		CMFCToolBarButtonEx *pButton = (CMFCToolBarButtonEx *)GetButton(index);
		if (pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarButtonEx)) && !pButton->IsDisabled())
		{
			pButton->OnLButtonUp(point);
		}
	}
}

void CMFCToolBarEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_pButtonTracking != NULL)
	{
		m_pButtonTracking->OnMouseMove(point);
	}
	else
	{
		CMFCToolBar::OnMouseMove(nFlags, point);

		int index = HitTest(point);
		if (index >= 0)
		{
			CMFCToolBarButtonEx *pButton = (CMFCToolBarButtonEx *)GetButton(index);
			if (pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarButtonEx)) && !pButton->IsDisabled())
			{
				pButton->OnMouseMove(point);
			}
		}
	}
}

void CMFCToolBarEx::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	CMFCToolBar::OnCaptureChanged(pWnd);

	if (m_pButtonTracking != NULL)
		m_pButtonTracking = NULL;
}

BOOL CMFCToolBarEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (m_pButtonTracking == NULL)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		int index = HitTest(point);
		if (index >= 0)
		{
			CMFCToolBarButtonEx *pButton = (CMFCToolBarButtonEx *)GetButton(index);
			if (pButton != NULL && pButton->IsKindOf(RUNTIME_CLASS(CMFCToolBarButtonEx)) && !pButton->IsDisabled())
			{
				if (pButton->OnMouseWheel(nFlags, zDelta, pt))
					return TRUE;
			}
		}
	}

	return CMFCToolBar::OnMouseWheel(nFlags, zDelta, pt);
}

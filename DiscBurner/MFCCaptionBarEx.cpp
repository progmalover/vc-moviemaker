// MFCCaptionBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCCaptionBarEx.h"
#include "MainFrm.h"

#include "..\Activation/Activation.h"

// CMFCCaptionBarEx

IMPLEMENT_DYNAMIC(CMFCCaptionBarEx, CMFCCaptionBar)

CMFCCaptionBarEx::CMFCCaptionBarEx()
{

}

CMFCCaptionBarEx::~CMFCCaptionBarEx()
{
}


BEGIN_MESSAGE_MAP(CMFCCaptionBarEx, CMFCCaptionBar)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CMFCCaptionBarEx message handlers


void CMFCCaptionBarEx::OnDrawText(CDC* pDC, CRect rect, const CString& strText)
{
	ASSERT_VALID(pDC);

	if (m_arTextParts.GetSize() == 1)
	{
		pDC->DrawText(strText, rect, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
		return;
	}

	int x = rect.left;

	BOOL bIsBold = FALSE;

	for (int i = 0; i < m_arTextParts.GetSize(); i++)
	{
		if (!m_arTextParts [i].IsEmpty())
		{
			CFont* pOldFont = NULL;
			COLORREF clrOld;

			if (bIsBold)
			{
				pOldFont = pDC->SelectObject(&afxGlobalData.fontUnderline);
				clrOld = pDC->SetTextColor(RGB(0, 0, 255));
			}

			CRect rectPart = rect;
			rectPart.left = x;

			CSize sizePart = pDC->GetTextExtent(m_arTextParts [i]);

			pDC->DrawText(m_arTextParts [i], rectPart, DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);

			if (pOldFont != NULL)
			{
				pDC->SelectObject(pOldFont);
				pDC->SetTextColor(clrOld);
			}

			x += sizePart.cx;
		}

		bIsBold = !bIsBold;
	}
}

BOOL CMFCCaptionBarEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));

	return TRUE;
}

void CMFCCaptionBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCCaptionBar::OnLButtonDown(nFlags, point);

	CDlgEnterKey dlg;
	if (dlg.DoModal() == IDOK)
	{
#ifdef _DVD
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		if (::IsWindow(pFrame->m_wndMessageBar.m_hWnd))
			pFrame->m_wndMessageBar.ShowPane(FALSE, FALSE, FALSE);
#endif
	}
}

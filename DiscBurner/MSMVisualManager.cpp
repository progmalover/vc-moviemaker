// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
// MSMVisualManager.cpp : implementation file
//

#include "stdafx.h"
#include "MSMVisualManager.h"

#include "MSMMenuBar.h"
#include "MainToolBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMSMVisualManager, CMFCVisualManagerBase)

/////////////////////////////////////////////////////////////////////////////
// CMSMVisualManager

BOOL _StretchBitmap (CDC* pDC, const CRect& rectClient, CBitmap& bmp)
{
	ASSERT_VALID (pDC);

	if (bmp.GetSafeHandle () != NULL)
	{
		BITMAP bm;
		ZeroMemory (&bm, sizeof (BITMAP));

		bmp.GetBitmap (&bm);
		
		CDC dc;
		dc.CreateCompatibleDC (pDC);

		// Select the bitmap into the in-memory DC
		CBitmap* pOldBitmap = dc.SelectObject (&bmp);

		int nOldStretchBltMode = pDC->SetStretchBltMode (COLORONCOLOR);

		pDC->StretchBlt(0, 0, rectClient.Width (), rectClient.Height (), &dc, 
						0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);


		dc.SelectObject (pOldBitmap);

		pDC->SetStretchBltMode (nOldStretchBltMode);

		return true;
	}

	return false;
}

int CalcToolBarComboBoxButtonWidth(CDC *pDC, CMFCToolBarComboBoxButton *pButton)
{
	int nWidth = 0;

	CFont *pOldFont = pDC->SelectObject(&afxGlobalData.fontRegular);


	int nCount = pButton->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		LPCTSTR lpsz = pButton->GetItem(i);
		nWidth = max(nWidth, pDC->GetTextExtent(lpsz).cx);
	}

	// Add margin space to the calculations
	nWidth += pDC->GetTextExtent(_T("0")).cx + CMenuImages::Size().cx * 2;

	pDC->SelectObject(pOldFont);

	return nWidth;
}

void _DrawReflectedGradient (CDC* pDC, COLORREF clrS, COLORREF clrM, COLORREF clrF,
							 const CRect& rect, BOOL bHorz)
{
	ASSERT_VALID (pDC);

	CDrawingManager dm (*pDC);

	long x1 = rect.left;
	long x2 = rect.right;
	long y1 = rect.top;
	long y2 = rect.bottom;

	if (bHorz)
	{
		CRect rt (x1, y1, x2, (y1 + y2) / 2);
		dm.FillGradient (rt, clrM, clrS, TRUE);
		rt.top = rt.bottom;
		rt.bottom = y2;
		dm.FillGradient (rt, clrF, clrM, TRUE);
	}
	else
	{
		CRect rt (x1, y1, (x1 + x2) / 2, y2);
		dm.FillGradient (rt, clrS, clrM, FALSE);
		rt.left  = rt.right;
		rt.right = x2;
		dm.FillGradient (rt, clrM, clrF, FALSE);
	}
}

CMSMVisualManager::CMSMVisualManager()
{
	CMFCVisualManagerOffice2007::Style nStyle = CMFCVisualManagerOffice2007::Office2007_Silver;

	m_Style = (CMFCVisualManagerOffice2007::Style)max(CMFCVisualManagerOffice2007::Office2007_LunaBlue, min(CMFCVisualManagerOffice2007::Office2007_Silver, AfxGetApp()->GetProfileInt(_T("Options\\Interface"), _T("Visual Style"), nStyle)));

	SetStyleFixed(m_Style);

	SetMenuShadowDepth(0);

	m_bParentActive = FALSE;

	//m_MemoryFilled = false;
	//m_MemorySize   = CSize (0, 0);

	/*
	m_brMenuBarButtonHighlighted.CreateSolidBrush (RGB ( 57, 134, 206));
	m_clrMenuBarButtonBorderHighlighted = RGB ( 57, 134, 206);

	m_clrMenuBarButtonTextRegular       = RGB (255, 255, 255);
	m_clrMenuBarButtonTextPressed       = RGB (255, 255, 255);
	m_clrMenuBarButtonTextHighlighted   = RGB (255, 255, 255);

	m_clrToolBarButtonTextRegular       = RGB (255, 255, 255);
	m_clrToolBarButtonTextPressed       = RGB (255, 223, 127);
	m_clrToolBarButtonTextHighlighted   = RGB (255, 223, 127);

	m_clrCategorieBarButtonTextRegular	= RGB (232, 243, 253);
	m_clrCategorieBarButtonTextPressed	= RGB (255, 223, 127);
	m_clrCategorieBarButtonTextHighlighted
										= RGB (255, 223, 127);

	m_clrToolBarSeparatorS              = RGB (119, 162, 211);
	m_clrToolBarSeparatorM              = RGB (255, 255, 255);
	m_clrToolBarSeparatorF              = RGB ( 85, 136, 198);

	m_clrComboBoxBorder					= RGB ( 78, 122, 171);

	m_clrComboBoxBtnRegularF			= RGB (208, 220, 252);
	m_clrComboBoxBtnRegularL			= RGB (182, 205, 251);
	m_clrComboBoxBtnRegularBorder		= RGB (177, 197, 245);
	m_clrComboBoxBtnHighlightedF		= RGB (226, 244, 254);
	m_clrComboBoxBtnHighlightedL		= RGB (203, 219, 252);
	m_clrComboBoxBtnHighlightedBorder	= RGB (140, 165, 225);
	m_clrComboBoxBtnPressedF			= RGB (122, 153, 242);
	m_clrComboBoxBtnPressedL			= RGB (159, 178, 235);
	m_clrComboBoxBtnPressedBorder		= RGB (181, 198, 242);

	m_clrCategorieBarButtonBorder       = RGB ( 53,  94, 159);
	m_clrCategorieBarButtonBorderM      = RGB (191, 209, 231);
	m_clrCategorieBarButtonLine         = RGB (123, 165, 210);
	m_clrCategorieBarButtonF            = RGB ( 77, 127, 193);
	m_clrCategorieBarButtonL            = RGB ( 40,  74, 144);

	m_brLinksBarBack.CreateSolidBrush (RGB ( 49,  85, 153));

	m_brLinksBarButtonHighlighted.CreateSolidBrush (RGB ( 38,  69, 135));
	m_clrLinksBarButtonBorderHighlighted
										= RGB ( 38,  69, 135);
										*/
}

void CMSMVisualManager::OnUpdateSystemColors ()
{
	CMFCVisualManagerBase::OnUpdateSystemColors();

	/*
	m_clrMenuLight						= RGB (210, 232, 253);
	m_brMenuLight.DeleteObject ();
	m_brMenuLight.CreateSolidBrush (m_clrMenuLight);

	m_clrHighlight						= RGB (152, 192, 235);
	m_brHighlight.DeleteObject ();
	m_brHighlight.CreateSolidBrush (m_clrHighlight);

	m_clrHighlightDn					= RGB (152, 192, 235);
	m_brHighlightDn.DeleteObject ();
	m_brHighlightDn.CreateSolidBrush (m_clrHighlightDn);

	m_clrHighlightChecked				= RGB (152, 192, 235);
	m_brHighlightChecked.DeleteObject ();
	m_brHighlightChecked.CreateSolidBrush (m_clrHighlightChecked);

	m_clrMenuItemBorder 				= RGB (152, 192, 235);

	m_clrMenuBorder 					= RGB (128, 128, 128);
	*/
}

CMSMVisualManager::~CMSMVisualManager()
{
}

BOOL __stdcall CMSMVisualManager::SetStyleFixed(Style style, LPCTSTR lpszPath)
{
	if (CMFCVisualManagerBase::SetStyle(style, lpszPath))
	{
		// VERY IMPORTANT! Base class's implementation will set m_bAutoFreeRes = TRUE, so when using a 
		// resource DLL, the resource DLL will be freed after calling SetStyle().
		m_bAutoFreeRes = FALSE;
		AfxGetApp()->WriteProfileInt(_T("Options\\Interface"), _T("Visual Style"), m_Style);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CMSMVisualManager::SetupMemoryBitmapSize (CDC* pDC, const CSize& size)
{
	if (pDC == NULL && size == CSize (0, 0))
	{
		return;
	}

	if (m_MemoryDC.GetSafeHdc () != NULL)
	{
		if (pDC->GetDeviceCaps (BITSPIXEL) != m_MemoryDC.GetDeviceCaps (BITSPIXEL))
		{
			m_MemoryBitmap.DeleteObject ();
			m_MemoryDC.DeleteDC ();
		}
	}

	if (m_MemoryDC.GetSafeHdc () == NULL)
	{
		m_MemoryDC.CreateCompatibleDC (pDC);
	}

	if (m_MemorySize != size || m_MemoryBitmap.GetSafeHandle () == NULL)
	{
		m_MemorySize = size;

		if (m_MemoryBitmap.GetSafeHandle () != NULL)
		{
			m_MemoryBitmap.DeleteObject ();
		}

		m_MemoryBitmap.CreateCompatibleBitmap (pDC, m_MemorySize.cx, m_MemorySize.cy);
		m_MemoryDC.SelectObject (m_MemoryBitmap);

		m_MemoryFilled = false;
	}

	if (m_MSMFrameBitmap.GetSafeHandle () != NULL)
	{
		m_MemoryFilled = _StretchBitmap (&m_MemoryDC, CRect (CPoint (0, 0), m_MemorySize), m_MSMFrameBitmap);
	}
}

void CMSMVisualManager::OnFillBarBackground (CDC* pDC, CBasePane* pBar,
											 CRect rectClient, CRect rectClip, BOOL bNCArea)
{
	BOOL bDefault = TRUE;

	//if (m_MSMFrameBitmap.GetSafeHandle () == NULL)
	//{
	//	m_MSMFrameBitmap.LoadBitmap (_T("IDB_MSM_FRAME_BKGND"));

	//	if (!m_MemoryFilled)
	//	{
	//		m_MemoryFilled = 
	//			_StretchBitmap (&m_MemoryDC, CRect (CPoint (0, 0), m_MemorySize), m_MSMFrameBitmap);
	//	}
	//}

	//if (rectClip.IsRectEmpty ())
	//{
	//	rectClip = rectClient;
	//}

	CRuntimeClass* pBarClass = pBar->GetRuntimeClass ();

	/*
	if (pBarClass == NULL || pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pDC->FillRect(&rectClient, &m_brFace);
		return;
	}

	if (pBarClass != NULL &&
		(pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMSMMenuBar)) ||
		 pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMainToolBar))
		))
	{
		if (!pBar->IsFloating ())
		{
			CRect rt (rectClient);

			CWnd* pWnd = pBar->GetTopLevelFrame ();
			if (pWnd != NULL)
			{
				CRect rtW;
				pBar->GetWindowRect (rtW);
				pWnd->ScreenToClient (rtW);

				rt.OffsetRect (rtW.TopLeft ());
			}

			int nOldStretchBltMode = pDC->SetStretchBltMode (COLORONCOLOR);

			pDC->BitBlt(rectClient.left, rectClient.top,
						rectClient.Width (), rectClient.Height (), 
						&m_MemoryDC, 
						rt.left, rt.top, 
						SRCCOPY);

			pDC->SetStretchBltMode (nOldStretchBltMode);

			bDefault = FALSE;
		}
	}

	if (pBarClass->IsDerivedFrom (RUNTIME_CLASS (CProfilePane)) || 
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CProfilePaneSimple)) ||
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CProfilePaneAdvanced)) || 
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CProfileSimpleToolBar)))
	{
		CDrawingManager dm (*pDC);
		dm.FillGradient (rectClient, m_clrToolBarGradientDark, m_clrToolBarGradientLight, TRUE);
		bDefault = FALSE;
	}
	*/

	if (/*pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMFCMenuBar)) ||*/
		//pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMFCPopupMenuBar))|| 
		//pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMFCPopupMenu)) || 
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CMFCToolBarEx)) || 
		pBarClass->IsDerivedFrom (RUNTIME_CLASS (CDockablePane)))
	{
		CDrawingManager dm (*pDC);
		dm.FillGradient (rectClient, CMFCVisualManagerBase::m_clrTaskPaneGradientDark, CMFCVisualManagerBase::m_clrTaskPaneGradientDark, FALSE);

		if (bNCArea)
		{
			CPen penLeft (PS_SOLID, 1, CMFCVisualManagerBase::m_clrTaskPaneGroupCaptionDark);
			CPen* pOldPen = pDC->SelectObject (&penLeft);

			pDC->MoveTo (rectClient.left, rectClient.bottom - 1);
			pDC->LineTo (rectClient.right, rectClient.bottom - 1);

			pDC->SelectObject (pOldPen);
		}

		return;
	}

	if (bDefault)
	{
		CMFCVisualManagerBase::OnFillBarBackground (pDC, pBar, rectClient, rectClip, bNCArea);
	}
}

COLORREF CMSMVisualManager::GetToolbarButtonTextColor (CMFCToolBarButton* pButton,
												CMFCVisualManager::AFX_BUTTON_STATE state)
{
	COLORREF clr = COLORREF (-1);

	/*
	CMFCToolBarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CMFCToolBarMenuButton, pButton);

	BOOL bIsMenuButton = pMenuButton != NULL;

	BOOL bIsPopupMenu = bIsMenuButton &&
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar));

	if (bIsMenuButton && !bIsPopupMenu)
	{
		if (IsParentActive())
		{
			switch (state)
			{
			case ButtonsIsRegular:
				clr = m_clrMenuBarButtonTextRegular;
				break;
			case ButtonsIsPressed:
				clr = m_clrMenuBarButtonTextPressed;
				break;
			case ButtonsIsHighlighted:
				clr = m_clrMenuBarButtonTextHighlighted;
				break;
			}
		}
		else
		{
			clr = RGB(221, 221, 221);
		}
	}
	*/

	if (clr == COLORREF (-1))
		return CMFCVisualManagerBase::GetToolbarButtonTextColor (pButton, state);
	else
		return clr;
}

void CMSMVisualManager::OnFillHighlightedArea (CDC* pDC, CRect rect, 
		CBrush* pBrush, CMFCToolBarButton* pButton)
{
	/*
	if (pButton == NULL)
	{
		CMFCVisualManagerBase::OnFillHighlightedArea (pDC, rect,
												pBrush, pButton);
		return;
	}
	*/

	ASSERT_VALID (pDC);
	ASSERT_VALID (pBrush);

	CMFCVisualManagerBase::OnFillHighlightedArea (pDC, rect, pBrush, pButton);

	/*
	BOOL bDefault = TRUE;

	CMFCToolBarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CMFCToolBarMenuButton, pButton);

	BOOL bIsMenuButton = pMenuButton != NULL;

	BOOL bIsPopupMenu = bIsMenuButton &&
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar));
	
	CBrush* pBr = pBrush;

	bIsMenuButton = bIsMenuButton && 
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCMenuBar));

	if (bIsMenuButton || bIsPopupMenu)
	{
		if (!bIsPopupMenu)
		{
			pBr = &m_brMenuBarButtonHighlighted;
		}
	}
	else if (pButton->IsKindOf (RUNTIME_CLASS (CMSMLinksBarButton)))
	{
		pBr = &m_brLinksBarButtonHighlighted;
		rect.InflateRect (0, 0, 1, 1);
	}
	else
	{
		bDefault = FALSE;
	}

	if (bDefault)
	{
		CMFCVisualManagerOfficeXP::OnFillHighlightedArea (pDC, rect, pBr, pButton);
		return;
	}
	else if (pButton->IsKindOf (RUNTIME_CLASS (CMSMCategoryBarButton)))
	{
		if ((pButton->m_nStyle & TBBS_CHECKED) != 0)
		{
			CDrawingManager dm(*pDC);
	
			dm.FillGradient (rect,
				m_clrCategorieBarButtonL, m_clrCategorieBarButtonF, TRUE);
		}
	}
	else
	{
		if (pMenuButton != NULL &&
			pMenuButton->GetParentWnd () != NULL &&
			pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMSMToolBar)))
		{
			rect.InflateRect (0, 0, 1, 1);

			CDrawingManager dm (*pDC);

			dm.HighlightRect (rect, 10, (COLORREF)-1, 0, RGB (255, 255, 255));
		}
	}

	if (bDefault)
		CMFCVisualManagerBase::OnFillHighlightedArea (pDC, rect, pBrush, pButton);
		*/
}

void CMSMVisualManager::OnDrawMenuBorder (CDC* pDC, CMFCPopupMenu *pMenu, CRect rect)
{
	//ASSERT_VALID (pDC);

	//pDC->Draw3dRect (rect, m_clrMenuBorder, m_clrMenuBorder);

	//rect.DeflateRect (1, 1);
	//pDC->Draw3dRect (rect, m_clrMenuLight, m_clrMenuLight);

	//CRect rectLeft (1, 1, 2, rect.bottom - 1);
	//pDC->FillRect (rectLeft, &m_brBarBkgnd);

	CMFCVisualManagerBase::OnDrawMenuBorder (pDC, pMenu, rect);
}

void CMSMVisualManager::OnDrawButtonBorder (CDC* pDC,
		CMFCToolBarButton* pButton, CRect rect, AFX_BUTTON_STATE state)
{
	/*
	ASSERT_VALID (pDC);
	ASSERT_VALID (pButton);

	if (state != ButtonsIsPressed && state != ButtonsIsHighlighted)
	{
		ASSERT ((pButton->m_nStyle & TBBS_CHECKED) == 0);
		return;
	}

	CMFCToolBarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CMFCToolBarMenuButton, pButton);

	BOOL bIsMenuButton = pMenuButton != NULL;

	BOOL bIsPopupMenu = bIsMenuButton &&
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar));

	bIsMenuButton = bIsMenuButton &&
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCMenuBar));

	if (bIsMenuButton || bIsPopupMenu)
	{
		COLORREF clrBorder = m_clrMenuItemBorder;

		switch (state)
		{
		case ButtonsIsPressed:
		case ButtonsIsHighlighted:
			clrBorder = bIsPopupMenu 
				? m_clrMenuItemBorder
				: m_clrMenuItemBorder;
			break;
		}


		if (!bIsPopupMenu || state != ButtonsIsHighlighted)
		{
			rect.right--;
			rect.bottom--;
		}

		if (bIsMenuButton && !bIsPopupMenu && 
			pMenuButton->IsDroppedDown ())
		{
			CMFCPopupMenu* pPopupMenu= pMenuButton->GetPopupMenu ();
			if (pPopupMenu != NULL && 
				(pPopupMenu->IsWindowVisible () || pPopupMenu->IsShown()))
			{
				ExtendMenuButton (pMenuButton, rect);

				BOOL bRTL = pPopupMenu->GetExStyle() & WS_EX_LAYOUTRTL;

				if (m_bShdowDroppedDownMenuButton && !bRTL && 
					CMFCMenuBar::IsMenuShadows () &&
					!CMFCToolBar::IsCustomizeMode () &&
					afxGlobalData.m_nBitsPerPixel > 8 &&
					!afxGlobalData.m_bIsBlackHighContrast &&
					!pPopupMenu->IsRightAlign ())
				{
					CDrawingManager dm (*pDC);

					dm.DrawShadow (rect, m_nMenuShadowDepth, 100, 75, NULL, NULL,
						m_clrMenuShadowBase);
				}
			}
		}

		if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
		{
			if (bIsPopupMenu && (pButton->m_nStyle & TBBS_CHECKED))
			{
				rect.bottom ++;
			}

			pDC->Draw3dRect (rect, clrBorder, clrBorder);
		}
	}
	else
	{
		if (pMenuButton != NULL &&
			pMenuButton->GetParentWnd () != NULL &&
			pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMainToolBar)))
		{
			CRect rt (rect);

			CDrawingManager dm (*pDC);

			rt.bottom = rt.top + 1;
			dm.HighlightRect (rt, 15, (COLORREF)-1, 0, RGB (255, 255, 255));

			rt.bottom = rect.bottom;
			rt.top    = rt.bottom - 1;
			dm.HighlightRect (rt, 15, (COLORREF)-1, 0, RGB (255, 255, 255));

			rt.top    = rect.top    + 1;
			rt.bottom = rect.bottom - 1;
			rt.right  = rt.left + 1;
			dm.HighlightRect (rt, 15, (COLORREF)-1, 0, RGB (255, 255, 255));

			rt.right  = rect.right;
			rt.left   = rt.right - 1;
			dm.HighlightRect (rt, 15, (COLORREF)-1, 0, RGB (255, 255, 255));
		}
		else
		{
			// TODO: paint toolbar buttons
			//CMFCVisualManagerBase::OnDrawButtonBorder (pDC, pButton, rect, state);
		}
	}
	*/

	CMFCVisualManagerBase::OnDrawButtonBorder (pDC, pButton, rect, state);
}

void CMSMVisualManager::OnHighlightMenuItem (CDC*pDC, CMFCToolBarMenuButton* pButton,
											CRect rect, COLORREF& clrText)
{
	/*
	CMFCToolBarMenuButton* pMenuButton = 
		DYNAMIC_DOWNCAST (CMFCToolBarMenuButton, pButton);

	BOOL bIsMenuButton = pMenuButton != NULL;

	BOOL bIsPopupMenu = bIsMenuButton &&
		pMenuButton->GetParentWnd () != NULL &&
		pMenuButton->GetParentWnd ()->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar));

	if (bIsPopupMenu && rect.Width() > pButton->Rect ().Width () / 2)
	{
		rect.right--;
	}
	*/

	CMFCVisualManagerBase::OnHighlightMenuItem (pDC, pButton, rect, clrText);
}

void CMSMVisualManager::OnDrawSeparator (CDC* pDC, CBasePane* pBar,
										 CRect rect, BOOL bHorz)
{
	/*
	ASSERT_VALID (pDC);
	ASSERT_VALID (pBar);

	if (pBar->IsDialogControl () || 
		pBar->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)) || 
		pBar->IsKindOf (RUNTIME_CLASS (CMFCColorBar)))
	{
		CMFCVisualManagerBase::OnDrawSeparator (pDC, pBar, rect, bHorz);
		return;
	}

	CRect rectSeparator = rect;

	int x1, x2;
	int y1, y2;

	if (bHorz)
	{
		x1 = x2 = (rect.left + rect.right) / 2;
		y1 = rect.top;
		y2 = rect.bottom - 1;
	}
	else
	{
		y1 = y2 = (rect.top + rect.bottom) / 2;
		x1 = rect.left;
		x2 = rect.right;

		if (pBar->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)) &&
			!pBar->IsKindOf (RUNTIME_CLASS (CMFCColorBar)))
		{
			
			x1 = rect.left + CMFCToolBar::GetMenuImageSize ().cx + 
						GetMenuImageMargin () + 1;

			CRect rectBar;
			pBar->GetClientRect (rectBar);

			if (rectBar.right - x2 < 50) // Last item in row
			{
				x2 = rectBar.right;
			}

			if (((CMFCPopupMenuBar*) pBar)->m_bDisableSideBarInXPMode)
			{
				x1 = 0;
			}

			//---------------------------------
			//	Maybe Quick Customize separator
			//---------------------------------
			if (pBar->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
			{
				CWnd* pWnd = pBar->GetParent();
				if (pWnd != NULL && pWnd->IsKindOf (RUNTIME_CLASS (CMFCPopupMenu)))
				{
					CMFCPopupMenu* pMenu = (CMFCPopupMenu*)pWnd;
					if (pMenu->IsCustomizePane())
					{
						x1 = rect.left + 2*CMFCToolBar::GetMenuImageSize ().cx + 
								3*GetMenuImageMargin () + 2;
					}
				}
			}
		}
	}

	if (x1 == x2)
	{
		x2++;
	}

	if (y1 == y2)
	{
		y2++;
	}

	//_DrawReflectedGradient (pDC, m_clrToolBarSeparatorS, 
	//	m_clrToolBarSeparatorM, m_clrToolBarSeparatorF, CRect (x1, y1, x2, y2), !!bHorz);

	CDrawingManager dm(*pDC);
	dm.FillGradient(CRect (x1, y1, x2, y2), m_clrToolBarSeparatorS, m_clrToolBarSeparatorM, bHorz);
	*/

	CMFCVisualManagerBase::OnDrawSeparator (pDC, pBar, rect, bHorz);
}

void CMSMVisualManager::OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton)
{
	COLORREF colorBorder = m_clrComboBorder;

	if (bDisabled)
	{
		colorBorder = m_clrComboBorderDisabled;
	}
	else if (bIsHighlighted)
	{
		colorBorder = m_clrComboBorderHighlighted;
	}

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);
		dm.DrawRect(rect, (COLORREF)-1, colorBorder);
	}
	else
	{
		pDC->Draw3dRect(&rect, colorBorder, colorBorder);
	}
}

void CMSMVisualManager::SetParentActive (BOOL bParentActive)
{
	if (m_bParentActive != bParentActive)
	{
		m_bParentActive = bParentActive;
		RedrawAll();
	}
}

BOOL CMSMVisualManager::IsParentActive () const
{
	return m_bParentActive;
}

BOOL CMSMVisualManager::GetToolTipInfo(CMFCToolTipInfo& params, UINT nType)
{
	BOOL ret = CMFCVisualManagerBase::GetToolTipInfo(params, nType);

	params.m_bRoundedCorners = FALSE;
	params.m_bDrawIcon = FALSE;
	params.m_bBoldLabel = TRUE;
	params.m_bDrawDescription = TRUE;
	params.m_bDrawSeparator = FALSE;

	return ret;
}

void CMSMVisualManager::OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	CMFCVisualManagerOffice2007::OnFillButtonInterior(pDC, pButton, rect, state);
}

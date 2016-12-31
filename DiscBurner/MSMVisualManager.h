// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

// MSMVisualManager.h : header file
//

#define CMFCVisualManagerBase CMFCVisualManagerOffice2007

class CMSMVisualManager : public CMFCVisualManagerBase
{
	friend class CMFCToolBarMenuButtonWithBorder;
	friend class CProfileComboBox;
	friend class CProfileMenuWnd;
	friend class CScrollList;
	friend class CProfileGroupListWnd;
	friend class CProfileItemListWnd;
	friend class CFileList;
	friend class CHeaderCtrlEx;
	friend class CMFCToolBarSlider;
	friend class CMFCToolBarButtonEx;

	DECLARE_DYNCREATE(CMSMVisualManager)

// Construction
public:
	CMSMVisualManager();

	virtual ~CMSMVisualManager();

	void SetupMemoryBitmapSize (CDC* pDC, const CSize& size);

	void SetParentActive (BOOL bParentActive = true);

	BOOL IsParentActive () const;

	// This fixed version will save the style to the registry. In the default implementation, while resume from
	// Sleep or Hibernation, the style will be reset to the default Office2007_LunaBlue.
	static BOOL __stdcall SetStyleFixed(Style style, LPCTSTR lpszPath = NULL);

protected:
	virtual void OnUpdateSystemColors ();

	virtual void OnDrawSeparator (CDC* pDC, CBasePane* pBar,
		CRect rect, BOOL bHorz);

	virtual COLORREF GetToolbarButtonTextColor (CMFCToolBarButton* pButton,
		CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnFillBarBackground (CDC* pDC, CBasePane* pBar,
		CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);

	virtual void OnFillHighlightedArea (CDC* pDC, CRect rect, 
		CBrush* pBrush, CMFCToolBarButton* pButton);

	virtual void OnDrawMenuBorder (CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);

	virtual void OnDrawButtonBorder (CDC* pDC,
		CMFCToolBarButton* pButton, CRect rect, AFX_BUTTON_STATE state);

	virtual void OnHighlightMenuItem (CDC*pDC, CMFCToolBarMenuButton* pButton,
		CRect rect, COLORREF& clrText);

	virtual void OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, 
		CMFCToolBarEditBoxButton* pButton);

	virtual BOOL GetToolTipInfo(CMFCToolTipInfo& params, UINT nType = (UINT)(-1));

	virtual BOOL IsOwnerDrawCaption() { return FALSE; }

	virtual BOOL IsOffsetPressedButton() const { return TRUE; }

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

private:
	BOOL	   m_bParentActive;

	CSize	   m_MemorySize;
	CDC 	   m_MemoryDC;
	CBitmap    m_MemoryBitmap;
	BOOL	   m_MemoryFilled;

	CBitmap    m_MSMFrameBitmap;

	/*
	CBrush	   m_brMenuBarButtonHighlighted;
	COLORREF   m_clrMenuBarButtonBorderHighlighted;

	COLORREF   m_clrMenuBarButtonTextRegular;
	COLORREF   m_clrMenuBarButtonTextPressed;
	COLORREF   m_clrMenuBarButtonTextHighlighted;

	COLORREF   m_clrToolBarButtonTextRegular;
	COLORREF   m_clrToolBarButtonTextPressed;
	COLORREF   m_clrToolBarButtonTextHighlighted;

	COLORREF   m_clrCategorieBarButtonTextRegular;
	COLORREF   m_clrCategorieBarButtonTextPressed;
	COLORREF   m_clrCategorieBarButtonTextHighlighted;

	COLORREF   m_clrToolBarSeparatorS;
	COLORREF   m_clrToolBarSeparatorM;
	COLORREF   m_clrToolBarSeparatorF;

	COLORREF   m_clrComboBoxBorder;

	COLORREF   m_clrComboBoxBtnRegularF;
	COLORREF   m_clrComboBoxBtnRegularL;
	COLORREF   m_clrComboBoxBtnRegularBorder;
	COLORREF   m_clrComboBoxBtnHighlightedF;
	COLORREF   m_clrComboBoxBtnHighlightedL;
	COLORREF   m_clrComboBoxBtnHighlightedBorder;
	COLORREF   m_clrComboBoxBtnPressedF;
	COLORREF   m_clrComboBoxBtnPressedL;
	COLORREF   m_clrComboBoxBtnPressedBorder;

	COLORREF   m_clrCategorieBarButtonBorder;
	COLORREF   m_clrCategorieBarButtonBorderM;
	COLORREF   m_clrCategorieBarButtonLine;
	COLORREF   m_clrCategorieBarButtonF;
	COLORREF   m_clrCategorieBarButtonL;

	CBrush	   m_brLinksBarBack;
	CBrush	   m_brLinksBarButtonHighlighted;
	COLORREF   m_clrLinksBarButtonBorderHighlighted;
	*/
};

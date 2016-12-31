// ProfileComboBoxButton.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "MFCToolBarHistoryComboBoxButton.h"
#include "HistoryComboBox.h"

// CMFCToolBarHistoryComboBoxButton

IMPLEMENT_SERIAL(CMFCToolBarHistoryComboBoxButton, CMFCToolBarComboBoxButton, 1)

CMFCToolBarHistoryComboBoxButton::CMFCToolBarHistoryComboBoxButton()
{

}

CMFCToolBarHistoryComboBoxButton::CMFCToolBarHistoryComboBoxButton(UINT uiId, int iImage, DWORD dwStyle, int iWidth) :
	CMFCToolBarComboBoxButton(uiId, iImage, dwStyle, iWidth)
{
}

CMFCToolBarHistoryComboBoxButton::~CMFCToolBarHistoryComboBoxButton()
{
}


// CMFCToolBarHistoryComboBoxButton message handlers

CComboBox* CMFCToolBarHistoryComboBoxButton::CreateCombo(CWnd* pWndParent, const CRect& rect)
{
	CHistoryComboBox* pWndCombo = new CHistoryComboBox;
	if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}

	pWndCombo->EnableAutoCompletion(FALSE);

	return pWndCombo;
}

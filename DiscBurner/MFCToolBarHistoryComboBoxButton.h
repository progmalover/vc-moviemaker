#pragma once

#include "HistoryComboBox.h"

// CMFCToolBarHistoryComboBoxButton

class CMFCToolBarHistoryComboBoxButton : public CMFCToolBarComboBoxButton
{
	DECLARE_SERIAL(CMFCToolBarHistoryComboBoxButton)

public:
	CMFCToolBarHistoryComboBoxButton();
	CMFCToolBarHistoryComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
	virtual ~CMFCToolBarHistoryComboBoxButton();

	CHistoryComboBox *GetComboBox() {return (CHistoryComboBox *)CMFCToolBarComboBoxButton::GetComboBox();}

protected:
	virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
};

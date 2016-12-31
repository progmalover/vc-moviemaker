// OwnerDrawComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "OwnerDrawComboBox.h"


// COwnerDrawComboBox

IMPLEMENT_DYNAMIC(COwnerDrawComboBox, CComboBox)

COwnerDrawComboBox::COwnerDrawComboBox()
{

}

COwnerDrawComboBox::~COwnerDrawComboBox()
{
}


BEGIN_MESSAGE_MAP(COwnerDrawComboBox, CComboBox)
	ON_MESSAGE(CB_GETDROPPEDSTATE, OnGetDroppedState)
END_MESSAGE_MAP()

// COwnerDrawComboBox message handlers


void COwnerDrawComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);

	//if (lpMeasureItemStruct->itemID != (UINT) -1)
	//{
	//	LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
	//	ASSERT(lpszText != NULL);
	//	CSize   sz;
	//	CDC*    pDC = GetDC();

	//	sz = pDC->GetTextExtent(lpszText);

	//	ReleaseDC(pDC);

	//	lpMeasureItemStruct->itemHeight = 2 *sz.cy;
	//}

	if (lpMeasureItemStruct->itemID == (UINT) -1)
		lpMeasureItemStruct->itemHeight = 50;
	//else
	//	__super::MeasureItem(lpMeasureItemStruct);
}

LRESULT COwnerDrawComboBox::OnGetDroppedState(WPARAM wp, LPARAM lp)
{
	return TRUE;
}

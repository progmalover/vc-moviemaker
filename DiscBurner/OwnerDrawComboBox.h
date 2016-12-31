#pragma once


// COwnerDrawComboBox

class COwnerDrawComboBox : public CComboBox
{
	DECLARE_DYNAMIC(COwnerDrawComboBox)

public:
	COwnerDrawComboBox();
	virtual ~COwnerDrawComboBox();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnGetDroppedState(WPARAM wp, LPARAM lp);
};



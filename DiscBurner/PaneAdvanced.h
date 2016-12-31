#pragma once

#include "PropertyGridCtrlAdvanced.h"
#include "Utils.h"
// CPaneAdvanced

class CPaneAdvanced : public CBasePane
{
	DECLARE_DYNAMIC(CPaneAdvanced)
    DECLARE_SINGLETON(CPaneAdvanced)
public:
	CPaneAdvanced();
	virtual ~CPaneAdvanced();

protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

public:
	CPropertyGridCtrlAdvanced m_wndPropList;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};



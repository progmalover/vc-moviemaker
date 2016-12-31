#pragma once

#include "Subjects.h"
#include "MenuTemplateManager.h"
#include "PropertyGridCtrlMenu.h"

// CPaneMenu

class CPaneMenu : public CBasePane
{
	DECLARE_DYNAMIC(CPaneMenu)

public:
	CPaneMenu();
	virtual ~CPaneMenu();

protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

public:
	CPropertyGridCtrlMenu m_wndPropList;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#pragma once

#include "ProfileToolBar.h"

// CProfilePane

class CProfilePane : public CBasePane
{
	DECLARE_DYNAMIC(CProfilePane)

public:
	CProfilePane();
	virtual ~CProfilePane();

protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

public:
	CProfileToolBar m_wndToolBar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnProfileSaveAs();
	afx_msg void OnProfileRename();
	afx_msg void OnProfileDelete();
	afx_msg void OnProfileDeleteMultiple();

	afx_msg void OnUpdateProfile(CCmdUI *pCmdUI);
};

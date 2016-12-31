#pragma once
#include "MenuWnd.h"
#include "MenuViewToolBar.h"
#ifdef _MENU_TEMPLATE_VIEW
// CMenuViewPane

class CMenuViewPane : public CBasePane
{
	DECLARE_DYNAMIC(CMenuViewPane)

public:
	CMenuViewPane();
	virtual ~CMenuViewPane();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPrePage();
	afx_msg void OnNextPage();
	afx_msg void OnUpdatePrePage(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePageIndex(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNextPage(CCmdUI *pCmdUI);
	afx_msg LRESULT OnMenuPageChange(WPARAM wParam, LPARAM lParam);
private:
	void  RepositionControls();
	void  ChangePageIndex();
	CMenuWnd  *m_pWndMenuView;
public:
	CMenuViewToolBar m_wndMenuToolBar;
	
};

#endif

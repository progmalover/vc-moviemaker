#pragma once
#include "MenuList.h"
#include "MenuPage.h"
#include "MenuTemplateEx.h"

#ifdef _MENU_TEMPLATE_VIEW
#define MENU_PAGE_CHANGE  WM_USER+151
// CMenuWnd
#define  ID_MENU_VIEW    1002
class CMenuWnd : public CWnd
{
	DECLARE_DYNAMIC(CMenuWnd)
	DECLARE_SINGLETON(CMenuWnd)
	//Profile Change
	DECLARE_OBSERVER(CMenuWnd, SUB_PROFILE_CHANGED)
    //File list
	DECLARE_OBSERVER(CMenuWnd, SUB_FILE_SEL_CHANGED)
	DECLARE_OBSERVER(CMenuWnd, SUB_FILE_LIST_CHANGED)
    //Simple
	DECLARE_OBSERVER(CMenuWnd, SUB_VIDEO_STANDARD_CHANGED)
	DECLARE_OBSERVER(CMenuWnd, SUB_VIDEO_ASPECT_CHANGED)
	//Advance
	DECLARE_OBSERVER(CMenuWnd, SUB_PROPERTY_GRID_CHANGED)
    //Menu
	DECLARE_OBSERVER(CMenuWnd, SUB_MENU_TEMPLATE_CHANGED)
    DECLARE_OBSERVER(CMenuWnd, SUB_MENU_TEMPLATE_PARAM_CHANGED)
    DECLARE_OBSERVER(CMenuWnd, SUB_BUTTON_TEXT_CHANDED)

public:
	CMenuWnd();
	virtual ~CMenuWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:	
	CBrush m_brBackGround;
	CMenuTemplateEx *m_pTemplateEx;
	HCURSOR  m_hImageCursor;
	CToolTipCtrlEx m_tooltip;

public:
	void OnMouseInBtn(int index);
	void OnMouseOverBtn(int index,CPoint point);
	void OnClickBtn(int index);
	void OnMouseOutBtn(int index);
};

#endif

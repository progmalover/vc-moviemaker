#pragma once
#include "MFCToolBarEx.h"
#ifdef _MENU_TEMPLATE_VIEW
// CMenuViewToolBar

class CMenuViewToolBar :  public CMFCToolBarEx
{
	DECLARE_DYNAMIC(CMenuViewToolBar)

public:
	CMenuViewToolBar();
	virtual ~CMenuViewToolBar();
	BOOL m_bInit;

protected:
	DECLARE_MESSAGE_MAP()

	virtual CSize CalcSize(BOOL bVertDock);
	virtual void OnReset();
	virtual BOOL LoadState(LPCTSTR lpszProfileName=NULL, int nIndex=-1, UINT uiID=(UINT)-1);
	virtual void AdjustLocations();
	virtual BOOL AllowShowOnList() const {return FALSE;}
public:
	BOOL ReDraw();
	BOOL UpdateButtonEx(int nID);
};
#endif


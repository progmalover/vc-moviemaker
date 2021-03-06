#pragma once

#include "MFCToolBarEx.h"

#define VIDEO_TRACK_MIN		0
#define VIDEO_TRACK_MAX		1000

// CPlayerToolbar

class CPlayerToolbar : public CMFCToolBarEx
{
	DECLARE_DYNAMIC(CPlayerToolbar)

public:
	CPlayerToolbar();
	virtual ~CPlayerToolbar();
	BOOL m_bInit;

protected:
	DECLARE_MESSAGE_MAP()

	virtual CSize CalcSize(BOOL bVertDock);
	virtual void OnReset();
	virtual BOOL LoadState(LPCTSTR lpszProfileName=NULL, int nIndex=-1, UINT uiID=(UINT)-1);
	virtual void AdjustLocations();
	virtual BOOL AllowShowOnList() const {return FALSE;}
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};



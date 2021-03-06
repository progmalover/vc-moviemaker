#pragma once

#include "ScrollList.h"

class CProfileGroup;

// CProfileGroupListWnd

class CProfileGroupListWnd : public CScrollList
{
public:
	CProfileGroupListWnd();
	virtual ~CProfileGroupListWnd();

	void LoadProfileGroups();
	int SelectProfileGroup(CProfileGroup *pGroup);

	CWnd *m_pWndBuddy;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawBackground(CDC *pDC, const CRect &rc);
	virtual void DrawItem(CDC *pDC, int index, const CRect &rc);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

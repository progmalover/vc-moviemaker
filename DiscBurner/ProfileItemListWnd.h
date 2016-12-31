#pragma once

#include "ScrollList.h"

class CProfileGroup;
class CProfileItem;

// CProfileItemListWnd

class CProfileItemListWnd : public CScrollList
{
public:
	CProfileItemListWnd();
	virtual ~CProfileItemListWnd();

	void LoadProfileItems(CProfileGroup *pGroup);
	int SelectProfileItem(CProfileItem *pItem);
	CWnd *m_pWndBuddy;
	CProfileGroup *m_pGroup;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawBackground(CDC *pDC, const CRect &rc);
	virtual void DrawItem(CDC *pDC, int index, const CRect &rc);

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

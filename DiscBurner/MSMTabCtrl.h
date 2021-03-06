#pragma once


// CMSMTabCtrl

class CMSMTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CMSMTabCtrl)

public:
	CMSMTabCtrl();
	virtual ~CMSMTabCtrl();

	virtual int GetTabsHeight() const
	{
		return CMFCTabCtrl::GetTabsHeight() + 3;
	}

protected:
	DECLARE_MESSAGE_MAP()
};



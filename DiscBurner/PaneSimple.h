#pragma once

#include "Subjects.h"
#include "PaneSimpleToolBar.h"
#include "Profile.h"
#include "Utils.h"
// CPaneSimple

class CPaneSimple : public CBasePane
{
	DECLARE_DYNAMIC(CPaneSimple)
	DECLARE_SINGLETON(CPaneSimple)
public:
	CPaneSimple();
	virtual ~CPaneSimple();

protected:
	DECLARE_MESSAGE_MAP()

	void RepositionControls();

	DECLARE_OBSERVER(CPaneSimple, SUB_PROFILE_CHANGED)
	DECLARE_OBSERVER(CPaneSimple, SUB_PROPERTY_GRID_CHANGED)

public:
	CPaneSimpleToolBar m_wndToolBar;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnUpdateProfile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVideoFrameRate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateVideoDimension(CCmdUI *pCmdUI);
    afx_msg void OnUpdateZoommode(CCmdUI *pCmd);
    afx_msg void OnUpdateVideoStandard(CCmdUI *pCmd);

	afx_msg void OnCbnSelchangeVideoDimension();
	afx_msg void OnCbnSelchangeBurnerList();
	afx_msg void OnCbnSelchangeDiscTypeList();
	//afx_msg void OnEnchangeDiscLabel();
    afx_msg void OnCbnSelchangeZoommode();
	afx_msg void OnCbnSelchangeVideoFrameRate();
	afx_msg void OnCbnSelchangeVideoStandard();
};

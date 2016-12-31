#pragma once

#include "MFCToolBarEx.h"
#include "Subjects.h"

class CProfileParamBitrate;

// CPaneSimpleToolBar

class CPaneSimpleToolBar : public CMFCToolBarEx
{
	DECLARE_DYNAMIC(CPaneSimpleToolBar)

public:
	CPaneSimpleToolBar();
	virtual ~CPaneSimpleToolBar();
	BOOL m_bInit;
	void SetQualityLevel(CMFCToolBarComboBoxButton *pButton, CProfileParamBitrate *pParam);
	void OnProfileChanged();

protected:
	DECLARE_MESSAGE_MAP()

	virtual CSize CalcSize(BOOL bVertDock);
	virtual void OnReset();
	virtual BOOL LoadState(LPCTSTR lpszProfileName=NULL, int nIndex=-1, UINT uiID=(UINT)-1);
	virtual void AdjustLocations();
	virtual BOOL AllowShowOnList() const {return FALSE;}
	BOOL  FillBurnerList();

public:
	afx_msg void OnDestroy();
};



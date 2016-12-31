#pragma once

#include "Subjects.h"
#include "Profile.h"

// CPropertyGridCtrlAdvanced

class CPropertyGridCtrlAdvanced : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrlAdvanced)

public:
	CPropertyGridCtrlAdvanced();
	virtual ~CPropertyGridCtrlAdvanced();

	void LoadProfile();
	void SaveState();

	CMFCPropertyGridProperty *FindProperty(PROFILE_PARAM_CAT cat, LPCTSTR lpszName);

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
	void OnFileChanged();
	void OnVideoTrimmed();
	void LoadState();

	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_PROFILE_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_DISC_TYPE_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_ZOOM_MODE)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_DISC_LABEL_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_FRAME_RATE_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_DIMENSION_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_STANDARD_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_FILE_SEL_CHANGED)
	DECLARE_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_TRIMMED)

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#pragma once

#include "Subjects.h"
#include "Profile.h"

enum MENU_PROP_CAT
{
	MENU_CAT_NONE = 0, 
	MENU_CAT_GENERAL, 
	MENU_CAT_TITLE, 
	MENU_CAT_BUTTON, 
    MENU_CAT_NAVIGATION,
};

#ifdef _DVD
#define IDS_AFTER_LAST_PLAYBACK_OPTIONS  IDS_AFTER_LAST_VIDEO_OPTIONS
#else
#define IDS_AFTER_LAST_PLAYBACK_OPTIONS  IDS_AFTER_LAST_TITLE_OPTIONS
#endif

// CPropertyGridCtrlMenu

class CPropertyGridCtrlMenu : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrlMenu)

public:
	CPropertyGridCtrlMenu();
	virtual ~CPropertyGridCtrlMenu();

	void LoadMenuTemplates();
	void SaveState();

	CMFCPropertyGridProperty *FindProperty(MENU_PROP_CAT cat, LPCTSTR lpszName);

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

	void OnTemplateChanged();
	void LoadState();

	DECLARE_OBSERVER(CPropertyGridCtrlMenu, SUB_FILE_SEL_CHANGED)

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

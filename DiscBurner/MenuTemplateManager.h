#pragma once

#include <vector>
#include <string>
#ifdef _BD
#include "BDMenuUtils.h"
#endif
#define MAX_CHAPTERS	99
#define MAX_BUTTONS		32

struct BUTTON
{
public:
	CRect rc[4];
#ifdef _BD
	int nNID; //Normal 
	int nSID; //Select
	int nAID; //Active
	BOOL bSRepeated;
#endif
};

typedef std::vector<BUTTON> BUTTON_LIST;

class CMenuTemplate
{
public:
	CMenuTemplate() {}
	~CMenuTemplate() {}

	BOOL m_bTemplate;

	CString m_strFolder;		// Template folder
	CString m_strName;
	CString m_strBackgroundImage;
	CString m_strBackgroundAudio;
	
	CString m_strTitleText;
	CString m_strTitleTextColor;
	CString m_strTitleFont;
	CString m_strTitleFontSize;
	CString m_strTitleX;
	CString m_strTitleY;

	CString m_strDuration;

	CString m_strButtonTextAlign;
	CString m_strButtonFont;
	CString m_strButtonFontSize;
	CString m_strButtonTextColor;
	
	CString m_strNameNew;
	CString m_strBackgroundImageNew;
	CString m_strBackgroundAudioNew;
	CString m_strTitleTextNew;
	CString m_strTitleFontNew;
	CString m_strTitleFontSizeNew;
	CString m_strTitleTextColorNew;
	CString m_strDurationNew;

	CString m_strButtonFontNew;
	CString m_strButtonFontSizeNew;
	CString m_strButtonTextColorNew;

	BUTTON_LIST m_listButtons;

	BOOL LoadMenuTemplate(LPCTSTR lpszFolder);
	BOOL SaveMenuTemplate(LPCTSTR lpszFile);
#ifdef _BD
	RESOURCE_FORMAT m_nResourceFormat;
	int m_nMapWidth;
	int m_nMapHeight;
	CString m_strMatrix;
	CString m_strSound;
    int     m_nSoundIDS;
	int     m_nSoundIDA;
#endif
};

typedef std::vector<CMenuTemplate *> MENU_TEMPLATE_LIST;
#define _RELOAD_MENU_TEMPLATES

class CMenuTemplateManager
{
public:
	CMenuTemplateManager(void);
	~CMenuTemplateManager(void);

	DECLARE_SINGLETON(CMenuTemplateManager)

protected:
	CMenuTemplate *m_pTemplate;

public:
	BOOL LoadMenuTemplates();
	MENU_TEMPLATE_LIST m_templates;

	void SetCurrentTemplate(CMenuTemplate *pTemplate);
	void SetCurrentTemplate(LPCTSTR lpszName);
	CMenuTemplate *GetCurrentTemplate();

#ifdef _RELOAD_MENU_TEMPLATES
	BOOL ReloadMenuTemplates();
private:
	BOOL ReadMenuName(LPCTSTR lpszFolder,CString &strMenuName);
	CMenuTemplate *FindTemplate(LPCTSTR lpszName);
#endif
};

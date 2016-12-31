
// Converter.h : main header file for the Converter application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <gdiplus.h>
using namespace Gdiplus;
// CConverterApp:
// See Converter.cpp for the implementation of this class
//

class CConverterApp : public CWinAppEx
{
public:
	CConverterApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

protected:
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void RegisterSubjects();

	BOOL AddToWhiteList(LPCTSTR lpszRegKey);
	void CheckIncompatibleFilters();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
private:
	//GDI+สนำร
	ULONG_PTR token;

public:
	afx_msg void OnLogEnableLogging();
	afx_msg void OnUpdateLogEnableLogging(CCmdUI *pCmdUI);
	afx_msg void OnLogViewLogFile();
	afx_msg void OnUpdateLogViewLogFile(CCmdUI *pCmdUI);
};

extern CConverterApp theApp;

#define AfxGetAppEx() ((CWinAppEx *)AfxGetApp())
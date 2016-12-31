// OptionsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "OptionsSheet.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COptionsSheet

int COptionsSheet::m_nSelectedPage	= 0;

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)
COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_pageGeneral);
	AddPage(&m_pageEncoding);
	AddPage(&m_pageBurner);
	AddPage(&m_pageInterface);

	m_nSelectedPage = iSelectPage;
}

COptionsSheet::~COptionsSheet()
{
}


BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// COptionsSheet message handlers

BOOL COptionsSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// TODO:  Add your specialized code here

	return TRUE;
}

void COptionsSheet::OnDestroy()
{
	CPropertySheet::OnDestroy();

	// TODO: Add your message handler code here

	m_nSelectedPage = GetTabControl()->GetCurSel();
}

INT_PTR COptionsSheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class

	INT_PTR ret = CPropertySheet::DoModal();
	if (ret != IDOK)
		return ret;

	// General
	COptions::Instance()->m_strTempFolder = m_pageGeneral.m_strTempFolder;

	// Interface
	COptions::Instance()->m_bShowHorzGridLines = m_pageInterface.m_bShowHorzGridLines;
	COptions::Instance()->m_bShowVertGridLines = m_pageInterface.m_bShowVertGridLines;
	COptions::Instance()->m_bSmoothScrolling = m_pageInterface.m_bSmoothScrolling;
	COptions::Instance()->m_nDoubleClickAction = m_pageInterface.m_nDoubleClickAction;

	COptions::Instance()->UpdateULStyle();

	// Encoding
	COptions::Instance()->m_nMaxConcurrentTasks = m_pageEncoding.m_nConcurrentTasks + 1;	// m_nConcurrentTasks is the list index
	COptions::Instance()->m_nEncodingThreads = m_pageEncoding.m_nEncodingThreads + 1;	// m_nEncodingThreads is the list index
	COptions::Instance()->m_bFixAspectRatio= m_pageEncoding.m_bFixAspectRatio;
	COptions::Instance()->m_bPromptStopConversion = m_pageEncoding.m_bPromptStopConversion;
	COptions::Instance()->m_bSoundFinished = m_pageEncoding.m_bSoundFinished;
	COptions::Instance()->m_strSoundFinished = m_pageEncoding.m_strSoundFinished;

	//Burner
	COptions::Instance()->m_bCheckDiscStatus = m_pageBurner.m_bCheckDiscStatus;
	COptions::Instance()->m_bPromptNonBlankDisc = m_pageBurner.m_bPromptNonBlankDisc;
	COptions::Instance()->m_bPromptMakeAnotherDisc = m_pageBurner.m_bPromptMakeAnotherDisc;

	return ret;
}

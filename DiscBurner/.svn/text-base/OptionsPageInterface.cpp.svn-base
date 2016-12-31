// OptionsPageInterface.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "OptionsPageInterface.h"
#include "Options.h"

// COptionsPageInterface dialog

IMPLEMENT_DYNAMIC(COptionsPageInterface, CPropertyPage)

COptionsPageInterface::COptionsPageInterface()
	: CPropertyPage(COptionsPageInterface::IDD)
{
	m_bShowHorzGridLines = COptions::Instance()->m_bShowHorzGridLines;
	m_bShowVertGridLines = COptions::Instance()->m_bShowVertGridLines;
	m_bSmoothScrolling = COptions::Instance()->m_bSmoothScrolling;
	m_nDoubleClickAction = COptions::Instance()->m_nDoubleClickAction;
}

COptionsPageInterface::~COptionsPageInterface()
{
}

void COptionsPageInterface::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_HORZ_GRID_LINES, m_bShowHorzGridLines);
	DDX_Check(pDX, IDC_CHECK_VERT_GRID_LINES, m_bShowVertGridLines);
	DDX_Check(pDX, IDC_CHECK_SMOOTH_SCROLLING, m_bSmoothScrolling);
	DDX_Radio(pDX, IDC_RADIO_DO_NOTHING, m_nDoubleClickAction);
}


BEGIN_MESSAGE_MAP(COptionsPageInterface, CPropertyPage)
END_MESSAGE_MAP()


// COptionsPageInterface message handlers

BOOL COptionsPageInterface::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COptionsPageInterface::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::OnApply();
}

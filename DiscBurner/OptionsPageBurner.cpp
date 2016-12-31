// OptionsPageBurner.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "OptionsPageBurner.h"
#include "Options.h"

// COptionsPageBurner dialog

IMPLEMENT_DYNAMIC(COptionsPageBurner, CPropertyPage)

COptionsPageBurner::COptionsPageBurner()
	: CPropertyPage(COptionsPageBurner::IDD)
	, m_bPromptNonBlankDisc(FALSE)
{
	m_bCheckDiscStatus = COptions::Instance()->m_bCheckDiscStatus;
	m_bPromptNonBlankDisc = COptions::Instance()->m_bPromptNonBlankDisc;
	m_bPromptMakeAnotherDisc = COptions::Instance()->m_bPromptMakeAnotherDisc;
}

COptionsPageBurner::~COptionsPageBurner()
{
}

void COptionsPageBurner::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_CHECK_DISC_STATUS, m_bCheckDiscStatus);
	DDX_Check(pDX, IDC_CHECK_PROMPT_NON_BLANK_DISC, m_bPromptNonBlankDisc);
	DDX_Check(pDX, IDC_CHECK_PROMPT_MAKE_ANOTHER_DISC, m_bPromptMakeAnotherDisc);
}


BEGIN_MESSAGE_MAP(COptionsPageBurner, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK_CHECK_DISC_STATUS, OnCheckCheckDiscStatus)
END_MESSAGE_MAP()


// COptionsPageBurner message handlers

BOOL COptionsPageBurner::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	OnCheckCheckDiscStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COptionsPageBurner::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::OnApply();
}

void COptionsPageBurner::OnCheckCheckDiscStatus()
{
	GetDlgItem(IDC_CHECK_PROMPT_NON_BLANK_DISC)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_CHECK_DISC_STATUS));
}

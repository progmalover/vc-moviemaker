#pragma once

#include "OptionsPageGeneral.h"
#include "OptionsPageInterface.h"
#include "OptionsPageEncoding.h"
#include "OptionsPageBurner.h"

// COptionsSheet

class COptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsSheet)

public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsSheet();

protected:
	DECLARE_MESSAGE_MAP()

	void RecalcLayout();

public:
	static int m_nSelectedPage;

	COptionsPageGeneral m_pageGeneral;
	COptionsPageInterface m_pageInterface;
	COptionsPageEncoding m_pageEncoding;
	COptionsPageBurner m_pageBurner;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual INT_PTR DoModal();
};

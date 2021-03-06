#pragma once


// COptionsPageInterface dialog

class COptionsPageInterface : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionsPageInterface)

public:
	COptionsPageInterface();
	virtual ~COptionsPageInterface();

// Dialog Data
	enum { IDD = IDD_OPTIONS_INTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	BOOL m_bShowHorzGridLines;
	BOOL m_bShowVertGridLines;
	BOOL m_bSmoothScrolling;
	int m_nDoubleClickAction;
};

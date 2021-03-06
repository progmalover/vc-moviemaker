// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include "MFCToolBarEx.h"

// MainToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainToolBar window

class CMainToolBar : public CMFCToolBarEx
{
	DECLARE_DYNCREATE(CMainToolBar)

// Construction
public:
	CMainToolBar();

	virtual ~CMainToolBar ();

// Attributes
public:

// Operations
public:

// Implementation
public:

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL LoadState(LPCTSTR lpszProfileName=NULL, int nIndex=-1, UINT uiID=(UINT)-1);
	virtual void OnReset();
	virtual void AdjustLocations();
};

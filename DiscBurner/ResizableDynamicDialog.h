#pragma once

#include "ResizableDialog.h"
#include <list>
#include <map>

// CResizableDynamicDialog dialog

class CResizableDynamicDialog : public CResizableDialog
{
public:
	CResizableDynamicDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResizableDynamicDialog();

// Dialog Data
	//enum { IDD = IDD_NOTIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bDefFullMode;

	#define BEGIN_DYNAMIC_CONTROL_MAP				\
	void AddDynamicControls()						\
	{

	#define DEFINE_DYNAMIC_CONTROL(nID, nIDNext)	\
	m_mapID[nID] = nIDNext;

	#define END_DYNAMIC_CONTROL_MAP					\
	}

	virtual void AddDynamicControls();
	virtual void AddAnchors();
	virtual void RemoveAnchors();
	virtual void OnModeChanged();

	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE);

	typedef std::map<UINT, UINT> CONTROL_ID_MAP;
	typedef std::map<CWnd *, LONG> WINDOW_OFFSET_MAP;
	CONTROL_ID_MAP m_mapID;
	BOOL m_bEnableSaveRestore;
	CString m_sSection;
	int m_nMinTrackSizeDelta;

private:
	WINDOW_OFFSET_MAP m_mapOffset;
	BOOL m_bFullMode;

public:
	BOOL GetMode();
	void SetMode(BOOL bFull);
	void ToggleMode();

protected:
	afx_msg void OnDestroy();
};

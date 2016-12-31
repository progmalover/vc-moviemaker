// ProfileSimpleToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "PaneSimpleToolBar.h"
#include "MFCToolBarLabel.h"
#include "MFCToolBarHistoryComboBoxButton.h"
#include "Options.h"
#include "Burner.h"
#include "ProfileManager.h"
#include "MainFrm.h"

// CPaneSimpleToolBar

IMPLEMENT_DYNAMIC(CPaneSimpleToolBar, CMFCToolBarEx)

CPaneSimpleToolBar::CPaneSimpleToolBar()
{
	m_bInit = FALSE;
}

CPaneSimpleToolBar::~CPaneSimpleToolBar()
{
}


BEGIN_MESSAGE_MAP(CPaneSimpleToolBar, CMFCToolBarEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CPaneSimpleToolBar message handlers

CSize CPaneSimpleToolBar::CalcSize(BOOL bVertDock)
{
	CRect rc;
	GetWindowRect(&rc);
	return rc.Size();
}

void CPaneSimpleToolBar::OnReset()
{
	CMFCToolBarEx::OnReset();

	ReplaceButton(ID_BURNER_LIST_LABEL, CMFCToolBarLabel(ID_BURNER_LIST_LABEL, LoadStringInline(IDS_BURNER_LIST_LABEL)));
	ReplaceButton(ID_BURNER_LIST, CMFCToolBarComboBoxButton(ID_BURNER_LIST, -1, CBS_DROPDOWNLIST));

	ReplaceButton(ID_DISC_TYPE_LIST_LABEL, CMFCToolBarLabel(ID_DISC_TYPE_LIST_LABEL, LoadStringInline(IDS_DISC_TYPE_LIST_LABEL)));
	ReplaceButton(ID_DISC_TYPE_LIST, CMFCToolBarComboBoxButton(ID_DISC_TYPE_LIST, -1, CBS_DROPDOWNLIST));

	ReplaceButton(ID_ZOOM_MODE_LABEL, CMFCToolBarLabel(ID_ZOOM_MODE_LABEL, LoadStringInline(IDS_ZOOM_MODE_LABEL)));
	ReplaceButton(ID_ZOOM_MODE, CMFCToolBarComboBoxButton(ID_ZOOM_MODE, -1));

#ifdef _BD
	ReplaceButton(ID_VIDEO_DIMENSION_LABEL, CMFCToolBarLabel(ID_VIDEO_DIMENSION_LABEL, LoadStringInline(IDS_VIDEO_DIMENSION_LABEL)));
	ReplaceButton(ID_VIDEO_DIMENSION, CMFCToolBarComboBoxButton(ID_VIDEO_DIMENSION, -1, CBS_DROPDOWNLIST));

	ReplaceButton(ID_VIDEO_FRAME_RATE_LABEL, CMFCToolBarLabel(ID_VIDEO_FRAME_RATE_LABEL, LoadStringInline(IDS_VIDEO_FRAME_RATE_LABEL)));
	ReplaceButton(ID_VIDEO_FRAME_RATE, CMFCToolBarComboBoxButton(ID_VIDEO_FRAME_RATE, -1, CBS_DROPDOWNLIST));
#else
	ReplaceButton(ID_VIDEO_STANDARD_LABEL, CMFCToolBarLabel(ID_VIDEO_STANDARD_LABEL, LoadStringInline(IDS_VIDEO_STANDARD_LABEL)));
	ReplaceButton(ID_VIDEO_STANDARD, CMFCToolBarComboBoxButton(ID_VIDEO_STANDARD, -1, CBS_DROPDOWNLIST));
#endif

	SetRouteCommandsViaFrame(FALSE);
	SetOwner(this);
	SetCustomizeMode(FALSE);

	FillBurnerList();
}

void CPaneSimpleToolBar::AdjustLocations()
{
	ASSERT_VALID(this);

	if (GetSafeHwnd() == NULL || !::IsWindow(m_hWnd) || !m_bInit)
	{
		return;
	}

	CMFCToolBarEx::AdjustLocations();

	CRect rc;
	GetClientRect(&rc);

	const int TOP = 8;
	const int LEFT = 5;
	const int RIGHT = 5;
	const int BOTTOM = 8;

	const int SPACE_MIN = 5;
	const int SPACE_MAX = 15;

	const UINT nIDs[] = 
	{
		ID_BURNER_LIST_LABEL,ID_BURNER_LIST,
		ID_DISC_TYPE_LIST_LABEL,ID_DISC_TYPE_LIST,
#ifdef _BD
		ID_VIDEO_DIMENSION_LABEL,ID_VIDEO_DIMENSION,
		ID_VIDEO_FRAME_RATE_LABEL,ID_VIDEO_FRAME_RATE,
#else
		ID_VIDEO_STANDARD_LABEL,ID_VIDEO_STANDARD,
#endif
		ID_ZOOM_MODE_LABEL,ID_ZOOM_MODE
	};

	CMFCToolBarComboBoxButton *pBurnerCombo = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_BURNER_LIST));
	BOOL bShow = pBurnerCombo->GetCount() > 0;
	GetButton(CommandToIndex(ID_BURNER_LIST_LABEL))->Show(bShow);
	GetButton(CommandToIndex(ID_BURNER_LIST))->Show(bShow);

	CRect rcLabel = GetButton(CommandToIndex(ID_DISC_TYPE_LIST_LABEL))->Rect();
	CRect rcButton = GetButton(CommandToIndex(ID_DISC_TYPE_LIST))->Rect();
	int i = bShow?0:2;
	int nTotalIDs  = sizeof(nIDs) / sizeof (UINT);
	int nSpace = (rc.Height() - TOP - (rcLabel.Height() + rcButton.Height()) * nTotalIDs/2 - BOTTOM) / (nTotalIDs/2-1);
	if (nSpace >= SPACE_MIN)
	{
		if (nSpace > SPACE_MAX)
			nSpace = SPACE_MAX;

		rcLabel.left = rc.left + LEFT;
		rcLabel.right = rc.right - RIGHT;

		rcButton.left = rc.left + LEFT;
		rcButton.right = rc.right - RIGHT;

		rcLabel.MoveToY(rc.top + TOP);
		rcButton.MoveToY(rc.top + TOP + rcLabel.Height());

		for (; i < nTotalIDs; i += 2)
		{
			GetButton(CommandToIndex(nIDs[i]))->SetRect(rcLabel);
			GetButton(CommandToIndex(nIDs[i + 1]))->SetRect(rcButton);

			rcLabel.OffsetRect(0, rcLabel.Height() + rcButton.Height() + nSpace);
			rcButton.OffsetRect(0, rcLabel.Height() + rcButton.Height() + nSpace);
		}
	}
	else
	{
		nSpace = (rc.Height() - TOP - rcLabel.Height() * nTotalIDs/2 - BOTTOM) /( nTotalIDs/2-1);
		if (nSpace > SPACE_MAX)
			nSpace = SPACE_MAX;

		int nLabelWidthMax = 0;
		for (int j = i; j < nTotalIDs; j += 2)
		{
			CRect rcTemp = GetButton(CommandToIndex(nIDs[j]))->Rect();
			nLabelWidthMax = max(nLabelWidthMax, rcTemp.Width());
		}

		rcLabel.left = rc.left + LEFT;
		rcLabel.right = rcLabel.left + nLabelWidthMax;

		rcButton.left = rcLabel.right;
		rcButton.right = rc.right - RIGHT;

		rcLabel.MoveToY(rc.top + TOP);
		rcButton.MoveToY(rc.top + TOP);

		for (; i < sizeof(nIDs) / sizeof (UINT); i += 2)
		{
			GetButton(CommandToIndex(nIDs[i]))->SetRect(rcLabel);
			GetButton(CommandToIndex(nIDs[i + 1]))->SetRect(rcButton);

			rcLabel.OffsetRect(0, rcLabel.Height() + nSpace);
			rcButton.OffsetRect(0, rcLabel.Height() + nSpace);
		}
	}

	UpdateTooltips();
}

BOOL CPaneSimpleToolBar::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

void CPaneSimpleToolBar::OnDestroy()
{
	CMFCToolBarEx::OnDestroy();

	// TODO: Add your message handler code here

	CMFCToolBarComboBoxButton *pBurnerCombo = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_BURNER_LIST));

	int index = pBurnerCombo->GetCurSel();

	for (int i = 0; i < pBurnerCombo->GetCount(); i++)
	{
		TCHAR *pszItem = (LPTSTR)pBurnerCombo->GetItemData(i);
		if (i == index)
			COptions::Instance()->m_strUniqueID = pszItem;
		free(pszItem);
	}
}

void CPaneSimpleToolBar::OnProfileChanged()
{
	USES_CONVERSION;

	CMFCToolBarComboBoxButton *pDiscType= (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_DISC_TYPE_LIST));
	CMFCPropertyGridProperty *pPropDiscType = GetPropertyGridAdvanced()->FindProperty(CAT_GENERAL, "disctype");
	for (int i = 0; i < pPropDiscType->GetOptionCount(); i++)
		pDiscType->AddItem(pPropDiscType->GetOption(i));
	if (!pDiscType->SelectItem(COptions::Instance()->m_strDiscType))//Notify CBN_SELENDOK message to parent
		pDiscType->SelectItem(OLE2A(pPropDiscType->GetValue().bstrVal));
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_DISC_TYPE_LIST, CBN_SELENDOK), (LPARAM)m_hWnd);

	//CMFCToolBarEditBoxButton *pDiscLabel = (CMFCToolBarEditBoxButton *)GetButton(CommandToIndex(ID_DISC_LABEL));
	//CTime time(time(0));
	//pDiscLabel->GetEditBox()->LimitText(MAX_DISC_LABEL);
	//pDiscLabel->GetEditBox()->SetWindowText(time.Format("%Y-%m-%d"));
	//GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_DISC_LABEL, EN_CHANGE), (LPARAM)m_hWnd);

	CTime time(time(0));
	CString strText = time.Format("%Y-%m-%d");
	CSubjectManager::Instance()->GetSubject(SUB_DISC_LABEL_CHANGED)->Notify((void *)(LPCTSTR)strText);

	CMFCToolBarComboBoxButton *pZoommode= (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_ZOOM_MODE));
	CMFCPropertyGridProperty *pPropZoommode = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vzoom");
	for (int i = 0; i < pPropZoommode->GetOptionCount(); i++)
		pZoommode->AddItem(pPropZoommode->GetOption(i));
	if (!pZoommode->SelectItem(COptions::Instance()->m_strZoommode))
		pZoommode->SelectItem(OLE2A(pPropZoommode->GetValue().bstrVal));
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ZOOM_MODE, CBN_SELENDOK), (LPARAM)m_hWnd);

#ifdef _BD
	CMFCToolBarComboBoxButton *pVideoDimension = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_VIDEO_DIMENSION));
	CMFCPropertyGridProperty *pPropDimension = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vdimension");
	for (int i = 0; i < pPropDimension->GetOptionCount(); i++)
		pVideoDimension->AddItem(pPropDimension->GetOption(i));
	if (!pVideoDimension->SelectItem(COptions::Instance()->m_strDimension))
		pVideoDimension->SelectItem(OLE2A(pPropDimension->GetValue().bstrVal));
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIDEO_DIMENSION, CBN_SELENDOK), (LPARAM)m_hWnd);

	CMFCToolBarComboBoxButton *pVideoFrameRate = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_VIDEO_FRAME_RATE));
	CMFCPropertyGridProperty *pPropFrameRate = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vframerate");
	for (int i = 0; i < pPropFrameRate->GetOptionCount(); i++)
		pVideoFrameRate->AddItem(pPropFrameRate->GetOption(i));
	if (!pVideoFrameRate->SelectItem(COptions::Instance()->m_strFrameRate))
		pVideoFrameRate->SelectItem(OLE2A(pPropFrameRate->GetValue().bstrVal));
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIDEO_FRAME_RATE, CBN_SELENDOK), (LPARAM)m_hWnd);
	LPCTSTR lpszText = "16:9";
	CSubjectManager::Instance()->GetSubject(SUB_VIDEO_ASPECT_CHANGED)->Notify((void *)lpszText);
#else
	CMFCToolBarComboBoxButton *pVideoStandard = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_VIDEO_STANDARD));
	CMFCPropertyGridProperty *pPropStandard = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vstandard");
	for (int i = 0; i < pPropStandard->GetOptionCount(); i++)
		pVideoStandard->AddItem(pPropStandard->GetOption(i));
	if (!pVideoStandard->SelectItem(COptions::Instance()->m_strVideoStandard))
		pVideoStandard->SelectItem(OLE2A(pPropStandard->GetValue().bstrVal));
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIDEO_STANDARD, CBN_SELENDOK), (LPARAM)m_hWnd);

	CMFCPropertyGridProperty * pPropAspect = GetPropertyGridAdvanced()->FindProperty(CAT_VIDEO, "vaspect");
	LPCTSTR lpszText = OLE2A(pPropAspect->GetValue().bstrVal);
	CSubjectManager::Instance()->GetSubject(SUB_VIDEO_ASPECT_CHANGED)->Notify((void *)lpszText);
#endif	
}

BOOL  CPaneSimpleToolBar::FillBurnerList()
{
	CBurner *pBurner = CxBurner::Instance();
	if (pBurner->IsOpen())
	{
		DEVICE_LIST devices;
		pBurner->GetBurnerList(devices);

		if (devices.size() > 0)
		{
			CMFCToolBarComboBoxButton *pBurnerCombo = (CMFCToolBarComboBoxButton *)GetButton(CommandToIndex(ID_BURNER_LIST));

			for (DEVICE_LIST::iterator it = devices.begin(); it != devices.end(); ++it)
			{
				CString strItem;

				DEVICE &device = *it;

				pBurnerCombo->AddItem(device.strName, (DWORD_PTR)_tcsdup(device.strUniqueID));
			}

			for (int i = 0; i < pBurnerCombo->GetCount(); i++)
			{
				TCHAR *pszUniqueID = (TCHAR *)pBurnerCombo->GetItemData(i);
				if (COptions::Instance()->m_strUniqueID.Compare(pszUniqueID) == 0)
				{
					pBurnerCombo->SelectItem(i);
					break;
				}
			}

			if (pBurnerCombo->GetCount() > 0 && pBurnerCombo->GetCurSel() < 0)
				pBurnerCombo->SelectItem(0);

			return pBurnerCombo->GetCount()>0;
		}
	}
	return FALSE;
}

// PropertyGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "PropertyGridCtrlAdvanced.h"
#include "ProfileManager.h"
#include "MainFrm.h"
#include "Burner.h"
#include "FileList.h"
#include "DlgTrim.h"
#include "Options.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CCustomValidateProp : public CMFCPropertyGridProperty
{
public:
	CCustomValidateProp(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);
	virtual BOOL OnUpdateValue();

	virtual BOOL HasButton() const
	{
		CProfileParam *pParam = (CProfileParam *)GetData();
		if (pParam != NULL)
		{
			if (pParam->m_strName == "trimstart" || pParam->m_strName == "trimend")
				return TRUE;
		}

		return CMFCPropertyGridProperty::HasButton();
	}

	virtual void OnClickButton(CPoint point)
	{
		if (CMFCPropertyGridProperty::HasButton())
		{
			CMFCPropertyGridProperty::OnClickButton(point);
			return;
		}

		CTask *pTask = CFileList::Instance()->GetCurTask();
		if (pTask != NULL)
		{
			CDlgTrim dlg;
			dlg.m_pTask = pTask;
			dlg.DoModal();
		}
	}

	virtual BOOL OnDblClk(CPoint point)
	{
		CProfileParam *pParam = (CProfileParam *)GetData();
		if (pParam != NULL)
		{
			if (pParam->m_strName == "trimstart" || pParam->m_strName == "trimend")
			{
				OnClickButton(point);
				return TRUE;
			}
		}

		return CMFCPropertyGridProperty::OnDblClk(point);
	}

protected:
	static BOOL Validate(CCustomValidateProp *pProp);
};

CCustomValidateProp::CCustomValidateProp(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,
	LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) : 
	CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
}

BOOL CCustomValidateProp::OnUpdateValue()
{
	return Validate(this);
}

BOOL CCustomValidateProp::Validate(CCustomValidateProp *pProp)
{
	ASSERT_VALID(pProp);
	ASSERT_VALID(pProp->m_pWndInPlace);
	ASSERT(::IsWindow(pProp->m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	pProp->m_pWndInPlace->GetWindowText(strText);

	if (pProp->FormatProperty() == strText)
		return TRUE;

	if (pProp->GetOptionCount() > 0)
	{
		for (int i = 0; i < pProp->GetOptionCount(); i++)
		{
			if (strText.Compare(pProp->GetOption(i)) == 0)
				return pProp->CMFCPropertyGridProperty::OnUpdateValue();
		}
	}

	CProfileParam *pParam = (CProfileParam *)pProp->GetData();
	ASSERT(pParam != NULL);

	if (pParam->m_strName == "disclabel")
	{
		BOOL bError = FALSE;
		CString strError;

		if (strText.GetLength() > MAX_DISC_LABEL)
		{
			bError = TRUE;
			strError.Format(IDS_E_DISC_LABEL1, MAX_DISC_LABEL);
		}

		if (bError)
		{
			static BOOL bRecursedHere = FALSE;
			if (bRecursedHere)
				return FALSE;
			bRecursedHere = TRUE;

			USES_CONVERSION;
			pProp->m_pWndInPlace->SetWindowText((_bstr_t)pProp->GetValue());

			// Prevent from displaying while app exit
			if (AfxGetMainWnd()->IsWindowVisible())
			{
				AfxMessageBox(strError);
				if (pProp->m_pWndList->EditItem(pProp))
				{
					((CEdit *)pProp->m_pWndInPlace)->SetSel(0, -1);
					pProp->m_pWndInPlace->SetFocus();
				}
			}

			bRecursedHere = FALSE;

			return FALSE;
		}
	}

	if (pParam->m_strName == "outputname")
	{
		BOOL bError = TRUE;

		if (IsValidFileName(strText))
		{
			bError = FALSE;
		}

		if (bError)
		{
			static BOOL bRecursedHere = FALSE;
			if (bRecursedHere)
				return FALSE;
			bRecursedHere = TRUE;

			USES_CONVERSION;
			pProp->m_pWndInPlace->SetWindowText((_bstr_t)pProp->GetValue());

			// Prevent from displaying while app exit
			if (AfxGetMainWnd()->IsWindowVisible())
			{
				AfxMessageBox(IDS_E_INVALID_FILE_NAME_CHARS);
				if (pProp->m_pWndList->EditItem(pProp))
				{
					((CEdit *)pProp->m_pWndInPlace)->SetSel(0, -1);
					pProp->m_pWndInPlace->SetFocus();
				}
			}

			bRecursedHere = FALSE;

			return FALSE;
		}
	}

	if (pParam->m_strName == "vdimension")
	{
		BOOL bError = TRUE;

		int w, h;
		if (_stscanf(strText, "%dx%d", &w, &h) == 2)
		{
			if (w >= 16 && w <= 1920 && h >= 16 && h <= 1080)
				bError = FALSE;
		}

		if (bError)
		{
			static BOOL bRecursedHere = FALSE;
			if (bRecursedHere)
				return FALSE;
			bRecursedHere = TRUE;

			USES_CONVERSION;
			pProp->m_pWndInPlace->SetWindowText((_bstr_t)pProp->GetValue());

			// Prevent from displaying while app exit
			if (AfxGetMainWnd()->IsWindowVisible())
			{
				AfxMessageBox(IDS_E_VIDEO_DIMENSION);
				if (pProp->m_pWndList->EditItem(pProp))
				{
					((CEdit *)pProp->m_pWndInPlace)->SetSel(0, -1);
					pProp->m_pWndInPlace->SetFocus();
				}
			}

			bRecursedHere = FALSE;

			return FALSE;
		}
	}

	if (pParam->m_strName == "vframerate")
	{
		BOOL bError = TRUE;

		float fps;
		if (_stscanf(strText, "%f", &fps) == 1)
		{
			if (fps > 0.0 && fps <= 30.0)
				bError = FALSE;
		}

		if (bError)
		{
			static BOOL bRecursedHere = FALSE;
			if (bRecursedHere)
				return FALSE;
			bRecursedHere = TRUE;

			USES_CONVERSION;
			pProp->m_pWndInPlace->SetWindowText((_bstr_t)pProp->GetValue());

			// Prevent from displaying while app exit
			if (AfxGetMainWnd()->IsWindowVisible())
			{
				AfxMessageBox(IDS_E_FRAME_RATE);
				if (pProp->m_pWndList->EditItem(pProp))
				{
					((CEdit *)pProp->m_pWndInPlace)->SetSel(0, -1);
					pProp->m_pWndInPlace->SetFocus();
				}
			}

			bRecursedHere = FALSE;

			return FALSE;
		}
	}

	if (pParam->m_strName == "vbitrate")
	{
		BOOL bError = TRUE;

		float bitrate;
		if (_stscanf(strText, "%d", &bitrate) == 1)
		{
			if (bitrate > 0)
				bError = FALSE;
		}

		if (bError)
		{
			static BOOL bRecursedHere = FALSE;
			if (bRecursedHere)
				return FALSE;
			bRecursedHere = TRUE;

			USES_CONVERSION;
			pProp->m_pWndInPlace->SetWindowText((_bstr_t)pProp->GetValue());

			// Prevent from displaying while app exit
			if (AfxGetMainWnd()->IsWindowVisible())
			{
				AfxMessageBox(IDS_E_VIDEO_BIT_RATE);
				if (pProp->m_pWndList->EditItem(pProp))
				{
					((CEdit *)pProp->m_pWndInPlace)->SetSel(0, -1);
					pProp->m_pWndInPlace->SetFocus();
				}
			}

			bRecursedHere = FALSE;

			return FALSE;
		}
	}

	return pProp->CMFCPropertyGridProperty::OnUpdateValue();
}

// CPropertyGridCtrlAdvanced

IMPLEMENT_DYNAMIC(CPropertyGridCtrlAdvanced, CMFCPropertyGridCtrl)

CPropertyGridCtrlAdvanced::CPropertyGridCtrlAdvanced()
{

}

CPropertyGridCtrlAdvanced::~CPropertyGridCtrlAdvanced()
{
}


BEGIN_MESSAGE_MAP(CPropertyGridCtrlAdvanced, CMFCPropertyGridCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CPropertyGridCtrlAdvanced message handlers

int CPropertyGridCtrlAdvanced::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPropertyGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea();
	SetVSDotNetLook();

#ifdef _DEBUG
	MarkModifiedProperties();
#endif

	return 0;
}

void CPropertyGridCtrlAdvanced::SaveState()
{
	for (UINT i = 0; i < GetPropertyCount(); i++)
	{
		CMFCPropertyGridProperty *pProp = GetProperty(i);
		switch (pProp->GetData())
		{
		case CAT_GENERAL:
			AfxGetApp()->WriteProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("General"), pProp->IsExpanded());
			break;

		case CAT_VIDEO:
			AfxGetApp()->WriteProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("Video"), pProp->IsExpanded());
			break;

		case CAT_AUDIO:
			AfxGetApp()->WriteProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("Audio"), pProp->IsExpanded());
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
}

void CPropertyGridCtrlAdvanced::LoadState()
{
	// Important: must call this function first, or else redrawing problem will 
	// be caused (the expanding state of the last node will be incorrect).
	ExpandAll();

	for (UINT i = 0; i < GetPropertyCount(); i++)
	{
		CMFCPropertyGridProperty *pProp = GetProperty(i);
		switch (pProp->GetData())
		{
		case CAT_GENERAL:
			pProp->Expand(AfxGetApp()->GetProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("General"), TRUE));
			break;

		case CAT_VIDEO:
			pProp->Expand(AfxGetApp()->GetProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("Video"), TRUE));
			break;

		case CAT_AUDIO:
			pProp->Expand(AfxGetApp()->GetProfileInt(_T("Profiles\\Advanced\\Expanded"), _T("Audio"), TRUE));
			break;

		default:
			ASSERT(FALSE);
			pProp->Expand();
			break;
		}
	}

}

void CPropertyGridCtrlAdvanced::LoadProfile()
{
	RemoveAll();

	CProfileManager *pManager = CProfileManager::Instance();

	CProfileItem *pItem = NULL;
	pManager->GetCurrentProfile(NULL, &pItem);

	if (pItem == NULL)
		return;

	SetRedraw(FALSE);

	CMFCPropertyGridProperty *pGeneralCat = NULL;
	CMFCPropertyGridProperty *pEditCat = NULL;
	CMFCPropertyGridProperty *pVideoCat = NULL;
	CMFCPropertyGridProperty *pAudioCat = NULL;

	for (int i = 0; i < pItem->m_params.size(); i++)
	{
		CProfileParam *pParam = pItem->m_params[i];

		CCustomValidateProp *pProp = new CCustomValidateProp(pParam->m_strDisplay, pParam->m_strValueDef, pParam->m_strDesc, (DWORD_PTR)pParam);
		pProp->AllowEdit(pParam->m_bEditable);
		pProp->Enable(pParam->m_bEnabled);

		if (pParam->m_bEnabled && pParam->m_type == TYPE_LIST)
		{
			for (int j = 0; j < ((CProfileParamList *)pParam)->m_values.size(); j++)
			{
				PROFILE_PARAM_VALUE &value = ((CProfileParamList *)pParam)->m_values[j];
				pProp->AddOption(value.m_strDisplay, TRUE);
			}
		}

		switch (pParam->m_cat)
		{
			case CAT_GENERAL:
			{
				if (pGeneralCat == NULL)
				{
					pGeneralCat = new CMFCPropertyGridProperty(pManager->m_strCatGeneral, (DWORD_PTR)CAT_GENERAL);
					AddProperty(pGeneralCat);
				}
				pGeneralCat->AddSubItem(pProp);

				break;
			}

			case CAT_VIDEO:
			{
				if (pVideoCat == NULL)
				{
					pVideoCat = new CMFCPropertyGridProperty(pManager->m_strCatVideo, (DWORD_PTR)CAT_VIDEO);
					AddProperty(pVideoCat);
				}
				pVideoCat->AddSubItem(pProp);

				break;
			}

			case CAT_AUDIO:
			{
				if (pAudioCat == NULL)
				{
					pAudioCat = new CMFCPropertyGridProperty(pManager->m_strCatAudio, (DWORD_PTR)CAT_AUDIO);
					AddProperty(pAudioCat);
				}
				pAudioCat->AddSubItem(pProp);

				break;
			}
		}
	}

	LoadState();

	SetRedraw(TRUE);
	Invalidate(FALSE);

	OnFileChanged();
}
void CPropertyGridCtrlAdvanced::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	USES_CONVERSION;
	TRACE("CPropertyGridCtrlAdvanced::OnPropertyChanged(): %s = %s\n", pProp->GetName(), OLE2A(pProp->GetValue().bstrVal));

	CProfileParam *pParam = (CProfileParam *)pProp->GetData();
	ASSERT(pParam != NULL);

	pParam->SetValue(OLE2A(pProp->GetValue().bstrVal));

	CSubjectManager::Instance()->GetSubject(SUB_PROPERTY_GRID_CHANGED)->Notify((void *)pProp);
}

CMFCPropertyGridProperty *CPropertyGridCtrlAdvanced::FindProperty(PROFILE_PARAM_CAT cat, LPCTSTR lpszName)
{
	CMFCPropertyGridProperty *pCat = FindItemByData(cat);
	if (pCat != NULL)
	{
		for (int i = 0; i < pCat->GetSubItemsCount(); i++)
		{
			CMFCPropertyGridColorProperty *pProp = (CMFCPropertyGridColorProperty *)pCat->GetSubItem(i);
			CProfileParam *pParam = (CProfileParam *)pProp->GetData();
			if (pParam != NULL && pParam->m_strName.CompareNoCase(lpszName) == 0)
				return pProp;
		}
	}

	return NULL;
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_PROFILE_CHANGED)
{
	if (::IsWindow(m_hWnd))
		LoadProfile();
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_DISC_TYPE_CHANGED)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_GENERAL, "disctype");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_DISC_LABEL_CHANGED)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_GENERAL, "disclabel");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_ZOOM_MODE)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_VIDEO, "vzoom");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
			COptions::Instance()->m_strZoommode = (LPCTSTR)pData;
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_FRAME_RATE_CHANGED)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_VIDEO, "vframerate");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_DIMENSION_CHANGED)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_VIDEO, "vdimension");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_STANDARD_CHANGED)
{
	CMFCPropertyGridProperty *pProp = FindProperty(CAT_VIDEO, "vstandard");
	if (pProp != NULL)
	{
		CProfileParam *pParam = (CProfileParam *)pProp->GetData();
		if (pParam != NULL)
		{
			pProp->SetValue((LPCTSTR)pData);
			pParam->SetValue((LPCTSTR)pData);
		}
	}
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_FILE_SEL_CHANGED)
{
	OnFileChanged();
}

IMPLEMENT_OBSERVER(CPropertyGridCtrlAdvanced, SUB_VIDEO_TRIMMED)
{
	OnVideoTrimmed();
}

void CPropertyGridCtrlAdvanced::OnFileChanged()
{
	CMFCPropertyGridProperty *pProp;
	pProp = FindProperty(CAT_GENERAL, "outputname");
	if (pProp != NULL)
	{
		CTask *pTask = CFileList::Instance()->GetCurTask();
		if (pTask != NULL)
		{
			pProp->Enable(TRUE);
			pProp->SetValue(pTask->m_strOutputName);
		}
		else
		{
			pProp->Enable(FALSE);
			pProp->SetValue("");
		}
	}

	OnVideoTrimmed();
}

void CPropertyGridCtrlAdvanced::OnVideoTrimmed()
{
	CMFCPropertyGridProperty *pProp;

	pProp = FindProperty(CAT_GENERAL, "trimstart");
	if (pProp != NULL)
	{
		CTask *pTask = CFileList::Instance()->GetCurTask();
		if (pTask != NULL)
		{
			pProp->Enable(TRUE);
			if (pTask->m_bTrimmed)
				pProp->SetValue(FormatTime(pTask->m_fTrimStart, 0, FALSE));
			else
				pProp->SetValue("00:00:00");
		}
		else
		{
			pProp->Enable(FALSE);
			pProp->SetValue("");
		}
	}

	pProp = FindProperty(CAT_GENERAL, "trimend");
	if (pProp != NULL)
	{
		CTask *pTask = CFileList::Instance()->GetCurTask();
		if (pTask != NULL)
		{
			pProp->Enable(TRUE);
			if (pTask->m_bTrimmed)
				pProp->SetValue(FormatTime(pTask->m_fTrimEnd, 0, FALSE));
			else
				pProp->SetValue(FormatTime(pTask->m_mi.m_fDuration, 0, FALSE));
		}
		else
		{
			pProp->Enable(FALSE);
			pProp->SetValue("");
		}
	}
}

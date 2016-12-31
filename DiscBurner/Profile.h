#pragma once

#include "Task.h"
#include "resource.h"
#include <vector>
#include <string>
#include "xmlutils.h"

enum PROFILE_PARAM_CAT
{
	CAT_NONE = 0, 
	CAT_GENERAL, 
	CAT_VIDEO, 
	CAT_AUDIO
};

enum PROFILE_PARAM_TYPE
{
	TYPPE_NONE = 0, 
	TYPE_TEXT, 
	TYPE_LIST, 
};

enum MENU_PROP_DATA
{
	MENU_PROP_TEMPLATES = 1, 
	MENU_PROP_BACKGROUND_IMAGE, 
	MENU_PROP_BACKGROUND_AUDIO, 
	MENU_PROP_DURATION, 

	MENU_PROP_TITLE_TEXT,
	MENU_PROP_TITLE_FONT, 

	MENU_PROP_BUTTON_TEXT, 
	MENU_PROP_BUTTON_FONT, 

    MENU_PROP_NAVIGATION_CUR,
    MENU_PROP_NAVIGATION_LAST,
};

int FontSizeToPointSize(LONG lHeight);
int PointSizeToFontSize(int nPointSize);
CString RGBToAVS(COLORREF rgb);
COLORREF AVSToRGB(LPCTSTR lpszColor);


struct PROFILE_PARAM_VALUE
{
	CString m_strValue;
	CString m_strDisplay;
};

typedef std::vector<PROFILE_PARAM_VALUE> PROFILE_PARAM_VALUE_LIST;

class CProfileParam
{
public:
	CProfileParam()
	{
		m_cat = CAT_NONE;
		m_type = TYPPE_NONE;
		m_bEditable = FALSE;
		m_bEnabled = TRUE;
	}
	virtual ~CProfileParam()
	{
	}

	virtual BOOL GetValue(CTask *pTask, CString &strValue)
	{
		strValue = m_strValueNew;
		return TRUE;
	}

	CString GetValue()
	{
		return m_strValueNew;
	}

	void SetValue(LPCTSTR lpszValue)
	{
		m_strValueNew = lpszValue;
	}

	PROFILE_PARAM_CAT m_cat;			// Category
	PROFILE_PARAM_TYPE	m_type;			// Param type
	BOOL m_bEditable;					// Editable?
	BOOL m_bEnabled;

	CString m_strName;					// Param name
	CString m_strDisplay;				// Display name
	CString m_strDesc;					// Description
	CString m_strValueDef;				// Param default value

	PROFILE_PARAM_VALUE_LIST m_values;	// Value list

protected:
	CString m_strValueNew;				// Param value
};

// Simple parameter list. Get value from display name
class CProfileParamList : public CProfileParam
{
public:
	CProfileParamList()
	{
	}
	~CProfileParamList()
	{
	}

	virtual BOOL GetValue(CTask *pTask, CString &strValue)
	{
		for (int i = 0; i < m_values.size(); i++)
		{
			PROFILE_PARAM_VALUE &value = m_values[i];
			if (m_strValueNew.CompareNoCase(value.m_strDisplay) == 0)
			{
				strValue = value.m_strValue;
				return TRUE;
			}
		}
		
		if (m_bEditable)
		{
			strValue = m_strValueNew;
			return TRUE;
		}
		else
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
};

// Video dimension list. Get value from display name such as "Original" or "320x240"
class CProfileParamDimension : public CProfileParamList
{
public:
	CProfileParamDimension()
	{
	}
	~CProfileParamDimension()
	{
	}

	static BOOL GetDimension(LPCTSTR lpszDimension, int &nWidth, int &nHeight)
	{
		if (_stscanf(lpszDimension, "%dx%d", &nWidth, &nHeight) != 2)
			return FALSE;
		return TRUE;
	}

	virtual BOOL GetValue(CTask *pTask, CString &strValue)
	{
		CString strValueNew;
		CProfileParamList::GetValue(pTask, strValueNew);
		strValueNew.Trim();

		int nWidth, nHeight;

		// "Original" is the data value, not the display value, so it should work after localized.
		if (strValueNew.CompareNoCase("Original") == 0)
		{
			nWidth = (pTask->m_mi.m_nWidth > 0 ? pTask->m_mi.m_nWidth : 320);
			nHeight = (pTask->m_mi.m_nHeight > 0 ? pTask->m_mi.m_nHeight : 240);
		}
		else
		{
			if (_stscanf(strValueNew, "%dx%d", &nWidth, &nHeight) != 2)
				return FALSE;
		}

		nWidth = nWidth / 4 * 4;
		nHeight = nHeight / 2 * 2;

		strValue.Format("%dx%d", nWidth, nHeight);

		return TRUE;
	}
};

// Video dimension list. Get value from display name such as "Original" or "320x240"
class CProfileParamFramerate : public CProfileParamList
{
public:
	CProfileParamFramerate()
	{
	}
	~CProfileParamFramerate()
	{
	}

	virtual BOOL GetValue(CTask *pTask, CString &strValue)
	{
		CString strValueNew;
		CProfileParamList::GetValue(pTask, strValueNew);
		strValueNew.Trim();

		// "Original" is the data value, not the display value, so it should work after localized.
		if (strValueNew.CompareNoCase("Original") == 0)
		{
			float fFramerate = max(MIN_FRAMERATE, (float)(fabs(pTask->m_mi.m_fFrameRate - 0.0) < FLT_EPSILON ? DEF_FRAMERATE : pTask->m_mi.m_fFrameRate));
			strValue.Format("%.3f", fFramerate);
		}
		else
		{
			strValue = strValueNew;
		}

		return TRUE;
	}
};

class CProfileParamBitrate : public CProfileParamList
{
public:
	CProfileParamBitrate()
	{
	}
	~CProfileParamBitrate()
	{
	}
};

typedef std::vector<CProfileParam *> PROFILE_PARAM_LIST;

enum CMD_TYPE{_AV,_VIDEO,_AUDIO};
class CProfileCommand
{
public:
	CString m_strEncoder;
	CString m_strExt;
	CMD_TYPE m_nType;
	CString m_strCommand;
	CProfileCommand():m_strEncoder(_T("")),m_strExt(_T("")),m_nType(_AV),m_strCommand(_T("")){};
	~CProfileCommand(){};
   BOOL Read(Node *pNode)
   {
	   try
	   {
		   m_strEncoder = GetNodeAttribute(pNode, "encoder").c_str();
		   m_strExt     = GetNodeAttribute(pNode, "ext").c_str();
		   m_nType      = (CMD_TYPE)GetNodeAttributeInt(pNode, "type");
		   m_strCommand = pNode->firstChild()->nodeValue().c_str();
	   }
	   catch (Exception& e)
	   {
		   AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_LOAD_PROFILE1, e.displayText().c_str());
		   return FALSE;
	   }
	   return TRUE;
   }
};
typedef std::vector<CProfileCommand>  COMMAND_LIST;

class CProfileItem
{
public:
	CProfileItem(void);
	virtual ~CProfileItem(void);

	CProfileParam *GetProfileParam(LPCTSTR lpszName)
	{
		for (int i = 0; i < m_params.size(); i++)
		{
			CProfileParam *pParam = m_params[i];
			if (pParam->m_strName.CompareNoCase(lpszName) == 0)
				return pParam;
		}
		return NULL;
	}

	int FindCommand(CMD_TYPE nType)
	{
		for (int i = 0; i < m_cmds.size(); i++)
		{
			if (m_cmds[i].m_nType == nType)
				return i;
		}
		return -1;
	}
public:
	UUID m_uuid;
	CString m_strFile;
	CString m_strName;
	CString m_strDesc;
	CString m_strExt;					// File extension

	BOOL m_bHasGeneral;
	BOOL m_bHasVideo;
	BOOL m_bHasAudio;

	PROFILE_PARAM_LIST m_params;
    COMMAND_LIST       m_cmds;

};

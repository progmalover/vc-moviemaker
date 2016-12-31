#pragma once

#include "Profile.h"
#include <vector>

class CTask;
class CEncoder;
#ifdef _DVD
#define BITRATE_SAFE_FACTOR 0.92
#else
#define BITRATE_SAFE_FACTOR 0.92
#endif

const int PROFILE_GROUP_WIDTH = 300;
const int PROFILE_ICON_SIZE = 32;
const int PROFILE_ICON_MARGIN = 4;
const int PROFILE_TEXT_MARGIN = 8;

enum QUALITY_LEVEL
{
	QUALITY_LOWEST = 0, 
	QUALITY_LOWER, 
	QUALITY_STANDARD, 
	QUALITY_HIGHER, 
	QUALITY_HIGHEST
};

typedef std::vector<CProfileItem *> PROFILE_ITEM_LIST;

class CProfileGroup
{
public:
	CProfileGroup(void)
	{
		m_uuid = GUID_NULL;
		m_hIcon = NULL;
	}
	virtual ~CProfileGroup(void)
	{
		for (PROFILE_ITEM_LIST::iterator it = m_items.begin(); it != m_items.end(); ++it)
			delete *it;
		m_items.clear();
	}

public:
	UUID m_uuid;
	CString m_strName;
	CString m_strDesc;
	HICON m_hIcon;

	PROFILE_ITEM_LIST m_items;
};

typedef std::vector<CProfileGroup *> PROFILE_GROUP_LIST;

class CProfileManager
{
	friend class CProfileMenuWnd;
	friend class CProfileGroupListWnd;

public:
	CProfileManager(void);
	~CProfileManager(void);

	DECLARE_SINGLETON(CProfileManager)

protected:
	PROFILE_GROUP_LIST m_groups;
	CProfileGroup *m_pCurGroup;
	CProfileItem *m_pCurProfile;
	CString m_strCustomizedProfilePath;

	BOOL LoadProfileGroups();
	BOOL LoadCustomizedGroup(){return FALSE;};
	BOOL SaveCustomizedGroup(){return FALSE;};
	CProfileItem *LoadProfileItem(LPCTSTR lpszFile);

	CRITICAL_SECTION m_cs;

public:
	CString m_strCatGeneral; 
	CString m_strCatVideo; 
	CString m_strCatAudio;

public:

	BOOL LoadProfiles();
	void LoadCurrentProfile();
	void SetCurrentProfile(CProfileGroup *pGroup, CProfileItem *pItem);
	void GetCurrentProfile(CProfileGroup **ppGroup, CProfileItem **ppItem);
	//CEncoder *CreateEncoder(CTask *pTask);
	CString GetEncoderCmd(taskAtom *atom, CTask *pTask,CMD_TYPE nTypes);
	CProfileParam *FindProfileParam(LPCTSTR lpszName);
	CProfileGroup *GetCustomizedGroup(){return NULL;};
	BOOL SaveCustomizedProfile(LPCTSTR lpszNameOld, LPCTSTR lpszName, LPCTSTR lpszDesc, BOOL bRename){return FALSE;};
	BOOL DeleteCustomizedProfile(LPCTSTR lpszName){return FALSE;};
	CProfileItem *FindCustomizedProfile(LPCTSTR lpszName){return NULL;};

	UINT m_nVBitrate;
	UINT m_nABitrate;

	BOOL CalclateBitrate(BOOL &bExceeded, BOOL bPromptToChange);
	//统一估算DVD空间大小的接口
    INT64  CalclateSize();
};

#include "StdAfx.h"
#include "ProfileManager.h"
#include "MainFrm.h"
#include "FileList.h"

#include "EncoderMEncoder.h"
#include "EncoderFFmpeg.h"
#include "Options.h"
#include "Burner.h"
#include "xmlutils.h"

#include <strstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SINGLETON(CProfileManager)

CProfileManager::CProfileManager(void)
{
	m_strCatGeneral.LoadString(IDS_CAT_GENERAL);
	m_strCatVideo.LoadString(IDS_CAT_VIDEO);
	m_strCatAudio.LoadString(IDS_CAT_AUDIO);

	m_pCurGroup = NULL;
	m_pCurProfile = NULL;

	m_nVBitrate = 0;
	m_nABitrate = 0;

	TCHAR szPath[_MAX_PATH];
	if (SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE))
	{
		::PathRemoveBackslash(szPath);
		m_strCustomizedProfilePath.Format("%s\\SourceTec\\%s\\Profiles", szPath, AfxGetApp()->m_pszProfileName);
	}

	::InitializeCriticalSection(&m_cs);
}

CProfileManager::~CProfileManager(void)
{
	if (m_pCurGroup != NULL && m_pCurProfile != NULL)
	{
		AfxGetApp()->WriteProfileString("Profiles", "Current Group", m_pCurGroup->m_strName);
		AfxGetApp()->WriteProfileString("Profiles", "Current Profile", m_pCurProfile->m_strName);
	}

	for (PROFILE_GROUP_LIST::iterator it = m_groups.begin(); it != m_groups.end(); ++it)
		delete *it;
	m_groups.clear();

	::DeleteCriticalSection(&m_cs);
}

void CProfileManager::SetCurrentProfile(CProfileGroup *pGroup, CProfileItem *pItem)
{
	// reset all parameters to default
	for (int i = 0; i < pItem->m_params.size(); i++)
	{
		CProfileParam *pParam = pItem->m_params[i];
		pParam->SetValue(pParam->m_strValueDef);
	}

	m_pCurGroup = pGroup;
	m_pCurProfile = pItem;

	CSubjectManager::Instance()->GetSubject(SUB_PROFILE_CHANGED)->Notify(NULL);
}

void CProfileManager::GetCurrentProfile(CProfileGroup **ppGroup, CProfileItem **ppItem)
{
	if (ppGroup != NULL)
		*ppGroup = m_pCurGroup;
	if (ppItem != NULL)
		*ppItem = m_pCurProfile;
}

BOOL CProfileManager::LoadProfiles()
{
	if (!LoadProfileGroups())
		return FALSE;

	return TRUE;
}

BOOL CProfileManager::LoadProfileGroups()
{
	CString strFile;
	strFile.Format("%s\\Profiles\\Profiles.xml", ::GetModuleFilePath());

	try
	{
		AutoPtr<Document> pDoc = OpenXMLDocument(strFile);
		//TRACE("Document: %s\n", pDoc->nodeName().c_str());

		Node *pRoot = pDoc->firstChild();
		ASSERT(pRoot);
		//TRACE(" Root: %s\n", pRoot->nodeName().c_str());
		if (pRoot->nodeName() != "profiles")
			return FALSE;

		Node *pGroupNode = pRoot->firstChild();
		while (pGroupNode)
		{
			//TRACE("  <%s>\n", pGroupNode->nodeName().c_str());
			if (pGroupNode->nodeName() == "group")
			{
				pGroupNode->normalize();

				CProfileGroup *pGroup = new CProfileGroup();
				pGroup->m_strName = GetNodeAttribute(pGroupNode, "name").c_str();
				pGroup->m_strDesc = GetNodeAttribute(pGroupNode, "desc").c_str();

				CString strIcon;
				strIcon.Format("%s\\Profiles\\%s", GetModuleFilePath(), GetNodeAttribute(pGroupNode, "icon").c_str());
				if (::PathFileExists(strIcon) && !::PathIsDirectory(strIcon))
					pGroup->m_hIcon = (HICON)LoadImage(AfxGetResourceHandle(), strIcon, IMAGE_ICON, 24, 24, LR_LOADFROMFILE); // ask for specific icon size
				if (pGroup->m_hIcon == NULL)
				{
					strIcon.Format("%s\\Profiles\\default.ico", GetModuleFilePath());
					pGroup->m_hIcon = (HICON)LoadImage(AfxGetResourceHandle(), strIcon, IMAGE_ICON, 24, 24, LR_LOADFROMFILE); // ask for specific icon size
				}

				Node *pItemNode = pGroupNode->firstChild();
				while (pItemNode)
				{
					//TRACE("    <%s>\n", pItemNode->nodeName().c_str());
					if (pItemNode->nodeName() == "profile")
					{
						std::string sName = GetNodeAttribute(pItemNode, "name");
						if (!sName.empty())
						{
							CString strItem;
							strItem.Format("%s\\Profiles\\%s.xml", ::GetModuleFilePath(), sName.c_str());
							CProfileItem *pItem = LoadProfileItem(strItem);
						#ifdef _DEBUG
							if (pItem == NULL)
								TRACE("Failed to load profile \"%s\"\n", sName.c_str());
						#endif
							if (pItem)
								pGroup->m_items.push_back(pItem);
						}
					}

					pItemNode = pItemNode->nextSibling();
				}

				m_groups.push_back(pGroup);
			}

			pGroupNode = pGroupNode->nextSibling();
		}

		return TRUE;
	}
	catch (Exception& e)
	{
		TRACE("Error: %s\n", e.displayText().c_str());
		return FALSE;
	}
}

CProfileItem *CProfileManager::LoadProfileItem(LPCTSTR lpszFile)
{
	CProfileItem *pItem = NULL;

	try
	{
		AutoPtr<Document> pDoc = OpenXMLDocument(lpszFile);
		//TRACE("Document: %s\n", pDoc->nodeName().c_str());

		Node *pItemNode = pDoc->firstChild();
		ASSERT(pItemNode != NULL);
		if (pItemNode->nodeName() != "profile")
			return NULL;

		pItemNode->normalize();

		pItem = new CProfileItem();

		pItem->m_strFile = lpszFile;
		pItem->m_strName = GetNodeAttribute(pItemNode, "name").c_str();
		pItem->m_strDesc = GetNodeAttribute(pItemNode, "desc").c_str();
		pItem->m_strExt = GetNodeAttribute(pItemNode, "ext").c_str();

		Node *pCatNode = pItemNode->firstChild();
		while (pCatNode)
		{
			//TRACE("     <%s>\n", pCatNode->nodeName().c_str());
			if (pCatNode->nodeName() == "general" || pCatNode->nodeName() == "video" || pCatNode->nodeName() == "audio")
			{
				if (pCatNode->nodeName() == "general")
					pItem->m_bHasGeneral = TRUE;
				if (pCatNode->nodeName() == "video")
					pItem->m_bHasVideo = TRUE;
				if (pCatNode->nodeName() == "audio")
					pItem->m_bHasAudio = TRUE;

				Node *pParamNode = pCatNode->firstChild();
				while (pParamNode)
				{
					//TRACE("      <%s>\n", pParamNode->nodeName().c_str());

					CProfileParam *pParam = NULL;

					std::string sType = GetNodeAttribute(pParamNode, "type");
					if (sType == "list")
					{
						if (_tcsicmp(pParamNode->nodeName().c_str(), "vdimension") == 0)
						{
							pParam = new CProfileParamDimension();
						}
						else if (_tcsicmp(pParamNode->nodeName().c_str(), "vframerate") == 0)
						{
							pParam = new CProfileParamFramerate();
						}
						else if (_tcsicmp(pParamNode->nodeName().c_str(), "vbitrate") == 0 || _tcsicmp(pParamNode->nodeName().c_str(), "abitrate") == 0)
						{
							pParam = new CProfileParamBitrate();
						}
						else
						{
							pParam = new CProfileParamList();
						}

						pParam->m_type = TYPE_LIST;

						Node *pValueNode = pParamNode->firstChild();
						while (pValueNode)
						{
							//TRACE("       <%s value=\"%s\">\n", pValueNode->nodeName().c_str(), GetNodeAttribute(pValueNode, "value").c_str());
							if (pValueNode->nodeName() == "list")
							{
								PROFILE_PARAM_VALUE value;
								value.m_strValue = GetNodeAttribute(pValueNode, "value").c_str();
								value.m_strDisplay = GetNodeAttribute(pValueNode, "display").c_str();

								pParam->m_values.push_back(value);
							}

							pValueNode = pValueNode->nextSibling();
						}
					}
					else
					{
						pParam = new CProfileParam();
						pParam->m_type = TYPE_TEXT;
					}

					if (pCatNode->nodeName() == "general")
						pParam->m_cat = CAT_GENERAL;
					else if (pCatNode->nodeName() == "video")
						pParam->m_cat = CAT_VIDEO;
					else
						pParam->m_cat = CAT_AUDIO;
					pParam->m_strName = pParamNode->nodeName().c_str();
					pParam->m_strDisplay = GetNodeAttribute(pParamNode, "display").c_str();
					pParam->m_strValueDef = GetNodeAttribute(pParamNode, "value").c_str();
					pParam->m_strDesc = GetNodeAttribute(pParamNode, "desc").c_str();
					pParam->m_bEditable = GetNodeAttribute(pParamNode, "editable") == "1" ? TRUE : FALSE;
					pParam->m_bEnabled = GetNodeAttribute(pParamNode, "enabled") == "0" ? FALSE : TRUE;

					pParam->SetValue(pParam->m_strValueDef);
					
					if (sType == "list")
					{
						if (pParam->m_values.size() <= 1)
							pParam->m_bEnabled = FALSE;
					}

					pItem->m_params.push_back(pParam);

					pParamNode = pParamNode->nextSibling();
				}
			}
			else if (pCatNode->nodeName() == "command")
			{
				pCatNode->normalize();
				CProfileCommand cmd;
				if(cmd.Read(pCatNode))
					pItem->m_cmds.push_back(cmd);
				else
				{
					ASSERT(FALSE);
				}
			}
			else
			{
				if (pCatNode->nodeName() != "#comment")
					ASSERT(FALSE);
			}

			pCatNode = pCatNode->nextSibling();
		}

		return pItem;
	}
	catch (Exception& e)
	{
		AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_LOAD_PROFILE1, e.displayText().c_str());

		if (pItem != NULL)
			delete pItem;

		return NULL;
	}
}

void CProfileManager::LoadCurrentProfile()
{
	SetCurrentProfile(m_groups[0], m_groups[0]->m_items[0]);
}


CProfileParam *CProfileManager::FindProfileParam(LPCTSTR lpszName)
{
	for (int i = 0; i < m_pCurProfile->m_params.size(); i++)
	{
		CProfileParam *pParam = m_pCurProfile->m_params[i];

		// generate video dimension
		if (pParam->m_strName == lpszName)
			return pParam;
	}

	return NULL;
}

CString CProfileManager::GetEncoderCmd(taskAtom *atom, CTask *pTask,CMD_TYPE nTypes)
{
	ASSERT(m_pCurProfile != NULL);
	CString strCommandLine;
	ASSERT(!atom->avs.IsEmpty() && ::PathFileExists(atom->avs));

	if(!m_pCurProfile) return strCommandLine;

	int nIndex = m_pCurProfile->FindCommand(nTypes);
	ASSERT(nIndex!=-1);
	strCommandLine = m_pCurProfile->m_cmds[nIndex].m_strCommand;


	UINT nVBitrate = 0;
	UINT nABitrate = 0;

	for (int i = 0; i < m_pCurProfile->m_params.size(); i++)
	{
		CProfileParam *pParam = m_pCurProfile->m_params[i];
		atom->ext=m_pCurProfile->m_cmds[nIndex].m_strExt;

		// generate video dimension
		if (pParam->m_strName == "vdimension")
		{
			if (strCommandLine.Find("$(width)") >= 0 || strCommandLine.Find("$(height)") >= 0)
			{
				CString strDimension;
				CProfileParamDimension *pDimension = (CProfileParamDimension *)pParam;
				if (!pDimension->GetValue(pTask, strDimension))
					return NULL;

				int nWidth, nHeight;
				if (!CProfileParamDimension::GetDimension(strDimension, nWidth, nHeight))
					return NULL;

				CString strWidth;
				CString strHeight;
				strWidth.Format("%d", nWidth);
				strHeight.Format("%d", nHeight);
				strCommandLine.Replace("$(width)", strWidth);
				strCommandLine.Replace("$(height)", strHeight);
			}

			continue;
		}
		else if (pParam->m_strName == "vbitrate")
		{
			continue;
		}
		else if (pParam->m_strName == "abitrate")
		{
			continue;
		}
		else if (pParam->m_strName == "vcodec" || pParam->m_strName == "acodec")
		{
			/*
		#ifdef _DEBUG
			strOutputName += "-" + pParam->m_strValueDef;
			strOutputName.Replace('/', '-');	// e.g. H.264/AVC
		#endif
		*/
		}

		CString strValue;
		if (!pParam->GetValue(pTask, strValue))
		{
		#ifdef _DEBUG
			AfxMessageBoxEx(0, "CProfileParam::GetValue(\"%s\") failed.\n", pParam->m_strName);
		#endif
			return NULL;
		}

		CString strToken;
		strToken.Format("$(%s)", pParam->m_strName);

		strCommandLine.Replace(strToken, strValue);
	}

	ASSERT(strCommandLine.Find("$(vbitrate)") > 0 || strCommandLine.Find("$(abitrate)") > 0);

	CString strVBitrate;
	strVBitrate.Format("%d", m_nVBitrate);
	strCommandLine.Replace("$(vbitrate)", strVBitrate);

	CString strABitrate;
	strABitrate.Format("%d", m_nABitrate);
	strCommandLine.Replace("$(abitrate)", strABitrate);

	// set input file (*.avs)
	strCommandLine.Replace("$(input)", (LPCTSTR)atom->avs);		// Use alternative AVS in release version

	// set encoding threads
	CString strEncodingThreads;
	strEncodingThreads.Format("%d", COptions::Instance()->m_nEncodingThreads);
	strCommandLine.Replace("$(threads)", strEncodingThreads);

	//CString strffPreset;
	//if(AfxGetApp()->GetProfileInt("Debug", "Use fpre", TRUE))
	//	strffPreset.Format(" -fpre \"%s\\encoder\\libx264-sothink-default.ffpreset\"", GetModuleFilePath());
	//else
	//	strffPreset = AfxGetApp()->GetProfileString("Debug", "Default fpre","-flags +loop -level 41 -partitions +all -coder 1 -g 24 -keyint_min 2");*/
	//strCommandLine.Replace("$(fpre)", strffPreset);

	return strCommandLine;
}

BOOL CProfileManager::CalclateBitrate(BOOL &bExceeded, BOOL bPromptIfbExceed)
{
	bExceeded = FALSE;

	double fTotalDuration = CFileList::Instance()->GetTotalDuration();
	if (fTotalDuration < FLT_EPSILON)
		return FALSE;

	int nWidth = 0, nHeight = 0;

	for (int i = 0; i < m_pCurProfile->m_params.size(); i++)
	{
		CProfileParam *pParam = m_pCurProfile->m_params[i];

		// generate video dimension
		if (pParam->m_strName == "vdimension")
		{
			CString strDimension;
			CProfileParamDimension *pDimension = (CProfileParamDimension *)pParam;
			if (!pDimension->GetValue(NULL, strDimension))
				return FALSE;

			if (!CProfileParamDimension::GetDimension(strDimension, nWidth, nHeight))
				return FALSE;
		}
		else if (pParam->m_strName == "vbitrate")
		{
			CString strVBitrate;
			if (!pParam->GetValue(NULL, strVBitrate))
				return FALSE;

			m_nVBitrate = atoi(strVBitrate);
		}
		else if (pParam->m_strName == "abitrate")
		{
			CString strABitrate;
			if (!pParam->GetValue(NULL, strABitrate))
				return FALSE;

			m_nABitrate = atoi(strABitrate);
		}
	}
	
	CMFCPropertyGridProperty *pPropDisc = GetPropertyGridAdvanced()->FindProperty(CAT_GENERAL, "disctype");
	if (pPropDisc == NULL)
		return FALSE;

	CProfileParam *pParam = (CProfileParam *)pPropDisc->GetData();
	CString strValue;
	pParam->GetValue(NULL, strValue);

	UINT nDiscCap = _ttoi(strValue) ;
	fTotalDuration += CFileList::Instance()->MenusDuration();

	UINT nTotalBitrate = (UINT)(nDiscCap * 8 / fTotalDuration);
	UINT nTotalBitrateSafe = (UINT)(nTotalBitrate * BITRATE_SAFE_FACTOR);
	nTotalBitrateSafe -= TSMUXER_BITRATE;
	if (m_nVBitrate == 0)	// Auto calculate
	{
	#ifdef _BD
		switch (nWidth)
		{
		#ifdef _DEBUG
			case 320:		//320x240
				m_nVBitrate = 1000;
				break;
		#endif
			case 1280:		//1280x720
				m_nVBitrate = 5000;
				break;
			case 1440:		//1440x1080
				m_nVBitrate = 8000;
				break;
			case 1920:		//1920x1080
				m_nVBitrate = 10000;
				break;
			default:
				ASSERT(FALSE);
				return FALSE;
		}
	#else
		m_nVBitrate = 6000;
	#endif
		if(nTotalBitrateSafe>m_nABitrate && nTotalBitrateSafe - m_nABitrate>MIN_VIDEO_BITRATE)
			m_nVBitrate = min(m_nVBitrate, nTotalBitrateSafe - m_nABitrate);
		else
		{
			m_nVBitrate = MIN_VIDEO_BITRATE;
			if (bPromptIfbExceed)
				AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_BIT_RATE_TOO_LOW,MIN_VIDEO_BITRATE) ;
			bExceeded = TRUE;
		}
	}
	else
	{
		if (m_nVBitrate + m_nABitrate > nTotalBitrateSafe)
		{
			if (bPromptIfbExceed && AfxMessageBoxEx(MB_ICONWARNING | MB_OKCANCEL, IDS_E_BIT_RATE_TOO_HIGH2, m_nVBitrate, nTotalBitrateSafe - m_nABitrate) == IDOK)
			{
				m_nVBitrate = nTotalBitrateSafe - m_nABitrate;
				if(m_nVBitrate < MIN_VIDEO_BITRATE)
				{
					if (bPromptIfbExceed)
						AfxMessageBoxEx(MB_ICONWARNING | MB_OK, IDS_E_BIT_RATE_TOO_LOW,MIN_VIDEO_BITRATE) ;
					bExceeded = TRUE;
				}
			}
			else
				bExceeded = TRUE;
		}
	} 
	return TRUE;
}

//byte 
INT64  CProfileManager::CalclateSize()
{
	INT64 nSize = 0;
	double fTotalDuration = CFileList::Instance()->GetTotalDuration();
	       fTotalDuration += CFileList::Instance()->MenusDuration();
	if (fTotalDuration > FLT_EPSILON)
	{
		int nTotalBitrate = m_nVBitrate + m_nABitrate;
		nTotalBitrate += TSMUXER_BITRATE;
		nSize = (INT64)nTotalBitrate*1000/8 * fTotalDuration/BITRATE_SAFE_FACTOR ;
	}
	return nSize;
}

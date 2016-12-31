#include "StdAfx.h"
#include "Converter.h"
#include "MenuTemplateManager.h"
#include "Options.h"
#include "xmlutils.h"
#include "FileList.h"
#include "ProfileManager.h"
#include "MenuTemplateEx.h"

#include <strstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SINGLETON(CMenuTemplateManager)

CMenuTemplateManager::CMenuTemplateManager(void)
{
	m_pTemplate = NULL;
}

CMenuTemplateManager::~CMenuTemplateManager(void)
{
	if (m_pTemplate != NULL)
		AfxGetApp()->WriteProfileString("Menu Templates", "Current Template", m_pTemplate->m_strName);

	for (MENU_TEMPLATE_LIST::iterator it = m_templates.begin(); it != m_templates.end(); ++it)
		delete *it;
	m_templates.clear();
}

void CMenuTemplateManager::SetCurrentTemplate(CMenuTemplate *pTemplate)
{
	m_pTemplate = pTemplate;

	//CProfileComboBoxButton *pComboBox = GetProfileComboBox();
	//if (pComboBox != NULL)
	//	pComboBox->Update();
}

void CMenuTemplateManager::SetCurrentTemplate(LPCTSTR lpszName)
{
	for (MENU_TEMPLATE_LIST::iterator it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		CMenuTemplate *pTemplate = *it;
		if (pTemplate->m_strName == lpszName)
		{
			SetCurrentTemplate(pTemplate);
			return;
		}
	}

	ASSERT(FALSE);
}

CMenuTemplate *CMenuTemplateManager::GetCurrentTemplate()
{
	return m_pTemplate;
}

BOOL CMenuTemplateManager::LoadMenuTemplates()
{
	CString strFind;
	strFind.Format("%s\\Templates\\", ::GetModuleFilePath());
	strFind += "*.*";

	CFileFind ff;
	BOOL ret = ff.FindFile(strFind);

	while (ret)
	{
		ret = ff.FindNextFile();
		if (!ff.IsDots() && ff.IsDirectory())
		{
			CMenuTemplate *pTemplate = new CMenuTemplate();
			if (pTemplate->LoadMenuTemplate(ff.GetFilePath()))
			{
				m_templates.push_back(pTemplate);
			}
			else
				delete pTemplate;
		}
	}

	CMenuTemplate *pTemplate = new CMenuTemplate();
	pTemplate->m_bTemplate = FALSE;
	pTemplate->m_strName.LoadString(IDS_NO_MENU);
	m_templates.push_back(pTemplate);

#ifdef _DVD
	CString strDefaultMenu = "";
#else
	CString strDefaultMenu = "window7";
#endif
	CString strName = AfxGetApp()->GetProfileString("Menu Templates", "Current Template", strDefaultMenu);

	for (MENU_TEMPLATE_LIST::iterator it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		CMenuTemplate *pTemplate = *it;
		if (pTemplate->m_strName.CompareNoCase(strName) == 0)
		{
			SetCurrentTemplate(pTemplate);
			return TRUE;
		}
	}

	for (MENU_TEMPLATE_LIST::iterator it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		CMenuTemplate *pTemplate = *it;
		if (pTemplate->m_strName.CompareNoCase("default") == 0)
		{
			SetCurrentTemplate(pTemplate);
			return TRUE;
		}
	}
	
	if (m_templates.size() > 0)
		SetCurrentTemplate(m_templates[0]);

	return TRUE;
}

BOOL CMenuTemplate::LoadMenuTemplate(LPCTSTR lpszFolder)
{
	m_strFolder = lpszFolder;

	CString strFile;
	strFile.Format("%s\\template.xml", lpszFolder);

	try
	{		
		AutoPtr<Document> pDoc = OpenXMLDocument(strFile);

		Node *pMenuNode = pDoc->getElementById("menu", "id");
		if (pMenuNode == NULL)
			throw Exception("No <menu> node found");
		pMenuNode->normalize();

		m_bTemplate = TRUE;
		m_strName = GetNodeAttribute(pMenuNode, "name").c_str();
		m_strBackgroundImage = GetNodeAttribute(pMenuNode, "background_image").c_str();
		m_strBackgroundAudio = GetNodeAttribute(pMenuNode, "background_audio").c_str();
		m_strTitleText = GetNodeAttribute(pMenuNode, "title_text").c_str();
		m_strTitleTextColor = GetNodeAttribute(pMenuNode, "title_color").c_str();
		m_strTitleFont = GetNodeAttribute(pMenuNode, "title_font").c_str();
		m_strTitleFontSize = GetNodeAttribute(pMenuNode, "title_font_size").c_str();
		m_strTitleX = GetNodeAttribute(pMenuNode, "title_x").c_str();
		m_strTitleY = GetNodeAttribute(pMenuNode, "title_y").c_str();
		m_strDuration = GetNodeAttribute(pMenuNode, "duration").c_str();
#ifdef _BD
		m_nResourceFormat = (RESOURCE_FORMAT)GetNodeAttributeInt(pMenuNode, "resource_format");
#endif

		Node *pButtonsNode = pDoc->getElementById("buttons", "id");
		if (pMenuNode == NULL)
			throw Exception("No <buttons> node found");

		m_strButtonTextAlign = GetNodeAttribute(pButtonsNode, "text_align").c_str();
		m_strButtonTextColor = GetNodeAttribute(pButtonsNode, "text_color").c_str();
		m_strButtonFont = GetNodeAttribute(pButtonsNode, "font").c_str();
		m_strButtonFontSize = GetNodeAttribute(pButtonsNode, "font_size").c_str();
#ifdef _BD
		//sound="sound.bdmv" sound_id="0"
		m_strSound  = GetNodeAttribute(pButtonsNode, "sound").c_str();
		m_nSoundIDS = GetNodeAttributeInt(pButtonsNode,"sound_id_s");
		m_nSoundIDA = GetNodeAttributeInt(pButtonsNode,"sound_id_a");
#endif
		AutoPtr<ElementsByTagNameList> pNodeList = (ElementsByTagNameList *)pDoc->getElementsByTagName("button");
		unsigned long length = pNodeList->length();
		for (int i = 0; i < length; i++)
		{
			Node *pButtonNode = pNodeList->item(i);
			BUTTON button;
			button.rc[0].left = GetNodeAttributeInt(pButtonNode, "x0");
			button.rc[0].top = GetNodeAttributeInt(pButtonNode, "y0");
			button.rc[0].right = button.rc[0].left + GetNodeAttributeInt(pButtonNode, "w0");
			button.rc[0].bottom = button.rc[0].top + GetNodeAttributeInt(pButtonNode, "h0");

			button.rc[1].left = GetNodeAttributeInt(pButtonNode, "x1");
			button.rc[1].top = GetNodeAttributeInt(pButtonNode, "y1");
			button.rc[1].right = button.rc[1].left + GetNodeAttributeInt(pButtonNode, "w1");
			button.rc[1].bottom = button.rc[1].top + GetNodeAttributeInt(pButtonNode, "h1");

			button.rc[2].left = GetNodeAttributeInt(pButtonNode, "x2");
			button.rc[2].top = GetNodeAttributeInt(pButtonNode, "y2");
			button.rc[2].right = button.rc[2].left + GetNodeAttributeInt(pButtonNode, "w2");
			button.rc[2].bottom = button.rc[2].top + GetNodeAttributeInt(pButtonNode, "h2");
#ifdef _BD
			button.bSRepeated = GetNodeAttributeInt(pButtonNode, "srepeat");
#endif
			button.rc[3].left = GetNodeAttributeInt(pButtonNode, "x3");
			button.rc[3].top = GetNodeAttributeInt(pButtonNode, "y3");
			button.rc[3].right = button.rc[3].left + GetNodeAttributeInt(pButtonNode, "w3");
			button.rc[3].bottom = button.rc[3].top + GetNodeAttributeInt(pButtonNode, "h3"); 

#ifdef _BD
			button.nNID  =  GetNodeAttributeInt(pButtonNode, "nid"); //Normal 
			button.nSID  =  GetNodeAttributeInt(pButtonNode, "sid"); //Select
			button.nAID  =  GetNodeAttributeInt(pButtonNode, "aid"); //Active
#endif
			m_listButtons.push_back(button);
		}

#ifdef _BD
		AutoPtr<ElementsByTagNameList> pNodeList2 = (ElementsByTagNameList *)pDoc->getElementsByTagName("map");
		Node *pMapNode = pNodeList2->item(0);
		m_nMapWidth  =  GetNodeAttributeInt(pMapNode, "width");
		m_nMapHeight =  GetNodeAttributeInt(pMapNode, "height");
		m_strMatrix  =  GetNodeAttribute(pMapNode,"Matrix").c_str();
#endif

		m_strNameNew = m_strName;
		m_strBackgroundImageNew = m_strBackgroundImage;
		m_strBackgroundAudioNew = m_strBackgroundAudio;
		m_strTitleTextNew = m_strTitleText;
		m_strTitleTextColorNew = m_strTitleTextColor;
		m_strTitleFontNew = m_strTitleFont;
		m_strTitleFontSizeNew = m_strTitleFontSize;
		m_strDurationNew = m_strDuration;
		m_strButtonFontNew = m_strButtonFont;
		m_strButtonFontSizeNew = m_strButtonFontSize;
		m_strButtonTextColorNew = m_strButtonTextColor;

		if (m_listButtons.size() <= 2)
			throw Exception("No enough <button> nodes found.");

		return TRUE;
	}
	catch (Exception& e)
	{
	#ifdef _DEBUG
		AfxMessageBoxEx(0, "Error: failed to parse %s. %s.", (LPCTSTR)strFile, e.displayText().c_str());
	#endif
		return FALSE;
	}
}

BOOL CMenuTemplate::SaveMenuTemplate(LPCTSTR lpszFile)
{
	std::strstream ss;
	try
	{
		CProfileItem *pProfile;
		CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);

		
		AutoPtr<Document> pDoc = new Document;

		// <menu>
		AutoPtr<Element> pMenuNode = pDoc->createElement("menu");

		CString strStandard = "NTSC";
		CString strAspect = "16:9";
		CString strValue;
		CProfileParam *pParam = NULL;
#ifdef _DVD
		pParam = pProfile->GetProfileParam("vstandard");
		strStandard = pParam->GetValue();

		pParam = pProfile->GetProfileParam("vaspect");
		strAspect = pParam->GetValue();
#else
		//disctype="$(disctype)"   width="$(width)"        height="$(height)" 
		//framerate="$(framerate)" vbitrate="$(vbitrate)" 
		//last_title_action="0"    channels="$(channels)" 
		//resource_format="" abitrate="" asamplerate="">
		SetNodeAttribute(pMenuNode, "disctype", COptions::Instance()->m_strDiscType.Left(4).CompareNoCase("DVD-") == 0?"avchd":"bd");

		pParam = pProfile->GetProfileParam("vdimension");
		strValue = pParam->GetValue();
		int nWidth=1920,nHeight=1080;
		//CProfileParamDimension::GetDimension(strValue,nWidth,nHeight);
		SetNodeAttribute(pMenuNode, "width",nWidth);
		SetNodeAttribute(pMenuNode, "height",nHeight);

		pParam = pProfile->GetProfileParam("vframerate");
		pParam->GetValue(NULL,strValue);
		SetNodeAttribute(pMenuNode, "framerate",strValue);
		

		SetNodeAttribute(pMenuNode, "last_title_action",COptions::Instance()->m_nAfterLastVideoOptions==BACK_FIRST_PLAYBACK?0xFFFF:COptions::Instance()->m_nAfterLastVideoOptions);
		SetNodeAttribute(pMenuNode, "resource_format", m_nResourceFormat);
		SetNodeAttribute(pMenuNode, "abitrate",CProfileManager::Instance()->m_nABitrate);
		pParam = pProfile->GetProfileParam("asamplerate");
		pParam->GetValue(NULL,strValue);
		SetNodeAttribute(pMenuNode, "asamplerate",strValue);

		SetNodeAttribute(pMenuNode, "first_title_id",FIRST_MPLS);
		SetNodeAttribute(pMenuNode, "first_playback_id",2);
#endif

		SetNodeAttribute(pMenuNode, "vbitrate",CProfileManager::Instance()->m_nVBitrate);
		pParam = pProfile->GetProfileParam("achannels");
		pParam->GetValue(NULL,strValue);
		SetNodeAttribute(pMenuNode, "channels",strValue);
		SetNodeAttribute(pMenuNode, "abitrate",CProfileManager::Instance()->m_nABitrate);

		SetNodeAttribute(pMenuNode, "id", "menu");
		SetNodeAttribute(pMenuNode, "standard", strStandard);
		SetNodeAttribute(pMenuNode, "aspect", strAspect);
		SetNodeAttribute(pMenuNode, "name", m_strName);

		SetNodeAttribute(pMenuNode, "background_image", m_strBackgroundImageNew);
		SetNodeAttribute(pMenuNode, "background_audio", m_strBackgroundAudioNew);

		SetNodeAttribute(pMenuNode, "title_text", m_strTitleTextNew);
		SetNodeAttribute(pMenuNode, "title_color", m_strTitleTextColorNew);
		SetNodeAttribute(pMenuNode, "title_font", m_strTitleFontNew);
		SetNodeAttribute(pMenuNode, "title_font_size", m_strTitleFontSizeNew);
		SetNodeAttribute(pMenuNode, "title_x", m_strTitleX);
		SetNodeAttribute(pMenuNode, "title_y", m_strTitleY);
		SetNodeAttribute(pMenuNode, "duration", m_strDurationNew);

		// <buttons>
		AutoPtr<Element> pButtonsNode = pDoc->createElement("buttons");
		SetNodeAttribute(pButtonsNode, "id", "buttons");
		SetNodeAttribute(pButtonsNode, "text_align", m_strButtonTextAlign);
		SetNodeAttribute(pButtonsNode, "text_color", m_strButtonTextColorNew);
		SetNodeAttribute(pButtonsNode, "font", m_strButtonFontNew);
		SetNodeAttribute(pButtonsNode, "font_size", m_strButtonFontSizeNew);
#ifdef _BD
		//sound="sound.bdmv" sound_id="0"
		int nVersion = COptions::Instance()->m_strDiscType.Left(4).CompareNoCase("DVD-") == 0?1:2;
		m_strSound.Format("sound_%d.bdmv",nVersion);
		SetNodeAttribute(pButtonsNode,"sound",""/*m_strSound*/);
		SetNodeAttribute(pButtonsNode,"sound_id_s",0xFF/*m_nSoundIDS*/);
		SetNodeAttribute(pButtonsNode,"sound_id_a",0xFF/*m_nSoundIDA*/);
#endif
		for (int i = 0; i < m_listButtons.size(); i++)
		{
			AutoPtr<Element> pButtonNode = pDoc->createElement("button");

			SetNodeAttribute(pButtonNode, "x0", m_listButtons[i].rc[0].left);
			SetNodeAttribute(pButtonNode, "y0", m_listButtons[i].rc[0].top);
			SetNodeAttribute(pButtonNode, "w0", m_listButtons[i].rc[0].Width());
			SetNodeAttribute(pButtonNode, "h0", m_listButtons[i].rc[0].Height());

			SetNodeAttribute(pButtonNode, "x1", m_listButtons[i].rc[1].left);
			SetNodeAttribute(pButtonNode, "y1", m_listButtons[i].rc[1].top);
			SetNodeAttribute(pButtonNode, "w1", m_listButtons[i].rc[1].Width());
			SetNodeAttribute(pButtonNode, "h1", m_listButtons[i].rc[1].Height());

			SetNodeAttribute(pButtonNode, "x2", m_listButtons[i].rc[2].left);
			SetNodeAttribute(pButtonNode, "y2", m_listButtons[i].rc[2].top);
			SetNodeAttribute(pButtonNode, "w2", m_listButtons[i].rc[2].Width());
			SetNodeAttribute(pButtonNode, "h2", m_listButtons[i].rc[2].Height());
#ifdef _BD
			if(m_listButtons[i].bSRepeated)
				SetNodeAttribute(pButtonNode, "srepeat", 1);
#endif

			SetNodeAttribute(pButtonNode, "x3", m_listButtons[i].rc[3].left);
			SetNodeAttribute(pButtonNode, "y3", m_listButtons[i].rc[3].top);
			SetNodeAttribute(pButtonNode, "w3", m_listButtons[i].rc[3].Width());
			SetNodeAttribute(pButtonNode, "h3", m_listButtons[i].rc[3].Height());
#ifdef _BD
			SetNodeAttribute(pButtonNode, "nid", m_listButtons[i].nNID);
			SetNodeAttribute(pButtonNode, "sid", m_listButtons[i].nSID);
			SetNodeAttribute(pButtonNode, "aid", m_listButtons[i].nAID);
#endif
			pButtonsNode->appendChild(pButtonNode);
		}

#ifdef _BD
		AutoPtr<Element> pMapNode = pDoc->createElement("map");
		SetNodeAttribute(pMapNode, "width",m_nMapWidth);
		SetNodeAttribute(pMapNode, "height",m_nMapHeight);
		SetNodeAttribute(pMapNode,"Matrix",m_strMatrix);
		pButtonsNode->appendChild(pMapNode);
#endif
		pMenuNode->appendChild(pButtonsNode);

		// <files>
		AutoPtr<Element> pFilesNode = pDoc->createElement("videos");

		CFileList *pFileList = CFileList::Instance();
		int count = pFileList->GetItemCount();
		
		CString strFiles;
		for (int i = 0; i < count; i++)
		{
			if (pFileList->GetCheck(i))
			{
				CTask *pTask = (CTask *)pFileList->GetItemData(i);
				if (pTask->m_nStatus == CONV_STATUS_COMPLETED)
				{
					AutoPtr<Element> pFileNode = pDoc->createElement("video");
					SetNodeAttribute(pFileNode, "path", pTask->m_Atoms[0].outfile);
					SetNodeAttribute(pFileNode, "text", pTask->m_strButtonText);
					pFilesNode->appendChild(pFileNode);
				}
			}
		}

		pMenuNode->appendChild(pFilesNode);

		pDoc->appendChild(pMenuNode);

		SaveXMLDocument(pDoc, lpszFile);

		return TRUE;
	}
	catch (Exception& e)
	{
		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_DVD_MENU_XML1, e.displayText().c_str());
		return FALSE;
	}
}

#ifdef _RELOAD_MENU_TEMPLATES
BOOL CMenuTemplateManager::ReloadMenuTemplates()
{
	CString strFind;
	strFind.Format("%s\\Templates\\", ::GetModuleFilePath());
	strFind += "*.*";

	CFileFind ff;
	BOOL ret = ff.FindFile(strFind);
	CMenuTemplate *pTemplate=NULL;
	CString strMenuName,strFindPath;

	while (ret)
	{
		ret = ff.FindNextFile();
		if (!ff.IsDots() && ff.IsDirectory())
		{
			strFindPath =  ff.GetFilePath();
			if(strFindPath.Find("\\.svn")!=-1)
				continue;

			if(!ReadMenuName(strFindPath,strMenuName))
			{
				AfxMessageBoxEx(0, "Error: failed to parse %s.", strFindPath);
				return FALSE;
			}

			pTemplate = FindTemplate(strMenuName);
			if(pTemplate==NULL)
			{
				pTemplate = new CMenuTemplate();
				if (!pTemplate->LoadMenuTemplate(strFindPath))
				{
					delete pTemplate;
					AfxMessageBoxEx(0, "Error: failed to parse %s.", strFindPath);
					return FALSE;
				}
				m_templates.push_back(pTemplate);
			}
			else
			{
				pTemplate->m_listButtons.clear();
				if (!pTemplate->LoadMenuTemplate(strFindPath))
				{
					AfxMessageBoxEx(0, "Error: failed to parse %s.", strFindPath);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CMenuTemplateManager::ReadMenuName(LPCTSTR lpszFolder,CString &strMenuName)
{
	CString strFile;
	strFile.Format("%s\\template.xml", lpszFolder);

	try
	{		
		AutoPtr<Document> pDoc = OpenXMLDocument(strFile);

		Node *pMenuNode = pDoc->getElementById("menu", "id");
		if (pMenuNode == NULL)
			throw Exception("No <menu> node found");
		pMenuNode->normalize();
		strMenuName = GetNodeAttribute(pMenuNode, "name").c_str();
	}
	catch (Exception& e)
	{
		AfxMessageBoxEx(0, "Error: failed to parse %s. %s.", (LPCTSTR)strFile, e.displayText().c_str());
		return FALSE;
	}
	return TRUE;
}

CMenuTemplate *CMenuTemplateManager::FindTemplate(LPCTSTR lpszName)
{
	CMenuTemplate *pTemplate=NULL;
	for (MENU_TEMPLATE_LIST::iterator it = m_templates.begin(); it != m_templates.end(); ++it)
	{
		pTemplate = *it;
		if (pTemplate->m_strName == lpszName)
			return pTemplate;
	}
	return NULL;
}

#endif
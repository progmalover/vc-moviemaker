#include "stdafx.h"
#include "Converter.h"
#include "FileList.h"
#include "Graphics.h"
#include "ProfileManager.h"
#include "MSMVisualManager.h"
#include "FileDialogEx.h"
#include "DlgProgress.h"
#include "MainFrm.h"
#include "Options.h"
#include "EncoderFFmpeg.h"
#include "EncoderBlueRay.h"
#include "EncoderDVDMenu.h"
#include "EncoderDVD.h"
#include "Burner.h"
#include "DlgBurn.h"
#include "CheckMessageBox.h"
#include "DlgSelectTempFolder.h"
#include "DlgTrim.h"
#include "xmlutils.h"
#include "iTunesCOMInterface.h"
#include "DlgShutDown.h"

#include <sys/stat.h>
#include <PowrProf.h>
#include "PGCAction.h"

#include "..\Activation/Activation.h"

#ifdef _DVD

BOOL CFileList::CreateNoticeVideo(LPCTSTR lpszTempFolder)
{
	CLog::Instance()->AppendLog("CreateNoticeVideo(%s)\r\n",lpszTempFolder);
	CWaitCursor wc;

	CProfileItem *pProfile;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);

	CProfileParam *pParamFramerate = pProfile->GetProfileParam("vstandard");
	CString strStandard = pParamFramerate->GetValue();
	BOOL bPAL = strStandard.CompareNoCase("PAL") == 0;

	CProfileParam *pParamAspect = pProfile->GetProfileParam("vaspect");
	CString strAspect = pParamAspect->GetValue();

	CString strChannles;
	CProfileParam *pParam = NULL;
	pParam = pProfile->GetProfileParam("achannels");
	pParam->GetValue(NULL,strChannles);
	int  nVBitrate = CProfileManager::Instance()->m_nVBitrate;
	int  nABitrate = CProfileManager::Instance()->m_nABitrate;
	pParam = pProfile->GetProfileParam("aenable");
	CString strEnableAudio="";
	pParam->GetValue(NULL,strEnableAudio);

	/*const TCHAR szScript[] = 
	"v = BlankClip(length=200, fps=25, color=$006600, width=720, height=480)\r\n"
	"v = v.Subtitle(\"This DVD was created with Sothink Movie DVD Maker free\\nversion. To exclude this message in your next DVD,\\nplease upgrade to the professional version.\", x = 360, y = 70, align=8, font=\"Arial\", size=24, text_color=$ffffff, halo_color=$ff000000, lsp=20)"
	"v = v.Subtitle(\"www.SothinkMedia.com\", x = 360, y = 220, align=8, font=\"Arial\", size=32, text_color=$ffffff, halo_color=$ff000000)"
	"v = v.Subtitle(\"Video Tools & Tips. All You Need For Juicy Digital Life.\", x = 360, y = 260, align=8, font=\"Arial\", size=20, text_color=$ffffff, halo_color=$ff000000)"
	"v = v.Subtitle(\"Sothink Movie DVD Maker\", x = 70, y = 420, align=1, font=\"Arial\", size=14, text_color=$ffffff, halo_color=$ff000000)"
	"v = v.Subtitle(\"http://www.sothinkmedia.com\", x = 650, y = 420, align=3, font=\"Arial\", size=14, text_color=$ffffff, halo_color=$ff000000)"
	"v = AudioDub(Tone(v.FrameCount / v.FrameRate, 2, 44100, 2, \"Silence\", 1.0), v)\r\n"
	"return v.FadeIn(25).FadeOut(50)";*/


#ifdef AHS
	const TCHAR szScriptFormat[]=
		"v = BlankClip(length=200, fps=25, color=$006600, width=720, height=480)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 70, align=8, font=\"MS UI Gothic\", size=24, text_color=$ffffff, halo_color=$ff000000, lsp=20)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 220, align=8, font=\"MS UI Gothic\", size=32, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 260, align=8, font=\"MS UI Gothic\", size=20, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 70, y = 420, align=1, font=\"MS UI Gothic\", size=14, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 650, y = 420, align=3, font=\"MS UI Gothic\", size=14, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = AudioDub(Tone(v.FrameCount / v.FrameRate, 2, 44100, 2, \"Silence\", 1.0), v)\r\n"
		"return v.FadeIn(25).FadeOut(50)";
#else 
	const TCHAR szScriptFormat[]=
		"v = BlankClip(length=200, fps=25, color=$006600, width=720, height=480)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 70, align=8, font=\"Arial\", size=24, text_color=$ffffff, halo_color=$ff000000, lsp=20)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 220, align=8, font=\"Arial\", size=32, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 360, y = 260, align=8, font=\"Arial\", size=20, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 70, y = 420, align=1, font=\"Arial\", size=14, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = v.Subtitle(\"%s\", x = 650, y = 420, align=3, font=\"Arial\", size=14, text_color=$ffffff, halo_color=$ff000000)\r\n"
		"v = AudioDub(Tone(v.FrameCount / v.FrameRate, 2, 44100, 2, \"Silence\", 1.0), v)\r\n"
		"return v.FadeIn(25).FadeOut(50)";
#endif

	CString strUP1,strM1,strM2,strDown1,strDown2;
	strUP1.LoadString(IDS_NOTICE_MPG_UP_1);
	strM1.LoadString(IDS_NOTICE_MPG_M_1);
	strM2.LoadString(IDS_NOTICE_MPG_M_2);
	strDown1.LoadString(IDS_NOTICE_MPG_DOWN_1);
	strDown2.LoadString(IDS_NOTICE_MPG_DOWN_2);

	TCHAR szScript[2*1024]={0};
	int i = _stprintf(szScript,szScriptFormat,strUP1.GetBuffer(),
		strM1.GetBuffer(),strM2.GetBuffer(),
		strDown1.GetBuffer(),strDown2.GetBuffer());
	ASSERT(i>0);
	strUP1.ReleaseBuffer();
	strM1.ReleaseBuffer();
	strM2.ReleaseBuffer();
	strDown1.ReleaseBuffer();
	strDown2.ReleaseBuffer();

	CString strAVS;
	strAVS.Format("%s\\notice.avs", lpszTempFolder);
	try
	{
		CStdioFile file(strAVS, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(szScript, _tcslen(szScript));
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	CEncoderFFmpeg encoder;
	encoder.m_pTask = new CTask(strAVS);
    
	taskAtom atom;
	atom.outfile.Format("%s\\notice.mpg", lpszTempFolder);
	atom.cmd.Format(" -i \"%s\" -vcodec mpeg2video -b %dk -minrate %dk -maxrate %dk -bufsize 1835k -muxrate 10080k -packetsize 2048 -aspect %s  \
-acodec ac3 -ar 48000 -ab %dk -ac %s -r %s -s %s %s -f dvd -y \"%s\"", 
		(LPCTSTR)strAVS,nVBitrate ,nVBitrate,nVBitrate,strAspect,
		nABitrate, strChannles,bPAL ? "25" : "29.97", bPAL ? "720x576" : "720x480", strEnableAudio,(LPCTSTR)atom.outfile);
	encoder.m_pTask->m_Atoms.push_back(atom);
	encoder.m_strCommandLine=atom.cmd;
	encoder.m_pTask->m_pCurAtom=&atom;

	BOOL ret = encoder.Start();
	// prevent CTask::~CTask() from deleting the output file.
	if (ret)
		encoder.m_pTask->m_Atoms.clear();
	delete encoder.m_pTask;
	return ret;
}

//没有菜单的情况下创建dvdauthor.xml
/*
	<titles>
			<video aspect="16:9" format="PAL" resolution="720x576"/>
			<pgc>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\GraphEdit-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\Gun-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\5-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\10-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\10-MPEG-2-AC3-1.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\earth-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\fool_woman00-MPEG-2-AC3.mpg"/>
				<vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\fool_woman-MPEG-2-AC3.mpg"/>
				<post>call menu;</post>
			</pgc>
		</titles>
*/
BOOL CFileList::CreateDVDAuthorXML(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder)
{	
	CLog::Instance()->AppendLog("CreateDVDAuthorXML(BOOL bEnableDVDMenu=%s)\r\n",bEnableDVDMenu?"TRUE":"FALSE");
	if(bEnableDVDMenu)
		return CreateDVDAuthorXML3(taskList,buttonList,bEnableDVDMenu,lpszTempFolder);

	int nVideosPerPage = buttonList.size() - 2;

	CString strTemplate;
	strTemplate.Format("%s\\Scripts\\DVDAuthor.xml", GetModuleFilePath());

	try
	{
		CProfileItem *pProfile;
		CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);

		CProfileParam *pParamFramerate = pProfile->GetProfileParam("vstandard");
		CString strStandard = pParamFramerate->GetValue();
		BOOL bPAL = strStandard.CompareNoCase("PAL") == 0;

		CProfileParam *pParamAspect = pProfile->GetProfileParam("vaspect");
		CString strAspect = pParamAspect->GetValue();

		AutoPtr<Document> pDoc = OpenXMLDocument(strTemplate);

		// Title menu
		Element *pTitleMenus = pDoc->getElementById("title_menu", "id");
		pTitleMenus->removeAttribute("id");

		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pTitleMenuVideo = pDoc->createElement("video");
			pTitleMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pTitleMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pTitleMenuVideo->setAttribute("aspect", "4:3");
			pTitleMenus->appendChild(pTitleMenuVideo);

			AutoPtr<Element> pTitleMenuPgc = pDoc->createElement("pgc");
			pTitleMenuPgc->setAttribute("entry", "title");

			AutoPtr<Element> pPre = pDoc->createElement("pre");
			AutoPtr<Text> pText = pDoc->createTextNode("jump titleset 1 menu;");
			pPre->appendChild(pText);

			pTitleMenuPgc->appendChild(pPre);

			pTitleMenus->appendChild(pTitleMenuPgc);
		}
		else
		{
			pTitleMenus->parentNode()->removeChild(pTitleMenus);
		}

		// Root menus
		Element *pRootMenus = pDoc->getElementById("root_menu", "id");
		pRootMenus->removeAttribute("id");

		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pRootMenuVideo = pDoc->createElement("video");
			pRootMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pRootMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pRootMenuVideo->setAttribute("aspect", "4:3");
			pRootMenus->appendChild(pRootMenuVideo);

			int nTotalPages = taskList.size() / nVideosPerPage;
			if (taskList.size() % nVideosPerPage > 0)
				nTotalPages++;

			for (int nPage = 0; nPage < nTotalPages; nPage++)
			{
				// start a new menu page
				AutoPtr<Element> pRootMenuPgc = pDoc->createElement("pgc");

				// set menu vob file
				CString strMenuVob;
				strMenuVob.Format("%s\\menu-%d.mpg", lpszTempFolder, nPage);
				AutoPtr<Element> pRootMenuVob = pDoc->createElement("vob");
				pRootMenuVob->setAttribute("file", UTF8Encode(strMenuVob).c_str());
				pRootMenuPgc->appendChild(pRootMenuVob);

				for (int i = 0; i < nVideosPerPage && nPage * nVideosPerPage + i < taskList.size(); i++)
				{
					CString strButton;
					strButton.Format("jump title %d chapter %d;", (nPage * nVideosPerPage + i) / MAX_CHAPTERS + 1, (nPage * nVideosPerPage + i) % MAX_CHAPTERS + 1);
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// prev button
				if (nPage > 0)
				{
					CString strButton;
					strButton.Format("jump menu %d;", nPage);
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// next button
				if (nPage < nTotalPages - 1)
				{
					CString strButton;
					strButton.Format("jump menu %d;", nPage + 2);
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// set menu post command
				CString strPost;
				strPost.Format("jump menu %d;", nPage + 1);
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPost);
				pPost->appendChild(pText);

				pRootMenuPgc->appendChild(pPost);

				pRootMenus->appendChild(pRootMenuPgc);
			}
		}
		else
		{
			pRootMenus->parentNode()->removeChild(pRootMenus);
		}

		// titles
		Element *pTitles = pDoc->getElementById("titles", "id");
		pTitles->removeAttribute("id");

		AutoPtr<Element> pTitlesVideo = pDoc->createElement("video");
		pTitlesVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
		pTitlesVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
		pTitlesVideo->setAttribute("aspect", (LPCTSTR)strAspect);
		pTitles->appendChild(pTitlesVideo);

		int nTotalTitles = taskList.size() / MAX_CHAPTERS;
		if (taskList.size() % MAX_CHAPTERS > 0)
			nTotalTitles++;
		for (int nTitle = 0; nTitle < nTotalTitles; nTitle++)
		{
			// start a new title
			AutoPtr<Element> pTitlePgc = pDoc->createElement("pgc");

			for (int i = 0; i < MAX_CHAPTERS && nTitle * MAX_CHAPTERS + i < taskList.size(); i++)
			{
				if (!CActivate::Instance()->IsActivated())
				{
					// set chapter free version notice vob file
					AutoPtr<Element> pTitlePgcVob = pDoc->createElement("vob");
					CString strNoticeMpg;
					strNoticeMpg.Format("%s\\notice.mpg", lpszTempFolder);
					pTitlePgcVob->setAttribute("file", UTF8Encode(strNoticeMpg).c_str());

					pTitlePgcVob->setAttribute("chapters", "00:00:00");

					pTitlePgc->appendChild(pTitlePgcVob);
				}

				// set chapter vob file
				CTask *pTask=taskList[nTitle * MAX_CHAPTERS + i];
				for(int n=0;n<(int)pTask->m_Atoms.size();n++)
				{

					AutoPtr<Element> pTitlePgcVob = pDoc->createElement("vob");
					pTitlePgcVob->setAttribute("file", UTF8Encode(pTask->m_Atoms[n].outfile).c_str());

					if(CActivate::Instance()->IsActivated() && n == 0)
					{
						pTitlePgcVob->setAttribute("chapters", "00:00:00");
					}

					pTitlePgc->appendChild(pTitlePgcVob);

				}
			}

			// set title post command
			CString strPost;
			if (nTitle == nTotalTitles - 1)	// the last title
			{
				if (bEnableDVDMenu)
					strPost = "call menu;";
			}
			else
			{
				strPost.Format("jump title %d chapter 1;", nTitle + 2);
			}

			if (!strPost.IsEmpty())
			{
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPost);
				pPost->appendChild(pText);

				pTitlePgc->appendChild(pPost);
			}

			pTitles->appendChild(pTitlePgc);
		}

		CString strDVDAuthorXML;
		strDVDAuthorXML.Format("%s\\dvdauthor.xml", lpszTempFolder);
		SaveXMLDocument(pDoc, strDVDAuthorXML);

		return TRUE;
	}
	catch (Exception e)
	{	
		CString strException = e.displayText().c_str(),strMsg ;
		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		strMsg.Format(IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		CLog::Instance()->AppendLog("%s\r\n",strMsg);
		return FALSE;
	}
}

//每个视频创建一个title
/*
<titleset>
   <titles>
        <pgc>
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\GraphEdit-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\Gun-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
             <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\5-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\10-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\10-MPEG-2-AC3-1.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\earth-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\fool_woman00-MPEG-2-AC3.mpg"/>
        </pgc>
        <pgc> 
            <vob chapters="00:00:00" file="F:\Sothink Movie DVD Maker\fool_woman-MPEG-2-AC3.mpg"/>
        </pgc>
   </titles>
</titleset>
*/
BOOL CFileList::CreateDVDAuthorXML2(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder)
{
	if(!bEnableDVDMenu)
		return FALSE;
	CLog::Instance()->AppendLog("CFileList::CreateDVDAuthorXML2()\r\n");
	int nVideosPerPage = buttonList.size() - 2;

	CString strTemplate;
	strTemplate.Format("%s\\Scripts\\DVDAuthor.xml", GetModuleFilePath());

	try
	{
		CProfileItem *pProfile;
		CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);

		CProfileParam *pParamFramerate = pProfile->GetProfileParam("vstandard");
		CString strStandard = pParamFramerate->GetValue();
		BOOL bPAL = strStandard.CompareNoCase("PAL") == 0;

		CProfileParam *pParamAspect = pProfile->GetProfileParam("vaspect");
		CString strAspect = pParamAspect->GetValue();

		AutoPtr<Document> pDoc = OpenXMLDocument(strTemplate);

		// Title menu
		Element *pTitleMenus = pDoc->getElementById("title_menu", "id");
		pTitleMenus->removeAttribute("id");
		//g1   返回的菜单
		//g2   当前菜单选择的按钮  -- button	
		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pTitleMenuVideo = pDoc->createElement("video");
			pTitleMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pTitleMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pTitleMenuVideo->setAttribute("aspect", "4:3");
			pTitleMenus->appendChild(pTitleMenuVideo);

			AutoPtr<Element> pTitleMenuPgc = pDoc->createElement("pgc");
			pTitleMenuPgc->setAttribute("entry", "title");

			AutoPtr<Element> pPre = pDoc->createElement("pre");
			AutoPtr<Text> pText = pDoc->createTextNode("g1=1;g2=1;jump titleset 1 menu;");
			pPre->appendChild(pText);

			pTitleMenuPgc->appendChild(pPre);

			pTitleMenus->appendChild(pTitleMenuPgc);
		}
		else
		{
			pTitleMenus->parentNode()->removeChild(pTitleMenus);
		}

		// Root menus
		Element *pRootMenus = pDoc->getElementById("root_menu", "id");
		pRootMenus->removeAttribute("id");

		//title-set menus
		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pRootMenuVideo = pDoc->createElement("video");
			pRootMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pRootMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pRootMenuVideo->setAttribute("aspect", "4:3");
			pRootMenus->appendChild(pRootMenuVideo);

			int nTotalPages = taskList.size() / nVideosPerPage;
			if (taskList.size() % nVideosPerPage > 0)
				nTotalPages++;

			for (int nPage = 0; nPage < nTotalPages; nPage++)
			{
				// start a new menu page
				AutoPtr<Element> pRootMenuPgc = pDoc->createElement("pgc");

				// pgc pre command
				AutoPtr<Element> pPrePgc = pDoc->createElement("pre");
				CString strPreCommand;
				if(nPage==0)
				{
					// <pre> {if (g1 ne 1) {jump menu 2;}button=g2*1024;</pre>
					CString strJump = _T(""),strTemp;
					for(int i=nPage+1;i<nTotalPages;i++)
					{
						strTemp.Format(_T("if (g1 == %d) {jump menu %d;}"),i+1,i+1);
						strJump += strTemp;
					}
					if(nTotalPages>1)
						strPreCommand.Format(_T("if (g1 ne 1) {%s}"),strJump);//if (g1 > 1)
				}
				else
					strPreCommand.Format(_T("g1 = %d;"),nPage+1);
				strPreCommand += _T("button=g2*1024;");
				TRACE(strPreCommand);
				AutoPtr<Text> pPreText = pDoc->createTextNode((LPCTSTR)strPreCommand);
				pPrePgc->appendChild(pPreText);
				pRootMenuPgc->appendChild(pPrePgc);

				// set menu vob file
				CString strMenuVob;
				strMenuVob.Format("%s\\menu-%d.mpg", lpszTempFolder, nPage);
				AutoPtr<Element> pRootMenuVob = pDoc->createElement("vob");
				pRootMenuVob->setAttribute("file", UTF8Encode(strMenuVob).c_str());
				pRootMenuPgc->appendChild(pRootMenuVob);

				int nTitleID=0,nChapterID=0,nTaskIndex=0,nChapters=-1;
				for (int i = 0; i < nVideosPerPage && nPage * nVideosPerPage + i < taskList.size(); i++)
				{
					//<button>g2 =1;jump title nTitleID chapter nChapterID;</button>
					nTaskIndex = nPage * nVideosPerPage + i;
					nChapters = GetTitleID(nTaskIndex,taskList.size(),nTitleID,nChapterID);
					ASSERT(nChapters>0);
					CString strButton;
					strButton.Format("g2 = %d;jump title %d chapter %d;",i+1,nTitleID ,nChapterID);
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// prev button
				if (nPage > 0)
				{
					CString strButton;
					strButton.Format("g1 = %d;g2 = 1;jump menu %d;", nPage,nPage);//默认选择第一个按钮
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// next button
				if (nPage < nTotalPages - 1)
				{
					CString strButton;
					strButton.Format("g1 = %d;g2 = 1;jump menu %d;", nPage + 2,nPage + 2);//默认选择第一个按钮
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// set menu post command---循环播放菜单
				CString strPost;
				strPost.Format("jump menu %d;", nPage + 1);
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPost);
				pPost->appendChild(pText);

				pRootMenuPgc->appendChild(pPost);

				pRootMenus->appendChild(pRootMenuPgc);
			}
		}
		else
		{
			pRootMenus->parentNode()->removeChild(pRootMenus);
		}

		//title-set titles
		Element *pTitles = pDoc->getElementById("titles", "id");
		pTitles->removeAttribute("id");

		AutoPtr<Element> pTitlesVideo = pDoc->createElement("video");
		pTitlesVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
		pTitlesVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
		pTitlesVideo->setAttribute("aspect", (LPCTSTR)strAspect);
		pTitles->appendChild(pTitlesVideo);

		int nTotalTitles = taskList.size(),
			nChapters = -1,nTitleID =-1,nChapterID=-1,nTitle = 0;
		BOOL bTrialKey =!CActivate::Instance()->IsActivated();
		for (; nTitle < nTotalTitles;)
		{
			nChapters = GetTitleID(nTitle,nTotalTitles,nTitleID,nChapterID);
			ASSERT(nChapters>0);
			if(nChapters<=0)
				throw Exception("Create DVD Menu error!");

			// start a new title
			AutoPtr<Element> pTitlePgc = pDoc->createElement("pgc");
			for(int m =0;m<nChapters;m++)
			{
				if (bTrialKey)
				{
					// set chapter free version notice vob file
					AutoPtr<Element> pTitlePgcVob = pDoc->createElement("vob");
					CString strNoticeMpg;
					strNoticeMpg.Format("%s\\notice.mpg", lpszTempFolder);
					pTitlePgcVob->setAttribute("file", UTF8Encode(strNoticeMpg).c_str());
					pTitlePgcVob->setAttribute("chapters", "00:00:00");
					pTitlePgc->appendChild(pTitlePgcVob);
				}

				// set chapter vob file
				CTask *pTask=taskList[nTitle+m];
				for(int n=0;n<(int)pTask->m_Atoms.size();n++)
				{
					AutoPtr<Element> pTitlePgcVob = pDoc->createElement("vob");
					pTitlePgcVob->setAttribute("file", UTF8Encode(pTask->m_Atoms[n].outfile).c_str());
					if(!bTrialKey && n==0)
						pTitlePgcVob->setAttribute("chapters", "00:00:00");
					pTitlePgc->appendChild(pTitlePgcVob);
				}
			}

			nTitle += nChapters;
			// set title post command
			CString strPost;
			if (nTitle == nTotalTitles)	// the last title
			{
				//PLAY_REPEATLY,PLAY_NEXT_VIDEO,,BACK_FIRST_MENU,
				switch (COptions::Instance()->m_nAfterLastVideoOptions)
				{
				case PLAY_REPEATLY:
					strPost.Format(g_strAction[PLAY_REPEATLY],nTitleID,1);
					break;
				case PLAY_NEXT_VIDEO:
					strPost.Format(g_strAction[PLAY_NEXT_VIDEO],1,1);
					break;
				case BACK_TO_MENU:
					strPost = g_strAction[BACK_TO_MENU];
					break;
				case BACK_FIRST_MENU:
					strPost = g_strAction[BACK_FIRST_MENU];
					break;
				default:
					ASSERT(FALSE);
				}
			}
			else
			{
				//PLAY_REPEATLY,PLAY_NEXT_VIDEO,BACK_TO_MENU
				switch (COptions::Instance()->m_nAfterCurVideoOptions)
				{
				case PLAY_REPEATLY:
					strPost.Format(g_strAction[PLAY_REPEATLY],nTitleID,1);
					break;
				case PLAY_NEXT_VIDEO:
					strPost.Format(g_strAction[PLAY_NEXT_VIDEO],nTitleID+1,1);
					break;
				case BACK_TO_MENU:
					strPost = g_strAction[BACK_TO_MENU];
					break;
				default:
					ASSERT(FALSE);
				}
			}

			if (!strPost.IsEmpty())
			{
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPost);
				pPost->appendChild(pText);
				pTitlePgc->appendChild(pPost);
			}
			pTitles->appendChild(pTitlePgc);
		}
		ASSERT(nTitle == taskList.size());
		CString strDVDAuthorXML;
		strDVDAuthorXML.Format("%s\\dvdauthor.xml", lpszTempFolder);
		SaveXMLDocument(pDoc, strDVDAuthorXML);

		return TRUE;
	}
	catch (Exception e)
	{
		CString strException = e.displayText().c_str(),strMsg;
		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		strMsg.Format(IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		CLog::Instance()->AppendLog("%s\r\n",strMsg);
		return FALSE;
	}
}

//每个视频创建一个Chapter
/*
<titles>
     <video aspect="16:9" format="PAL" resolution="720x576"/>
     <pgc>
           <vob file="F:\Sothink Movie DVD Maker\fool_woman.mpg">
                <cell chapter="1">call menu;</cell>
           </vob>
            <vob file="F:\Sothink Movie DVD Maker\Gun-MPEG-2-AC3.mpg">
                 <cell chapter="1">call menu;</cell>
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\5-MPEG-2-AC3.mpg">
                <cell chapter="1">call menu;</cell>
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\10-MPEG-2-AC3.mpg">
                <cell chapter="1">call menu;</cell>
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\[幸运星]DVD影像特典 12.mpg">
                <cell chapter="1">call menu;</cell>
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\earth-MPEG-2-AC3.mpg">
                <cell chapter="1">call menu;</cell> 
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\GraphEdit-MPEG-2-AC3.mpg">
                <cell chapter="1">call menu;</cell>
           </vob>
           <vob file="F:\Sothink Movie DVD Maker\300斯巴达勇士.mpg" chapters="00:00:00">
           </vob>
           <post>call menu;</post>
      </pgc>
</titles>
*/
BOOL CFileList::CreateDVDAuthorXML3(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder)
{
	CLog::Instance()->AppendLog("CFileList::CreateDVDAuthorXML2()\r\n");
	if(!bEnableDVDMenu)
		return FALSE;

	int nVideosPerPage = buttonList.size() - 2;

	CString strTemplate;
	strTemplate.Format("%s\\Scripts\\DVDAuthor.xml", GetModuleFilePath());

	try
	{
		CProfileItem *pProfile;
		CProfileManager::Instance()->GetCurrentProfile(NULL, &pProfile);

		CProfileParam *pParamFramerate = pProfile->GetProfileParam("vstandard");
		CString strStandard = pParamFramerate->GetValue();
		BOOL bPAL = strStandard.CompareNoCase("PAL") == 0;

		CProfileParam *pParamAspect = pProfile->GetProfileParam("vaspect");
		CString strAspect = pParamAspect->GetValue();

		AutoPtr<Document> pDoc = OpenXMLDocument(strTemplate);

		// Title menu
		Element *pTitleMenus = pDoc->getElementById("title_menu", "id");
		pTitleMenus->removeAttribute("id");
		//g1   返回的菜单
		//g2   当前菜单选择的按钮  -- button	
		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pTitleMenuVideo = pDoc->createElement("video");
			pTitleMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pTitleMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pTitleMenuVideo->setAttribute("aspect", "4:3");
			pTitleMenus->appendChild(pTitleMenuVideo);

			AutoPtr<Element> pTitleMenuPgc = pDoc->createElement("pgc");
			pTitleMenuPgc->setAttribute("entry", "title");

			AutoPtr<Element> pPre = pDoc->createElement("pre");
			AutoPtr<Text> pText = pDoc->createTextNode("g1=1;g2=1;jump titleset 1 menu;");
			pPre->appendChild(pText);

			pTitleMenuPgc->appendChild(pPre);

			pTitleMenus->appendChild(pTitleMenuPgc);
		}
		else
		{
			pTitleMenus->parentNode()->removeChild(pTitleMenus);
		}

		// Root menus
		Element *pRootMenus = pDoc->getElementById("root_menu", "id");
		pRootMenus->removeAttribute("id");

		//title-set menus
		if (bEnableDVDMenu)
		{
			AutoPtr<Element> pRootMenuVideo = pDoc->createElement("video");
			pRootMenuVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
			pRootMenuVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
			pRootMenuVideo->setAttribute("aspect", "4:3");
			pRootMenus->appendChild(pRootMenuVideo);

			int nTotalPages = taskList.size() / nVideosPerPage;
			if (taskList.size() % nVideosPerPage > 0)
				nTotalPages++;

			for (int nPage = 0; nPage < nTotalPages; nPage++)
			{
				// start a new menu page
				AutoPtr<Element> pRootMenuPgc = pDoc->createElement("pgc");

				// pgc pre command
				AutoPtr<Element> pPrePgc = pDoc->createElement("pre");
				CString strPreCommand;
				if(nPage==0)
				{
					// <pre> {if (g1 ne 1) {jump menu 2;}button=g2*1024;</pre>
					CString strJump = _T(""),strTemp;
					for(int i=nPage+1;i<nTotalPages;i++)
					{
						strTemp.Format(_T("if (g1 == %d) {jump menu %d;}"),i+1,i+1);
						strJump += strTemp;
					}
					if(nTotalPages>1)
						strPreCommand.Format(_T("if (g1 ne 1) {%s}"),strJump);//if (g1 > 1)
				}
				else
					strPreCommand.Format(_T("g1 = %d;"),nPage+1);
				strPreCommand += _T("button=g2*1024;");
				TRACE(strPreCommand);
				AutoPtr<Text> pPreText = pDoc->createTextNode((LPCTSTR)strPreCommand);
				pPrePgc->appendChild(pPreText);
				pRootMenuPgc->appendChild(pPrePgc);

				// set menu vob file
				CString strMenuVob;
				strMenuVob.Format("%s\\menu-%d.mpg", lpszTempFolder, nPage);
				AutoPtr<Element> pRootMenuVob = pDoc->createElement("vob");
				pRootMenuVob->setAttribute("file", UTF8Encode(strMenuVob).c_str());
				pRootMenuPgc->appendChild(pRootMenuVob);

				int nTitleID=0,nChapterID=0,nTaskIndex=0;
				for (int i = 0; i < nVideosPerPage && nPage * nVideosPerPage + i < taskList.size(); i++)
				{
					//<button>g2 = i;jump title nTitleID chapter nChapterID;</button>
					nTaskIndex = nPage * nVideosPerPage + i;
					nTitleID =  nTaskIndex/MAX_CHAPTERS+1;
					nChapterID = nTaskIndex%MAX_CHAPTERS+1;
					CString strButton;
					strButton.Format("g2 = %d;jump title %d chapter %d;",i+1,nTitleID ,nChapterID);
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// prev button
				if (nPage > 0)
				{
					CString strButton;
					strButton.Format("g1 = %d;g2 = 1;jump menu %d;", nPage,nPage);//默认选择第一个按钮
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// next button
				if (nPage < nTotalPages - 1)
				{
					CString strButton;
					strButton.Format("g1 = %d;g2 = 1;jump menu %d;", nPage + 2,nPage + 2);//默认选择第一个按钮
					AutoPtr<Element> pButton = pDoc->createElement("button");
					AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strButton);
					pButton->appendChild(pText);

					pRootMenuPgc->appendChild(pButton);
				}

				// set menu post command---循环播放菜单
				CString strPost;
				strPost.Format("jump menu %d;", nPage + 1);
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPost);
				pPost->appendChild(pText);

				pRootMenuPgc->appendChild(pPost);

				pRootMenus->appendChild(pRootMenuPgc);
			}
		}
		else
		{
			pRootMenus->parentNode()->removeChild(pRootMenus);
		}

		//title-set titles----------------------------------------------------------------------------
		Element *pTitles = pDoc->getElementById("titles", "id");
		pTitles->removeAttribute("id");

		AutoPtr<Element> pTitlesVideo = pDoc->createElement("video");
		pTitlesVideo->setAttribute("format", bPAL ? "PAL" : "NTSC");
		pTitlesVideo->setAttribute("resolution", bPAL ? "720x576" : "720x480");
		pTitlesVideo->setAttribute("aspect", (LPCTSTR)strAspect);
		pTitles->appendChild(pTitlesVideo);

		int nTotalTitles = taskList.size() / MAX_CHAPTERS;
		if (taskList.size() % MAX_CHAPTERS > 0)
			nTotalTitles++;
		BOOL bTrialKey = !CActivate::Instance()->IsActivated();
		BOOL bLastTitle = FALSE;
		BOOL bLastPGCTask = FALSE;
		BOOL bMakePoint = FALSE;
        int  nCurrentChapter = -1;

		for (int nTitle = 0; nTitle < nTotalTitles; nTitle++)
		{
			nCurrentChapter = 0;
			AutoPtr<Element> pTitlePgc = pDoc->createElement("pgc");
			for (int i = 0; i < MAX_CHAPTERS && nTitle * MAX_CHAPTERS + i < taskList.size(); i++)
			{
				bLastPGCTask = (i == MAX_CHAPTERS-1) || (nTitle * MAX_CHAPTERS + i == taskList.size()-1);
				nCurrentChapter++;

				bMakePoint = FALSE;
				if (bTrialKey)
				{
					//添加Notice.mpg----并添加chapters标记(make  point)
					AutoPtr<Element> pChapterNotice = pDoc->createElement("vob");
					CString strNoticeMpg;
					strNoticeMpg.Format("%s\\notice.mpg", lpszTempFolder);
					pChapterNotice->setAttribute("file", UTF8Encode(strNoticeMpg).c_str());
					pChapterNotice->setAttribute("chapters", "00:00:00");
					bMakePoint = TRUE;
					pTitlePgc->appendChild(pChapterNotice);
				}

				CTask *pTask=taskList[nTitle * MAX_CHAPTERS + i];
				for(int n=0;n<(int)pTask->m_Atoms.size();n++)
				{
					AutoPtr<Element> pChapterTask = pDoc->createElement("vob");
					pChapterTask->setAttribute("file", UTF8Encode(pTask->m_Atoms[n].outfile).c_str());
					
					if(n==pTask->m_Atoms.size()-1)
					{
						//需要添加导航命令
						AutoPtr<Element> pCell = pDoc->createElement("cell");
						if(!bMakePoint)
							pCell->setAttribute("chapter", "1");

						if(!bLastPGCTask)
						{
							CString strCellCommand;
							switch (COptions::Instance()->m_nAfterCurVideoOptions)
							{
							case PLAY_REPEATLY:
								strCellCommand.Format(g_strAction[PLAY_REPEATLY],nTitle+1,nCurrentChapter);
								break;
							case PLAY_NEXT_VIDEO:
								//strCellCommand.Format(g_strAction[PLAY_NEXT_VIDEO],nTitleID+1,1);
								strCellCommand.Empty();
								break;
							case BACK_TO_MENU:
								strCellCommand = g_strAction[BACK_TO_MENU];
								break;
							default:
								ASSERT(FALSE);
							}

							if(!strCellCommand.IsEmpty())
							{
								AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strCellCommand);
								pCell->appendChild(pText);
							}
						}
						pChapterTask->appendChild(pCell);
					}
					else if(n==0)
					{
						if(!bTrialKey)
						{
							pChapterTask->setAttribute("chapters", "00:00:00");
							bMakePoint  = TRUE;
						}
					}
					pTitlePgc->appendChild(pChapterTask);
				}
			}//end current PGC 

			CString strPostCommand;
			if(nTitle==nTotalTitles-1)
			{
				switch (COptions::Instance()->m_nAfterLastVideoOptions)
				{
				case PLAY_REPEATLY:
					strPostCommand.Format(g_strAction[PLAY_REPEATLY],nTitle+1,nCurrentChapter);
					break;
				case PLAY_NEXT_VIDEO:
					strPostCommand.Format(g_strAction[PLAY_NEXT_VIDEO],1,1);
					break;
				case BACK_TO_MENU:
					strPostCommand = g_strAction[BACK_TO_MENU];
					break;
				case BACK_FIRST_MENU:
					strPostCommand = g_strAction[BACK_FIRST_MENU];
					break;
				default:
					ASSERT(FALSE);
				}
			}
			else
			{
				switch (COptions::Instance()->m_nAfterCurVideoOptions)
				{
				case PLAY_REPEATLY:
					strPostCommand.Format(g_strAction[PLAY_REPEATLY],nTitle+1,nCurrentChapter);
					break;
				case PLAY_NEXT_VIDEO:
					strPostCommand.Format(g_strAction[PLAY_NEXT_VIDEO],nTitle+2,1);
					break;
				case BACK_TO_MENU:
					strPostCommand = g_strAction[BACK_TO_MENU];
					break;
				default:
					ASSERT(FALSE);
				}
			}

			if(!strPostCommand.IsEmpty())
			{
				AutoPtr<Element> pPost = pDoc->createElement("post");
				AutoPtr<Text> pText = pDoc->createTextNode((LPCTSTR)strPostCommand);
				pPost->appendChild(pText);
				pTitlePgc->appendChild(pPost);
			}

			pTitles->appendChild(pTitlePgc);
		}

		CString strDVDAuthorXML;
		strDVDAuthorXML.Format("%s\\dvdauthor.xml", lpszTempFolder);
		SaveXMLDocument(pDoc, strDVDAuthorXML);

		return TRUE;
	}
	catch (Exception e)
	{
		CString strException = e.displayText().c_str(),strMsg;
		AfxMessageBoxEx(MB_ICONERROR | MB_OK, IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		strMsg.Format(IDS_E_CREATE_DVD_AUTHOR_XML2, (LPCTSTR)strTemplate, strException);
		CLog::Instance()->AppendLog("%s\r\n",strMsg);
		return FALSE;
	}
}

#endif
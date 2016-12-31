// DVDMenu.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DVDMenu.h"
#include "xmlutils.h"

#include <strstream>

#include <gdiplus.h>
#include <atlimage.h>
#include <sys/stat.h>

using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct BUTTON
{
public:
	CRect rc[4];
};

struct VIDEO
{
	std::string sPath;
	std::string sText;
};

typedef std::vector<VIDEO> VIDEO_LIST;
typedef std::vector<BUTTON> BUTTON_LIST;
typedef std::vector<CRect> BUTTON_HEIGHTLIGHT_LIST;

CString g_strOutputFolder;
CString g_strResourceFolder;

std::string g_sStandard;
std::string g_sAspect;
std::string g_sBackgroundImage;
std::string g_sBackgroundMask;
std::string g_sBackgroundAudio;
std::string g_sVideoBitrate;
std::string g_sAudioBitrate;
std::string g_sChannels;

std::string g_sTitleText;
std::string g_sTitleFont;
std::string g_sTitleFontColor;
std::string g_sTitleFontSize;
std::string g_sTitleX;
std::string g_sTitleY;

std::string g_sDuration;
std::string g_sButtonTextAlign;
std::string g_sButtonTextColor;
std::string g_sButtonFont;
std::string g_sButtonFontSize;

VIDEO_LIST g_listVideos;
BUTTON_LIST g_listButtons;
BUTTON_HEIGHTLIGHT_LIST g_listHightlightButton;

int g_nTotalPages = 0;
int g_nPage = 0;
INT64 g_nMenuBackFileSize = 0;

BOOL g_bFFmpegSucceeded = FALSE;
BOOL g_bSpuMuxSucceeded = FALSE;

float g_fProgress = 0.0;

__inline BOOL IsPAL()
{
	return _tcsicmp(g_sStandard.c_str(), "PAL") == 0;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

// The one and only application object

CWinApp theApp;

using namespace std;
//DVDMenu -1 -2 -3
//  1: 当前菜单模板的XML文件路径   F:\Sothink Movie DVD Maker\spumux-test\dvdmenu.xml
//  2: 视频文件路径  C:\Program Files\SourceTec\Sothink Movie DVD Maker\Templates\yellow
//  3: 输出文件路径  F:\Sothink Movie DVD Maker\spumux-test
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	TRACE("************************\n");
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		printf("Error: MFC initialization failed.\r\n");
		return 1;
	}

	if (argc != 4)
	{
		printf("Usage: dvdmenu xmlfile resourcefolder outputfolder\r\n");
		return 1;
	}

	g_strResourceFolder = argv[2];
	::PathRemoveBackslash(g_strResourceFolder.GetBuffer());
	g_strResourceFolder.ReleaseBuffer();
	if (!::PathIsDirectory(g_strResourceFolder))
	{
		printf("Error: the resource folder \"%s\" does not exist.\r\n", (LPCTSTR)g_strResourceFolder);
		return 1;
	}

	g_strOutputFolder = argv[3];
	::PathRemoveBackslash(g_strOutputFolder.GetBuffer());
	g_strOutputFolder.ReleaseBuffer();
	if (!::PrepareDirectory(g_strOutputFolder, FALSE))
	{
		printf("Error: failed to create folder \"%s\".\r\n", (LPCTSTR)g_strOutputFolder);
		return 1;
	}

	if (!ParseXML(argv[1]))
	{
		printf("Error: ParseXML() failed.\r\n");
		return 1;
	}

	GdiplusStartupInput si;
	ULONG_PTR token;
	GdiplusStartup(&token, &si, NULL);

	BOOL ret  = GenerateMenu();

	GdiplusShutdown(token);

	if (!ret)
	{
		printf("Error: GenerateMenu() failed.\r\n");
		return 1;
	}

	return 0;
}

BOOL ParseXML(LPCTSTR lpszFile)
{
	try
	{
		AutoPtr<Document> pDoc = OpenXMLDocument(lpszFile);

		Node *pMenuNode = pDoc->getElementById("menu", "id");
		if (pMenuNode == NULL)
			throw Exception("No <menu> node found");
		pMenuNode->normalize();

		g_sStandard = GetNodeAttribute(pMenuNode, "standard");
		g_sAspect = GetNodeAttribute(pMenuNode, "aspect");
		g_sBackgroundImage = GetNodeAttribute(pMenuNode, "background_image");
		g_sBackgroundAudio = GetNodeAttribute(pMenuNode, "background_audio");
		g_sTitleText = GetNodeAttribute(pMenuNode, "title_text");
		g_sTitleFontColor = GetNodeAttribute(pMenuNode, "title_color");
		g_sTitleFont = GetNodeAttribute(pMenuNode, "title_font");
		g_sTitleFontSize = GetNodeAttribute(pMenuNode, "title_font_size");
		g_sTitleX = GetNodeAttribute(pMenuNode, "title_x");
		g_sTitleY = GetNodeAttribute(pMenuNode, "title_y");
		g_sDuration = GetNodeAttribute(pMenuNode, "duration");
		g_sVideoBitrate = GetNodeAttribute(pMenuNode, "vbitrate");
		g_sAudioBitrate = GetNodeAttribute(pMenuNode, "abitrate");
		g_sChannels = GetNodeAttribute(pMenuNode, "channels");


		if (::PathIsRelative(g_sBackgroundImage.c_str()))
			g_sBackgroundImage = g_strResourceFolder + "\\" + g_sBackgroundImage.c_str();

		if (::PathIsRelative(g_sBackgroundAudio.c_str()))
			g_sBackgroundAudio = g_strResourceFolder + "\\" + g_sBackgroundAudio.c_str();

		g_sBackgroundMask = g_strResourceFolder + "\\background_mask.png";

		AutoPtr<ElementsByTagNameList> pFileNodeList = (ElementsByTagNameList *)pDoc->getElementsByTagName("video");
		for (int i = 0; i < pFileNodeList->length(); i++)
		{
			Node *pFileNode = pFileNodeList->item(i);
			VIDEO video;
			video.sPath = GetNodeAttribute(pFileNode, "path");
			video.sText = GetNodeAttribute(pFileNode, "text");
			g_listVideos.push_back(video);
		}

		Element *pButtonsNode = pDoc->getElementById("buttons", "id");
		if (pButtonsNode == NULL)
			throw Exception("No <buttons> node found");

		g_sButtonTextAlign = GetNodeAttribute(pButtonsNode, "text_align").c_str();
		g_sButtonTextColor = GetNodeAttribute(pButtonsNode, "text_color").c_str();
		g_sButtonFont = GetNodeAttribute(pButtonsNode, "font").c_str();
		g_sButtonFontSize = GetNodeAttribute(pButtonsNode, "font_size").c_str();

		AutoPtr<ElementsByTagNameList> pButtonNodeList = (ElementsByTagNameList *)pDoc->getElementsByTagName("button");
		for (int i = 0; i < pButtonNodeList->length(); i++)
		{
			Node *pButtonNode = pButtonNodeList->item(i);

			BUTTON button;
			button.rc[0].left = _ttoi(GetNodeAttribute(pButtonNode, "x0").c_str());
			button.rc[0].top = _ttoi(GetNodeAttribute(pButtonNode, "y0").c_str());
			button.rc[0].right = button.rc[0].left + _ttoi(GetNodeAttribute(pButtonNode, "w0").c_str()) / 2 * 2;
			button.rc[0].bottom = button.rc[0].top + _ttoi(GetNodeAttribute(pButtonNode, "h0").c_str()) / 2 * 2;

			button.rc[1].left = _ttoi(GetNodeAttribute(pButtonNode, "x1").c_str());
			button.rc[1].top = _ttoi(GetNodeAttribute(pButtonNode, "y1").c_str());
			button.rc[1].right = button.rc[1].left + _ttoi(GetNodeAttribute(pButtonNode, "w1").c_str()) / 2 * 2;
			button.rc[1].bottom = button.rc[1].top + _ttoi(GetNodeAttribute(pButtonNode, "h1").c_str()) / 2 * 2;

			button.rc[2].left = _ttoi(GetNodeAttribute(pButtonNode, "x2").c_str());
			button.rc[2].top = _ttoi(GetNodeAttribute(pButtonNode, "y2").c_str());
			button.rc[2].right = button.rc[2].left + _ttoi(GetNodeAttribute(pButtonNode, "w2").c_str()) / 2 * 2;
			button.rc[2].bottom = button.rc[2].top + _ttoi(GetNodeAttribute(pButtonNode, "h2").c_str()) / 2 * 2;

			button.rc[3].left = _ttoi(GetNodeAttribute(pButtonNode, "x3").c_str());
			button.rc[3].top = _ttoi(GetNodeAttribute(pButtonNode, "y3").c_str());
			button.rc[3].right = button.rc[3].left + _ttoi(GetNodeAttribute(pButtonNode, "w3").c_str()) / 2 * 2;
			button.rc[3].bottom = button.rc[3].top + _ttoi(GetNodeAttribute(pButtonNode, "h3").c_str()) / 2 * 2;

			g_listButtons.push_back(button);
		}

		// default coordinates target NTSC standard
		if (IsPAL())
		{
			for (int i = 0; i < g_listButtons.size(); i++)
			{
				for (int j = 0; j < sizeof(g_listButtons[i].rc) / sizeof(CRect); j++)
				{
					int h = g_listButtons[i].rc[j].Height();
					g_listButtons[i].rc[j].top = (int)(g_listButtons[i].rc[j].top * (float)576 / 480);
					g_listButtons[i].rc[j].bottom = g_listButtons[i].rc[j].top + (int)(h * (float)576 / 480) / 2 * 2;
				}
			}
		}

		if (g_listVideos.size() == 0)
		{
			printf("Error: no files found.\r\n");
			return FALSE;
		}

		if (g_listButtons.size() < 2)
		{
			printf("Error: no buttons defined.\r\n");
			return FALSE;
		}

		return TRUE;
	}
	catch (Exception& e)
	{
		printf("Error: %s\r\n", e.displayText().c_str());
		return FALSE;
	}
}

BOOL GenerateMenu()
{
	int nVideosPerPage = g_listButtons.size() - 2;
	g_nTotalPages = g_listVideos.size() / nVideosPerPage;
	if (g_listVideos.size() % nVideosPerPage > 0)
		g_nTotalPages++;

	USES_CONVERSION;

	Image imgHighlight(A2W(g_strResourceFolder + "\\video.wmf"), TRUE);
	if (imgHighlight.GetLastStatus() != Ok)
	{
		printf("Error: failed to open %s\\video.wmf.\r\n", g_strResourceFolder);
		return FALSE;
	}

	Image imgPrev(A2W(g_strResourceFolder + "\\prev.wmf"), TRUE);
	if (imgPrev.GetLastStatus() != Ok)
	{
		printf("Error: failed to open %s\\prev.wmf.\r\n", g_strResourceFolder);
		return FALSE;
	}

	Image imgNext(A2W(g_strResourceFolder + "\\next.wmf"), TRUE);
	if (imgNext.GetLastStatus() != Ok)
	{
		printf("Error: failed to open %s\\next.wmf.\r\n", g_strResourceFolder);
		return FALSE;
	}

	BUTTON &prev = g_listButtons[g_listButtons.size() - 2];
	BUTTON &next = g_listButtons[g_listButtons.size() - 1];

	for (g_nPage = 0; g_nPage < g_nTotalPages; g_nPage++)
	{
		if(!g_listHightlightButton.empty())
			g_listHightlightButton.clear();
		CString strScript;
		try
		{
			CString strTemplate;
			strTemplate.Format("%s\\Scripts\\DVDMenu.avs", GetModuleFilePath());

			CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
			ULONGLONG len = file.GetLength();
			file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
			strScript.ReleaseBuffer();

			strScript.Replace("$(modulepath)", GetModuleFilePath());

			strScript.Replace("$(background_image)", g_sBackgroundImage.c_str());
			strScript.Replace("$(background_audio)", g_sBackgroundAudio.c_str());

			strScript.Replace("$(title_text)", g_sTitleText.c_str());
			strScript.Replace("$(title_color)", g_sTitleFontColor.c_str());
			strScript.Replace("$(title_font)", g_sTitleFont.c_str());
			strScript.Replace("$(title_font_size)", g_sTitleFontSize.c_str());
			strScript.Replace("$(title_x)", g_sTitleX.c_str());
			strScript.Replace("$(title_y)", g_sTitleY.c_str());

			strScript.Replace("$(width)", "720");
			strScript.Replace("$(height)", IsPAL() ? "576" : "480");
			strScript.Replace("$(fps)", IsPAL() ? "25" : "29.97");

			strScript.Replace("$(duration)", g_sDuration.c_str());


			CString strFrames;
			CString strButtons;
			CString strLayers;

			for (int i = 0; i < nVideosPerPage && i + g_nPage * nVideosPerPage < g_listVideos.size(); i++)
			{
				BUTTON &button = g_listButtons[i];

				// frames
				CString strFrame;
				strFrame.Format(
					"\r\n\tm%d = ImageSource(\"%s\\video.png\", fps=%s, pixel_type=\"rgb32\").BilinearResize(%d, %d)\r\n"
					"\tm%d = Layer(v.Crop(%d, %d, %d, %d), m%d, \"add\", 256, 0, 0).ResetMask().ColorKeyMask($000000)\r\n", 
					i, (LPCTSTR)g_strResourceFolder, IsPAL() ? "25" : "29.97", button.rc[1].Width(), button.rc[1].Height(), 
					i, button.rc[1].left, button.rc[1].top, button.rc[1].Width(), button.rc[1].Height(),
					i);

				strFrames += strFrame;


				// buttons
				VIDEO &video = g_listVideos[i + g_nPage * nVideosPerPage];
				LPCTSTR pszPath = video.sPath.c_str();

				CString strButton;
				strButton.Format(
					"\r\n\tc%d = DirectShowSource(\"%s\", fps=%s, ConvertFPS=true, audio=false).BilinearResize(%d, %d).ConvertToRGB32().Loop()\r\n"
					"\tv = Layer(v, c%d, \"add\", 256, %d, %d)\r\n", 
					i, pszPath, IsPAL() ? "25" : "29.97", button.rc[0].Width(), button.rc[0].Height(), 
					i, button.rc[0].left, button.rc[0].top);

				strButtons += strButton;

				// Layers
				CString strLayer;
				strLayer.Format("\r\n\tv = Layer(v, m%d, \"add\", 256, %d, %d)\r\n", i, button.rc[1].left, button.rc[1].top);

				strLayers += strLayer;

				// Button text
				int x, y;
				int nAlign;
				if (_tcsicmp(g_sButtonTextAlign.c_str(), "left") == 0)
				{
					nAlign = 7;
					x = button.rc[3].left;
					y = button.rc[3].top;
				}
				else if (_tcsicmp(g_sButtonTextAlign.c_str(), "right") == 0)
				{
					nAlign = 9;
					x = button.rc[3].right;
					y = button.rc[3].top;
				}
				else
				{
					nAlign = 8;
					x = button.rc[3].left + button.rc[3].Width() / 2;
					y = button.rc[3].top;
				}

				CString strButtonText;
				strButtonText.Format("\r\n\tv = Subtitle(v, \"\"\"%s\"\"\", %d, %d, align=%d, font=\"%s\", size=%s, text_color=%s, halo_color=$ff000000)\r\n", 
					video.sText.c_str(), x, y, 
					nAlign, 
					g_sButtonFont.c_str(), g_sButtonFontSize.c_str(), g_sButtonTextColor.c_str());

				strLayers += strButtonText;
			}

			CString strPage;

			if (g_nPage > 0)
			{
				strPage.Format("\r\n\tprev = ImageSource(\"%s\", pixel_type=\"rgb32\").BilinearResize(%d, %d).ConvertToRGB32().ColorKeyMask($00000000)\r\n" 
					"\tv = Layer(v, prev, \"add\", 256, %d, %d)\r\n", 
					g_strResourceFolder + "\\prev.png",
					prev.rc[0].Width(), prev.rc[0].Height(), 
					prev.rc[0].left, prev.rc[0].top);
				strButtons += strPage;
			}

			if (g_nPage < g_nTotalPages - 1)
			{
				strPage.Format("\r\n\tnext = ImageSource(\"%s\", pixel_type=\"rgb32\").BilinearResize(%d, %d).ConvertToRGB32().ColorKeyMask($00000000)\r\n"
					"\tv = Layer(v, next, \"add\", 256, %d, %d)\r\n", 
					g_strResourceFolder + "\\next.png", 
					next.rc[0].Width(), next.rc[0].Height(), 
					next.rc[0].left, next.rc[0].top);
				strButtons += strPage;
			}

			Bitmap imgBackground(720, IsPAL() ? 576 : 480, PixelFormat24bppRGB);
			if (imgBackground.GetLastStatus() != Ok)
			{
				printf("Error: failed to create background image.\r\n");
				return FALSE;
			}

			Graphics g(&imgBackground);
			if (g.GetLastStatus() != Ok)
				return FALSE;

			g.SetSmoothingMode(SmoothingModeNone);
			g.SetPixelOffsetMode(PixelOffsetModeNone);

			SolidBrush brBackground(Color(0, 0, 0, 0));

			g.FillRectangle(&brBackground, 0, 0, 720, IsPAL() ? 576 : 480);

			for (int i = 0; i < nVideosPerPage && i + g_nPage * nVideosPerPage < g_listVideos.size(); i++)
			{
				CRect &rect = g_listButtons[i].rc[2];
				if (g.DrawImage(&imgHighlight, rect.left, rect.top, rect.Width(), rect.Height()) != Ok)
					return FALSE;
				g_listHightlightButton.push_back(rect);
			}

			if (g_nPage > 0)
			{
				if (g.DrawImage(&imgPrev, prev.rc[2].left, prev.rc[2].top, prev.rc[2].Width(), prev.rc[2].Height()) != Ok)
					return FALSE;
				g_listHightlightButton.push_back(prev.rc[2]);

			}

			if (g_nPage < g_nTotalPages - 1)
			{
				if (g.DrawImage(&imgNext, next.rc[2].left, next.rc[2].top, next.rc[2].Width(), next.rc[2].Height()) != Ok)
					return FALSE;
				g_listHightlightButton.push_back(next.rc[2]);
			}

			CLSID pngClsid;
			GetEncoderClsid(L"image/png", &pngClsid);
			CString strMenuPng;
			strMenuPng.Format("%s\\menu-%d.png", (LPCTSTR)g_strOutputFolder, g_nPage);
			if (imgBackground.Save(A2W(strMenuPng), &pngClsid, NULL) != Ok)
				return FALSE;

			strScript.Replace("$(frames)", (LPCTSTR)strFrames);

			strScript.Replace("$(buttons)", (LPCTSTR)strButtons);

			strScript.Replace("$(layers)", (LPCTSTR)strLayers);

			ASSERT(strScript.Find("$(") < 0);
		}
		catch (CException *e)
		{
			TCHAR szError[256];
			e->GetErrorMessage(szError, 256);
			printf("Error: %s.\r\n", szError);

			e->Delete();
			return FALSE;
		}

		CString strMenuAVS;
		strMenuAVS.Format("%s\\dvdmenu-%d.avs", (LPCTSTR)g_strOutputFolder, g_nPage);

		try
		{
			CStdioFile file(strMenuAVS, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

			file.Write(strScript, strScript.GetLength());

			file.Flush();
			file.Close();
		}
		catch (CException *e)
		{
			TCHAR szError[256];
			e->GetErrorMessage(szError, 256);
			printf("Error: %s.\r\n", szError);

			e->Delete();
			return FALSE;
		}

		CString strMenuMpg;
		strMenuMpg.Format("%s\\menuback-%d.mpg", (LPCTSTR)g_strOutputFolder, g_nPage);

		if (!EncodeMenu())
		{
			printf("Error: EncodeMenu() failed.\r\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL EncodeMenu()
{
	CString strMenuAvs;
	strMenuAvs.Format("%s\\dvdmenu-%d.avs", (LPCTSTR)g_strOutputFolder, g_nPage);

	CString strMenuPng;
	strMenuPng.Format("%s\\menu-%d.png", (LPCTSTR)g_strOutputFolder, g_nPage);

	CString strMenuBackMpg;
	strMenuBackMpg.Format("%s\\menuback-%d.mpg", (LPCTSTR)g_strOutputFolder, g_nPage);
		
	//ffmpeg.exe -i "C:\Users\ch\Desktop\DVDAuthor\menu.avs" -vcodec mpeg2video -b 8000k -minrate 8000k -maxrate 8000k -bufsize 1835k -muxrate 10080k -packetsize 2048 -aspect 16:9 -acodec ac3 -ar 48000 -ab 448k -ac 6 -r 29.97 -s 720x480 -f dvd -y "C:\Users\ch\Desktop\DVDAuthor\menuback.mpg"

	CString strApplication;
	CString strParameters;

	strApplication.Format("%s\\encoder\\ffmpeg.exe", (LPCTSTR)GetModuleFilePath());
	strParameters.Format(" -i \"%s\" -vcodec mpeg2video -b %sk -minrate %sk -maxrate %sk -bufsize 1835k -muxrate 10080k -packetsize 2048 -aspect %s \
-acodec ac3 -ar 48000 -ab %sk -ac %s -r %s -s %s -f dvd -y \"%s\"", 
		strMenuAvs,g_sVideoBitrate.c_str(),g_sVideoBitrate.c_str(),g_sVideoBitrate.c_str(), "4:3", 
g_sAudioBitrate.c_str(),g_sChannels.c_str(),IsPAL() ? "25" : "29.97", IsPAL() ? "720x576" : "720x480", strMenuBackMpg);

     g_bFFmpegSucceeded = FALSE;

#ifdef _TEST_SPEED
	DWORD dwCurTime = GetTickCount(),dwSpend;
#endif

	if (!ExecuCommand(strApplication, strParameters, ParseFFmpegOutput))
	{
		printf("Error: ExecuCommand Failed( %s %s).\r\n", strApplication ,strParameters);
		return FALSE;
	}

	if (!g_bFFmpegSucceeded)
	{
		printf("Error:  g_bFFmpegSucceeded = FALSE(%s %s).\r\n", strApplication ,strParameters);
		return FALSE;
	}

#ifdef _TEST_SPEED
	dwSpend = GetTickCount()-dwCurTime;
	CString strTime ;
	strTime.Format("ExecuCommand(strApplication, strParameters, ParseFFmpegOutput):spend time %d milliseconds .\r\n",dwSpend);
	OutputDebugString(strTime);
#endif

	CString strSpuMuxXmlTemplate;
	strSpuMuxXmlTemplate.Format("%s\\Scripts\\spumux.xml", GetModuleFilePath());

	CString strSpuMuxXml;
	strSpuMuxXml.Format("%s\\spumux-%d.xml", (LPCTSTR)g_strOutputFolder, g_nPage);

	try
	{
		AutoPtr<Document> pDoc = OpenXMLDocument(strSpuMuxXmlTemplate);

		Element *pSpuNode = pDoc->getElementById("spu", "id");
		ASSERT(pSpuNode != NULL);
		if (pSpuNode == NULL)
		{
			printf("Error: <spu> node not found.\r\n");
			return FALSE;
		}

		pSpuNode->removeAttribute("id");
		pSpuNode->setAttribute("highlight", UTF8Encode(strMenuPng).c_str());
		pSpuNode->removeAttribute("autooutline");
		for(int i=0;i<g_listHightlightButton.size();i++)
		{

			AutoPtr<Element> pButtonNode = pDoc->createElement("button");
			SetNodeAttribute(pButtonNode, "x0", g_listHightlightButton[i].left);
			SetNodeAttribute(pButtonNode, "y0", g_listHightlightButton[i].top);
			SetNodeAttribute(pButtonNode, "x1", g_listHightlightButton[i].right);
			SetNodeAttribute(pButtonNode, "y1", g_listHightlightButton[i].bottom);
			pSpuNode->appendChild(pButtonNode);
		}

		SaveXMLDocument(pDoc, strSpuMuxXml);
	}
	catch (Exception& e)
	{
		printf("Error: %s\r\n", e.displayText().c_str());
		return FALSE;
	}

	CString strMenuMpg;
	strMenuMpg.Format("%s\\menu-%d.mpg", g_strOutputFolder, g_nPage);

	// spumux.exe -m dvd -P spumux.xml < menuback.mpg > menu.mpg
	CString strScript;
	strScript.Format("\"%s\\encoder\\spumux.exe\" -m dvd -P \"%s\" < \"%s\" > \"%s\"", 
		GetModuleFilePath(), strSpuMuxXml, strMenuBackMpg, strMenuMpg);

	CString strSpuMuxCmd;
	strSpuMuxCmd.Format("%s\\spumux-%d.cmd", (LPCTSTR)g_strOutputFolder, g_nPage);
	try
	{
		CStdioFile file(strSpuMuxCmd, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

		file.Write(strScript, strScript.GetLength());

		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		TCHAR szError[256];
		e->GetErrorMessage(szError, 256);
		printf("Error: %s.\r\n", szError);

		e->Delete();
		return FALSE;
	}

	struct _stat64 fs;
	if (_stati64(strMenuBackMpg, &fs) != 0)
	{
		printf("Error: failed to get the size of %s.\r\n", (LPCTSTR)strMenuBackMpg);
		return FALSE;
	}

	g_nMenuBackFileSize = fs.st_size;
	if (g_nMenuBackFileSize == 0)
	{
		printf("Error: zero-length file %s.\r\n", (LPCTSTR)strMenuBackMpg);
		return FALSE;
	}

	strApplication = "cmd.exe";
	strParameters.Format("/C \"%s\"", strSpuMuxCmd);

	g_bSpuMuxSucceeded = FALSE;
#ifdef _TEST_SPEED
	dwCurTime = GetTickCount();
#endif

	ExecuCommand(strApplication, strParameters, ParseSPUMuxOutput);	// ignore the exit code of cmd.exe

	if (!g_bSpuMuxSucceeded)
		return FALSE;
#ifdef _TEST_SPEED
	dwSpend = GetTickCount()-dwCurTime;
	strTime.Format("ExecuCommand(strApplication, strParameters, ParseSPUMuxOutput):spend time %d milliseconds .\r\n",dwSpend);
	OutputDebugString(strTime);
#endif

	return TRUE;
}

BOOL ParseFFmpegOutput(LPCTSTR lpszLine)
{
	// Video: frame=  191 fps=142 q=1.6 Lsize=    1089kB time=12.73 bitrate= 700.6kbits/s
	// Audio: size=     791kB time=40.49 bitrate= 160.0kbits/s

	Regexx re;
	if (re.exec(lpszLine, "^frame=[ \t]*([0-9]+).*time=[ \t]*[0-9\\.]+") > 0)
	{
		std::string m_sFrame = re.match[0].atom[0].str();
		int nFrame = _ttoi(m_sFrame.c_str());

		// if DVDMenu.avs encounted an error, a 2-frame video was returned.
		if (nFrame > 2)
			g_bFFmpegSucceeded = TRUE;

		int nTotalFrames = (int)((IsPAL() ? 25 : 29.97 ) * _ttoi(g_sDuration.c_str()));
		float fProgress = ((float)g_nPage / g_nTotalPages + (float)nFrame / nTotalFrames / g_nTotalPages / 2) * 100;
		if (fProgress - g_fProgress > 1.0)
		{
			g_fProgress = fProgress;
			printf("Progress: %.1f%%\r\n", fProgress);
			_flushall();
		}

		return TRUE;
	}
	else
	{
		printf("%s\r\n", lpszLine);
		_flushall();
		return FALSE;
	}
}

BOOL ParseSPUMuxOutput(LPCTSTR lpszLine)
{
	// INFO: 3016704 bytes of data written
	// INFO: 1 subtitles added, 0 subtitles skipped, stream: 32, offset: 0.50
	Regexx re;
	if (re.exec(lpszLine, "^INFO\\:[ \t]*([0-9]+)[ \t]+bytes of data written") > 0)
	{
		std::string m_sSize = re.match[0].atom[0].str();

		int nSize = _ttoi(m_sSize.c_str());

		float fProgress = (((float)g_nPage + 0.5f) / g_nTotalPages + (float)nSize / g_nMenuBackFileSize / g_nTotalPages / 2) * 100;
		if (fProgress - g_fProgress > 1.0)
		{
			g_fProgress = fProgress;
			printf("Progress: %.1f%%\r\n", fProgress);
			_flushall();
		}

		return TRUE;
	}
	else if (re.exec(lpszLine, "^INFO\\:[ \t]*([0-9]+)[ \t]+subtitles added") > 0)
	{
		std::string m_sAdded = re.match[0].atom[0].str();

		int nAdded = _ttoi(m_sAdded.c_str());
		if (nAdded == 1)
			g_bSpuMuxSucceeded = TRUE;

		return TRUE;
	}
	else
	{
		printf("%s\r\n", lpszLine);
		_flushall();
		return FALSE;
	}
}

#define PIPE_BUFFER_LEN    64*1024
BOOL ExecuCommand(LPCTSTR lpszApplication, LPCTSTR lpszParameters, PARSE_OUTPUT_PROC pfnParseOutput)
{
 	BOOL bResult = FALSE;
 
 	HANDLE hOutputReadTmp, hOutputRead, hOutputWrite;
 
 	SECURITY_ATTRIBUTES sa;
 	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
 	sa.lpSecurityDescriptor = NULL;
 	sa.bInheritHandle = TRUE;
 	if (::CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, PIPE_BUFFER_LEN))
 	{
 		::DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
 			GetCurrentProcess(),
 			&hOutputRead, // Address of new handle.
 			0,FALSE, // Make it uninheritable.
 			DUPLICATE_SAME_ACCESS);
 
 		// Close inheritable copies of the handles you do not want to be
 		// inherited.
 		::CloseHandle(hOutputReadTmp);
 
 		STARTUPINFO si;
 
 		// Set up the start up info struct.
 		ZeroMemory(&si,sizeof(STARTUPINFO));
 		si.cb = sizeof(STARTUPINFO);
 		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
 		si.wShowWindow = SW_HIDE;
 		si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
 		si.hStdOutput = hOutputWrite;
 		si.hStdError = hOutputWrite;
 
 		PROCESS_INFORMATION pi;
 		memset(&pi, 0, sizeof(pi));
 
 		CString strCommandLine;
 		strCommandLine.Format("\"%s\" %s", (LPCTSTR)lpszApplication, (LPCTSTR)lpszParameters);
 		TRACE("%s\n\n", strCommandLine);
 
 		BOOL ret = ::CreateProcess(NULL, strCommandLine.GetBuffer() /* may be modified in UNICODE version */, 
 			NULL, NULL, TRUE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);
 
 		strCommandLine.ReleaseBuffer();
 
 		if (!ret)
 		{
 			printf("Error: failed to execute \"%s\". %s\r\n", lpszApplication, (LPCTSTR)GetLastErrorMessage());
 		}
 		else
 		{
 			::WaitForInputIdle(pi.hProcess, 1000 * 10);
 
 			BOOL bExitProcess = FALSE;
 
 			CString strLine;
 			char szRead[PIPE_BUFFER_LEN];
 
 			while (true)
 			{
 				if (!bExitProcess)
 				{
 					DWORD ret = ::WaitForSingleObject(pi.hProcess, 0);
 					if (ret == WAIT_OBJECT_0)
 						bExitProcess = TRUE;
 				}
 
 				DWORD dwRead = 0;
 
 				if (!::PeekNamedPipe(hOutputRead, NULL, 0, NULL, &dwRead, NULL))
 				{
 					printf("Error: PeekNamedPipe() failed.\r\n");
 					break;
 				}
 
				if (dwRead == 0)
				{
					if (bExitProcess)
					{
						break;
					}
					else
					{
						Sleep(200);
#ifdef _TEST_SPEED
						printf("Sleep............%d\r\n",200);
#endif
						continue;
					}
				}
#ifdef _TEST_SPEED
				else
					printf("no Sleep............%d\r\n",dwRead);
#endif
 		
 
 
 				if (!::ReadFile(hOutputRead, szRead, PIPE_BUFFER_LEN-1, &dwRead, NULL) || dwRead == 0)
 				{
 					printf("Error: ReadFile() failed.\r\n");
 					break;
 				}
 
 				if (pfnParseOutput != NULL)
 				{
 					szRead[dwRead] = '\0';
 					strLine += szRead;
 
 					while (true)
 					{
 						int pos = strLine.FindOneOf("\r\n");
 						if (pos >= 0)
 						{
 							while (strLine[pos + 1] == '\r' || strLine[pos + 1] == '\n')
 								pos++;
 
 							int len = strLine.GetLength();
 							strLine.SetAt(pos, '\0');
 
 						#ifdef _DEBUG
 							CString strTemp = strLine;
 							strTemp.TrimRight("\r\n");
 							TRACE("%s\n", strTemp);
 						#endif
 
 							pfnParseOutput(strLine);
 
 							strLine = strLine.Right(len - (pos + 1));
 						}
 						else
 						{
 							break;
 						}
 					}
 				}
 			}
 
 			DWORD dwCode;
 			GetExitCodeProcess(pi.hProcess, &dwCode);
 			TRACE("Encoder (%s) exit code: %d\n", lpszApplication, dwCode);
 
 			bResult = (dwCode == 0);
 
 			::CloseHandle(pi.hProcess);
 			::CloseHandle(pi.hThread);
 		}
 	}
 
 	TRACE("Close Pipe\n");
 	if (hOutputWrite)
 	{
 		::CloseHandle(hOutputWrite);
 		hOutputWrite = NULL;
 	}
 	if (hOutputRead)
 	{
 		::CloseHandle(hOutputRead);
 		hOutputRead = NULL;
 	}

	return bResult;
}

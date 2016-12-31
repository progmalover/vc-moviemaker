#pragma once
#include "MenuTemplateManager.h"
#include "Task.h"
#include "Profile.h"

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _MENU_TEMPLATE_VIEW
#define ALIGN(x,a)		(((x)+(a)-1)&~((a)-1))

BOOL    SaveImage(Bitmap *pMask,CString strName);
void    DrawBitmapEx(HDC hDC, int xDC, int yDC, int cx, int cy, HBITMAP hBitmap, int xBmp, int yBmp,int cxBmp=0, int cyBmp=0);
HBITMAP CreateSection(HDC MemDC,UINT bits,int bmWidth,int bmHeight,DWORD &bufLen,LPBYTE **lpBits);
void    GrayImageBuffer(BYTE *lpBits,DWORD dwLen,int nW,int nH,int nBits,int nnGray);
HBITMAP GrayBimap(HBITMAP hBitmap,int nGray/*[0,100]*/,int xDC=0, int yDC=0, int cx=-1, int cy=-1);
void    DrawTransParent(HDC hDC, int xDC, int yDC, int cx, int cy, HBITMAP hBitmap, int xBmp, int yBmp,int cxBmp=0, int cyBmp=0);
HBITMAP CopyFromMemDC(HDC memDC,const CRect &rcSou,const CSize &szDst);

//由于CMenuTemplate中存有大量的数据，对Menu界面的印象比较大
#define MAX_MENU_TITLE_LEN   20
#define MAX_VIDEO_TITLE_LEN  5

#define BUTTON_BORDER    3
//分别对应不同的矩形
enum BTN_RECT{VIDEO,MENU,MENU_H,VIDEO_T};
//VIDEO --视频矩形区域
//MENU -- 菜单按钮矩形区域
//MENU_H -- 菜单按钮高亮矩形区域
//VIDEO_T -- 视频标题矩形区域
class CMenuTemplateEx
{
public:
	CMenuTemplateEx(void);
	~CMenuTemplateEx(void);
	DECLARE_SINGLETON(CMenuTemplateEx)

	BOOL DrawBG(CDC *pDC,CRect rcClient);
	BOOL DrawPageIndex(CDC *pDC);
	//int nIndex是Button在当前菜单按钮中的索引
	BOOL DrawTaskBtn(CDC *pDC,int nIndex,CTask *pTask,BOOL bHover,BOOL bFocus);
   //int nIndex:PAGE_BTN_PRE(Preview Button),PAGE_BTN_NEXT(Next Button)
	BOOL DrawPageBtn(CDC *pDC,int nIndex,BOOL bHover,BOOL bFocus);
	int  PtInButton(CPoint point,int nButtons,BOOL bPre,BOOL bNext);
	int  GetButtons();
	BOOL EnableMenu();
	CRect GetItemRect(int nIndex,BTN_RECT btnIndex = VIDEO);
public:
	//模板改变或者当前改变对模板有影响
	void OnVStandardChange(LPCTSTR lpszText);
	void OnAspectChange(LPCTSTR lpszText);
	BOOL OnPropChange( MENU_PROP_DATA mID);
private:
    CMenuTemplate *m_pTemplate;
	CBitmap   m_bmpBG;
	CBitmap   m_bmpBG2;//No Menu
	//在模板中已经定义且不可改变的
	Bitmap   *m_bmpVideo;
	Bitmap   *m_bmpVideo2;
	Bitmap   *m_bmpPre;
	Bitmap   *m_bmpPre2;
	Bitmap   *m_bmpNext;
	Bitmap   *m_bmpNext2;

	float m_ScaleX;
	float m_ScaleY;
	CRect m_rcMenu;

	int m_nAspectX;
	int m_nAspectY;

	//在实际代码中没有使用
	CFont m_ftTitleTxt;
	COLORREF m_clrTitleTxt;
	CFont m_ftBtnTxt;
	COLORREF m_clrBtnTxt;
private:
	void Attach(CMenuTemplate *pTemplate);
	BOOL LoadImage();
#ifdef _BD
	Bitmap  *LoadFirstBlock(CString strFormat,BUTTON &btn);
#endif
	BOOL LoadFont();
	void DeleteImage();
	BOOL GetAspect(int &nWidth,int &nHeight);
	//根据当前的数据，计算实际的区域
	float ZoomRatio(CRect rcClient,int nAspectX,int nAspectY);
	HFONT CreateFont(CString strFont,CString strFontSize);
	BOOL  DrawTitle(CDC *pDC,CRect rcClient);
	BOOL  LoadTitleFont();
	BOOL  LoadButtonFont();
	BOOL  DrawButton(CDC *pDC,int nIndex,BOOL bHover,BOOL bFocus,HBITMAP hbBG,CRect rcBG);
	Bitmap *DrawMask(int nIndex,HBITMAP hbBG,CRect rcBG);
	Bitmap *DrawMask2(int nIndex,HBITMAP hbBG,CRect rcBG);
};

#endif
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

void DrawBitmap(HDC hDC, int xDC, int yDC, int cx, int cy, HBITMAP hBitmap, int xBmp, int yBmp);
void DrawTransparent(CDC *pDC, int xDC, int yDC, int cx, int cy, CBitmap *pBmp, int xBmp, int yBmp, COLORREF crTrans);
void DrawDisabled(CDC *pDC, int xDC, int yDC, int cx, int cy, CBitmap *pBmp, int xBmp, int yBmp);
void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HBITMAP hbm, int nXSrc, int nYSrc, COLORREF crBack);

//
// Inline functions
//
COLORREF ChangeSValue(COLORREF cr, double percent);
COLORREF ChangeVValue(COLORREF cr, double percent);

#define FACE_COLOR				(GetSysColor(COLOR_3DFACE))
#define LIGHT_COLOR				(GetSysColor(COLOR_3DLIGHT))
#define HILIGHT_COLOR			(GetSysColor(COLOR_3DHILIGHT))
#define SHADOW_COLOR			(GetSysColor(COLOR_3DSHADOW))
#define DKSHADOW_COLOR			(GetSysColor(COLOR_3DDKSHADOW))
#define TEXT_COLOR				(GetSysColor(COLOR_WINDOWTEXT))
#define HIGHLIGHTTEXT_COLOR		(GetSysColor(COLOR_HIGHLIGHTTEXT))
#define HIGHLIGHT_COLOR			(GetSysColor(COLOR_HIGHLIGHT))
#define DITHER_COLOR			(ChangeVValue(FACE_COLOR, 110))

struct HSVType;
struct RGBType
{
	int r,g,b;
	COLORREF toRGB(){return RGB(r,g,b);}
	HSVType toHSV();
};


struct HSVType
{
	int h,s,v;
	RGBType toRGB();
};

// RGB(255, 255, 0) => #ffff00
CString RGBToString(COLORREF rgb);
COLORREF StringToRGB(LPCTSTR lpszColor);


#endif

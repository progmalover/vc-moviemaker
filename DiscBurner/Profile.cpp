#include "StdAfx.h"
#include "Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int FontSizeToPointSize(LONG lHeight)
{
	HDC hDC = ::GetDC(NULL);
	int nPointSize = -::MulDiv(lHeight, 72, ::GetDeviceCaps(hDC, LOGPIXELSY));
	::ReleaseDC(NULL, hDC);
	return nPointSize;
}

int PointSizeToFontSize(int nPointSize)
{
	HDC hDC = ::GetDC(NULL);
	LONG lHeight = -MulDiv(nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	return lHeight;
}

CString RGBToAVS(COLORREF rgb)
{
	CString strColor;
	strColor.Format("$%02x%02x%02x", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
	return strColor;
}

COLORREF AVSToRGB(LPCTSTR lpszColor)
{
	int r = 0;
	int g = 0;
	int b = 0;

	CString strColor = lpszColor;
	int len = strColor.GetLength();
	if (len > 0)
	{
		int nNewLen = 6;
		int nStart = 0;
		if (strColor[0] == '$')
		{
			nStart++;
			nNewLen++;
		}

		for (int i = 0; i < nNewLen - len; i++)
			strColor += '0';

		sscanf(strColor.Mid(nStart, 2), "%x", &r);
		sscanf(strColor.Mid(nStart + 2, 2), "%x", &g);
		sscanf(strColor.Mid(nStart + 4, 2), "%x", &b);
	}

	return RGB((BYTE)r, (BYTE)g, (BYTE)b);
}


CProfileItem::CProfileItem(void)
{
	m_uuid = GUID_NULL;
	m_bHasGeneral = FALSE;
	m_bHasVideo = FALSE;
	m_bHasAudio = FALSE;
}

CProfileItem::~CProfileItem(void)
{
	for (PROFILE_PARAM_LIST::iterator it = m_params.begin(); it != m_params.end(); ++it)
		delete *it;
	m_params.clear();
}

#pragma once
#include "mfctoolbarbuttonex.h"
class CMFCToolBarButtonEx2 :public CMFCToolBarButtonEx
{
	DECLARE_SERIAL(CMFCToolBarButtonEx2);
public:
	CMFCToolBarButtonEx2(UINT uiID, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE, BOOL bLocked = FALSE,BOOL bLeftText = TRUE)
		: CMFCToolBarButtonEx(uiID, iImage, lpszText, bUserButton, bLocked)
	{
		m_bLeftText = bLeftText;
	};

	CMFCToolBarButtonEx2()
	{
		m_bLeftText = TRUE;
	};
	~CMFCToolBarButtonEx2(void){};
	virtual void CopyFrom(const CMFCToolBarButton& src)
	{
		CMFCToolBarButton::CopyFrom(src);
		CMFCToolBarButtonEx2 *pS = (CMFCToolBarButtonEx2 *)&src;
		m_bLeftText = pS->m_bLeftText;
	};
protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, 
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,						
		BOOL bHighlight = FALSE,						
		BOOL bDrawBorder = TRUE, 
		BOOL bGrayDisabledButtons = TRUE);
protected:
	BOOL m_bLeftText;
};

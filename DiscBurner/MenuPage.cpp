#include "StdAfx.h"
#include "MenuPage.h"
#include "Task.h"
#include "MenuTemplateEx.h"
#include "MenuWnd.h"
#include "MenuList.h"
#include "resource.h"

#ifdef _MENU_TEMPLATE_VIEW
CMenuPage::CMenuPage(void)
:m_bEnablePreBtn(FALSE),
m_bEnableNextBtn(FALSE),
m_iFocus(NULL_BTN),
m_iHover(NULL_BTN),
m_bLftBtnDown(TRUE)
{

}

CMenuPage::~CMenuPage(void)
{

}

void CMenuPage::OnDraw(CDC *pDC,CRect rcClient)
{
	if(m_BtnList.size()<1)
		return ;
	CFileList *pFileList = CFileList::Instance();
	int index = -1;
	CTask *pTask = NULL;
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	for(int i=0;i<m_BtnList.size();i++)
	{
		index = m_BtnList.at(i);
		pTask = (CTask *)pFileList->GetItemData(index);
		ASSERT(pFileList->GetCheck(index));
		pTemplate->DrawTaskBtn(pDC,i,pTask,m_iHover==index,m_iFocus==index);
	}

	if(m_bEnablePreBtn)
		pTemplate->DrawPageBtn(pDC,PAGE_BTN_PRE,m_iHover==PAGE_BTN_PRE,m_iFocus==PAGE_BTN_PRE);

	if(m_bEnableNextBtn)
		pTemplate->DrawPageBtn(pDC,PAGE_BTN_NEXT,m_iHover==PAGE_BTN_NEXT,m_iFocus==PAGE_BTN_NEXT);
	//if(CMenuList::Instance()->GetPages()>0)
	//	pTemplate->DrawPageIndex(pDC);
}

void CMenuPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(pTemplate->EnableMenu()&&!m_BtnList.empty())
		m_bLftBtnDown = TRUE;
}

void CMenuPage::OnMouseMove(UINT nFlags, CPoint point)
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(pTemplate->EnableMenu()&&!m_BtnList.empty())
	{
		int nItem = pTemplate->PtInButton(point,m_BtnList.size(),m_bEnablePreBtn,m_bEnableNextBtn);
		int nHover = nItem;
		//ASSERT(nHover<m_BtnList.size());
		if(nHover>=0)nHover = m_BtnList.at(nHover);

		if(nHover!=m_iHover)
		{
			if(nHover==NULL_BTN)
			{
				//TRACE(_T("Mouse move out: %d\n"),m_iHover);
				CMenuWnd::Instance()->OnMouseOutBtn(nHover);
			}
			else
			{
				//TRACE(_T("Mouse move in: %d\n"),nHover);
				CMenuWnd::Instance()->OnMouseInBtn(nHover);
			}
			m_iHover = nHover;
			CMenuWnd::Instance()->Invalidate();
		}
		CMenuWnd::Instance()->OnMouseOverBtn(nHover,point);
	}
}

void CMenuPage::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(pTemplate->EnableMenu()&&!m_BtnList.empty())
	{
		if(m_bLftBtnDown)
		{
			int nFocus = pTemplate->PtInButton(point,m_BtnList.size(),m_bEnablePreBtn,m_bEnableNextBtn);
			if(nFocus!=NULL_BTN)
			{
				if(nFocus>=0)nFocus = m_BtnList.at(nFocus);
				//TRACE(_T("Mouse click: %d\n"),nFocus);
				CMenuWnd::Instance()->OnClickBtn(nFocus);
				m_iFocus = nFocus;
				CMenuWnd::Instance()->Invalidate();
			}
		}
	}
}

BOOL CMenuPage::OnSetCursor(UINT nHitTest,CPoint point)
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(pTemplate->EnableMenu()&&!m_BtnList.empty())
	{
		int nFocus = pTemplate->PtInButton(point,m_BtnList.size(),m_bEnablePreBtn,m_bEnableNextBtn);
		return nFocus!=NULL_BTN;
	}
	return FALSE;
}

void CMenuPage::OnKillFocus()
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(pTemplate->EnableMenu()&&!m_BtnList.empty())
	{
		m_iFocus = NULL_BTN;
		CMenuWnd::Instance()->Invalidate();
	}
}


#ifdef _DEBUG
void CMenuPage::Dump()
{
	TRACE(_T("----------Current menu page:-------\n"));
	TRACE(_T("Video Buttons:%d\n"),m_BtnList.size());
	CFileList *pFileList = CFileList::Instance();
	int index = -1;
	CTask *pTask = NULL;
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	for(int i=0;i<m_BtnList.size();i++)
	{
		index = m_BtnList.at(i);
		pTask = (CTask *)pFileList->GetItemData(index);
        TRACE(_T("\t%s\n"),pTask->m_strSourceFile);
	}
	TRACE(_T("Page Buttons:%s,%s\n"),m_bEnablePreBtn?_T("Preview":_T("")),m_bEnableNextBtn?_T("Next":_T("")));
}
#endif

BOOL CMenuPage::GetToolTipText(int index,CString &strText,CRect &rcItem)
{
	CMenuTemplateEx *pTemplate = CMenuTemplateEx::Instance();
	if(index==PAGE_BTN_PRE || index == PAGE_BTN_NEXT)
	{
		strText = LoadStringInline(index==PAGE_BTN_PRE?ID_MENU_PRE_PAGE_T:ID_MENU_NEXT_PAGE_T);
		index = pTemplate->GetButtons() + abs(index)-1;
		rcItem = pTemplate->GetItemRect(index);
		rcItem.InflateRect(10,12);
		return TRUE;
	}
	return FALSE;
}
#endif
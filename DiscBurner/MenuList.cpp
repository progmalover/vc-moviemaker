#include "StdAfx.h"
#include "MenuList.h"
#include "MenuWnd.h"
#include "MenuTemplateEx.h"
#include "MenuTemplateManager.h"

#ifdef _MENU_TEMPLATE_VIEW

IMPLEMENT_SINGLETON(CMenuList)
CMenuList::CMenuList(void)
: m_nCurPage(0)
, m_nPages(-1)
,m_pCurPage(NULL)
,m_pFilelist(NULL)
,m_nPageButtons(0)
{
	m_pFilelist = CFileList::Instance();
	m_pCurPage = new CMenuPage();
}

CMenuList::~CMenuList(void)
{
	delete m_pCurPage;
}

BOOL CMenuList::PreviewPage(BOOL bRedraw)
{
	if(m_nCurPage==FIRST_MENU_PAGE)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	int nCPage=  m_nCurPage-1;
	m_nCurPage = SetViewPage(nCPage,bRedraw);
	return TRUE;
}

BOOL CMenuList::NextPage(BOOL bRedraw)
{
	if(m_nCurPage>=m_nPages)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	int nCPage=  m_nCurPage+1;
	m_nCurPage = SetViewPage(nCPage,bRedraw);
	return TRUE;
}

//改变当前需要显色界面
int CMenuList::SetViewPage(int nPage,BOOL bRedraw)
{
	if(m_nPages>0)
	{
		m_nCurPage = min(nPage,m_nPages);
		LoadPage(m_nCurPage);
		if(bRedraw)ReDrawMenu();
	}
	else
		m_nCurPage = -1;
	return m_nCurPage;
}

//菜单按钮需要改变
void CMenuList::OnFileListChange()
{
	LoadCurentPage();
	ReDrawMenu();
}

//菜单背景需要改变----No Menu下buttons=0;
void CMenuList::OnMenuTemplateChange(MENU_PROP_DATA mID)
{
	CMenuTemplateEx *pTemplatex = CMenuTemplateEx::Instance();
	pTemplatex->OnPropChange(mID);
	if(mID == MENU_PROP_TEMPLATES)
		m_nPageButtons = pTemplatex->GetButtons();
	ReDrawMenu();
}

//当前选择的菜单改变
void  CMenuList::OnFileSelectedChange()
{
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		pCurPage->OnKillFocus();
		ReDrawMenu();
	}
}

//Button text改变
void CMenuList::OnButtonTextChange(CTask *pTask)
{
	ReDrawMenu();
}

//加载菜单的当前显示界面的数据
BOOL CMenuList::LoadPage(int nPage)
{
	ASSERT(nPage>0 && m_nPageButtons>0 && m_nPages>0);
	m_pCurPage->ClearAll();

	if(m_sFileList.size()>0)
	{
		int nStart = (nPage-1)*m_nPageButtons;
		for(int i=nStart;i<m_sFileList.size() && i<nPage*m_nPageButtons;i++)
			m_pCurPage->AddButton(m_sFileList.at(i));
	}
	//BOOL bEnablePre,int bEnableNext
	m_pCurPage->EnablePageBtn(nPage>FIRST_MENU_PAGE,nPage<m_nPages);
//#ifdef _DEBUG
//	Dump();
//	m_pCurPage->Dump();
//#endif
	return m_pCurPage!=NULL;
}

int  CMenuList::IndexToPage(int nIndex)
{
	ASSERT(nIndex>=0 && nIndex<m_pFilelist->GetItemCount());
	if(m_pFilelist->GetCheck(nIndex))
	{
		int nStart=0,nEnd=0;
		for (int i=0;i<m_nPages;i++)
		{
			PageRange(i,nStart,nEnd);
			if(nIndex>=nStart && nIndex<=nEnd)
				return i;
		}
	}
	return -1;
}

BOOL  CMenuList::PageRange(int nPage,int &nStart,int &nEnd)
{
	ASSERT(nPage>0 && nPage<=m_nPages);
	int nS = (m_nCurPage-1)*m_nPageButtons,
		nE = min(m_nCurPage*m_nPageButtons,m_sFileList.size());
	nStart = m_sFileList.at(nS-1);
	nEnd = m_sFileList.at(nE-1);
	return nStart!=nEnd;
}


int  CMenuList::CheckFileList()
{
	m_sFileList.clear();
	if(m_nPageButtons>0)
	{
		for(int i=0;i<m_pFilelist->GetItemCount();i++)
		{
			if(m_pFilelist->GetCheck(i))
				m_sFileList.push_back(i);
		}
		m_nPages = m_sFileList.size()/m_nPageButtons;
		if(m_sFileList.size()%m_nPageButtons!=0)
			m_nPages++;
		return m_sFileList.size();
	}
    return 0;
}

//实时刷新--所有数据都是获取最新的
CMenuPage* CMenuList::GetViewPage(BOOL bCheck)
{
	if(bCheck)
		LoadCurentPage();
	return m_pCurPage;
}

BOOL  CMenuList::LoadCurentPage()
{
	CheckFileList();
	if(m_nPages>0)
	{
		if(m_nCurPage<1)m_nCurPage = FIRST_MENU_PAGE;
		m_nCurPage = min(m_nCurPage,m_nPages);
		return LoadPage(m_nCurPage);
	}
	else
	{
		m_pCurPage->ClearAll();
		m_pCurPage->EnablePageBtn(FALSE,FALSE);
	}
	return FALSE;
}

void  CMenuList::ReDrawMenu()
{
	CMenuWnd *pMenuview = CMenuWnd::Instance();
	if(pMenuview->GetSafeHwnd()!=NULL)
		pMenuview->Invalidate();
}

#ifdef _DEBUG
void CMenuList::Dump()
{
	TRACE(_T("----------Menu pages:-------\n"));
	TRACE(_T("All Page:%d\n"),m_nPages);
	TRACE(_T("Current Page:%d\n"),m_nCurPage);
}
#endif


#endif
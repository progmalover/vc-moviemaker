#pragma once
#include <vector>
#include "MenuPage.h"
#include "FileList.h"
#include "Task.h"
#include "Profile.h"

#ifdef _MENU_TEMPLATE_VIEW
//主要作用:根据CFileList中的节目链表信息，将数据以CMenuPage的形式封装，作为CFileList、CMenuPage、CMenuWnd的数据中心；
#define FIRST_MENU_PAGE   1
class CMenuList
{
public:
	CMenuList(void);
	virtual ~CMenuList(void);
	DECLARE_SINGLETON(CMenuList)

protected:
	int m_nCurPage;//1,2,3
	int m_nPages;
	CMenuPage* m_pCurPage;
	int   m_nPageButtons;//当前模板每页的按钮数（不包括翻页按钮）
	CFileList *m_pFilelist;
	typedef std::vector<int> SEL_FILE_LIST;
	SEL_FILE_LIST m_sFileList;

public:
	BOOL PreviewPage(BOOL bRedraw=FALSE);
	BOOL NextPage(BOOL bRedraw=FALSE);
	int SetViewPage(int nPage,BOOL bRedraw=FALSE);
	CMenuPage* GetViewPage(BOOL bCheck=TRUE);
	int  GetCurentPageIndex(){return m_nCurPage;};
	int  GetPages(){return m_nPages;};
	void OnFileListChange();
	void OnFileSelectedChange();
	void OnMenuTemplateChange(MENU_PROP_DATA mID);
	void OnButtonTextChange(CTask *pTask);
	void OnMenuTextChange();
private:
	int   IndexToPage(int nIndex);
	BOOL  PageRange(int nPage,int &nStart,int &nEnd);
#ifdef _DEBUG
	void Dump();
#endif
protected:
	virtual int   CheckFileList();
	virtual BOOL  LoadPage(int nPage);
	virtual BOOL  LoadCurentPage();
	void    ReDrawMenu();
};

#endif

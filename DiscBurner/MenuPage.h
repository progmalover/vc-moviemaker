#pragma once
#include <vector>
#include "MenuTemplateManager.h"

#ifdef _MENU_TEMPLATE_VIEW
//��Ҫ���ã�
//��1���滭��ǰҳ���ϵĲ˵���ť���߿�����ͼ�����֡�����ҳ��ť������ͼ��
//��2����Ӧ�û��Խ���Ĳ���������ƶ�����굥��������֪ͨ������Ҫ���»滭

//��CMenuPage����ʾ��Ч���������ܵ������С�����ƣ���Ҫ�ȱ���С��Ŵ�
#define PAGE_BTN_PRE  -1
#define PAGE_BTN_NEXT -2
#define NULL_BTN      -3

class CMenuPage
{
public:
	CMenuPage(void);
	virtual ~CMenuPage(void);
   
public:
	void OnDraw(CDC *pDC,CRect rcClient);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnSetCursor(UINT nHitTest,CPoint point);
    void OnKillFocus();
private:
	void OnNotify(WPARAM wParam, LPARAM lParam);
private:
	typedef std::vector<int> BTN_LIST;
	BTN_LIST m_BtnList;
	int  m_iHover;
	BOOL m_bLftBtnDown;
public:
	int  m_iFocus;
	BOOL m_bEnablePreBtn;
	BOOL m_bEnableNextBtn;

public:
	void AddButton(int nIndex){m_BtnList.push_back(nIndex);};
	void ClearAll(){m_BtnList.clear();};
	void EnablePageBtn(BOOL bEnablePre,int bEnableNext){m_bEnablePreBtn = bEnablePre; m_bEnableNextBtn = bEnableNext;};
#ifdef _DEBUG
    void Dump();
#endif
	BOOL GetToolTipText(int index,CString &strText,CRect &rcItem);
};

#endif
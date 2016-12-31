// MenuWnd.cpp : implementation file
#include "stdafx.h"
#include "Converter.h"
#include "MenuWnd.h"
#include "MainFrm.h"
#include "MenuTemplateEx.h"

#define MENU_BG_COLOR  RGB(0,0,0)

#ifdef _MENU_TEMPLATE_VIEW
// CMenuWnd
IMPLEMENT_DYNAMIC(CMenuWnd, CWnd)
IMPLEMENT_SINGLETON(CMenuWnd)
CMenuWnd::CMenuWnd():
m_pTemplateEx(NULL)
{
	
}

CMenuWnd::~CMenuWnd()
{
	::DeleteObject(m_hImageCursor);
}


BEGIN_MESSAGE_MAP(CMenuWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CMenuWnd message handlers
BOOL CMenuWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMenuWnd::CreateEx(0, dwStyle, rect, pParentWnd, nID);
}

BOOL CMenuWnd::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW), ::GetSysColorBrush(COLOR_WINDOW), NULL);
	dwStyle |= WS_CLIPCHILDREN;
	return CWnd::CreateEx(dwExStyle, lpszClass, "CMenuWnd", dwStyle, rect, pParentWnd, nID, NULL);
}

int CMenuWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_brBackGround.CreateSolidBrush(MENU_BG_COLOR);
	m_pTemplateEx = CMenuTemplateEx::Instance();
	m_hImageCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_MY_HAND));

	m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_tooltip.SetMaxTipWidth(100);
	m_tooltip.SetDelayTime(TTDT_AUTOPOP, 15 * 1000);

	return 0;
}

BOOL CMenuWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CMenuWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(&bmp);
	//1.填充背景颜色
    dcMem.FillRect(&rc,&m_brBackGround);
    //2.绘制菜单背景
	m_pTemplateEx->DrawBG(&dcMem,rc);
	//3.绘制菜单上的按钮（video button,page button）
	if(m_pTemplateEx->EnableMenu())
	{
		CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage();
		if(pCurPage!=NULL)
			pCurPage->OnDraw(&dcMem,rc);
	}

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
}

BOOL bIn = FALSE;
void CMenuWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	 //TODO: Add your message handler code here and/or call default
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		pCurPage->OnMouseMove(nFlags, point);
		return ;
	}
}

void CMenuWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		pCurPage->OnLButtonDown(nFlags, point);
		return ;
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CMenuWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		pCurPage->OnLButtonUp(nFlags, point);
		return ;
	}
	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CMenuWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		if(pCurPage->OnSetCursor(nHitTest,point))
		{
			SetCursor(m_hImageCursor);	
			return TRUE;
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMenuWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here
	/*CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL)
	{
		pCurPage->OnKillFocus();
	}*/
}


BOOL CMenuWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}


BOOL CMenuWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		m_tooltip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}


void CMenuWnd::OnMouseInBtn(int index)
{
	
}

void CMenuWnd::OnMouseOverBtn(int index,CPoint point)
{
	static int s_nOverIndex = NULL_BTN;
	CMenuPage* pCurPage = CMenuList::Instance()->GetViewPage(FALSE);
	if(pCurPage!=NULL )
	{
		if(s_nOverIndex!=index)
		{
			s_nOverIndex = index;
			m_tooltip.Pop();
			m_tooltip.DelTool(this, 1);

			CString strText;
			CRect rcToolTip;
			if(pCurPage->GetToolTipText(index, strText,rcToolTip))
			{
				TOOLINFO ti;
				memset(&ti, 0, sizeof(ti));
				ti.cbSize = sizeof(ti);
				ti.hinst = 0;
				ti.hwnd = m_hWnd;
				ti.uFlags = TTF_CENTERTIP| TTF_TRANSPARENT;
				ti.lpszText = (LPSTR)(LPCTSTR)strText;
				ti.uId = 1;
				ti.rect = rcToolTip;
				m_tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
				m_tooltip.Activate(TRUE);
			}
		}
	}
}

void CMenuWnd::OnClickBtn(int index)
{
	ASSERT(index!=NULL_BTN);
	BOOL bRedraw = TRUE;
	if(index==PAGE_BTN_PRE)
	{
		CMenuList::Instance()->PreviewPage(bRedraw);
		GetParent()->SendMessage(MENU_PAGE_CHANGE);

	}
	else if(index==PAGE_BTN_NEXT)
	{
		CMenuList::Instance()->NextPage(bRedraw);
		GetParent()->SendMessage(MENU_PAGE_CHANGE);
	}
	else
	{
		ASSERT(index>=0);
		CFileList::Instance()->UnselectAllItems();
		CFileList::Instance()->SelectItem(index,TRUE);
		CFileList::Instance()->EnsureVisible(index);
	}
}

void CMenuWnd::OnMouseOutBtn(int index)
{
	
}

//Profile change
IMPLEMENT_OBSERVER(CMenuWnd, SUB_PROFILE_CHANGED)
{

};

//"TV system" change--Simple
IMPLEMENT_OBSERVER(CMenuWnd, SUB_VIDEO_STANDARD_CHANGED)
{
	m_pTemplateEx->OnVStandardChange(LPCTSTR (pData));
}

IMPLEMENT_OBSERVER(CMenuWnd, SUB_VIDEO_ASPECT_CHANGED)
{
	m_pTemplateEx->OnAspectChange(LPCTSTR (pData));
}

//CMFCPropertyGridCtrl item changed--Advance
IMPLEMENT_OBSERVER(CMenuWnd, SUB_PROPERTY_GRID_CHANGED)
{
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty *)pData;
	CProfileParam *pParam = (CProfileParam *)pProp->GetData();
	if (pParam == NULL)
		return;

	if (pParam->m_strName.CompareNoCase("vstandard") == 0)
	{
		//"TV system" change
		CString strValue = pParam->GetValue();
		m_pTemplateEx->OnVStandardChange(LPCTSTR (strValue.GetBuffer()));
		strValue.ReleaseBuffer();
		return;
	}

	if (pParam->m_strName.CompareNoCase("vaspect") == 0)
	{
		//"Video Aspect" change
		CString strValue = pParam->GetValue();
		m_pTemplateEx->OnAspectChange(strValue.GetBuffer());
		strValue.ReleaseBuffer();
		return;
	}
}

//Template change
IMPLEMENT_OBSERVER(CMenuWnd, SUB_MENU_TEMPLATE_CHANGED)
{
	CMenuList::Instance()->OnMenuTemplateChange(MENU_PROP_TEMPLATES);
	if(IsWindow(m_hWnd))
		GetParent()->SendMessage(MENU_PAGE_CHANGE);
}

IMPLEMENT_OBSERVER(CMenuWnd,SUB_MENU_TEMPLATE_PARAM_CHANGED)
{
	MENU_PROP_DATA mTemplateID= *((MENU_PROP_DATA *)pData);
	CMenuList::Instance()->OnMenuTemplateChange(mTemplateID);
}

IMPLEMENT_OBSERVER(CMenuWnd,SUB_BUTTON_TEXT_CHANDED)
{
	CTask *pTask = (CTask *)pData;
	ASSERT(pTask!=NULL);
	CMenuList::Instance()->OnButtonTextChange(pTask);
}

//File list change
IMPLEMENT_OBSERVER(CMenuWnd, SUB_FILE_SEL_CHANGED)
{
	CMenuList::Instance()->OnFileSelectedChange();
}

//最后CFileList的删除操作会调用
IMPLEMENT_OBSERVER(CMenuWnd, SUB_FILE_LIST_CHANGED)
{
	CMenuList::Instance()->OnFileListChange();
	if(IsWindow(m_hWnd))
		GetParent()->SendMessage(MENU_PAGE_CHANGE);
}

#endif

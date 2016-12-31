// ProfileGroupListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ProfileGroupListWnd.h"
#include "ProfileManager.h"
#include "MSMVisualManager.h"
#include "Graphics.h"

// CProfileGroupListWnd

CProfileGroupListWnd::CProfileGroupListWnd()
{
	m_pWndBuddy = NULL;

	SetItemHeight(PROFILE_ICON_SIZE + PROFILE_ICON_MARGIN * 2);
	SetScrollerHeight((PROFILE_ICON_SIZE + PROFILE_ICON_MARGIN * 2) * 2 / 5);
}

CProfileGroupListWnd::~CProfileGroupListWnd()
{
}


BEGIN_MESSAGE_MAP(CProfileGroupListWnd, CScrollList)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CProfileGroupListWnd message handlers

void CProfileGroupListWnd::LoadProfileGroups()
{
	DeleteAllItems();

	for (int i = 0; i < (int)CProfileManager::Instance()->m_groups.size(); i++)
	{
		CProfileGroup *pGroup = CProfileManager::Instance()->m_groups[i];
		InsertItem(i, (LPARAM)pGroup);
	}
}

int CProfileGroupListWnd::SelectProfileGroup(CProfileGroup *pGroup)
{
	// force reload current group (for customized group change)
	SetSelIndex(-1);

	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemData(i) == (LPARAM)pGroup)
		{
			SetSelIndex(i);
			return i;
		}
	}

	ASSERT(FALSE);

	SetSelIndex(0);
	return 0;
}

void CProfileGroupListWnd::DrawBackground(CDC *pDC, const CRect &rc)
{
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_WINDOW));
}

void CProfileGroupListWnd::DrawItem(CDC *pDC, int index, const CRect &rc)
{
	CProfileGroup *pGroup = (CProfileGroup *)GetItemData(index);
	ASSERT(pGroup != NULL);

	CMSMVisualManager *pManager = (CMSMVisualManager *)CMFCVisualManager::GetInstance();

	BOOL bHilight;
	
	if (GetHoverIndex() == -1)
		bHilight = (index  == GetSelIndex());
	else
		bHilight = (index  == GetHoverIndex());

	CRect rcItem = rc;
	rcItem.InflateRect(-1, -1);

	if (bHilight)
	{
		pManager->m_ctrlMenuHighlighted[0].Draw(pDC, rcItem);
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(bHilight ? pManager->m_clrMenuTextHighlighted : pManager->m_clrMenuText);

	CRect rcIcon = rcItem;
	rcIcon.right = rcIcon.left + PROFILE_ICON_SIZE;
	DrawIconEx(*pDC, rcIcon.left + (rcIcon.Width() - 24) / 2, rcIcon.top + (rcIcon.Height() - 24) / 2, pGroup->m_hIcon, 24, 24, 0, NULL, DI_NORMAL);

	CRect rcText = rcItem;
	rcText.left += rcIcon.right;
	rcText.right -= CMenuImages::Size().cx * 2;

	CFont *pOldFont = pDC->SelectObject(&afxGlobalData.fontBold);
	CRect rcName = rcText;
	rcName.bottom = (rcText.bottom + rcText.top) / 2;
	pDC->DrawText(pGroup->m_strName, &rcName, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

	pDC->SelectObject(&afxGlobalData.fontRegular);
	CRect rcDesc = rcText;
	rcDesc.top = (rcText.bottom + rcText.top) / 2;
	pDC->DrawText(pGroup->m_strDesc, &rcDesc, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOPREFIX | DT_END_ELLIPSIS);

	pDC->SelectObject(pOldFont);

	CRect rcArrow = rcItem;
	rcArrow.left = rcArrow.right - CMenuImages::Size().cx * 2;
	CMenuImages::Draw (pDC, CMenuImages::IdArrowRightLarge, rcArrow);

	if (bHilight && GetFocus() == this)
	{
		rcItem.InflateRect(-2, -2);
		pDC->DrawFocusRect(&rcItem);
	}
}

void CProfileGroupListWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if (nChar == VK_RIGHT && m_pWndBuddy != NULL)
	{
		m_pWndBuddy->SetFocus();
		m_pWndBuddy->SendMessage(WM_KEYDOWN, nChar, MAKELPARAM(nRepCnt, nFlags));
	}

	CScrollList::OnKeyDown(nChar, nRepCnt, nFlags);
}

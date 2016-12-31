// ProfileGroupListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "ProfileItemListWnd.h"
#include "ProfileManager.h"
#include "MSMVisualManager.h"
#include "Graphics.h"

// CProfileItemListWnd

CProfileItemListWnd::CProfileItemListWnd()
{
	m_pWndBuddy = NULL;
	m_pGroup = NULL;

	SetItemHeight(PROFILE_ICON_SIZE + PROFILE_ICON_MARGIN * 2);
	SetScrollerHeight((PROFILE_ICON_SIZE + PROFILE_ICON_MARGIN * 2) * 2 / 5);
}

CProfileItemListWnd::~CProfileItemListWnd()
{
}


BEGIN_MESSAGE_MAP(CProfileItemListWnd, CScrollList)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CProfileItemListWnd message handlers


void CProfileItemListWnd::LoadProfileItems(CProfileGroup *pGroup)
{
	DeleteAllItems();

	m_pGroup = pGroup;

	for (int i = 0; i < (int)pGroup->m_items.size(); i++)
	{
		CProfileItem *pItem = pGroup->m_items[i];
		InsertItem(i, (LPARAM)pItem);
	}

	SetHoverIndex(-1);
	SetSelIndex(-1);
	SetTopIndex(0);
}

int CProfileItemListWnd::SelectProfileItem(CProfileItem *pItem)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetItemData(i) == (LPARAM)pItem)
		{
			SetHoverIndex(i);
			SetSelIndex(i);
			SetTopIndex(i > 0 ? i - 1 : i);
			return i;
		}
	}

	SetHoverIndex(-1);
	SetSelIndex(-1);
	SetTopIndex(0);

	return -1;
}

void CProfileItemListWnd::DrawBackground(CDC *pDC, const CRect &rc)
{
	pDC->FillSolidRect(&rc, ::ChangeVValue(::GetSysColor(COLOR_WINDOW), 0.95));

	if (m_pGroup != NULL && m_pGroup == CProfileManager::Instance()->GetCustomizedGroup() && GetItemCount() == 0)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(afxGlobalData.clrGrayedText);

		CRect rcText = rc;
		rcText.InflateRect(-10, -10);

		CFont *pOldFont = pDC->SelectObject(&afxGlobalData.fontRegular);
		pDC->DrawText("No customized profiles available. After modifying the settings of an existing profile, you can click the \"Save as New Profile\" button above to create customized profiles.", 
			&rcText, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);

		pDC->SelectObject(pOldFont);
	}
}

void CProfileItemListWnd::DrawItem(CDC *pDC, int index, const CRect &rc)
{
	CProfileItem *pItem = (CProfileItem *)GetItemData(index);
	ASSERT(pItem != NULL);

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

	CRect rcText = rcItem;
	rcText.left += PROFILE_TEXT_MARGIN;
	rcText.right -= PROFILE_TEXT_MARGIN;

	CFont *pOldFont = pDC->SelectObject(&afxGlobalData.fontBold);
	CRect rcName = rcText;
	rcName.bottom = (rcText.bottom + rcText.top) / 2;
	pDC->DrawText(pItem->m_strName, &rcName, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

	pDC->SelectObject(&afxGlobalData.fontRegular);
	CRect rcDesc = rcText;
	rcDesc.top = (rcText.bottom + rcText.top) / 2;
	pDC->DrawText(pItem->m_strDesc, &rcDesc, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOPREFIX | DT_END_ELLIPSIS);

	if (bHilight && GetFocus() == this)
	{
		rcItem.InflateRect(-2, -2);
		pDC->DrawFocusRect(&rcItem);
	}

	pDC->SelectObject(pOldFont);
}

void CProfileItemListWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar)
	{
		case VK_RIGHT:
		{
			if (GetItemCount() > 0 && GetHoverIndex() == -1)
			{
				SetHoverIndex(0, FALSE);
				SetSelIndex(0, TRUE);
				SetTopIndex(0);
			}
			break;
		}

		case VK_LEFT:
		{
			if (m_pWndBuddy != NULL)
				m_pWndBuddy->SetFocus();
			break;
		}

		case VK_RETURN:
		{
			int index = GetSelIndex();
			if (index >= 0)
			{
				CRect rc;
				GetItemRect(index, &rc);
				CPoint point = rc.CenterPoint();
				ClientToScreen(&point);
				GetParent()->ScreenToClient(&point);
				GetParent()->SendMessage(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
			}
			break;
		}
	}

	CScrollList::OnKeyDown(nChar, nRepCnt, nFlags);
}

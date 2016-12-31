// DlgDiskCost.cpp : implementation file
//

#include "stdafx.h"
#include "Converter.h"
#include "DlgDiskCost.h"


// CDlgDiskCost dialog

IMPLEMENT_DYNAMIC(CDlgDiskCost, CDialog)

CDlgDiskCost::CDlgDiskCost(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDiskCost::IDD, pParent)
{

}

CDlgDiskCost::~CDlgDiskCost()
{
}

void CDlgDiskCost::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CDlgDiskCost, CDialog)
END_MESSAGE_MAP()


// CDlgDiskCost message handlers

BOOL CDlgDiskCost::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_list.InsertColumn(0, LoadStringInline(IDS_COL_VOLUME), LVCFMT_LEFT, 60);
	m_list.InsertColumn(1, LoadStringInline(IDS_COL_TOTAL_SIZE), LVCFMT_RIGHT, 120);
	m_list.InsertColumn(2, LoadStringInline(IDS_COL_FREE_SPACE), LVCFMT_RIGHT, 120);

	if (m_imgDisk.Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0))
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_DISK);
		m_imgDisk.Add(&bmp, RGB(255, 0, 255));
		m_list.SetImageList(&m_imgDisk, LVSIL_SMALL);
	}

	DWORD dwDrives = ::GetLogicalDrives();
	int j = 0;
	for (int i = 0; i < 26; i++)
	{
		if ((dwDrives & ((DWORD)1 << i)) != 0)
		{
			CString strDrive;
			strDrive.Format("%C:", 'A' + i);
			if (::GetDriveType(strDrive) == DRIVE_FIXED)
			{
				ULARGE_INTEGER nTotalBytes;
				ULARGE_INTEGER nFreeBytes;
				if (::GetDiskFreeSpaceEx(strDrive, &nFreeBytes, &nTotalBytes, NULL))
				{
					int index = m_list.InsertItem(j, strDrive, 0);
					if (index >= 0)
					{
						m_list.SetItemText(j, 1, GetFileSizeString((INT64)nTotalBytes.QuadPart));
						m_list.SetItemText(j, 2, GetFileSizeString((INT64)nFreeBytes.QuadPart));
						j++;
					}
				}
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

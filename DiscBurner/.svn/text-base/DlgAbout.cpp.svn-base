// DlgAbout.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog


CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAbout)
	//}}AFX_DATA_INIT
}


void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAbout)
	DDX_Control(pDX, IDC_STATIC_PRODUCT, m_stcProduct);
	DDX_Control(pDX, IDC_STATIC_BUILD, m_stcBuild);
	DDX_Control(pDX, IDC_STATIC_WEBSITE, m_stcWebsite);
	DDX_Control(pDX, IDC_STATIC_SUPPORT, m_stcSupport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	//{{AFX_MSG_MAP(CDlgAbout)
	ON_BN_CLICKED(IDC_STATIC_SUPPORT, OnSupport)
	ON_BN_CLICKED(IDC_STATIC_WEBSITE, OnWebsite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout message handlers

BOOL CDlgAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strProduct;
	strProduct.LoadString(AFX_IDS_APP_TITLE);

	TCHAR szModule[_MAX_PATH + 1];
	::GetModuleFileName(NULL, szModule, _MAX_PATH);
	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(szModule, &dwHandle);
	if (dwSize > 0)
	{
		unsigned char *pData = new unsigned char[dwSize];
		if (pData)
		{
			if (::GetFileVersionInfo(szModule, 0, dwSize, pData))
			{
				void *pBuffer = NULL;
				UINT nLen = 0;
				if (::VerQueryValue(pData, "\\", &pBuffer, &nLen))
				{
					VS_FIXEDFILEINFO *pvi = (VS_FIXEDFILEINFO *)pBuffer;
					DWORD dwMajor = pvi->dwProductVersionMS;
					
					if (::VerQueryValue(pData, "\\VarFileInfo\\Translation", &pBuffer, &nLen))
					{
						DWORD dwCodePage = (*(DWORD *)pBuffer) << 16 | (*(DWORD *)pBuffer) >> 16;

						CString strSub;
						strSub.Format("\\StringFileInfo\\%08x\\PrivateBuild", dwCodePage);
						if (::VerQueryValue(pData, strSub.GetBuffer(), &pBuffer, &nLen))
						{
							CString strVersion;
							strVersion.Format(" %d.%d", HIWORD(dwMajor), LOWORD(dwMajor));
							strProduct += strVersion;

							CString strBuild;
							strBuild.Format("Build: %s", (char *)pBuffer);

							m_stcBuild.SetWindowText(strBuild);
						}
					}
				}
			}
			delete pData;
		}
	}

	m_stcProduct.SetWindowText(strProduct);
	m_stcProduct.SetFontSize(11);
	m_stcProduct.SetFontBold(TRUE);

	m_stcWebsite.ModifySTStyle(0, STS_AUTOSIZE);
	m_stcSupport.ModifySTStyle(0, STS_AUTOSIZE);

	HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_HAND));
	m_stcWebsite.SetHoverCursor(hCursor);
	m_stcSupport.SetHoverCursor(hCursor);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAbout::OnSupport() 
{
	// TODO: Add your control notification handler code here

	CString strSupport;
	strSupport.LoadString(IDS_SUPPORT_URL);
	ShellExecute(m_hWnd, NULL, strSupport, NULL, NULL, SW_SHOWNORMAL);
}

void CDlgAbout::OnWebsite() 
{
	// TODO: Add your control notification handler code here
	
	CString strWebsite;
	strWebsite.LoadString(IDS_URL);
	ShellExecute(m_hWnd, NULL, strWebsite, NULL, NULL, SW_SHOWNORMAL);
}



#pragma once

// CDlgProgress dialog

class CDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgProgress)

public:
	CDlgProgress(CWnd* pParent, LPCTSTR lpszCaption, LPCTSTR lpszPrompt);   // standard constructor
	virtual ~CDlgProgress();

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	CString m_strCaption;
	CString m_strPrompt;

	CStatic m_stcPrompt;
	CProgressCtrl m_progress;
	HANDLE m_hEvent;
	BOOL m_bCanceled;
	BOOL m_bAllowCancel;

public:
	CProgressCtrl &GetProgressCtrl() {return m_progress;}
	void WaitForInitialize();
	void AllowCancel(BOOL bAllow);
	BOOL IsCanceled() {return m_bCanceled;}
	void End(UINT nIDResult);
	void SetPrompt(LPCTSTR lpszPrompt);

protected:
	virtual void OnCancel();
	virtual void OnOK();
	LRESULT OnProgressMessage(WPARAM wp, LPARAM lp);
};

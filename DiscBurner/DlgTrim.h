#pragma once

#include "ResizableDialog.h"
#include "VideoSliderCtrl.h"
#include "VideoPlayerWnd.h"
#include "AVIReader.h"

class CTask;

// CDlgTrim dialog

class CDlgTrim : public CResizableDialog
{
public:
	CDlgTrim(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrim();

// Dialog Data
	enum { IDD = IDD_TRIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_stcPreview;
	CVideoSliderCtrl m_wndSlider;
	CTask *m_pTask;
	HBITMAP m_hBmpPreview;
	CVideoPlayerWnd m_wndPlayer;
	CString m_strAVSFile;
	CAVIFileReader m_aviReader;
	LONG m_lSample;

	BOOL InitVideoPlayer();
	void RepositionControls();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnSliderPosChanged();
	afx_msg void OnSliderSelChanged();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

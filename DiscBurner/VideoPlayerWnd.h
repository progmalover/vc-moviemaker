#pragma once

#include <dshow.h>
#include <reftime.h>

extern UINT WM_GRAPHNOTIFY;

const int State_None			= (State_Stopped - 1);	// Media file is not opened yet

/*
typedef enum _FilterState
{
	State_Stopped	= 0,
	State_Paused	= ( State_Stopped + 1 ) ,
	State_Running	= ( State_Paused + 1 ) 
}
FILTER_STATE;
*/

// CVideoPlayerWnd

class CVideoPlayerWnd : public CWnd
{
	DECLARE_DYNAMIC(CVideoPlayerWnd)

public:
	CVideoPlayerWnd();
	virtual ~CVideoPlayerWnd();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetBackgroundColor(COLORREF clrBackground);
	void SetBitmap(HBITMAP hBitmap, BOOL bStretch);
	BOOL OpenFile(LPCTSTR lpszFile);
	void CloseFile();
	OAFilterState WaitForStateTransition();
	void Play();
	void Pause();
	void Stop();
	void StopWhenReady();
	int GetState() { return m_nState; }
	LONGLONG GetDuration();
	LONGLONG GetPosition();
	BOOL SeekTo(LONGLONG lPostion, BOOL bFlushData);
	void SetVolume(int nVolume);
	int GetVolume() { return m_nVolume; }
	void Mute(BOOL bMute);
	BOOL IsMute() { return m_bMute; }

protected:
	DECLARE_MESSAGE_MAP()

	CComPtr<IGraphBuilder> m_pGraphBuilder;
	CComPtr<IVideoWindow> m_pVideoWindow;
	CComPtr<IMediaControl> m_pMediaControl;
	CComPtr<IMediaEventEx> m_pMediaEvent;
	CComPtr<IMediaSeeking> m_pMediaSeeking;
	CComPtr<IBasicAudio> m_pBasicAudio;

	BOOL m_bFileOpened;
	BOOL m_bAudioOnly;
	CBitmap m_bmpDefault;
	int m_nState;
	int m_nVolume;			// 0 - 100
	BOOL m_bMute;

	COLORREF m_clrBackground;
	HBITMAP m_hBitmap;
	BOOL m_bStretchBitmap;

	void Init();
	void CheckVisibility();
	void RepositionControls();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnMediaEvent(WPARAM wp, LPARAM lp);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
};

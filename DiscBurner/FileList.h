#pragma once

#include "UltraListCtrl.h"
#include "Task.h"
#include "Subjects.h"
#include "MenuTemplateManager.h"

// CFileList
//#define _TEST_PROCESS 
#define  MENU_VIDEO_BITRATE    5000  //K 
#define  MENU_AUDIO_BITRATE     192  //K 
#define  NOTICE_VIDEO_BITRATE  5000  // K 
#define  NOTICE_AUDIO_BITRATE  192   // K 
#define  NOTICE_DURATION         8   // 200/25

#ifdef _BD
#define  TSMUXER_BITRATE        160  //K  --- 1s:20K,1h:72M 
//160K只是一个合理的值，目前确定的范围在[120K，220K]之间，与输出视频的大小等有关系;
#define  FIRST_MPLS             200
#else
#define  TSMUXER_BITRATE        24  //K  --- 1s:3K,1h:10.8M 
#endif

#ifdef _DVD
#define MIN_VIDEO_BITRATE  400
#else
#define MIN_VIDEO_BITRATE  600
#endif

//#define  _TEST_BD_MENU
class CFileList : public CUltraListCtrl
{
public:
	CFileList();
	virtual ~CFileList();
	DECLARE_SINGLETON(CFileList)

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_OBSERVER(CFileList, SUB_DISC_TYPE_CHANGED)
	DECLARE_OBSERVER(CFileList, SUB_VIDEO_DIMENSION_CHANGED)
	DECLARE_OBSERVER(CFileList, SUB_VIDEO_FRAME_RATE_CHANGED)
	DECLARE_OBSERVER(CFileList, SUB_PROPERTY_GRID_CHANGED)
	DECLARE_OBSERVER(CFileList, SUB_VIDEO_TRIMMED)
	DECLARE_OBSERVER(CFileList, SUB_MENU_TEMPLATE_CHANGED)

	virtual void DrawItem(CDC *pDC, int index, const CRect *pRect);
	virtual COLORREF GetSelectionRectColor();
	virtual BOOL GetToolTipText(int index, CString &strText);
	virtual void GetCheckBoxRect(int nItem, RECT *pRect);

	virtual BOOL CanCheckItem(int index, BOOL bCheck);
	virtual void OnCheckItem(int index, BOOL bCheck);

	HACCEL m_hAccel;
	CImageList m_imgList;
	BOOL m_bInit;
	CTask *m_pCurTask;
	BOOL m_bDestroy;
	BOOL m_bAddingFiles;
	BOOL m_bConverting;
	BOOL m_bPaused;
	BOOL m_bUpdateStatusTimerEnabled;
	BOOL m_bDrawFocusRect;
	CSize m_sizeThumbnail;
	int m_nRunningTasks;
	BOOL m_bBurnDisc;

#ifdef _BD
	void PostProcess();
#elif defined _DVD
	void PostProcess(CString &strTempFolder);
	BOOL CreateNoticeVideo(LPCTSTR lpszTempFolder);
	BOOL CreateDVDAuthorXML(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder);
	BOOL CreateDVDAuthorXML2(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder);
	BOOL CreateDVDAuthorXML3(std::vector <CTask *> &taskList, std::vector<BUTTON> &buttonList, BOOL bEnableDVDMenu, LPCTSTR lpszTempFolder);
#endif

	void DrawThumbnail(CDC *pDC, int nItem, const LPRECT pRect);
	void DrawProgress(CDC *pDC, int nItem, const LPRECT pRect);
	void OnResetColumns();

	void EnableUpdateStatusTimer(BOOL bEnable) {m_bUpdateStatusTimerEnabled = bEnable;}

	void ShutDownWindows();

	static DWORD AddFileThread(LPVOID lpParameter);
	static DWORD EncodingThread(LPVOID lpParameter);
	static DWORD EncodingThreadImpl(LPVOID lpParameter);
	static DWORD AddToiTunesThread(LPVOID lpParameter);

public:
	void Init();
	void SetThumbnailSize(int width, int height);
	CSize GetThumbnailSize() {return m_sizeThumbnail;}
	void UpdateThumbnailColumnWidth();

	virtual int GetSubItemIdealWith(int nItem);
	void SetReportView(BOOL bReport);
	BOOL CanAddFiles();

	LONGLONG m_nStartTick;
	LONGLONG m_nPauseTick;

	void Start(CTask *pTask);
	void Start(int index);
	void Stop(int index);
	void Delete(int index,BOOL bNotify = TRUE);
	void Open(int index);

	void CheckQueue();
	void StopTasks(CTask **pTasks, int nCount);

	void QueueSelected();
	void StopSelected();
	void DeleteSelected();
	void OpenSelected();

	void QueueAll();
	void PauseAll();
	void ResumeAll();
	void StopAll();

	CTask *ParseFile(LPCTSTR lpszFile);
	int AddTask(CTask *pTask);
	void AddFiles(std::vector<std::string> &list);

	void UpdateStatus();
	void LoadTasks();
	CTask *GetCurTask();
	void LoadCurLog();
	void SaveColumnState();
	void RestoreColumnState();

	BOOL IsConverting() {return m_bConverting;}
	BOOL IsPaused() {return m_bPaused;}
	void OnTaskCompletedAll();

	double GetTotalDuration();
	void UpdateSummary();

	UINT MenusDuration();

	BOOL MakeFileList();
	BOOL FastTrim();
	void RemoveOutputFiles(BOOL bDeleteFile);
public:
	afx_msg LRESULT OnTaskAdd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskFailed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskCanceled(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnDeleteItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnUlnSelChanged();
	afx_msg void OnUlnDblClk();
	afx_msg void OnHdnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg void OnFileAdd();
	afx_msg void OnFileRemove();
	afx_msg void OnFileRemoveAll();
	afx_msg void OnFileMoveUp();
	afx_msg void OnFileMoveDown();
	afx_msg void OnFileConvert();
	afx_msg void OnFilePause();
	afx_msg void OnFileStop();
	afx_msg void OnFilePreview();
	afx_msg void OnFilePlay();
	afx_msg void OnFileProperties();
	afx_msg void OnFileTrim();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateFileAdd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileRemove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileRemoveAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileMoveUp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileMoveDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileConvert(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileStop(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePreview(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileProperties(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileTrim(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI *pCmdUI);
	afx_msg void OnFileOpenSourceFolder();
	afx_msg void OnUpdateFileOpenSourceFolder(CCmdUI *pCmdUI);
	afx_msg void OnFileAddFolder();
	afx_msg void OnUpdateFileAddfolder(CCmdUI *pCmdUI);
};
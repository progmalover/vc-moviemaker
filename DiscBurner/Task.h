#pragma once

#include <list>
#include <vfw.h>

#include "MediaInfo.h"
#include "Encoder.h"
#include "vector"

using namespace std;

class CTask;
class CProfileGroup;
class CProfileItem;

#define TRIAL_TIME_LIMIT			(60 * 5)	// seconds

const UINT MIN_THUMBNAIL_WIDTH		= 24;
const UINT MIN_THUMBNAIL_HEIGHT		= 18;
const UINT MAX_THUMBNAIL_WIDTH		= 160;
const UINT MAX_THUMBNAIL_HEIGHT		= 120;
const UINT DEF_THUMBNAIL_WIDTH		= 64;
const UINT DEF_THUMBNAIL_HEIGHT		= 42;

const float MIN_FRAMERATE			= 1.0f;
const float MAX_FRAMERATE			= 30.0f;
const float DEF_FRAMERATE			= 25.0f;

extern UINT WM_TASK_ADD;
extern UINT WM_TASK_COMPLETED;
extern UINT WM_TASK_FAILED;
extern UINT WM_TASK_CANCELED;

const DWORD UPDATE_SOURCE_FILE		= 0x00000001L << 0;
const DWORD UPDATE_DIMENSION		= 0x00000001L << 1;
const DWORD UPDATE_DURATION			= 0x00000001L << 2;
const DWORD UPDATE_STATUS			= 0x00000001L << 3;
const DWORD UPDATE_LOG				= 0x80000000L;

const UINT UPDATE_ALL				= UPDATE_SOURCE_FILE | 
									  UPDATE_DIMENSION | 
							 		  UPDATE_DURATION | 
							 		  UPDATE_STATUS | 
									  UPDATE_LOG;


enum CONV_STATUS
{
	CONV_STATUS_NONE = 0, 
	CONV_STATUS_WAITING, 
	CONV_STATUS_CONVERTING, 
	CONV_STATUS_COMPLETED, 
	CONV_STATUS_FAILED, 
	CONV_STATUS_CANCELED, 
};

struct CONV_PROC_PARAM
{
	HANDLE hEvent;
	CTask *pTask;
};

enum LOG_TYPE
{
	LOG_INFORMATION, 
	LOG_WARNING, 
	LOG_ERRO, 
};

class CLogItem
{
public:
	CLogItem(LOG_TYPE nType)
	{
		m_nType = nType;
		m_time = time(0);
	}
	LOG_TYPE m_nType;
	time_t m_time;
	CString m_strLog;
};

typedef std::list<CLogItem *> LOG_LIST;

struct taskAtom
{
	CString avs;
	CString cmd;
	CString outfile;
	CString ext;

	double starttime;//starttime 仅作进度计算之用，即使进行了 Trim 操作，也从 0 开始
	__int64 startframe;
	__int64 endframe;
	BOOL  novideo;
};

#ifdef _BD
#define AC3_AFTEN 
#endif

#ifdef AC3_AFTEN 
extern float  g_fEncoderAC3;
#endif

class CTask
{
public:
	CTask(LPCTSTR lpszFile);
	virtual ~CTask(void);

	BOOL Lock();
	void Unlock();

	BOOL IsResumed();
	BOOL IsPaused();
	BOOL IsCanceled();

	BOOL FindSubtitle(CString &strSubtitle);

	BOOL CreateTempFolder();
	const CString &GetTempFolder() {return m_strTempFolder;}

	BOOL PrepareThumbnail();
	BOOL PrepareConversion();
	void ClearTasks();
	void DeleteFiles();

	CString GetMediaInfo();

	static DWORD WINAPI ConvProc(PVOID lp);
	void Pause();
	void Resume();
	void Cancel();

	LPTHREAD_START_ROUTINE m_lpfnConvProc;
	HANDLE m_hPauseEvent;
	HANDLE m_hResumeEvent;
	HANDLE m_hCancelEvent;

	HANDLE m_hPauseEvent2;
	HANDLE m_hResumeEvent2;

	BOOL m_bCanceled;
	BOOL m_bPaused;


	vector<taskAtom> m_Atoms;
	taskAtom *m_pCurAtom;
	double m_fDuration;//转出文件的总长度

protected:
	CRITICAL_SECTION m_csTask;
	CString m_strTempFolder;

// media info
public:
	CString m_strSourceFile;
	CString m_strSourceFileAlt;
	CString m_strOutputName;
	CString m_strPreview;
	CBitmap m_bmpFirstFrame;	// Get from AviSynth, first frame
	CBitmap m_bmpNonBlankFrame;	// Get from AviSynth, first non-blank frame
	static CBitmap m_bmpAudio;

	union
	{
		double m_fEncodedTime;		// for FFmpeg
		double m_fEncodedPercent;	// for tsMuxeR
	};

	BOOL m_bTrimmed;
	double m_fTrimStart;
	double m_fTrimEnd;

	CMediaInfo m_mi;
	CEncoder *m_pEncoder;

	CString m_strAVSPreview;
	//CString m_strAVSEncoding;

public:
	CONV_STATUS m_nStatus;
	UINT m_nUpdate;

	HANDLE m_hConvThread;

	LONGLONG m_nStartTick;
	LONGLONG m_nPauseTick;

	//LOG_LIST m_oOldLog;
	//LOG_LIST m_oNewLog;

#ifdef _DEBUG
	DWORD m_dwLock;
	DWORD m_dwTlsIndex;
#endif
	CString m_strButtonText;

#ifdef AC3_AFTEN
private:
	float m_fVideoStream;
	BOOL AudioAtom(vector<taskAtom> *pAtoms);
#endif
};

#ifdef _BD
    extern BOOL g_bIsActivated;
#endif


BOOL isNTFSVolume(CString PathName);
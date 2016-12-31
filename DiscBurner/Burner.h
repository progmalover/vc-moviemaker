#pragma once

#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>

#ifdef _DVD
#include "CDVDDevice.h"
#endif // _DVD

#include "DiscFormatEraseEvent.h"
#include "FileSystemImageEvent.h"
#include "DiscFormatDataEvent.h"

#ifdef _BD
#define MAX_DISC_LABEL 64
#elif defined _DVD
#define MAX_DISC_LABEL 32
#endif


extern UINT WM_BURN;

//注意程序中直接用到了项的数值
enum
{
	DISC_CAP_UNKNOWN = -1, 
	DISC_CAP_DVD5 = 0, 
	DISC_CAP_DVD9 = 1, 
#ifdef _BD
	DISC_CAP_BD25 = 2, 
	DISC_CAP_BD50 = 3
#endif
};

enum
{
	BURN_START, 
	BURN_ERASE_BEGIN_EVENT,
	BURN_ERASE_END_EVENT,
	BURN_ERASE_EVENT,
	BURN_IMAGE_EVENT, 
	BURN_DATA_EVENT, 
	BURN_SUCCEEDED, 
	BURN_FAILED,
	BURN_WRITE_PORGRESS,
	BURN_ERASE_PROGRESS,
	BURN_PAD_PROCESS,
	BURN_CLOSE_PROCESS,
};

class DEVICE
{
public:
	CString strUniqueID;
	CString strName;
	CString strDrive;

	bool operator< (DEVICE &dev)
	{
		return strDrive < dev.strDrive;
	}
};

struct MediaInfo
{
	IMAPI_MEDIA_PHYSICAL_TYPE MediaType;
	BOOL isBlank;
	double MediaSize;
	double MediaFreeSize;
	CString MediaLabel;
};

typedef std::list <DEVICE> DEVICE_LIST;

class CBurner
{
public:
	CBurner(void){;}
	virtual ~CBurner(void){;}

	virtual BOOL Open()=0;
	virtual void Close()=0;
	virtual BOOL IsOpen()=0;

	virtual long GetTotalDevices()=0;
	virtual void GetBurnerList(DEVICE_LIST &devices)=0;

	virtual BOOL GetMediaInfo(LPCTSTR lpszUniqueID, MediaInfo &Info){return 0;}
	LPCTSTR GetMediaTypeString(IMAPI_MEDIA_PHYSICAL_TYPE type);
	UINT GetDiscCapaticy(IMAPI_MEDIA_PHYSICAL_TYPE type);


	virtual BOOL CheckDiscStatus(LPCTSTR lpszUniqueID, BOOL &bEraseNeeded)=0;
	virtual BOOL Erase(LPCTSTR lpszUniqueID, BOOL bFull, HWND hWndNotify)=0;
	virtual BOOL Burn(LPCTSTR lpszUniqueID, LPCTSTR lpszDir, LPCTSTR lpszTempDir, LPCTSTR lpszLabel, HWND hWndNotify)=0;
	
	virtual void Cancel()=0;
	virtual BOOL IsCanceled()=0;
	virtual CString GetErrorMessage()=0;
};


class CMsBurner : public CBurner
{
public:
	DECLARE_SINGLETON(CMsBurner)

public:
	CMsBurner(void);
	~CMsBurner(void);

protected:
	void Init();

public:
	BOOL Open();
	void Close();
	BOOL IsOpen() {return m_bOpen; }
	long GetTotalDevices();
	void GetBurnerList(DEVICE_LIST &devices);

	BOOL GetMediaInfo(LPCTSTR lpszUniqueID, MediaInfo &Info);
	BOOL CheckDiscStatus(LPCTSTR lpszUniqueID, BOOL &bEraseNeeded);

	BOOL Erase(LPCTSTR lpszUniqueID, BOOL bFull, HWND hWndNotify);
	BOOL Burn(LPCTSTR lpszUniqueID, LPCTSTR lpszDir, LPCTSTR lpszTempDir, LPCTSTR lpszLabel, HWND hWndNotify);
	void Cancel();
	BOOL IsCanceled() { return m_bCancel; }
	CString GetErrorMessage() { return m_strError; }

protected:
	BOOL m_bOpen;
	CComPtr <IDiscMaster2> m_pMaster;
	CComPtr<IDiscFormat2Data> m_pData;

	HWND m_hWndNotify;
	BOOL m_bCancel;
	CString m_strError;

	BOOL GetDeviceUniqueID(long index, CString &strUniqueID);
	BOOL GetSupportedProfiles(LPCTSTR lpszUniqueID, SAFEARRAY **ppProfiles);
	BOOL GetDeviceDisplayName(LPCTSTR lpszUniqueID, CString &strName, CString &strDrive);
	BOOL Erase(IDiscRecorder2 *pRecorder, BOOL bFull, HWND hWndNotify);
};

#ifdef _DVD
class CSBBurner : public CBurner
{
public:
	DECLARE_SINGLETON(CSBBurner)

	CSBBurner();
	~CSBBurner();

	BOOL Open();
	void Close();
	BOOL IsOpen() {return m_bOpen; }
	long GetTotalDevices();
	void GetBurnerList(DEVICE_LIST &devices);

	BOOL GetMediaInfo(LPCTSTR lpszUniqueID, MediaInfo &Info);
	BOOL CheckDiscStatus(LPCTSTR lpszUniqueID, BOOL &bEraseNeeded);

	BOOL Erase(LPCTSTR lpszUniqueID, BOOL bFull, HWND hWndNotify);
	BOOL Burn(LPCTSTR lpszUniqueID, LPCTSTR lpszDir, LPCTSTR lpszTempDir, LPCTSTR lpszLabel, HWND hWndNotify);

	void Cancel();
	BOOL IsCanceled();
	CString GetErrorMessage() { return m_strError; }

protected:
	BOOL m_bOpen;
	CString m_strError;

	CDevice *GetDevice(LPCTSTR lpszUniqueID);
	IMAPI_MEDIA_PHYSICAL_TYPE SBMediaType2IMAPIMediaType(DISC_TYPE SBType);
	void SetWriteSpeed();
	BOOL WaitDeviceReady();

public:
	CDevice *dvc;
	TSpeed spd;
};

#define CxBurner CSBBurner
#endif // _DVD

#ifdef _BD
#define CxBurner CMsBurner
#endif // _BD
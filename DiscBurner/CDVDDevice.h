 /*
 Copyright (c) Rocket Division Software 2001-2008. All rights reserved.

File Name:
                                                                                                                   
    CDVDDevice.h

Description:

    CDevice class which include all need operatios with CD/DVD devices

Author:

    Andery Trubeckoy
*/
//---------------------------------------------------------------------------

#ifndef CDVDDeviceH
#define CDVDDeviceH
#include "tchar.h"
#include "wtypes.h"
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include "StarBurn.h"
#include "Logger.h"


#define MAX_CD_TEXT_SIZE_ARTIST_SIZE 32
#define MAX_CD_TEXT_SIZE_TITLE_SIZE 32
#define CD_STANDARD_SPEED_COUNT 16
#define DVD_STANDARD_SPEED_COUNT 10
#define BLURAY_STANDARD_SPEED_COUNT 4
#define CD_TOC_SIZE_IN_MB 50   //real 15 ; def 50
#define DVD_TOC_SIZE_IN_MB 700  //real 150; def 700
#define ERASING_SPEED_CORRECTION_FACTOR 1
#define AUDIO_FILE_ONE_SECOND_STREAM_SIZE_IN_UCHARS 176400

typedef char char256[256];
typedef char char1024[1024];

// Device Status Type
enum TDeviceStatus
{
        NoAction = 0,
        Burning,
        Erasing,
        Grabbing,
        Ejecting,
		Loading,
        Canceling
};

enum SCSI_TRANSPORT
{
	SCSI_TRANSPORT_UNKNOWN = 0,
	SCSI_TRANSPORT_ASPI,
	SCSI_TRANSPORT_SPTI,
	SCSI_TRANSPORT_SPTD
};


//Type of layout of burning Audio Track
struct TAudioTrackLayout{
        char TrackName[ MAX_PATH ];
        char TrackCdTextArtist[MAX_CD_TEXT_SIZE_ARTIST_SIZE];
        char TrackCdTextTitle[MAX_CD_TEXT_SIZE_TITLE_SIZE];
};
//Type of layout of burning Audio CD
struct TAudioDiscLayout
{
        int NumberOfTracks;
        TAudioTrackLayout AudioTrackLayout[NUMBER_OF_TRACKS];
};

//Grab track info
struct TGrabTrackInfo
{
        int Number;          // Number of track in TOC
        char FullFileName[MAX_PATH];    // Grabbing track destination (full file name)

};

typedef std::vector<TGrabTrackInfo> TGrabTracksInfo;

//CD/DVD Device Speed Type
struct TSpeed
{
//public:

        ULONG SpeedKBps;  //CD/DVD device speed in KB per second
        char *SpeedX;     //CD/DVD device speed in char* for printout
};

bool operator == (TSpeed first,TSpeed second);


//CD/DVD Device Speeds Type
typedef std::vector<TSpeed> TSpeeds;

//Audio Burn Options Type
struct TBurnOptions
{
        bool OPC;
        TSpeed Speed;
        WRITE_MODE WriteMethod;
        bool TestWrite;
};
//Erase Options Type
struct TEraseOptions
        {
                ERASE_TYPE EraseType;
                TSpeed Speed;   //KBps
        };

//Device Info Type
struct TDeviceInfo
{
        char VendorID[ 1024 ];
        char ProductID[ 1024 ];
        char ProductRevisionLevel[ 1024 ];
        char DeviceLetter[3];
        unsigned long BufferSizeInUCHARs;
};
//Device Supported Media Formats Type
struct TDeviceSupportedMediaFormats
{
		bool IsTestWrite;

	    bool IsCDROMRead;
        bool IsCDRRead;
		bool IsCDRWrite;
        bool IsCDERead;
		bool IsCDEWrite;
		bool IsCDRWRead;
		bool IsCDRWWrite;				
		
        bool IsDVDROMRead;

        bool IsDVDRRead;
		bool IsDVDRWrite;

		bool IsDVDRDLRead;
		bool IsDVDRDLWrite;

		bool IsDVDRWRead;
		bool IsDVDRWWrite;
		bool IsDVDRWDLRead;
		bool IsDVDRWDLWrite;

		bool IsDVDRAMRead;
		bool IsDVDRAMWrite;

        bool IsDVDPLUSRWRead;
		bool IsDVDPLUSRWWrite;

        bool IsDVDPLUSRRead;        
        bool IsDVDPLUSRWrite;

		bool IsDVDPLUSRDLRead;
        bool IsDVDPLUSRDLWrite;

		bool IsDVDPLUSRWDLRead;
		bool IsDVDPLUSRWDLWrite;

		bool IsBDROMRead;
		bool IsBDRRead;
		bool IsBDRWrite;
		bool IsBDRDLRead;
		bool IsBDRDLWrite;
		bool IsBDRERead;
		bool IsBDREWrite;
		bool IsBDREDLRead;
		bool IsBDREDLWrite;
		bool IsHDDVDROMRead;
		bool IsHDDVDRRead;
		bool IsHDDVDRWrite;
		bool IsHDDVDRDLRead;
		bool IsHDDVDRDLWrite;
		bool IsHDDVDRWRead;
		bool IsHDDVDRWWrite;
		bool IsHDDVDRWDLRead;
		bool IsHDDVDRWDLWrite;

};





// RPC (region play code) and some additional DVD region management information Type
struct TDVDFeatures
{
	UCHAR RegionMask;
	UCHAR RPCScheme;
	UCHAR ChangesLeftVendor;
	UCHAR ChangesLeftUser;
	UCHAR TypeCode;
	BOOL  Valid;
};
//Read/Write Modes Type
struct TReadWriteModes
{
	bool IsDAOPQ;
	bool IsSAO;
	bool IsTAO;
	bool IsDAOrawPW;
	bool IsCooked;
	bool IsRaw;
	bool IsRawPQ;
	bool IsRawRawPW;
	bool IsPQ;
	bool IsRawPW;
};

//Media Info Type
struct TMediaInfo
{
        DISC_TYPE MediaType;
        double MediaSize;    //non free capacity in byte
        double MediaFreeSize; //total(non free+ free) capacity in byte
        char256 MediaLabel;
};


// CD/DVD Device Class
class CDevice
{
private:
//-----------------------private property of CDevice-----------------------\\
//SCSI Device Address
        SCSI_DEVICE_ADDRESS m_SCSIDeviceAddress;
//SCSI Device Name
        char m_InternalDeviceName[1024];
//SCSI Transport
        SCSI_TRANSPORT m_SCSITransport;
//Pointer to CdvdBurnerGrabber Object
        mutable void* m_CdvdBurnerGrabber;
//True is m_CdvdBurnerGrabber handle to device which open with exclusive access 
		bool m_IsCdvdBurnerGrabberExclusiveAccess;		
//Array of CD Standard Speeds
        static TSpeed m_CDStandardSpeeds[CD_STANDARD_SPEED_COUNT];
//Array of DVD Standard Speeds
        static TSpeed m_DVDStandardSpeeds[DVD_STANDARD_SPEED_COUNT];
//Array of Blu-Ray Standard Speeds
		static TSpeed m_BLURAYStandardSpeeds[BLURAY_STANDARD_SPEED_COUNT];
//CD/DVD Device Supported Media Formats
        TDeviceSupportedMediaFormats m_DeviceSupportedMediaFormats;
// RPC (region play code) and some additional DVD region management information
        TDVDFeatures m_DVDFeatures;
//Read/Write modes
        TReadWriteModes m_ReadWriteModes;
//CD/DVD Device Info
        TDeviceInfo m_DeviceInfo;
//CD/DVD Media(Disc) Info
        TMediaInfo m_MediaInfo;
//Percent which was last wtitten(burning,erasing,grabbing)
        int m_LastWrittenPercent;
//CD/DVD Audio Burn Options
        TBurnOptions m_BurnOptions;
//CD/DVD Erase Options
        TEraseOptions m_EraseOptions;
//Total time of erasing
        long m_TotalTimeErasing;
//Status of CD/DVD device
        TDeviceStatus m_DeviceStatus;
// TOC Information
        TOC_INFORMATION m_TOCInfo;
// Is door or tray open
		bool m_IsDoorOrTrayOpen;

//Hidden track information
//        STARBURN_TRACK_INFORMATION m_HiddenTrackInformation;

//Error info
        unsigned long  m_SystemError;
        char m_ExceptionText[ 1024 ];
        CDB_FAILURE_INFORMATION  m_CDBFailureInfo;
        EXCEPTION_NUMBER m_ExceptionNumber;
        int m_WritingTrack;
//Critical sections
        CRITICAL_SECTION m_CRITICAL_SECTION_CurrentTrackIndex;
        CRITICAL_SECTION m_CRITICAL_SECTION_LastWrittenPercent;
//Writing TAO track
        int m_CurrentTrackIndex; // -1:  No track writing

//Write speeds
	TSpeeds m_WriteSpeeds;
//-----------------------private methods of CDevice-----------------------\\

//Create CdvdBurnerGrabber Object
        bool CdvdBurnerGrabberCreate(PCALLBACK, bool ForReadOperation, bool IsExclusiveAccess = false);

//Get total time of erasing
        long GetTotalTimeErasing(
                                TEraseOptions* //(IN)Erasing options which use in method for calc of total time of erasing
                                );
//Delete CdvdBurnerGrabber Object
        void CleanUp();

//Refresh media(disc) size(not free space)
        bool  RefreshMediaSize();
//Refresh media(disc) label
        bool RefreshMediaLabel();
//Refresh media(disc) type
        bool RefreshMediaType();
//Refresh audio media(disc) free media size
        bool RefreshAudioMediaFreeSize();
//Refresh video media(disc) free media size
		bool RefreshVideoMediaFreeSize();
//Refresh data media(disc) free media size
        bool RefreshDataMediaFreeSize();
//Refresh letter for devices which accessible through ASPI and SPTI transport
//Return true if success, otherwise false
        bool RefreshDeviceLetter(char* DeviceLetter);

//Try set write speed
        bool TrySetWriteSpeed(TSpeed* Speed); //IN OUT Speed, IN - need write speed, OUT - write speed which was set (IN speed can be not equal OUT speed)
//Try set read speed
        bool TrySetReadSpeed(TSpeed* Speed); //IN OUT Speed, IN - need read speed, OUT - read speed which was set (IN speed can be not equal OUT speed)
        

//Dispatched BurnCallback
        void DefaultBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                    );
//Dispatched VideoCDBurnCallback
        void VideoCDBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                );
//Dispatched DVDVideoBurnCallback
        void DVDVideoBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                    );

//Dispatched GrabberCallback
        void CDevice::GrabberCallback(
                  IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                  IN PVOID p__PVOID__CallbackSpecial1,
                  IN PVOID p__PVOID__CallbackSpecial2
                    );
//StarBurn empty callback
        static void __stdcall Callback(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );
//StarBurn callback for StarBurn_Finddevice
//Create CDevice objects, which save in Devices;
        static void __stdcall FindDeviceCallback(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );
//StarBurn callback for StarBurn_CdvdBurnerGrabber_DiscAtOnceRawPWFromFile or
//                      StarBurn_CdvdBurnerGrabber_DiscAtOncePQFromFile
        static void __stdcall DefaultBurnCallbackDispatch(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );
        static void __stdcall VideoCDBurnCallbackDispatch(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );

        static void __stdcall DVDVideoBurnCallbackDispatch(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );

        static void __stdcall CDevice::GrabberCallbackDispatch(
                IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
                IN PVOID p__PVOID__CallbackContext,
                IN PVOID p__PVOID__CallbackSpecial1,
                IN PVOID p__PVOID__CallbackSpecial2
                );

//Thread whish set erase percent to LARGE_INTEGER m_LastWrittenPercent;
        static DWORD WINAPI SetErasePercentThread(
                                                  void* //(OUT)Pointer to CDevice object
                                                  );

        void SetLastWrittenPercent(int Percent);
        void SetCurrentTrackIndex(int Index);

//Wrappers for internal methods with device object creating

		//Check unit ready status 
		bool TestUnitReadyInternal();

		// Refresh  m_VendorID,m_ProductID,m_ProductRevisionLevel,m_Name 
		bool RefreshDeviceInfoInternal(); 

		//Refresh m_DVDFeatures (RPC (region play code) and some additional DVD region management information)
		bool RefreshDVDFeaturesInternal();

		//Refresh device read/write modes;
		bool RefreshReadWriteModesInternal();

		//Refresh m_TocInfo
		bool RefreshTOCInfoInternal();

		//Refresh supported write speeds
		bool RefreshWriteSpeedsInternal();

		//Get supported read speeds
		bool GetReadSpeedsInternal(
			TSpeeds *Speeds //(OUT)Pointer to variable which will be filling
			);

		//Get track information
		bool GetTrackInfoInternal(unsigned char l_Track,  //Track number
			STARBURN_TRACK_INFORMATION* pTrackInformation //(OUT)Pointer to variable which will be filling
			);

		//Burn Audio CD
		bool BurnAudioCDInternal(
			TBurnOptions* pBurnOptions,//(IN)Pointer to  Burn Options which use in method
			TAudioDiscLayout* pAudioDiscLayout,  //(IN) Pointer to Audio Disc Layout use in method
                        char* pCDTextTitle,     //(IN) CD-Text Title
                        char* pCDTextArtist     //(IN) CD-Text Artist
			);

		//Burn Video CD
		bool BurnVideoCDInternal(
			TBurnOptions* BurnOptions,				//(IN)Pointer to  Burn Options which use in method
			char* FilePath,							//(IN)Pointer to VCD/MPEG1 image file located in a file and stored on the hard disk.
			bool IsExclusiveDriveAccess = true);	//(IN)Flag working olny if used SPTD SCSI transport

		//Burn Super Video CD
		bool BurnSuperVideoCDInternal(
			TBurnOptions* BurnOptions,				//(IN)Pointer to Burn Options which use in method
			char* FilePath,							//(IN)Pointer to SVCD/MPEG2 image file located in a file and stored on the hard disk.
			bool IsExclusiveDriveAccess = true);	//(IN)Flag working olny if used SPTD SCSI transport

		//Burn DVD-Video
		bool BurnDVDVideoInternal(
			TBurnOptions* BurnOptions,				//(IN)Pointer to Burn Options which use in method
			char* FilePath,							//(IN)Pointer to DVD-Video image file located in a file and stored on the hard disk.
			char* DiskLabel,
			bool IsExclusiveDriveAccess = true);	//(IN)Flag working olny if used SPTD SCSI transport


		//Burn disc from image
		bool BurnDiscFromImageInternal(TBurnOptions* BurnOptions,
			char* ImageFilePath,
			char * DiscTitle, 
			char * DiscDescription);
		//Grab tracks
		bool GrabTracksInternal(TGrabTracksInfo* GrabTracksInfo);

		//Erase CD/DVD media(disc)
		bool EraseInternal(TEraseOptions*);

		//Eject media(disc)
		bool EjectInternal();

		//Load media(disc)
		bool LoadInternal();

		//Refresh is Tray or Door Open
		bool RefreshIsDoorOrTrayOpenInternal();

		//Cancel current read or write process on the CD/DVD burner.
		void CancelRWOperationInternal();

		//Check media(disc) on blank
		bool IsDiscBlankInternal();

		//Lock device
		//Return true in success, otherwise false.
		bool LockInternal();

		//Release device
		//Return true in success, otherwise false.
		bool ReleaseInternal();

		//Get device lock. 
		//Parameter IsLock return lock status
		//Return true in success, otherwise false.
		bool GetLockInternal(bool &p_IsLocked);

		//Refresh audio media(disc) info for all CD/DVD devices
		bool RefreshAudioMediaInfoInternal();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices

		//Refresh video media(disc) info for all CD/DVD devices
		bool RefreshVideoMediaInfoInternal();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices

		//Refresh data media(disc) info for all CD/DVD devices
		bool RefreshDataMediaInfoInternal();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices


public:
//-----------------------public property of CDevice-----------------------\\

        static std::vector<CDevice> Devices;

//-----------------------public  methods of CDevice-----------------------\\

		//Default constructor for device 
		CDevice();
		//Constructor for device which accessible through ASPI transport
        CDevice(SCSI_DEVICE_ADDRESS *pSCSIDeviceAddress);
		//Constructor for device which accessible through SPTI transport
        CDevice(
			char* InternalDeviceName,		// InternalDeviceName - something like this "CdRom0" or "CdRom1" ...
			SCSI_TRANSPORT SCSITransport = SCSI_TRANSPORT_SPTI);  // SCSI_TRANSPORT_SPTI or SCSI_TRANSPORT_SPTD, SCSI_TRANSPORT_ASPI ignored
		//Copy constructor
		CDevice(const CDevice& rhs);
		//Overload operator =	
		CDevice& operator = (const CDevice& rhs);

		//Destructor
        virtual ~CDevice();

		//Get SCSI device address used to create device
        PSCSI_DEVICE_ADDRESS GetCurrentSCSIDeviceAddress() ;

		//Get internal device name used to create device
        char* GetCurrentInternalDeviceName() ;


		//Create Device Object
        bool CreateDeviceObject(
			bool IsExclusiveAccess = false //Work only for SPTD SCSI transport
			);

		//Create Device Object
		void FreeDeviceObject();


		//Check unit ready status
        bool TestUnitReady();

		//Initialization of StarBurn Library
        static bool InitStarBurnDll();
		//Initialization of StarBurn Library
		static bool ShutDownStarBurnDll();
		//Find all CD/DVD devices use SPTD transport
		static void FindSPTDDevices();
		//Find all CD/DVD devices use SPTI transport
        static void FindSPTIDevices();
		//Find all CD/DVD devices use ASPI transport
        static void FindASPIDevices();
		//Get SPTD driver version
		static unsigned long GetSPTDVersion(unsigned char* pMajorVersion, unsigned char* MinorVersion);
		
        const char1024* GetVendorID();
        const char1024* GetProductID();
        const char1024* GetProductRevisionLevel();
        const char*  GetDeviceLetter();

		// Refresh  m_VendorID,m_ProductID,m_ProductRevisionLevel,m_Name
        bool RefreshDeviceInfo(); 

		//Return inserted disc media type
        const DISC_TYPE GetMediaType();
		//Return inserted disc media type string
		void GetMediaType(char* MediaType);
		//Return true is inserted disc media type is CD
		bool IsMediaTypeCD();
		//Return true is inserted disc media type is DVD
		bool IsMediaTypeDVD();		
		//Return true is inserted disc media type is Blu-Ray
		bool IsMediaTypeBluRay();
		//Return true is inserted disc media type is HDDVD
		bool IsMediaTypeHDDVD();
        const double GetMediaSize();
        const double GetMediaFreeSize();
        const char256* GetMediaLabel();
		//Refresh audio media(disc) info for all CD/DVD devices
        bool RefreshAudioMediaInfo();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices
		//Refresh video media(disc) info for all CD/DVD devices
		bool RefreshVideoMediaInfo();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices
		//Refresh data media(disc) info for all CD/DVD devices
        bool RefreshDataMediaInfo();// Refresh  MediaType, MediaSize, MediaTotalSize, MediaLabel for all CD/DVD devices
		//Refresh m_DVDFeatures (RPC (region play code) and some additional DVD region management information)
        bool RefreshDVDFeatures();
		//Refresh device read/write modes;
        bool RefreshReadWriteModes();
		//Refresh m_TocInfo
        bool RefreshTOCInfo();
		//Get m_DVDFeatures (RPC (region play code) and some additional DVD region management information)
        void GetDVDFeatures(TDVDFeatures* DVDFeatures);
		//Get m_ReadWriteModes
        void GetReadWriteModes(TReadWriteModes* ReadWriteModes);
		//Get Supported media formats
        void GetDeviceSupportedMediaFormats(TDeviceSupportedMediaFormats* DeviceSupportedMediaFormats);
		//Get supported write speeds
        bool GetWriteSpeeds(
                            TSpeeds *Speeds //(OUT)Pointer to variable which will be filling
                            );

		//Refresh supported write speeds
        bool RefreshWriteSpeeds();



		//Get supported read speeds
        bool GetReadSpeeds(
                            TSpeeds *Speeds //(OUT)Pointer to variable which will be filling
                            );
		//Get buffer size
        unsigned long GetBufferSize();

        TDeviceStatus GetDeviceStatus();

		//Get track information
        bool GetTrackInfo(unsigned char l_Track,       //Track number
                          STARBURN_TRACK_INFORMATION* //(OUT)Pointer to variable which will be filling
                          );

        //PSTARBURN_TRACK_INFORMATION GetHiddenTrackInformation();

		//Get TOC information
        void GetTOCInfo(
                        TOC_INFORMATION* pTOCInfo  //(OUT)Pointer to variable which will be filling
                        );

		//Burn Audio CD
        bool BurnAudioCD(
                       TBurnOptions* BurnOptions,//(IN)Pointer to  Burn Options which use in method
                       TAudioDiscLayout* AudioDiscLayout,  //(IN) Pointer to Audio Disc Layout use in method
                       char* pCDTextTitle = "Audio CD", //(IN) CD-Text Title
                       char* pCDTextArtist = "Unknown Artist"  //(IN) CD-Text Artist
                       );
		//Burn Video CD
        bool BurnVideoCD(TBurnOptions* BurnOptions, //(IN)Pointer to  Burn Options which use in method
                         char* FilePath,				//(IN)Pointer to VCD/MPEG1 image file located in a file and stored on the hard disk.
						 bool IsExclusiveDriveAccess = true);	//(IN)Flag working olny if used SPTD SCSI transport
		//Burn Super Video CD
        bool BurnSuperVideoCD(TBurnOptions* BurnOptions, //(IN)Pointer to Burn Options which use in method
                         char* FilePath,				//(IN)Pointer to SVCD/MPEG2 image file located in a file and stored on the hard disk.
						 bool IsExclusiveDriveAccess = true);	//(IN)Flag working olny if used SPTD SCSI transport
		//Burn DVD-Video
        bool BurnDVDVideo(TBurnOptions* BurnOptions,     //(IN)Pointer to Burn Options which use in method
                         char* FilePath,                 //(IN)Pointer to DVD-Video image file located in a file and stored on the hard disk.
						 char* DiskLabel,
						 bool IsExclusiveDriveAccess = true);	 //(IN)Flag working olny if used SPTD SCSI transport
		//Burn disc from image
        bool BurnDiscFromImage(TBurnOptions* BurnOptions,char* ImageFilePath,char * DiscTitle, char * DiscDescription);
		//Grab tracks
        bool GrabTracks(TGrabTracksInfo* GrabTracksInfo);

		//Erase CD/DVD media(disc)
        bool Erase(TEraseOptions*);
		//Eject media(disc)
        bool Eject();
		//Load media(disc)
		bool Load();
		//Is Tray or Door Open
		bool IsDoorOrTrayOpen();
		//Refresh is Tray or Door Open
		bool RefreshIsDoorOrTrayOpen();
		//Cancel current read or write process on the CD/DVD burner.
        void CancelRWOperation();
		//Check media(disc) on blank
        bool IsDiscBlank();
		//Check Audio File Supported
        static  bool IsAudioFileSupported(char* FileName);
		//Return Last written percent (burning,erasing)
        int GetLastWrittenPercent();
		//Return stream size in seconds for audio file
        static  long GetAudioFileStreamSizeInSeconds(char* AudioFilePath);
                //Return stream size in UCHARs for audio file
        static  long GetAudioFileStreamSizeInUCHARs(char* AudioFilePath);
		//Get size of directory
        static void GetDirSize(TCHAR* p__PTCHAR__DirName,
                               PLARGE_INTEGER p__PLARGE_INTEGER__DirSize);
		//This function retrieves device SCSI address for StarPort device by its target ID.
		//return true is success, else false
        static bool GetDeviceSCSIAddress(
					LONG StarPortDeviceTargetId,
					PSCSI_DEVICE_ADDRESS p__PSCSI_DEVICE_ADDRESS);
		//This function retrieves SCSI SPTI device internal name by device letter
		//return true is success, else false
		static bool GetDeviceSPTIInternalNameByLetter(const char* DeviceLetter, char* DeviceInternalName);
		//Get SCSI transport which used for device access
        SCSI_TRANSPORT GetSCSITransport();
		//Get current writing TAO track index
        int GetCurrentTrackIndex();
		//Lock device
		//Return true in success, otherwise false.
		bool Lock();
		//Release device
		//Return true in success, otherwise false.
		bool Release();
		//Get device lock. 
		//Parameter IsLock return lock status
		//Return true in success, otherwise false.
		bool GetLock(bool &p_IsLocked);

		//Return pointer to inner CdvdBurnerGrabber object.
		// Use for your own risk!
		void* GetCdvdBurnerGrabber() const
        {
        	return m_CdvdBurnerGrabber;
        }

        //
        //Get last error info
        //
        unsigned long GetLastSystemError();
        EXCEPTION_NUMBER GetLastExceptionNumber();
        void GetLastExceptionText(char* p_csExceptionText, unsigned int p_intBufferSizeInUCHARs); 
		void GetLastCDBFailureInformation(PCDB_FAILURE_INFORMATION p_pCDBFailureInfo);

};

//
//class CVirtualDevice
//used for work with StarPort devices
//

class CVirtualDevice: public CDevice
{
private:
	unsigned int m_uTargetID;	//StarPort target ID
public:
	CVirtualDevice();
	CVirtualDevice(char* InternalDeviceName, unsigned int uTargetID);
	CVirtualDevice(CDevice& Device, unsigned int uTargetID);	
	unsigned int GetTargetID();
};

//CCompressor class and types

enum TCompressMode
{
        Simple,
        Complex
};

enum TCompressorStatus
{
        CompStatNoAction,
        CompStatCompressing,
        CompStatUncompressing,
        CompStatRecompressing,
        CompStatCanceling,
        CompStatCanceled,
        CompStatError,
        CompStatDone
};


class CCompressor
{
private:
//Compressor callback dispatch function
        static ULONG __stdcall CompressorCallbackDispatch(
	IN ULONG p__ULONG__Reason,
	IN ULONG p__ULONG__Parameter,
	IN PVOID p__PVOID__Context
	);

//Compressor callback function
        void __stdcall CompressorCallback(
	IN ULONG p__ULONG__Reason,
	IN ULONG p__ULONG__Parameter
	);

void __stdcall AudioCompressor_WaveFileHeaderFormat(
                	OUT PWAVE_FILE_HEADER p__PWAVE_FILE_HEADER,
                        IN ULONG p__ULONG__DataSizeInUCHARs
                            );
//Percent which was last compressed
LONG m_LastCompressedPercent;

//Used for cancel compressing operation
TCompressorStatus m_CompressorStatus;

public:

//Constructor
CCompressor();

bool __stdcall AudioCompressor_CompressedFileUncompress(
                        IN TCompressMode Mode,
                	IN PCHAR p__PCHAR__SourceCompressedFileName,
	                IN PCHAR p__PCHAR__DestinationUncompressedFileName,
                	IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
	                IN BOOLEAN p__BOOLEAN__IsWavHeaderRequired,
                	OUT PULONG p__PULONG__UncompressedSizeInUCHARs
                	);

bool __stdcall AudioCompressor_UncompressedFileCompress(
                        IN TCompressMode Mode,
                	IN PCHAR p__PCHAR__SourceUncompressedFileName,
        	        IN PCHAR p__PCHAR__DestinationCompressedFileName,
                	IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
        	        IN STARBURN_STARWAVE_COMPRESSION p__STARBURN_STARWAVE_COMPRESSION
        	        );

bool __stdcall AudioCompressor_CompressedFileRecompress(
                          IN TCompressMode Mode,
                          IN PCHAR p__PCHAR__SourceCompressedFileName,
                          IN PCHAR p__PCHAR__DestinationRecompressedFileName,
                          IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
                          OUT PULONG p__PULONG__UncompressedSizeInUCHARs,
                          IN STARBURN_STARWAVE_COMPRESSION p__STARBURN_STARWAVE_COMPRESSION
        	        );

void CancelCompressorOperation();

TCompressorStatus GetCompressorStatus();

double GetLastCompressedPercent();

};

/*
class CFileTree
{
private:
static void __stdcall FileTreeCallbackDispatch(
				 IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
				 IN PVOID p__PVOID__CallbackContext,
				 IN PVOID p__PVOID__CallbackSpecial1,
				 IN PVOID p__PVOID__CallbackSpecial2
				 );

void FileTreeCallbackD(
		      	 IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
		      	 IN PVOID p__PVOID__CallbackSpecial1,
		      	 IN PVOID p__PVOID__CallbackSpecial2
		      	 );

bool ReplaceSymbol(
                        PCHAR pStr,
			CHAR chSearch,
			CHAR chReplace
			 );

void CorrectISO9660Name(char *p__PCHAR__Name);

void CorrectJolietName(WCHAR *p__PWCHAR__Name);

void CreateTree();
AddFiles();
}
public:

}
*/
//---------------------------------------------------------------------------
#endif

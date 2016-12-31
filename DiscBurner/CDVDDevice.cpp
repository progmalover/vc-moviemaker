/*
 Copyright (c) Rocket Division Software 2001-2008. All rights reserved.

File Name:

    CDVDDevice.cpp

Description:
    CDevice class which include all need operatios with CD/DVD devices

Author:

    Andery Trubeckoy

*/

//---------------------------------------------------------------------------
#include "stdafx.h"
#include "CDVDDevice.h"
#include "StarBurnKey.h"  // header with StarBurn key


#ifdef _CBUILDER
	#pragma hdrstop
	#pragma link "StarBurn.lib"
	#define __FUNCTION__ __FUNC__
    #include "sysutils.hpp"
#endif

ULONG abs(ULONG x)
{
	return x<0?-x:x;
}

TSpeed CDevice::m_CDStandardSpeeds[CD_STANDARD_SPEED_COUNT] = {{CD_SPEED_IN_KBPS_1X,"1.0x"},
                                                           {CD_SPEED_IN_KBPS_2X,"2.0x"},
                                                          {CD_SPEED_IN_KBPS_2P2X,"2.2x"},
                                                          {CD_SPEED_IN_KBPS_3X,"3.0x"},
                                                          {CD_SPEED_IN_KBPS_4X, "4.0x"},
                                                          {CD_SPEED_IN_KBPS_8X,"8.0x"},
                                                          {CD_SPEED_IN_KBPS_10X,"10.0x"},
                                                          {CD_SPEED_IN_KBPS_12X,"12.0x"},
                                                          {CD_SPEED_IN_KBPS_16X,"16.0x"},
                                                          {CD_SPEED_IN_KBPS_20X,"20.0x"},
                                                          {CD_SPEED_IN_KBPS_24X,"24.0x"},
														  {CD_SPEED_IN_KBPS_32X,"32.0x"},
                                                          {CD_SPEED_IN_KBPS_40X,"40.0x"},
                                                          {CD_SPEED_IN_KBPS_44X,"44.0x"},
                                                          {CD_SPEED_IN_KBPS_48X,"48.0x"},
                                                          {CD_SPEED_IN_KBPS_52X,"52.0x"}
																						};

TSpeed CDevice::m_DVDStandardSpeeds[DVD_STANDARD_SPEED_COUNT] = {{DVD_SPEED_IN_KBPS_1X,"1.0x"},
                                                             {DVD_SPEED_IN_KBPS_2X,"2.0x"},
                                                             {DVD_SPEED_IN_KBPS_2DOT4X,"2.4x"},
                                                             {DVD_SPEED_IN_KBPS_3X,"3.0x"},
                                                             {DVD_SPEED_IN_KBPS_4X,"4.0x"},
                                                             {DVD_SPEED_IN_KBPS_5X,"5.0x"},
                                                             {DVD_SPEED_IN_KBPS_6X,"6.0x"},
                                                             {DVD_SPEED_IN_KBPS_8X,"8.0x"},
                                                             {DVD_SPEED_IN_KBPS_12X,"12.0x"},
                                                             {DVD_SPEED_IN_KBPS_16X,"16.0x"}};

TSpeed CDevice::m_BLURAYStandardSpeeds[BLURAY_STANDARD_SPEED_COUNT] = {{BLURAY_SPEED_IN_KBPS_1X,"1.0x"},															
																	   {BLURAY_SPEED_IN_KBPS_2X,"2.0x"},
																	   {BLURAY_SPEED_IN_KBPS_1X*4,"4.0x"},
																	   {BLURAY_SPEED_IN_KBPS_1X*6,"6.0x"}};

std::vector<CDevice> CDevice::Devices;



CDevice::CDevice():
m_SCSITransport(SCSI_TRANSPORT_UNKNOWN),
m_CdvdBurnerGrabber(NULL),
m_IsCdvdBurnerGrabberExclusiveAccess(false),
m_LastWrittenPercent(0),
m_TotalTimeErasing(0),
m_DeviceStatus(NoAction),
m_SystemError(0),
m_ExceptionNumber(EN_SUCCESS),
m_WritingTrack(0),
m_CurrentTrackIndex(-1),
m_IsDoorOrTrayOpen(false)
{
	memset(&m_InternalDeviceName[0], 0,  sizeof(m_InternalDeviceName));
	memset(&m_SCSIDeviceAddress, 0, sizeof(m_SCSIDeviceAddress));		
	memset(&m_TOCInfo, 0, sizeof(m_TOCInfo));
	memset(&m_CDBFailureInfo, 0, sizeof(m_CDBFailureInfo));		
	memset(&m_ExceptionText, 0, sizeof(m_ExceptionText));
	memset(&m_DeviceSupportedMediaFormats, 0, sizeof(m_DeviceSupportedMediaFormats));		
	memset(&m_DVDFeatures, 0, sizeof(m_DVDFeatures));		
	memset(&m_ReadWriteModes, 0, sizeof(m_ReadWriteModes));		
	memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));		
	memset(&m_MediaInfo, 0, sizeof(m_MediaInfo));

	memset(&m_BurnOptions, 0, sizeof(m_BurnOptions));
	m_BurnOptions.WriteMethod = WRITE_MODE_TRACK_AT_ONCE;

	memset(&m_EraseOptions, 0, sizeof(m_EraseOptions));	
	m_EraseOptions.EraseType = ERASE_TYPE_BLANK_DISC_FAST;

	InitializeCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
	InitializeCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
};

CDevice::CDevice(SCSI_DEVICE_ADDRESS *l_SCSIDeviceAddress):
		m_SCSIDeviceAddress(*l_SCSIDeviceAddress),		
		m_SCSITransport(SCSI_TRANSPORT_ASPI),
		m_CdvdBurnerGrabber(NULL),	
		m_IsCdvdBurnerGrabberExclusiveAccess(false),
		m_LastWrittenPercent(0),		
		m_TotalTimeErasing(0),
		m_DeviceStatus(NoAction),
		m_SystemError(0),		
		m_ExceptionNumber(EN_SUCCESS),
		m_WritingTrack(0),
		m_CurrentTrackIndex(-1),
		m_IsDoorOrTrayOpen(false)
{
		memset(&m_TOCInfo, 0, sizeof(m_TOCInfo));
		memset(&m_CDBFailureInfo, 0, sizeof(m_CDBFailureInfo));
		memset(&m_InternalDeviceName, 0, sizeof(m_InternalDeviceName));
		memset(&m_ExceptionText, 0, sizeof(m_ExceptionText));
		memset(&m_DeviceSupportedMediaFormats, 0, sizeof(m_DeviceSupportedMediaFormats));		
		memset(&m_DVDFeatures, 0, sizeof(m_DVDFeatures));		
		memset(&m_ReadWriteModes, 0, sizeof(m_ReadWriteModes));		
		memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));		
		memset(&m_MediaInfo, 0, sizeof(m_MediaInfo));

		memset(&m_BurnOptions, 0, sizeof(m_BurnOptions));
		m_BurnOptions.WriteMethod = WRITE_MODE_TRACK_AT_ONCE;

		memset(&m_EraseOptions, 0, sizeof(m_EraseOptions));	
		m_EraseOptions.EraseType = ERASE_TYPE_BLANK_DISC_FAST;

        InitializeCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
        InitializeCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
};

CDevice::CDevice(
				 char* InternalDeviceName, 
				 SCSI_TRANSPORT SCSITransport/*= SCSI_TRANSPORT_SPTI*/):		
		m_CdvdBurnerGrabber(NULL),
		m_IsCdvdBurnerGrabberExclusiveAccess(false),
		m_LastWrittenPercent(0),
		m_TotalTimeErasing(0),
		m_DeviceStatus(NoAction),
		m_SystemError(0),
		m_ExceptionNumber(EN_SUCCESS),
		m_WritingTrack(0),
		m_CurrentTrackIndex(-1),
		m_IsDoorOrTrayOpen(false)
{
		if (SCSITransport == SCSI_TRANSPORT_SPTD)
		{
			m_SCSITransport = SCSI_TRANSPORT_SPTD;
		}
		else
		{
			m_SCSITransport = SCSI_TRANSPORT_SPTI;
		}

        strcpy(&m_InternalDeviceName[0], InternalDeviceName);
		memset(&m_SCSIDeviceAddress, 0, sizeof(m_SCSIDeviceAddress));		
		memset(&m_TOCInfo, 0, sizeof(m_TOCInfo));
		memset(&m_CDBFailureInfo, 0, sizeof(m_CDBFailureInfo));		
		memset(&m_ExceptionText, 0, sizeof(m_ExceptionText));
		memset(&m_DeviceSupportedMediaFormats, 0, sizeof(m_DeviceSupportedMediaFormats));		
		memset(&m_DVDFeatures, 0, sizeof(m_DVDFeatures));		
		memset(&m_ReadWriteModes, 0, sizeof(m_ReadWriteModes));		
		memset(&m_DeviceInfo, 0, sizeof(m_DeviceInfo));		
		memset(&m_MediaInfo, 0, sizeof(m_MediaInfo));

		memset(&m_BurnOptions, 0, sizeof(m_BurnOptions));
		m_BurnOptions.WriteMethod = WRITE_MODE_TRACK_AT_ONCE;

		memset(&m_EraseOptions, 0, sizeof(m_EraseOptions));	
		m_EraseOptions.EraseType = ERASE_TYPE_BLANK_DISC_FAST;

        InitializeCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
        InitializeCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
};

CDevice::CDevice(const CDevice& rhs)
{
	m_SCSITransport = rhs.m_SCSITransport;
	m_SCSIDeviceAddress = rhs.m_SCSIDeviceAddress;
	strcpy(m_InternalDeviceName, rhs.m_InternalDeviceName);
	m_CdvdBurnerGrabber = NULL;
	m_IsCdvdBurnerGrabberExclusiveAccess = false;
	m_LastWrittenPercent = rhs.m_LastWrittenPercent;
	m_TotalTimeErasing = rhs.m_TotalTimeErasing;
	m_DeviceStatus = rhs.m_DeviceStatus;
	m_SystemError = rhs.m_SystemError;
	m_ExceptionNumber = rhs.m_ExceptionNumber;
	m_WritingTrack = rhs.m_WritingTrack;
	m_CurrentTrackIndex = rhs.m_CurrentTrackIndex;	
	m_TOCInfo = rhs.m_TOCInfo;
	m_CDBFailureInfo = rhs.m_CDBFailureInfo;
	strcpy(m_ExceptionText, rhs.m_ExceptionText);
	m_DeviceSupportedMediaFormats = rhs.m_DeviceSupportedMediaFormats;
	m_DVDFeatures = rhs.m_DVDFeatures;
	m_ReadWriteModes = rhs.m_ReadWriteModes;
	m_DeviceInfo = rhs.m_DeviceInfo;
	m_MediaInfo = rhs.m_MediaInfo;
	m_BurnOptions = rhs.m_BurnOptions;
	m_EraseOptions = rhs.m_EraseOptions;
	m_CRITICAL_SECTION_CurrentTrackIndex = rhs.m_CRITICAL_SECTION_CurrentTrackIndex;
	m_CRITICAL_SECTION_LastWrittenPercent = rhs.m_CRITICAL_SECTION_LastWrittenPercent;
	m_IsDoorOrTrayOpen = rhs.m_IsDoorOrTrayOpen;
	//CdvdBurnerGrabberCreate(&Callback, true);

}

CDevice& CDevice::operator = (const CDevice& rhs)
{	
	if(this != &rhs)
	{
		m_SCSITransport = rhs.m_SCSITransport;
		m_SCSIDeviceAddress = rhs.m_SCSIDeviceAddress;
		strcpy(m_InternalDeviceName, rhs.m_InternalDeviceName);
		m_CdvdBurnerGrabber = NULL;
		m_IsCdvdBurnerGrabberExclusiveAccess = false;
		m_LastWrittenPercent = rhs.m_LastWrittenPercent;
		m_TotalTimeErasing = rhs.m_TotalTimeErasing;
		m_DeviceStatus = rhs.m_DeviceStatus;
		m_SystemError = rhs.m_SystemError;
		m_ExceptionNumber = rhs.m_ExceptionNumber;
		m_WritingTrack = rhs.m_WritingTrack;
		m_CurrentTrackIndex = rhs.m_CurrentTrackIndex;	
		m_TOCInfo = rhs.m_TOCInfo;
		m_CDBFailureInfo = rhs.m_CDBFailureInfo;
		strcpy(m_ExceptionText, rhs.m_ExceptionText);
		m_DeviceSupportedMediaFormats = rhs.m_DeviceSupportedMediaFormats;
		m_DVDFeatures = rhs.m_DVDFeatures;
		m_ReadWriteModes = rhs.m_ReadWriteModes;
		m_DeviceInfo = rhs.m_DeviceInfo;
		m_MediaInfo = rhs.m_MediaInfo;
		m_BurnOptions = rhs.m_BurnOptions;
		m_EraseOptions = rhs.m_EraseOptions;
		m_CRITICAL_SECTION_CurrentTrackIndex = rhs.m_CRITICAL_SECTION_CurrentTrackIndex;
		m_CRITICAL_SECTION_LastWrittenPercent = rhs.m_CRITICAL_SECTION_LastWrittenPercent;
		m_IsDoorOrTrayOpen = rhs.m_IsDoorOrTrayOpen;
	}
	return *this;
}

CDevice::~CDevice()
{
     CleanUp();		
}

void CDevice::CleanUp()
{
	if(NULL != m_CdvdBurnerGrabber)
	{
		StarBurn_Destroy(&m_CdvdBurnerGrabber );
		m_CdvdBurnerGrabber = NULL;

			m_IsCdvdBurnerGrabberExclusiveAccess = false;
		}
}




bool CDevice::CdvdBurnerGrabberCreate(PCALLBACK FCallback, bool ForReadOperation, bool IsExclusiveAccess)
{
    m_ExceptionNumber = EN_SUCCESS; // Assume success by default
    m_SystemError = ERROR_GEN_FAILURE;
    ULONG l_CacheSize = ForReadOperation ? STARBURN_CACHE_SIZE_READ_ONLY : STARBURN_CACHE_SIZE_READ_WRITE;

    switch(m_SCSITransport)
    {
	case SCSI_TRANSPORT_ASPI:
		{
			m_ExceptionNumber =
				StarBurn_CdvdBurnerGrabber_Create(
				&m_CdvdBurnerGrabber,
				( PCHAR )(&m_ExceptionText),
				sizeof( m_ExceptionText ),
				&m_SystemError,
				&m_CDBFailureInfo,
				( PCALLBACK )( FCallback ),
				this,
				m_SCSIDeviceAddress.m__UCHAR__PortID,
				m_SCSIDeviceAddress.m__UCHAR__BusID,
				m_SCSIDeviceAddress.m__UCHAR__TargetID,
				m_SCSIDeviceAddress.m__UCHAR__LUN,
				l_CacheSize
				);
			if ( m_ExceptionNumber != EN_SUCCESS )
			{
				CLogger::Log(Debug,lmtError,
					"\n\nCDevice::CdvdBurnerGrabberCreate: StarBurn_CdvdBurnerGrabber_Create() failed, exception %ld, status %ld, text '%s'\n",
					m_ExceptionNumber,
					m_SystemError,
					m_ExceptionText
					);

				return false;
			}					 
		}
		break;
    case SCSI_TRANSPORT_SPTI:
        {
                m_ExceptionNumber =
			StarBurn_CdvdBurnerGrabber_CreateEx(
                &m_CdvdBurnerGrabber,
                ( PCHAR )(&m_ExceptionText),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                ( PCALLBACK )( FCallback ),
				this,
				&m_InternalDeviceName[0],
				l_CacheSize
				);

		if (m_ExceptionNumber != EN_SUCCESS)
		{
			CLogger::Log(Debug,lmtError,
				"\n\nCDevice::CdvdBurnerGrabberCreate: StarBurn_CdvdBurnerGrabber_CreateEx() failed, exception %ld, status %ld, text '%s'\n",
                                m_ExceptionNumber,
                                m_SystemError,
                                m_ExceptionText
                                );
                        return false;
                }
        }	
        break;
	case SCSI_TRANSPORT_SPTD:
		{
			WCHAR wcsDeviceName[ MAX_PATH ];

			ZeroMemory(
				&wcsDeviceName,
				sizeof( wcsDeviceName )
				);

			MultiByteToWideChar(
				CP_ACP,
				0,
				&m_InternalDeviceName[0],
				strlen( m_InternalDeviceName ),
				( PWCHAR )( &wcsDeviceName[0] ),
				(int)( sizeof( wcsDeviceName ) / 2 )
				);

			m_ExceptionNumber =
				StarBurn_CdvdBurnerGrabber_CreateExEx(
				&m_CdvdBurnerGrabber,
				( PCHAR )(&m_ExceptionText),
				sizeof( m_ExceptionText ),
				&m_SystemError,
				&m_CDBFailureInfo,
				( PCALLBACK )( FCallback ),
				this,
				&wcsDeviceName[0],
				IsExclusiveAccess,
				l_CacheSize
				);

			if (m_ExceptionNumber != EN_SUCCESS)
			{
				CLogger::Log(Debug,lmtError,
					"\n\nCDevice::CdvdBurnerGrabberCreate: StarBurn_CdvdBurnerGrabber_CreateEx() failed, exception %ld, status %ld, text '%s'\n",
					m_ExceptionNumber,
					m_SystemError,
					m_ExceptionText
					);

				return false;
			}	

			m_IsCdvdBurnerGrabberExclusiveAccess = IsExclusiveAccess;
		}
		break;
    }


    return true;
}



void
__stdcall
CDevice::Callback(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )
{
}



void
__stdcall
CDevice::FindDeviceCallback(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )

{

    // Process depending of callback number.

    switch ( p__CALLBACK_NUMBER )
    {
        // if this is FIND_DEVICE

        case CN_FIND_DEVICE:
        {
            SCSI_DEVICE_ADDRESS* l_pScsiDeviceAddress;
            l_pScsiDeviceAddress = (SCSI_DEVICE_ADDRESS*)p__PVOID__CallbackSpecial1;
            CDevice Device = CDevice(l_pScsiDeviceAddress);

            CLogger::Log(Debug,lmtInformation,
                "\nCDevice::FindDeviceCallback): Probing SCSI address %ld:%ld:%ld:%ld for CD/DVD device... ",
                l_pScsiDeviceAddress->m__UCHAR__PortID,
                l_pScsiDeviceAddress->m__UCHAR__BusID,
                l_pScsiDeviceAddress->m__UCHAR__TargetID,
                l_pScsiDeviceAddress->m__UCHAR__LUN
                );
            Devices.push_back(Device);

        }
    }
};

bool CDevice::CreateDeviceObject(bool IsExclusiveAccess /*== false*/)
{
	//Create CdvdBurnerGrabber object if need
    if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true, IsExclusiveAccess))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

}

void CDevice::FreeDeviceObject()
{
	CleanUp();
}

bool CDevice::TestUnitReady()
{
	bool bResult(false);
	bResult = TestUnitReadyInternal();
	CleanUp();	   
	return bResult;
};

bool CDevice::TestUnitReadyInternal()
{
	m_SystemError = ERROR_GEN_FAILURE;
	m_ExceptionNumber = EN_SUCCESS; // Assume success by default

	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true))
			return false;

	m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_TestUnitReady(
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo
		);

	if (m_ExceptionNumber != EN_SUCCESS)
	{

		return false;
	}
	else
	{

		return true;
	}

};

bool CDevice::RefreshDeviceLetter(char* DeviceLetter)
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true))
			return false;

        //
        // Try to get device letter
		//
        char l_DeviceLetter[3] ={0};
        memset(l_DeviceLetter,0,3);

        switch(m_SCSITransport)
        {
		case SCSI_TRANSPORT_SPTI:
        case SCSI_TRANSPORT_SPTD:
                {
					
                    m_ExceptionNumber =
			        StarBurn_GetDeviceLetter(
				        &m_InternalDeviceName[0],
						&l_DeviceLetter[0],
						&m_SystemError
						);

                    //
                    // Check for success
                    //
                	if ( m_ExceptionNumber != EN_SUCCESS )
		            {
						 CLogger::Log(Debug,lmtError,
				 			"\n\nCDevice::RefreshDeviceLetter: StarBurn_GetDeviceLetter() failed, exception %ld, status %ld \n",
				 			m_ExceptionNumber,
				 			m_SystemError
				 	);
					
                    return false;
				}
						
                }
                break;
        case SCSI_TRANSPORT_ASPI:
                {
                        char l_DeviceName[1024] = {0};
                	m_ExceptionNumber =
		                 StarBurn_GetDeviceNameByDeviceAddress(
                                        (unsigned char)m_SCSIDeviceAddress.m__UCHAR__PortID,
                                        (unsigned char)m_SCSIDeviceAddress.m__UCHAR__BusID,
                                        (unsigned char)m_SCSIDeviceAddress.m__UCHAR__TargetID,
                                        (unsigned char)m_SCSIDeviceAddress.m__UCHAR__LUN,
                                        (char*)l_DeviceName
                                        );

                        //
                        // Check for success
                        //
                	if ( m_ExceptionNumber != EN_SUCCESS )
		                {
                                    CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::RefreshDeviceLetter: StarBurn_GetDeviceNameByDeviceAddress() failed, exception %d \n",
                                        m_ExceptionNumber
                                        );
									
                                    return false;
                                 }

                        strncpy( l_DeviceLetter, (char*)l_DeviceName + 4, 3);
                }
                break;
        }

        strcpy(DeviceLetter, &l_DeviceLetter[0]);
		
        return true;
}


bool CDevice::RefreshDeviceInfo()
{
	bool bResult(false);
	bResult = RefreshDeviceInfoInternal();

	CleanUp();
	return bResult;
}

bool CDevice::RefreshDeviceInfoInternal()
{

       //Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
	   {
			if(!CdvdBurnerGrabberCreate(&Callback, true))
			{
				return false;
			}
	   }

		
		EXCEPTION_NUMBER ExceptionNumber(EN_SUCCESS);
      // Try to get CD/DVD burner information

                StarBurn_CdvdBurnerGrabber_GetDeviceInformation(
                    m_CdvdBurnerGrabber,
                    ( PCHAR )( &m_DeviceInfo.VendorID),
                    ( PCHAR )( &m_DeviceInfo.ProductID),
                    ( PCHAR )( &m_DeviceInfo.ProductRevisionLevel),
                    &m_DeviceInfo.BufferSizeInUCHARs
                    );

                CLogger::Log(Debug,lmtInformation,
                    "OK!\nCDevice::RefreshDeviceInfo()): Found CD/DVD device '%s' '%s' '%s'\n",
                    m_DeviceInfo.VendorID,
                    m_DeviceInfo.ProductID,
                    m_DeviceInfo.ProductRevisionLevel
                    );

                CLogger::Log(Debug,lmtInformation,
                    "CDevice::RefreshDeviceInfo(): With %ld UCHAR(s) of cache\n",
                    m_DeviceInfo.BufferSizeInUCHARs
                    );

                m_SCSIDeviceAddress.m__BOOLEAN__IsValid = true;

                //
                // Try to get device letter
        		//

				RefreshDeviceLetter(&m_DeviceInfo.DeviceLetter[0]); 

                
                // Get CdvdBurnerGrabber device extended information
                //
                StarBurn_CdvdBurnerGrabber_GetSupportedMediaFormats(
                    this->m_CdvdBurnerGrabber,
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsCDRRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsCDERead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDROMRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDRRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDRAMRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsTestWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsCDRWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsCDEWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDRWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDRAMWrite)
                    );

                //
                // Get CdvdBurnerGrabber device extended information for DVD+R(w)
                //
                StarBurn_CdvdBurnerGrabber_GetSupportedMediaFormatsExEx(
                    this->m_CdvdBurnerGrabber,
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRWRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRDLRead),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRWWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRWrite),
                    (PBOOLEAN)(&m_DeviceSupportedMediaFormats.IsDVDPLUSRDLWrite)
                    );

				STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS;
				RtlZeroMemory(&l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS, sizeof(l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS));

				l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__ULONG__SizeInUCHARs = sizeof(l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS);

				ExceptionNumber = 
				StarBurn_CdvdBurnerGrabber_GetAdvancedSupportedMediaFormats(
					this->m_CdvdBurnerGrabber, 
					&l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS
					);

				if (ExceptionNumber == EN_SUCCESS)
				{				
					m_DeviceSupportedMediaFormats.IsCDROMRead =  l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsCDROMRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsCDRRead =  l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsCDRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsCDRWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsCDRWrite == TRUE ? true:false;					
					m_DeviceSupportedMediaFormats.IsCDRWRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsCDRWRead == TRUE ? true:false;					
					m_DeviceSupportedMediaFormats.IsCDRWWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsCDRWWrite == TRUE ? true:false;					
					m_DeviceSupportedMediaFormats.IsDVDROMRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDROMRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRWRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRWRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRWWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRWWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRWDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRWDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRWDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRWDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRAMRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRAMRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDRAMWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDRAMWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRWRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRWRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRWWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRWWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRWDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRWDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRWDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRWDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsDVDPLUSRDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsDVDPLUSRDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDROMRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDROMRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDRRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDRWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDRWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDRDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDRDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDRDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDRDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDRERead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDRERead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDREWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDREWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDREDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDREDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsBDREDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsBDREDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDROMRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDROMRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRDLWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRWRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRWRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRWWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRWWrite == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRWDLRead = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRWDLRead == TRUE ? true:false;
					m_DeviceSupportedMediaFormats.IsHDDVDRWDLWrite = l__STARBURN_ADVANCED_SUPPORTED_MEDIA_FORMATS.m__BOOLEAN__IsHDDVDRWDLWrite == TRUE ? true:false;
				}

				RefreshIsDoorOrTrayOpen();

		
        return true;
};


bool CDevice::RefreshMediaType()
{

        //Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true))
                         return false;


      m_MediaInfo.MediaType = StarBurn_CdvdBurnerGrabber_GetInsertedDiscType( m_CdvdBurnerGrabber );

	  
      return true;
};



void
__stdcall
CDevice::DefaultBurnCallbackDispatch(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )

{
        //      Dispatch callback to from callback handler

        CDevice* device = ( CDevice* ) p__PVOID__CallbackContext;
        device->DefaultBurnCallback(
                        p__CALLBACK_NUMBER,
                        p__PVOID__CallbackSpecial1,
                        p__PVOID__CallbackSpecial2
                        );
}

void
__stdcall
CDevice::DVDVideoBurnCallbackDispatch(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )

{
        //      Dispatch callback to from callback handler

        CDevice* device = ( CDevice* ) p__PVOID__CallbackContext;
        device->DVDVideoBurnCallback(
                        p__CALLBACK_NUMBER,
                        p__PVOID__CallbackSpecial1,
                        p__PVOID__CallbackSpecial2
                        );
 }

void
__stdcall
CDevice::VideoCDBurnCallbackDispatch(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )

{
        //      Dispatch callback to from callback handler

        CDevice* device = ( CDevice* ) p__PVOID__CallbackContext;
        device->VideoCDBurnCallback(
                        p__CALLBACK_NUMBER,
                        p__PVOID__CallbackSpecial1,
                        p__PVOID__CallbackSpecial2
                        );
 }


void CDevice::VideoCDBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                    )


{
    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInLBs;

    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInUCHARs;

    LARGE_INTEGER l__LARGE_INTEGER__LBsWritten;

    LARGE_INTEGER l__LARGE_INTEGER__CurrentPercent;

    ULONG l__ULONG__WaitTimeInMs;

    // Process depending of callback number

    switch ( p__CALLBACK_NUMBER )
    {
        // If this is file analyze begin

        case CN_TARGET_FILE_ANALYZE_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): TARGET_FILE_ANALYZE_BEGIN\n" );

        }
        break;
        // If this is file analyze end

        case CN_TARGET_FILE_ANALYZE_END:
        {
            // Get parameters passed with callback

            l__LARGE_INTEGER__FileSizeInUCHARs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            CLogger::Log(Debug,lmtInformation,
                "CDevice::BurnCallback(): Target file size in UCHARs %I64d, LBs %I64d\n",
                l__LARGE_INTEGER__FileSizeInUCHARs,
                l__LARGE_INTEGER__FileSizeInLBs
                );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): TARGET_FILE_ANALYZE_END\n" );

        }
        break;
        // If this is cache full begin

        case CN_WAIT_CACHE_FULL_BEGIN:
        {
            // Get parameters passed with callback

            l__ULONG__WaitTimeInMs = *( PULONG )( p__PVOID__CallbackSpecial1 );

            m_WritingTrack ++;

            switch (m_WritingTrack)
            {
                case 1:
                {
                        CLogger::Log(Debug,lmtInformation, "Writing ISO track..." );
                        CLogger::Log(User,lmtInformation, "Writing ISO track..." );
                }
                break;
                case 2:
                {
                        CLogger::Log(Debug,lmtInformation,"Writing MPEG track...");
                        CLogger::Log(User,lmtInformation,"Writing MPEG track...");
                }
            }

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): WAIT_CACHE_FULL_BEGIN\n" );
            CLogger::Log(Debug,lmtInformation,
                "CDevice::BurnCallback(): Waiting for cache full not more then %ld ms... ",
                l__ULONG__WaitTimeInMs
                );
            CLogger::Log(User,lmtInformation,
                "Waiting for cache full not more then %ld ms... ",
                l__ULONG__WaitTimeInMs
                );



        }
        break;
        // If this is cache full end

        case CN_WAIT_CACHE_FULL_END:
        {
            CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnCallback(): WAIT_CACHE_FULL_END\n" );
            CLogger::Log(User,lmtInformation,"Cache is Full.");
            CLogger::Log(User,lmtInformation,"Burning..." );
        }
        break;
        // If this is synchronize cache begin

        case CN_SYNCHRONIZE_CACHE_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "\nCDevice::BurnCallback(): SYNCHRONIZE_CACHE_BEGIN\n" );
        }
        break;

        // If this is synchronize cache end

        case CN_SYNCHRONIZE_CACHE_END:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): SYNCHRONIZE_CACHE_END\n" );
        }
            break;
        // If this is DVD+RW format begin

        case CN_DVDPLUSRW_FORMAT_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "\nCDevice::BurnCallback(): DVDPLUSRW_FORMAT_BEGIN\n" );
        }
            break;
        // If this is DVD+RW format end

        case CN_DVDPLUSRW_FORMAT_END:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): DVDPLUSRW_FORMAT_END\n" );
        }
            break;
        // If this is packet write

        case CN_CDVD_WRITE_PROGRESS:
        {
            // Get parameters passed with callback


            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__LBsWritten = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            // Calculate number of percent written

            l__LARGE_INTEGER__CurrentPercent.QuadPart =
                ( ( l__LARGE_INTEGER__LBsWritten.QuadPart * 100 ) / l__LARGE_INTEGER__FileSizeInLBs.QuadPart );

            // Check if current calculated was already written

            if ( l__LARGE_INTEGER__CurrentPercent.QuadPart == GetLastWrittenPercent())
            {
                // Do nothing...
            }
            else
            {
                // Update last written percent with current value
                SetLastWrittenPercent((int)l__LARGE_INTEGER__CurrentPercent.LowPart);

                // Print this percent

                CLogger::Log(Debug,lmtInformation,
                    "%d%% ",
                    GetLastWrittenPercent()
                    );

            }     
        }
        break;
    }
}

void CDevice::DVDVideoBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                    )
/*++

Routine Description:

    Callback passed to library objects to show progress indication (tree node item added or removed, LBA assigned, file
    opened etc)

Arguments:

    Callback number,
    Parameter 1,
    Parameter 2

Return Value:

    Nothing

--*/

{
    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInLBs;

    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInUCHARs;

    LARGE_INTEGER l__LARGE_INTEGER__LBsWritten;

    LARGE_INTEGER l__LARGE_INTEGER__CurrentPercent;

    ULONG l__ULONG__WaitTimeInMs = 0;

    ULONG l__ULONG__TrackPaddingSizeInLBs = 0;


    // Process depending of callback number

    switch ( p__CALLBACK_NUMBER )
    {

        // If this DVD media padding burn started
        case CN_DVD_MEDIA_PADDING_BEGIN:
        {
                CLogger::Log(Debug,
                            lmtInformation,
                            "CDevice::DVDVideoBurnCallback(): DVD_MEDIA_PADDING_BEGIN\n"
                             );
        }
        break;

        // If this DVD media padding burn completed
        case CN_DVD_MEDIA_PADDING_END:
        {
                CLogger::Log(Debug,
                            lmtInformation,
                            "CDevice::DVDVideoBurnCallback(): DVD_MEDIA_PADDING_END\n"
                            );
        }
        break;

        // If this DVD media padding size report
        case CN_DVD_MEDIA_PADDING_SIZE:
        {
            // Get parameters passed with callback
            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__ULONG__TrackPaddingSizeInLBs = *( PULONG )( p__PVOID__CallbackSpecial2 );

            CLogger::Log(Debug,
                        lmtInformation,
                        "CDevice::DVDVideoBurnCallback(): Target file size in LBs %I64d, track padding size in LBs %ld\n",
                        l__LARGE_INTEGER__FileSizeInLBs,
	        	l__ULONG__TrackPaddingSizeInLBs
                        );

            CLogger::Log(Debug,
                        lmtInformation,
                         "CDevice::DVDVideoBurnCallback(): CN_DVD_MEDIA_PADDING_SIZE\n"
                         );
        }
        break;


        // If this is file analyze begin
        case CN_TARGET_FILE_ANALYZE_BEGIN:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                         "CDevice::DVDVideoBurnCallback(): TARGET_FILE_ANALYZE_BEGIN\n"
                         );
        }
        break;

        //
        // If this is file analyze end
        //
        case CN_TARGET_FILE_ANALYZE_END:
        {
            //
            // Get parameters passed with callback
            //

            l__LARGE_INTEGER__FileSizeInUCHARs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            CLogger::Log(Debug,
                        lmtInformation,
                        "CDevice::DVDVideoBurnCallback(): Target file size in UCHARs %I64d, LBs %I64d\n",
                        l__LARGE_INTEGER__FileSizeInUCHARs,
                        l__LARGE_INTEGER__FileSizeInLBs
                        );

            CLogger::Log(Debug,
                        lmtInformation,
                         "CDevice::DVDVideoBurnCallback(): TARGET_FILE_ANALYZE_END\n"
                         );
        }
        break;

        // If this is cache full begin
        case CN_WAIT_CACHE_FULL_BEGIN:
        {
            //
            // Get parameters passed with callback
            //

            l__ULONG__WaitTimeInMs = *( PULONG )( p__PVOID__CallbackSpecial1 );

            CLogger::Log(Debug,
                        lmtInformation,
                        "CDevice::DVDVideoBurnCallback(): WAIT_CACHE_FULL_BEGIN\n"
                        );
            
            CLogger::Log(Debug,
                        lmtInformation,
                        "CDevice::DVDVideoBurnCallback(): Waiting for cache full not more then %ld ms... ",
                        l__ULONG__WaitTimeInMs
                        );
        }
        break;

        // If this is cache full end
        case CN_WAIT_CACHE_FULL_END:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                         "OK!\nCDevice::DVDVideoBurnCallback(): WAIT_CACHE_FULL_END\n"
                         );
            CLogger::Log(User,lmtInformation,"Cache is Full.");
            CLogger::Log(User,lmtInformation,"Burning..." );
        }
        break;

        // If this is synchronize cache begin
        case CN_SYNCHRONIZE_CACHE_BEGIN:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                        "\nCDevice::DVDVideoBurnCallback(): SYNCHRONIZE_CACHE_BEGIN\n"
                        );
        }
        break;

        // If this is synchronize cache end
        case CN_SYNCHRONIZE_CACHE_END:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                         "CDevice::DVDVideoBurnCallback(): SYNCHRONIZE_CACHE_END\n"
                         );
        }
        break;

        // If this is DVD+RW format begin
        case CN_DVDPLUSRW_FORMAT_BEGIN:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                         "\nCDevice::DVDVideoBurnCallback(): DVDPLUSRW_FORMAT_BEGIN\n"
                         );
        }
        break;

        // If this is DVD+RW format end
        case CN_DVDPLUSRW_FORMAT_END:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                        "CDevice::DVDVideoBurnCallback(): DVDPLUSRW_FORMAT_END\n"
                        );
        }
        break;

        // If this is DVD-RW quick format begin
        case CN_DVDRW_QUICK_FORMAT_BEGIN:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                        "\nCDevice::DVDVideoBurnCallback(): DVDRW_QUICK_FORMAT_BEGIN\n"
                        );
        }
        break;

        //
        // If this is DVD-RW quick format end
        //
        case CN_DVDRW_QUICK_FORMAT_END:
        {
            CLogger::Log(Debug,
                        lmtInformation,
                         "CDevice::DVDVideoBurnCallback(): DVDRW_QUICK_FORMAT_END\n"
                         );
        }
        break;

        // If this is packet write
        case CN_CDVD_WRITE_PROGRESS:
        {
            // Get parameters passed with callback
            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__LBsWritten = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            // Calculate number of percent written
            l__LARGE_INTEGER__CurrentPercent.QuadPart =
                ( ( l__LARGE_INTEGER__LBsWritten.QuadPart * 100 ) / l__LARGE_INTEGER__FileSizeInLBs.QuadPart );


            // Check if current calculated was already written

            if ( l__LARGE_INTEGER__CurrentPercent.QuadPart == GetLastWrittenPercent() )
            {
                // Do nothing...
            }
            else
            {
                // Update last written percent with current value
                SetLastWrittenPercent((int)l__LARGE_INTEGER__CurrentPercent.LowPart);

                // Print this percent
                CLogger::Log(Debug,
                        lmtInformation,
                        "%d%% ",
                        GetLastWrittenPercent()
                        );
            }

        }
        break;
    }
}


void CDevice::DefaultBurnCallback(
        IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
        IN PVOID p__PVOID__CallbackSpecial1,
        IN PVOID p__PVOID__CallbackSpecial2
                    )


{
    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInLBs;

    LARGE_INTEGER l__LARGE_INTEGER__FileSizeInUCHARs;

    LARGE_INTEGER l__LARGE_INTEGER__LBsWritten;

    LARGE_INTEGER l__LARGE_INTEGER__CurrentPercent;

    ULONG l__ULONG__WaitTimeInMs;

    // Process depending of callback number

    switch ( p__CALLBACK_NUMBER )
    {
        // If this is file analyze begin

        case CN_TARGET_FILE_ANALYZE_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): TARGET_FILE_ANALYZE_BEGIN\n" );

        }
        break;
        // If this is file analyze end

        case CN_TARGET_FILE_ANALYZE_END:
        {
            // Get parameters passed with callback

            l__LARGE_INTEGER__FileSizeInUCHARs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            CLogger::Log(Debug,lmtInformation,
                "CDevice::BurnCallback(): Target file size in UCHARs %I64d, LBs %I64d\n",
                l__LARGE_INTEGER__FileSizeInUCHARs,
                l__LARGE_INTEGER__FileSizeInLBs
                );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): TARGET_FILE_ANALYZE_END\n" );

        }
        break;
        // If this is cache full begin

        case CN_WAIT_CACHE_FULL_BEGIN:
        {
            // Get parameters passed with callback

            l__ULONG__WaitTimeInMs = *( PULONG )( p__PVOID__CallbackSpecial1 );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): WAIT_CACHE_FULL_BEGIN\n" );

            CLogger::Log(Debug,lmtInformation,
                "CDevice::BurnCallback(): Waiting for cache full not more then %ld ms... ",
                l__ULONG__WaitTimeInMs
                );
            CLogger::Log(User,lmtInformation,
                "Waiting for cache full not more then %ld ms... ",
                l__ULONG__WaitTimeInMs
                );



        }
        break;
        // If this is cache full end

        case CN_WAIT_CACHE_FULL_END:
        {
            CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnCallback(): WAIT_CACHE_FULL_END\n" );
            CLogger::Log(User,lmtInformation,"Cache is Full.");
            CLogger::Log(User,lmtInformation,"Burning..." );
        }
        break;
        // If this is synchronize cache begin

        case CN_SYNCHRONIZE_CACHE_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "\nCDevice::BurnCallback(): SYNCHRONIZE_CACHE_BEGIN\n" );
        }
        break;

        // If this is synchronize cache end

        case CN_SYNCHRONIZE_CACHE_END:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): SYNCHRONIZE_CACHE_END\n" );
        }
            break;
        // If this is DVD+RW format begin

        case CN_DVDPLUSRW_FORMAT_BEGIN:
        {
            CLogger::Log(Debug,lmtInformation, "\nCDevice::BurnCallback(): DVDPLUSRW_FORMAT_BEGIN\n" );
        }
            break;
        // If this is DVD+RW format end

        case CN_DVDPLUSRW_FORMAT_END:
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnCallback(): DVDPLUSRW_FORMAT_END\n" );
        }
            break;
        // If this is packet write

        case CN_CDVD_WRITE_PROGRESS:
        {
            // Get parameters passed with callback


            l__LARGE_INTEGER__FileSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__LBsWritten = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            // Calculate number of percent written

            l__LARGE_INTEGER__CurrentPercent.QuadPart =
                ( ( l__LARGE_INTEGER__LBsWritten.QuadPart * 100 ) / l__LARGE_INTEGER__FileSizeInLBs.QuadPart );

            // Check if current calculated was already written   

            if ( l__LARGE_INTEGER__CurrentPercent.QuadPart ==  GetLastWrittenPercent() )
            {
                // Do nothing...
            }
            else
            {
                // Update last written percent with current value
                SetLastWrittenPercent((int)l__LARGE_INTEGER__CurrentPercent.LowPart);

                // Print this percent

                CLogger::Log(Debug,lmtInformation,
                    "%d%% ",
                     GetLastWrittenPercent()
                    );

            }

        }
        break;
    }
}

void __stdcall CDevice::GrabberCallbackDispatch(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackContext,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2)
{
        //      Dispatch callback to form callback handler

        CDevice* device = ( CDevice* ) p__PVOID__CallbackContext;
        device->GrabberCallback(
                        p__CALLBACK_NUMBER,
                        p__PVOID__CallbackSpecial1,
                        p__PVOID__CallbackSpecial2
                        );
}

void CDevice::GrabberCallback(
    IN CALLBACK_NUMBER p__CALLBACK_NUMBER,
    IN PVOID p__PVOID__CallbackSpecial1,
    IN PVOID p__PVOID__CallbackSpecial2
    )
{
    LARGE_INTEGER l__LARGE_INTEGER__TrackSizeInLBs;
    LARGE_INTEGER l__LARGE_INTEGER__LBsReaden;
    LARGE_INTEGER l__LARGE_INTEGER__CurrentPercent;
    LARGE_INTEGER l__LARGE_INTEGER__NumberOfBadBlocks;
    LARGE_INTEGER l__LARGE_INTEGER__NumberOfReadRetries;
    LONG l__LONG__LBA = 0;
    ULONG l__ULONG__NumberOfLBs = 0;
    l__LARGE_INTEGER__NumberOfBadBlocks.QuadPart = 0;
    l__LARGE_INTEGER__NumberOfReadRetries.QuadPart = 0;

    //
    // Process depending of callback number.
    //
    switch ( p__CALLBACK_NUMBER )
    {
        //
        // If this is packet read
        //
        case CN_CDVD_READ_PROGRESS:
        {
            //
            // Get parameters passed with callback
            //

            l__LARGE_INTEGER__TrackSizeInLBs = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial1 );

            l__LARGE_INTEGER__LBsReaden = *( PLARGE_INTEGER )( p__PVOID__CallbackSpecial2 );

            //
            // Calculate number of percent written
            //

            l__LARGE_INTEGER__CurrentPercent.QuadPart =
                ( ( l__LARGE_INTEGER__LBsReaden.QuadPart * 100 ) / l__LARGE_INTEGER__TrackSizeInLBs.QuadPart );

            //
            // Update last written percent with current value
            //

	     SetLastWrittenPercent((int)l__LARGE_INTEGER__CurrentPercent.LowPart);

	    //
	    // Print this percent
	    //
            CLogger::Log(Debug,lmtInformation,
                        "%d%% ",
                        GetLastWrittenPercent()); 

        }
        break;

        //
        // If this is bad block hit
        //
        case CN_CDVD_READ_BAD_BLOCK_HIT:
        {
            //
            // Get parameters passed with the callback
            //

            l__LONG__LBA = *( PLONG )( p__PVOID__CallbackSpecial1 );

            l__ULONG__NumberOfLBs = * ( PULONG )( p__PVOID__CallbackSpecial2 );

            //
            // Update number of bad blocks hit
            //
            l__LARGE_INTEGER__NumberOfBadBlocks.QuadPart += l__ULONG__NumberOfLBs;
            CLogger::Log(Debug,lmtInformation,
                        "\n\nGrabbelCallback:%ld bad blocks at LBA %ld hit, %I64d bad blocks total hit...\n",
                        l__ULONG__NumberOfLBs,
                        l__LONG__LBA,
                        l__LARGE_INTEGER__NumberOfBadBlocks
                );
        }
        break;

        //
        // If this is read retry
        //
        case CN_CDVD_READ_RETRY:
        {
            //
            // Get parameters passed with the callback
            //
            l__LONG__LBA = *( PLONG )( p__PVOID__CallbackSpecial1 );

            //
            // Update number of read retries
            //
            l__LARGE_INTEGER__NumberOfReadRetries.QuadPart++;

            CLogger::Log(Debug,lmtInformation,
                        "\n\nGrabbelCallback:Read retry at LBA %ld, %I64d read retries total...\n",
                        l__LONG__LBA,
                        l__LARGE_INTEGER__NumberOfReadRetries
                        );
        }
        break;

        default:
		break;
    }
}




void CDevice::FindASPIDevices()
{
        InitStarBurnDll();
        Devices.clear();

        StarBurn_FindDevice(
                SCSI_DEVICE_RO_DIRECT_ACCESS,
                FALSE,
                ( PCALLBACK )(&FindDeviceCallback),
	    		NULL
                           );

        CLogger::Log(Debug,lmtInformation, "CDevice::FindASPIDevices(): found %d devices \r\n",Devices.size());
         return;
}


void CDevice::FindSPTIDevices()
{
    InitStarBurnDll();
    Devices.clear();

	CHAR l_ExceptionText[ 1024 ];
    CDB_FAILURE_INFORMATION l__CDB_FAILURE_INFORMATION;
	EXCEPTION_NUMBER l_ExceptionNumber(EN_SUCCESS);
	PVOID l_CdvdBurnerGrabber = NULL;
	ULONG l_Status = 0;	
	long longIndex(0);
	CHAR chDeviceName[ MAX_PATH ];
	CHAR chNtDeviceName[ MAX_PATH ];
	CHAR chDeviceCdRomName[ MAX_PATH ];	
	unsigned long ulNtDeviceNameSizeInUCHARs(0);
	unsigned long ulDeviceCdRomIndex(0);

	//
	//Clear buffers
	//
	ZeroMemory(
		&l_ExceptionText,
		sizeof( l_ExceptionText )
		);

	ZeroMemory(
		&chDeviceName,
		sizeof( chDeviceName )
		);

	ZeroMemory(
		&chNtDeviceName,
		sizeof( chNtDeviceName )
		);

	ZeroMemory(
		&chDeviceCdRomName,
		sizeof( chDeviceCdRomName )
		);

	//
	//Find devices loop
	//
	for ( longIndex = ( long )( 'C' ); longIndex < ( ( long )( 'Z' ) + 1 ); longIndex++ )
	{
		//
		// Generate device name w/o starting "\\.\" prefix and w/o "\" postfix
		//

		sprintf(
			chDeviceName,
			"%C:",
			( CHAR )( longIndex )
			);

		//
		// Try to get device type
		//
		if ( GetDriveType( chDeviceName ) == DRIVE_CDROM ) 
		{

			//
			// Try to query DOS device name
			//
			ulNtDeviceNameSizeInUCHARs =
				QueryDosDevice(
				chDeviceName,
				( PCHAR )( &chNtDeviceName ),
				sizeof( chNtDeviceName )
				); 

			//
			// Check for success
			//
			if ( ulNtDeviceNameSizeInUCHARs > 0 )
			{
				//
				// Check do we have \Device\Cdrom%u string subset
				//
				if ( 
					strstr(
					chNtDeviceName,
					"\\Device\\CdRom"
					)
					)
				{
					//
					// Parse device name parameter
					//
					sscanf(
						chNtDeviceName,
						"\\Device\\CdRom%u",
						&ulDeviceCdRomIndex
						);

					//
					// Generate CdRom style device name
					//
					sprintf(
						chDeviceCdRomName,
						"CdRom%u",
						ulDeviceCdRomIndex
						);

                //
                // Try to construct CD/DVD burner
                //
					l_ExceptionNumber =
						StarBurn_CdvdBurnerGrabber_CreateEx(
							&l_CdvdBurnerGrabber,
							( PCHAR )( &l_ExceptionText ),
							sizeof( l_ExceptionText ),
							&l_Status,
							&l__CDB_FAILURE_INFORMATION,
							( PCALLBACK )(FindDeviceCallback),
							NULL,
							chDeviceCdRomName,
							STARBURN_CACHE_SIZE_READ_ONLY
							);
					//
					// Check for success
					//
					if ( l_ExceptionNumber == EN_SUCCESS )
					{

						if ( l_CdvdBurnerGrabber != NULL )
						{
        					//
	        				// Free allocated memory
		        			//
			       			StarBurn_Destroy( &l_CdvdBurnerGrabber );
							l_CdvdBurnerGrabber = NULL;
	        			}

						CDevice l_Device = CDevice(&chDeviceCdRomName[0]);

		    			CLogger::Log(Debug, lmtInformation,"Found device: <%s>\n",&chDeviceCdRomName[0]);
						Devices.push_back(l_Device);
					}
				}
			}
		}
	}

    CLogger::Log(Debug,lmtInformation, "CDevice::FindSPTIDevices(): found %d devices \r\n",Devices.size());
    return;
}

void CDevice::FindSPTDDevices()
{
	InitStarBurnDll();
	Devices.clear();

	CHAR l_ExceptionText[ 1024 ];
	EXCEPTION_NUMBER l_ExceptionNumber(EN_SUCCESS);
	CDB_FAILURE_INFORMATION l_CDB_FAILURE_INFORMATION;
	ULONG l_Status = 0;
	PVOID l_CdvdBurnerGrabber = NULL;		
	long longIndex(0);
	CHAR chDeviceName[ MAX_PATH ];
	CHAR chNtDeviceName[ MAX_PATH ];
	CHAR chDeviceCdRomName[ MAX_PATH ];	
	WCHAR l_wcsDeviceName[ MAX_PATH ];
	unsigned long ulNtDeviceNameSizeInUCHARs(0);
	unsigned long ulDeviceCdRomIndex(0);

	//
	//Clear buffers
	//

	ZeroMemory(
		&l_ExceptionText,
		sizeof( l_ExceptionText )
		);

	ZeroMemory(
		&chDeviceName,
		sizeof( chDeviceName )
		);

	ZeroMemory(
		&chNtDeviceName,
		sizeof( chNtDeviceName )
		);

	ZeroMemory(
		&chDeviceCdRomName,
		sizeof( chDeviceCdRomName )
		);

	ZeroMemory(
		&l_wcsDeviceName,
		sizeof( l_wcsDeviceName )
		);

	//
	//Find devices loop
	//

	for ( longIndex = ( long )( 'C' ); longIndex < ( ( long )( 'Z' ) + 1 ); longIndex++ )
	{
		//
		// Generate device name w/o starting "\\.\" prefix and w/o "\" postfix
		//

		sprintf(
			chDeviceName,
			"%C:",
			( CHAR )( longIndex )
			);

		//
		// Try to get device type
		//
		if ( GetDriveType( chDeviceName ) == DRIVE_CDROM ) 
		{

			//
			// Try to query DOS device name
			//
			ulNtDeviceNameSizeInUCHARs =
				QueryDosDevice(
				chDeviceName,
				( PCHAR )( &chNtDeviceName ),
				sizeof( chNtDeviceName )
				); 

			//
			// Check for success
			//
			if ( ulNtDeviceNameSizeInUCHARs > 0 )
			{
				//
				// Check do we have \Device\Cdrom%u string subset
				//
				if ( 
					strstr(
					chNtDeviceName,
					"\\Device\\CdRom"
					)
					)
				{
					//
					// Parse device name parameter
					//
					sscanf(
						chNtDeviceName,
						"\\Device\\CdRom%u",
						&ulDeviceCdRomIndex
						);

					//
					// Generate CdRom style device name
					//
					sprintf(
						chDeviceCdRomName,
						"CdRom%u",
						ulDeviceCdRomIndex
						);


					MultiByteToWideChar(
						CP_ACP,
						0,
						&chDeviceCdRomName[0],
						strlen( chDeviceCdRomName ),
						( PWCHAR )( &l_wcsDeviceName[0] ),
						(int)( sizeof( l_wcsDeviceName ) / 2 )
						);


					//
					// Try to construct CD/DVD burner
					//
					l_ExceptionNumber =
						StarBurn_CdvdBurnerGrabber_CreateExEx(
						&l_CdvdBurnerGrabber,
						( PCHAR )( &l_ExceptionText ),
						sizeof( l_ExceptionText ),
						&l_Status,
						&l_CDB_FAILURE_INFORMATION,
						( PCALLBACK )(FindDeviceCallback),
						NULL,
						l_wcsDeviceName,
						FALSE,
						STARBURN_CACHE_SIZE_READ_ONLY
						);
					//
					// Check for success
					//
					if ( l_ExceptionNumber == EN_SUCCESS )
					{

						if ( l_CdvdBurnerGrabber != NULL )
						{
							//
							// Free allocated memory
							//
							StarBurn_Destroy( &l_CdvdBurnerGrabber );
							l_CdvdBurnerGrabber = NULL;
						}

						CDevice l_Device = CDevice(&chDeviceCdRomName[0], SCSI_TRANSPORT_SPTD);

						CLogger::Log(Debug, lmtInformation,"Found device: <%s>\n",&chDeviceCdRomName[0]);
						Devices.push_back(l_Device);
					}
				}
			}
		}
	}

	CLogger::Log(Debug,lmtInformation, "CDevice::FindSPTDDevices(): found %d devices \r\n",Devices.size());
	return;
}

unsigned long CDevice::GetSPTDVersion(unsigned char* pMajorVersion, unsigned char* pMinorVersion)
{
	//
	//Parameters validation
	//
	if (!pMajorVersion ||
		!pMinorVersion) 
	{
		return ERROR_INVALID_PARAMETER;
	}

	*pMajorVersion = 0;
	*pMinorVersion  = 0;

	ULONG SystemError(0);
	EXCEPTION_NUMBER ExceptionNumber(EN_SUCCESS);

	ExceptionNumber = 
		StarBurn_SPTD_GetVersion(pMajorVersion, pMinorVersion, &SystemError);

	//Check success
	if (EN_SUCCESS != ExceptionNumber)
	{
		return SystemError;
	}

	return ERROR_SUCCESS;
}

bool CDevice::RefreshWriteSpeeds()
{
	bool bResult(false);
	bResult = RefreshWriteSpeedsInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshWriteSpeedsInternal()
{
        ULONG l_CurrentReadSpeed(0);
        ULONG l_MaximumReadSpeed(0);
        ULONG l_CurrentWriteSpeed(0);
        ULONG l_MaximumWriteSpeed(0);
        TSpeed l_Speed,l_tempSpeed;

        m_WriteSpeeds.clear();
//Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true))
                         return false;
/*
          // Try to test unit ready
       if(!TestUnitReadyInternal())
               return false;
*/

        //Get current speeds in order to etimate erase time


        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetSpeeds(
                m_CdvdBurnerGrabber,
                ( PCHAR )( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                &l_CurrentReadSpeed,
                &l_MaximumReadSpeed,
                &l_CurrentWriteSpeed,
                &l_MaximumWriteSpeed
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
       CLogger::Log(Debug,
                    lmtInformation,
                    "\n\nGetWriteSpeeds(): StarBurn_CdvdBurnerGrabber_GetSpeeds() failed, exception %ld, status %ld, text '%s'\n",
                    m_ExceptionNumber,
                    m_SystemError,
                    m_ExceptionText
                     );

			
            return false;
       }
       else
       {
          switch(GetMediaType())
             {
				case DISC_TYPE_CDROM:
                case DISC_TYPE_CDR:
                case DISC_TYPE_CDRW:
                case DISC_TYPE_DDCDR:
                case DISC_TYPE_DDCDRW:
                {
                                l_Speed.SpeedKBps = 0;
                                l_tempSpeed.SpeedKBps = 0;
                                TSpeeds::iterator result;
                                for(int i=0; CD_STANDARD_SPEED_COUNT>i ;i++)
                                {

                                // Try check supported speeds by setting this speed
                                        if(m_CDStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
                                        {
                                                        l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
                                                        l_Speed.SpeedKBps = m_CDStandardSpeeds[i].SpeedKBps;
                                                        // Check for supported
                                                        if (TrySetWriteSpeed(&l_Speed))
                                                        {
                                                            for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && CD_STANDARD_SPEED_COUNT>=j+1 ; j++)
                                                            {

                                                                switch (j)
                                                                {
                                                                        case 0:
                                                                        {
                                                                                if(l_Speed.SpeedKBps <= m_CDStandardSpeeds[j].SpeedKBps)
                                                                                {
																					    l_Speed.SpeedX = m_CDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                        }
                                                                        break;
                                                                        default:
                                                                        {
                                                                           if(l_Speed.SpeedKBps >= m_CDStandardSpeeds[j-1].SpeedKBps && l_Speed.SpeedKBps <= m_CDStandardSpeeds[j].SpeedKBps)
                                                                           {

                                                                                if((abs(m_CDStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_CDStandardSpeeds[j].SpeedKBps)))
                                                                                {



                                                                                        l_Speed.SpeedX = m_CDStandardSpeeds[j-1].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }


                                                                                }
                                                                                else
                                                                                {


                                                                                        l_Speed.SpeedX = m_CDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }


                                                                                }
                                                                            }
                                                                        }
                                                                }
                                                           }
                                                     }
                                        }
                                       if(m_CDStandardSpeeds[i].SpeedKBps == l_MaximumWriteSpeed)
                                       {
                                                break; 
                                       }
                                }

                }
                break;
				case DISC_TYPE_DVDROM:
                case DISC_TYPE_DVDR:
                case DISC_TYPE_DVDR_DL:
                case DISC_TYPE_DVDRAM:
                case DISC_TYPE_DVDRWRO:
                case DISC_TYPE_DVDRWSR:
                case DISC_TYPE_DVDPLUSRW:
                case DISC_TYPE_DVDPLUSR:
                case DISC_TYPE_DVDPLUSR_DL:
                {
                                l_Speed.SpeedKBps = 0;
                                l_tempSpeed.SpeedKBps = 0;
								TSpeeds::iterator result;
                                for(int i=0; DVD_STANDARD_SPEED_COUNT>i;i++)
                                {
                                // Try check supported speeds by setting this speed
                                        if(m_DVDStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
                                        {
                                                        l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
                                                        l_Speed.SpeedKBps = m_DVDStandardSpeeds[i].SpeedKBps;
                                                        // Check for supported
                                                        if (TrySetWriteSpeed(&l_Speed))
                                                        {
                                                            for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && DVD_STANDARD_SPEED_COUNT>=j+1 ; j++)
                                                            {

                                                                switch (j)
                                                                {
                                                                        case 0:
                                                                        {
                                                                                if(l_Speed.SpeedKBps <= m_DVDStandardSpeeds[j].SpeedKBps)
                                                                                {
																					    l_Speed.SpeedX = m_DVDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                        }
                                                                        break;
                                                                        default:
                                                                        {
                                                                           if(l_Speed.SpeedKBps >= m_DVDStandardSpeeds[j-1].SpeedKBps &&  l_Speed.SpeedKBps <= m_DVDStandardSpeeds[j].SpeedKBps)
                                                                           {

                                                                                if((abs(m_DVDStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_DVDStandardSpeeds[j].SpeedKBps)))
                                                                                {



                                                                                        l_Speed.SpeedX = m_DVDStandardSpeeds[j-1].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }


                                                                                }
                                                                                else
                                                                                {


                                                                                        l_Speed.SpeedX = m_DVDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
                                                                                        if(result == m_WriteSpeeds.end())
                                                                                        {
                                                                                                m_WriteSpeeds.push_back(l_Speed);
                                                                                        }
                                                                                }
                                                                            }
                                                                        }
                                                                }
                                                           }
                                                     }
                                        }
                                }
                }
                break;				
				case DISC_TYPE_BDR:
				case DISC_TYPE_BDRE:
				case DISC_TYPE_BDROM:
				{
					STARBURN_TRACK_INFORMATION l_TrackInfo;
					RtlZeroMemory(&l_TrackInfo, sizeof(l_TrackInfo));
					GetTrackInfoInternal(TRACK_NUMBER_INVISIBLE, &l_TrackInfo);			

					l_Speed.SpeedKBps = 0;
					l_tempSpeed.SpeedKBps = 0;
					TSpeeds::iterator result;
					for(int i=0; BLURAY_STANDARD_SPEED_COUNT>i;i++)
					{
						//Check on not formated BD-RE disc type
						if(!l_TrackInfo.m__BOOLEAN__IsNextWritableAddressValid && DISC_TYPE_BDRE == GetMediaType() && i)
						{
							break;
						}

						// Try check supported speeds by setting this speed
						if(m_BLURAYStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
						{
							l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
							l_Speed.SpeedKBps = m_BLURAYStandardSpeeds[i].SpeedKBps;

							//Check on not formated BD-RE disc type
							if(!l_TrackInfo.m__BOOLEAN__IsNextWritableAddressValid && DISC_TYPE_BDRE == GetMediaType())
							{
								l_Speed.SpeedKBps = l_MaximumWriteSpeed;
							}
							else
							{
								if (!TrySetWriteSpeed(&l_Speed)) 
								{
									continue;
								}
							}

							for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && BLURAY_STANDARD_SPEED_COUNT>=j+1 ; j++)
							{

								switch (j)
								{
								case 0:
									{
										if(l_Speed.SpeedKBps <= m_BLURAYStandardSpeeds[j].SpeedKBps)
										{
											l_Speed.SpeedX = m_BLURAYStandardSpeeds[j].SpeedX;
											result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
											if(result == m_WriteSpeeds.end())
											{
												m_WriteSpeeds.push_back(l_Speed);
											}

										}
									}
									break;
								default:
									{
										if(l_Speed.SpeedKBps >= m_BLURAYStandardSpeeds[j-1].SpeedKBps &&  l_Speed.SpeedKBps <= m_BLURAYStandardSpeeds[j].SpeedKBps)
										{

											if((abs(m_BLURAYStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_BLURAYStandardSpeeds[j].SpeedKBps)))
											{



												l_Speed.SpeedX = m_BLURAYStandardSpeeds[j-1].SpeedX;
												result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
												if(result == m_WriteSpeeds.end())
												{
													m_WriteSpeeds.push_back(l_Speed);
												}


											}
											else
											{


												l_Speed.SpeedX = m_BLURAYStandardSpeeds[j].SpeedX;
												result = std::find(m_WriteSpeeds.begin(),m_WriteSpeeds.end(),l_Speed);
												if(result == m_WriteSpeeds.end())
												{
													m_WriteSpeeds.push_back(l_Speed);
												}
											}
										}
									}
								}
								
							}
						}
					}
				}
				break;
                case DISC_TYPE_NO_MEDIA:
                {
                        for(int i=0; m_CDStandardSpeeds[i].SpeedKBps <= l_MaximumWriteSpeed && CD_STANDARD_SPEED_COUNT>i; i++)
                        {
                                m_WriteSpeeds.push_back(m_CDStandardSpeeds[i]);
                        }
                }
             }

        }

		
		return true;
}

bool CDevice::GetWriteSpeeds(TSpeeds * Speeds)
{
	if (m_WriteSpeeds.empty())
        {
        	RefreshWriteSpeeds();
        }
        
        Speeds->assign(m_WriteSpeeds.begin(), m_WriteSpeeds.end());
		
        return true;
}

bool CDevice::GetReadSpeeds(TSpeeds * Speeds)
{
	bool bResult(false);
	bResult = GetReadSpeedsInternal(Speeds);
	CleanUp();

	return bResult;
}

bool CDevice::GetReadSpeedsInternal(TSpeeds * Speeds)
{
        ULONG l_CurrentReadSpeed;
        ULONG l_MaximumReadSpeed;
        ULONG l_CurrentWriteSpeed;
        ULONG l_MaximumWriteSpeed;
        TSpeed l_Speed,l_tempSpeed;

         Speeds->clear();

       //Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true))
                         return false;


        //Get current speeds in order to etimate erase time


        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetSpeeds(
                m_CdvdBurnerGrabber,
                ( PCHAR )( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                &l_CurrentReadSpeed,
                &l_MaximumReadSpeed,
                &l_CurrentWriteSpeed,
                &l_MaximumWriteSpeed
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
			CLogger::Log(Debug,
               lmtInformation,
				 "\n\nGetReadSpeeds(): StarBurn_CdvdBurnerGrabber_GetSpeeds() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

			
            return false;
       }
       else
       {
          switch(GetMediaType())
             {
                case DISC_TYPE_CDR:
                case DISC_TYPE_CDRW:
                case DISC_TYPE_CDROM:
                case DISC_TYPE_DDCDROM:
                case DISC_TYPE_DDCDR:
                case DISC_TYPE_DDCDRW:
                {
                                l_Speed.SpeedKBps = 0;
                                l_tempSpeed.SpeedKBps = 0;
                                TSpeeds::iterator result;
                                for(int i=0; CD_STANDARD_SPEED_COUNT>i;i++)
                                {
                                // Try check supported speeds by setting this speed
                                        if(m_CDStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
                                        {
                                                        l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
                                                        l_Speed.SpeedKBps = m_CDStandardSpeeds[i].SpeedKBps;
                                                        // Check for supported
                                                        if (TrySetReadSpeed(&l_Speed))
                                                        {
                                                            for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && CD_STANDARD_SPEED_COUNT>=j+1 ; j++)
                                                            {

                                                                switch (j)
                                                                {
                                                                        case 0:
                                                                        {
                                                                                if(l_Speed.SpeedKBps <= m_CDStandardSpeeds[j].SpeedKBps)
                                                                                {



                                                                                        l_Speed.SpeedX = m_CDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                        }
                                                                        break;
                                                                        default:
                                                                        {
                                                                           if(l_Speed.SpeedKBps >= m_CDStandardSpeeds[j-1].SpeedKBps && l_Speed.SpeedKBps <= m_CDStandardSpeeds[j].SpeedKBps)
                                                                           {

                                                                                if((abs(m_CDStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_CDStandardSpeeds[j].SpeedKBps)))
                                                                                {

                                                                                        l_Speed.SpeedX = m_CDStandardSpeeds[j-1].SpeedX;
                                                                                        result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                                else
                                                                                {


                                                                                        l_Speed.SpeedX = m_CDStandardSpeeds[j].SpeedX;
                                                                                        result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                            }
                                                                        }
                                                                }
                                                            }
                                        }
                                }
                        }
                }
                break;
                case DISC_TYPE_DVDROM:
                case DISC_TYPE_DVDR:
                case DISC_TYPE_DVDR_DL:
                case DISC_TYPE_DVDRAM:
                case DISC_TYPE_DVDRWRO:
                case DISC_TYPE_DVDRWSR:
                case DISC_TYPE_DVDPLUSRW:
                case DISC_TYPE_DVDPLUSR:
                case DISC_TYPE_DVDPLUSR_DL:
                {
                                l_Speed.SpeedKBps = 0;
                                l_tempSpeed.SpeedKBps = 0;
                                for(int i=0; DVD_STANDARD_SPEED_COUNT>i;i++)
                                {
                                // Try check supported speeds by setting this speed
                                        if(m_DVDStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
                                        {
                                                        l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
                                                        l_Speed.SpeedKBps = m_DVDStandardSpeeds[i].SpeedKBps;
                                                        // Check for supported
                                                        if (TrySetReadSpeed(&l_Speed))
                                                        {
                                                            for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && DVD_STANDARD_SPEED_COUNT>=j+1 ; j++)
                                                            {

                                                                switch (j)
                                                                {
                                                                        case 0:
                                                                        {
                                                                                if(l_Speed.SpeedKBps <= m_DVDStandardSpeeds[j].SpeedKBps)
                                                                                {

                                                                                        l_Speed.SpeedX = m_DVDStandardSpeeds[j].SpeedX;
                                                                                        TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                        }
                                                                        break;
                                                                        default:
                                                                        {
                                                                           if(l_Speed.SpeedKBps >= m_DVDStandardSpeeds[j-1].SpeedKBps && l_Speed.SpeedKBps <= m_DVDStandardSpeeds[j].SpeedKBps)
                                                                           {

                                                                                if((abs(m_DVDStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_DVDStandardSpeeds[j].SpeedKBps)))
                                                                                {
                                                                                        l_Speed.SpeedX = m_DVDStandardSpeeds[j-1].SpeedX;
                                                                                        TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                                else
                                                                                {

                                                                                        l_Speed.SpeedX = m_DVDStandardSpeeds[j].SpeedX;
                                                                                        TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
                                                                                        if(result == Speeds->end())
                                                                                        {
                                                                                                Speeds->push_back(l_Speed);
                                                                                        }

                                                                                }
                                                                            }
                                                                        }
                                                                }
                                                           }
                                                     }
                                        }
                                }
                }
                break;
				case DISC_TYPE_BDR:
				case DISC_TYPE_BDRE:
				case DISC_TYPE_BDROM:
					{
						l_Speed.SpeedKBps = 0;
						l_tempSpeed.SpeedKBps = 0;

						STARBURN_TRACK_INFORMATION l_TrackInfo;
						RtlZeroMemory(&l_TrackInfo, sizeof(l_TrackInfo));
						GetTrackInfoInternal(TRACK_NUMBER_INVISIBLE, &l_TrackInfo);					
						

						for(int i=0; BLURAY_STANDARD_SPEED_COUNT>i;i++)
						{
							//Check on not formated BD-RE disc type
							if(!l_TrackInfo.m__BOOLEAN__IsNextWritableAddressValid && DISC_TYPE_BDRE == GetMediaType() && i)
							{
								break;
							}

							// Try check supported speeds by setting this speed
							if(m_BLURAYStandardSpeeds[i].SpeedKBps > l_Speed.SpeedKBps)
							{
								l_tempSpeed.SpeedKBps = l_Speed.SpeedKBps;
								l_Speed.SpeedKBps = m_BLURAYStandardSpeeds[i].SpeedKBps;
								//
								// Check speed supporting
								//

								//Check on not formated BD-RE disc type
								if(!l_TrackInfo.m__BOOLEAN__IsNextWritableAddressValid && DISC_TYPE_BDRE == GetMediaType())
								{
									l_Speed.SpeedKBps = l_MaximumReadSpeed;
								}
								else
								{
									if (!TrySetReadSpeed(&l_Speed)) 
									{
										continue;
									}
								}

								for(int j=1;l_tempSpeed.SpeedKBps != l_Speed.SpeedKBps && BLURAY_STANDARD_SPEED_COUNT>=j+1 ; j++)
								{

									switch (j)
									{
									case 0:
										{
											if(l_Speed.SpeedKBps <= m_BLURAYStandardSpeeds[j].SpeedKBps)
											{

												l_Speed.SpeedX = m_BLURAYStandardSpeeds[j].SpeedX;
												TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
												if(result == Speeds->end())
												{
													Speeds->push_back(l_Speed);
												}

											}
										}
										break;
									default:
										{
											if(l_Speed.SpeedKBps >= m_BLURAYStandardSpeeds[j-1].SpeedKBps && l_Speed.SpeedKBps <= m_BLURAYStandardSpeeds[j].SpeedKBps)
											{

												if((abs(m_BLURAYStandardSpeeds[j-1].SpeedKBps - l_Speed.SpeedKBps)) <= (abs(l_Speed.SpeedKBps - m_BLURAYStandardSpeeds[j].SpeedKBps)))
												{
													l_Speed.SpeedX = m_BLURAYStandardSpeeds[j-1].SpeedX;
													TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
													if(result == Speeds->end())
													{
														Speeds->push_back(l_Speed);
													}

												}
												else
												{

													l_Speed.SpeedX = m_BLURAYStandardSpeeds[j].SpeedX;
													TSpeeds::iterator result = std::find(Speeds->begin(),Speeds->end(),l_Speed);
													if(result == Speeds->end())
													{
														Speeds->push_back(l_Speed);
													}

												}
											}
										}
									}
								}
								
							}
						}
					}
					break;
                case DISC_TYPE_NO_MEDIA:
                {
                        for(int i=0; m_CDStandardSpeeds[i].SpeedKBps <= l_MaximumReadSpeed && CD_STANDARD_SPEED_COUNT>i; i++)
                        {
                                Speeds->push_back(m_CDStandardSpeeds[i]);
                        }
                }
             }

        }
		
        return true;
}


DWORD WINAPI CDevice::SetErasePercentThread(void* pointer)
{
        long l_EraseTime = 0;
        long l_Percent = 0;
        CDevice *device = (CDevice*)(pointer);

        device->SetLastWrittenPercent(0);

        while ( l_EraseTime < device->m_TotalTimeErasing )
        {
                Sleep(1000);
                l_EraseTime ++;
                l_Percent = l_EraseTime * 100 / device->m_TotalTimeErasing;
                device->SetLastWrittenPercent((int)l_Percent);  
        }
		
        return 0;
}

bool CDevice::Erase(TEraseOptions* UserEraseOptions)
{
	bool bResult(false);
	bResult = EraseInternal(UserEraseOptions);
	CleanUp();

	return bResult;
}


bool CDevice::EraseInternal(TEraseOptions* UserEraseOptions)
{

       CLogger::Log(User,lmtInformation,"Srarted.");

       m_EraseOptions = *UserEraseOptions;
//Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
				  
				  //Set 2 parameter in true (read operation) because we don't need cache for erasing				  
                  if(!CdvdBurnerGrabberCreate(&Callback, true, false)) 
                         return false;


          // Try to test unit ready
       CLogger::Log(User,lmtInformation,"Testing unit ready...");

            if(!TestUnitReadyInternal())
                {
					CLogger::Log(User,lmtError,"Unit is not ready!");

					
                    return false;
                }
       CLogger::Log(User,lmtInformation,"Unit ready.");

//Try setting burn speed
       CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::Erase(): Setting speed %s (%ld KBps)... ",
                     m_EraseOptions.Speed.SpeedX,
                     m_EraseOptions.Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     m_EraseOptions.Speed.SpeedX,
                     m_EraseOptions.Speed.SpeedKBps
                     );


        if (TrySetWriteSpeed(&m_EraseOptions.Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            m_EraseOptions.Speed.SpeedX,
                            m_EraseOptions.Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            m_EraseOptions.Speed.SpeedX,
                            m_EraseOptions.Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::Erase():Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }


        CLogger::Log(User,lmtInformation,"Erasing...");

        m_DeviceStatus = Erasing;

        m_TotalTimeErasing = GetTotalTimeErasing(&m_EraseOptions);

       //Execute SetErasePercentThread

        DWORD dwErasePercentThread;
        HANDLE hPErasePercentThread =
                CreateThread(
                        0,
                        2048*1024,
                        SetErasePercentThread,
                        this,
                        0,
                        &dwErasePercentThread
                        );
   // Erase disc
        m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_Blank(
                        m_CdvdBurnerGrabber,
                        ( PCHAR )( &m_ExceptionText ),
                        sizeof( m_ExceptionText ),
                        &m_SystemError,
                        &m_CDBFailureInfo,
                        m_EraseOptions.EraseType
                        );

        // Check for success
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
           CLogger::Log(Debug,lmtError,
                "\nCDevice::Erase(): StarBurn_CdvdBurnerGrabber_Blank() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

			
            return false;
        }

        DWORD dwExitCode(0);
        TerminateThread(hPErasePercentThread,dwExitCode);
        CloseHandle(hPErasePercentThread);

        SetLastWrittenPercent(100);

        m_DeviceStatus = NoAction;
        CLogger::Log(User,lmtInformation,"Erasing Complete");

		
        return true;
}


long CDevice::GetTotalTimeErasing(TEraseOptions* EraseOptions)
{
        LARGE_INTEGER l_TotalBytes;


//Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true))
                         return 0;


          // Try to test unit ready

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_TestUnitReady(
                m_CdvdBurnerGrabber,
                ( PCHAR )( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo
                );


        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
           CLogger::Log(Debug,lmtError,
                "\nCDevice::GetTotalTimeErasing(): StarBurn_CdvdBurnerGrabber_TestUnitReady() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

			
            return 0;
        }
		

        // Try to set  CD/DVD speeds here

        //Try set Speed
         if(TrySetWriteSpeed(&EraseOptions->Speed))
         {
                 switch ( GetMediaType())
                 {

                       	case DISC_TYPE_CDRW:
                        case DISC_TYPE_DDCDRW:
                                if (EraseOptions->EraseType)
                                {
                                       l_TotalBytes.QuadPart = CD_TOC_SIZE_IN_MB * 1024; // ÑD TOC capacity
                                }
                                else
                                {
                                        l_TotalBytes.QuadPart = (LONGLONG)(this->GetMediaSize() + this->GetMediaFreeSize())/1024 + CD_TOC_SIZE_IN_MB * 1024; //CD capacity + TOC capacity
                                }
                        break;

                       	case DISC_TYPE_DVDRWRO:
	        	case DISC_TYPE_DVDRWSR:
		        case DISC_TYPE_DVDRAM:
                        case DISC_TYPE_DVDPLUSRW:
                        {
                                if (!EraseOptions->EraseType)
                                {
                                        l_TotalBytes.QuadPart = DVD_TOC_SIZE_IN_MB * 1024;
                                }
                                else
                                {
                                        l_TotalBytes.QuadPart = (LONGLONG)(this->GetMediaSize()+this->GetMediaFreeSize())/1024 + DVD_TOC_SIZE_IN_MB * 1024;
                                }
                  	}
                  }
         }
         else
         {
				
                return 0;
         }

		
        return  (LONG) l_TotalBytes.QuadPart / (EraseOptions->Speed.SpeedKBps*ERASING_SPEED_CORRECTION_FACTOR) ;
}

bool CDevice::Load()
{
	bool bResult(false);
	bResult = LoadInternal();
	CleanUp();

	return bResult;
}

bool CDevice::LoadInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true, false))
			return 0;

	m_DeviceStatus = Loading;

	m_ExceptionNumber  =
		StarBurn_CdvdBurnerGrabber_Load(
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo
		);
	

	if ( m_ExceptionNumber != EN_SUCCESS )
	{

		CLogger::Log(Debug,lmtError,
			"\n\nCDevice::LoadInternal(): StarBurn_CdvdBurnerGrabber_Load() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
			);

		CLogger::Log(User,lmtError,"Loading Error!");

		m_DeviceStatus = NoAction;
		
		return false;
	}

	RefreshIsDoorOrTrayOpen();

	m_DeviceStatus = NoAction;

	CLogger::Log(User,lmtInformation,"Load");

	
	return true;
}

bool CDevice::Eject()
{
	bool bResult(false);
	bResult = EjectInternal();
	CleanUp();

	return bResult;
}
bool CDevice::EjectInternal()
{
//Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true, false))
                         return 0;

        m_DeviceStatus = Ejecting;
        CLogger::Log(User,lmtInformation,"Eject");
            m_ExceptionNumber  =
                    StarBurn_CdvdBurnerGrabber_Eject(
                    m_CdvdBurnerGrabber,
                    ( PCHAR )( &m_ExceptionText ),
                    sizeof( m_ExceptionText ),
                    &m_SystemError,
                    &m_CDBFailureInfo
                        );        

        if ( m_ExceptionNumber != EN_SUCCESS )
        {

                CLogger::Log(Debug,lmtError,
                           "\n\nCDevice::EjectInternal()): StarBurn_CdvdBurnerGrabber_Eject() failed, exception %ld, status %ld, text '%s'\n",
                           m_ExceptionNumber,
                           m_SystemError,
                           m_ExceptionText
                            );

                CLogger::Log(User,lmtError,"Ejecting Error!");

				m_DeviceStatus = NoAction;

				
                return false;
        }

		RefreshIsDoorOrTrayOpen();

		m_DeviceStatus = NoAction;

        return true;
}

bool CDevice::RefreshIsDoorOrTrayOpen()
{
	bool bResult(false);
	bResult = RefreshIsDoorOrTrayOpenInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshIsDoorOrTrayOpenInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true))
			return 0;

	BOOLEAN l_IsMediaPresent(FALSE);
	BOOLEAN l_IsDoorOrTrayOpen(FALSE);

	m_ExceptionNumber  =
		StarBurn_CdvdBurnerGrabber_GetMediaTrayStatus(
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo,
		&l_IsMediaPresent,
		&l_IsDoorOrTrayOpen
		);	

	if ( m_ExceptionNumber != EN_SUCCESS )
	{

		CLogger::Log(Debug,lmtError,
			"\n\nCDevice::RefreshIsDoorOrTrayOpenInternal(): StarBurn_CdvdBurnerGrabber_GetMediaTrayStatus() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
			);

		CLogger::Log(User,lmtError,"Get media tray status Error!");

		
		return false;
	}

	m_IsDoorOrTrayOpen = l_IsDoorOrTrayOpen == TRUE ? true:false;

	
	return true;
}

bool CDevice::IsDoorOrTrayOpen()
{
	return m_IsDoorOrTrayOpen;
}


void CDevice::CancelRWOperation()
{
	CancelRWOperationInternal();
	return;
}

void CDevice::CancelRWOperationInternal()
{

	if (m_CdvdBurnerGrabber)
	{
		m_DeviceStatus = Canceling;
		StarBurn_CdvdBurnerGrabber_Cancel(m_CdvdBurnerGrabber);
	}

	
	return;
}

bool CDevice::InitStarBurnDll()
{

            EXCEPTION_NUMBER l_ExceptionNumber = 
				StarBurn_UpStartEx(
				( PVOID )( &g__UCHAR__RegistrationKey ),
				sizeof( g__UCHAR__RegistrationKey )
				);

         if ( l_ExceptionNumber != EN_SUCCESS )
         {
                 CLogger::Log(User,lmtError,"Can't UpStartEx StarBurn");

                 CLogger::Log(Debug,lmtError,
                 "\n\nCDevice::InitStarBurnDll():StarBurn_UpStartEx() failed, exception %ld \n",
                 l_ExceptionNumber
                  );
                return false;
           }

         return true;
}

bool CDevice::GetTrackInfo(unsigned char Track, STARBURN_TRACK_INFORMATION *pTrackInformation)
{
	bool bResult(false);
	bResult = GetTrackInfoInternal(Track, pTrackInformation);
	CleanUp();

	return bResult;
}

bool CDevice::GetTrackInfoInternal(unsigned char Track, STARBURN_TRACK_INFORMATION *l_TrackInformation)
{

        //Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true))
                         return 0;


        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetTrackInformation(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 Track,
                 l_TrackInformation
                 );

        //
        // Check for success
        //

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(
                Debug,
                lmtError,
                "\n\nCDevice::GetTrackInfo: StarBurn_CdvdBurnerGrabber_GetTrackInformation() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            //
            // Get out of here
            //

			
            return false;
        }

		
        return true;
}

bool CDevice::BurnDiscFromImage(TBurnOptions* BurnOptions,char* ImageFilePath, char* DiscTitle, char * DiscDescription)
{
	bool bResult(false);
	bResult = BurnDiscFromImageInternal(
				BurnOptions, 
				ImageFilePath, 
				DiscTitle, 
				DiscDescription);
	CleanUp();

	return bResult;
}

bool CDevice::BurnDiscFromImageInternal(TBurnOptions* BurnOptions,char* ImageFilePath, char* DiscTitle, char * DiscDescription)
{
    bool l_IsBUPEnabled;

    bool l_IsBUPSupported;

    bool l_IsSendOPCSuccessful = FALSE;

    bool l_IsTrackAtOnce = FALSE;

    bool l_IsSessionAtOnce = FALSE;

    bool l_IsDiscAtOncePQ = FALSE;

    bool l_IsDiscAtOnceRawPW = FALSE;

    DAO_DISC_LAYOUT l_DAODiscLayout;

	BOOLEAN l_TestWrite = BurnOptions->TestWrite ? TRUE:FALSE;

    DISC_LAYOUT l_DiscLayout;

    CLogger::Log(User,lmtInformation,"Started");


   //Create CdvdBurnerGrabber object if need
    if(!CdvdBurnerGrabberCreate(&DefaultBurnCallbackDispatch, false, true))
                  {
                          CLogger::Log(User,lmtError,"Started Error!");
                         return 0;
                  }

        CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Probing supported write modes... " );
        CLogger::Log(User,lmtInformation,"Probing supported write modes...");

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 (PBOOLEAN)&l_IsTrackAtOnce,
                 (PBOOLEAN)&l_IsSessionAtOnce,
                 (PBOOLEAN)&l_IsDiscAtOncePQ,
                 (PBOOLEAN)&l_IsDiscAtOnceRawPW
                 );

        //
        // Check for success
        //
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );


             CLogger::Log(User,lmtError,"Probing supported write modes Error!");

            //
            // Get out of here
            //

			
            return false;
        }


        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnDiscFromImage(): Track-At-Once: %s, Session-At-Once: %s, Disc-At-Once PQ: %s, Disc-At-Once raw P-W: %s\n",
            l_IsTrackAtOnce ? "Yes" : "No",
            l_IsSessionAtOnce ? "Yes" : "No",
            l_IsDiscAtOncePQ ? "Yes" : "No",
            l_IsDiscAtOnceRawPW ? "Yes" : "No"
            );

        //
		//Check is supported selected mode for burning
		//

        CLogger::Log(User,lmtInformation,"Check is supported selected mode for burning");

        switch(BurnOptions->WriteMethod)
		{
		// Check DAO raw P-W mode
		case WRITE_MODE_DISC_AT_ONCE_RAW_PW:
			{
				if(!l_IsDiscAtOnceRawPW	)
				{
					CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnDiscFromImage): Disc-At-Once raw P-W are unsupported!!!\n" );
					CLogger::Log(User,lmtError,"Disc-At-Once raw P-W  not supported!");
					
					
					return false;                                                      			
				}
			}
			break;
		// Check DAO PQ mode
		case WRITE_MODE_DISC_AT_ONCE_PQ:
			{
				if(!l_IsDiscAtOncePQ)
				{
					CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnDiscFromImage): Disc-At-Once PQ are unsupported!!!\n" );
					CLogger::Log(User,lmtError,"Disc-At-Once PQ not supported!");

					
					return false;
				}
			}
			break;
			// Check  TAO mode
		case WRITE_MODE_TRACK_AT_ONCE:
			{
				if(!l_IsTrackAtOnce)
				{
		            CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnDiscFromImage): Mode Track-At-Once is unsupported!!!\n" );
		            CLogger::Log(User,lmtError,"Mode Track-At-Once is unsupported!!!");
					
					return false;
				}
			}
			break;
			// Check SAO mode
		case WRITE_MODE_SESSION_AT_ONCE:
			{
				if(!l_IsSessionAtOnce)
				{
		            CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnDiscFromImage): Mode Session-At-Once is unsupported!!!\n" );
		            CLogger::Log(User,lmtError,"Mode Session-At-Once is unsupported!!!");
					
					return false;
				}
			}
			break;
		}

         CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Testing unit ready... " );
         CLogger::Log(User,lmtInformation,"Testing unit ready...");

        // Try to test unit ready

        if(!TestUnitReadyInternal())
        {
                  CLogger::Log(User,lmtError,"Unit is not ready!");

				  
                  return false;
        }
        else
        {
                  CLogger::Log(User,lmtInformation,"Unit is ready!");
        }


        CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnDiscFromImage(): Getting BUP (Buffer Underrun Protection) support... " );

        CLogger::Log(User,lmtInformation,"Getting BUP (Buffer Underrun Protection) support...");

        // Try to get BUP here

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                (PBOOLEAN)&l_IsBUPEnabled,
                (PBOOLEAN)&l_IsBUPSupported
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnDiscFromImage()): StarBurn_CdvdBurnerGrabber_GetBUP() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Getting BUP failed!");

			
            return false;
        }

        CLogger::Log(User,lmtInformation,"Getting BUP successfully!");

        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnDiscFromImage(): BUP (Buffer Underrun Protection) Enabled: %s, Supported: %s\n",
            l_IsBUPEnabled ? "Yes" : "No",
            l_IsBUPSupported ? "Yes" : "No"
            );

        // Check if the BUP supported try to enable it (for now we'll try to enable it ALWAYS)

        if ( /* l__BOOLEAN__IsBUPSupported == */ true )
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Enabling BUP (Buffer Underrun Protection)... " );
            CLogger::Log(User,lmtInformation,"Enabling BUP (Buffer Underrun Protection)...");

            // Try to set BUP status

            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 true
                    );

            // Check for success

            if ( m_ExceptionNumber != EN_SUCCESS )
            {
                CLogger::Log(Debug,lmtWarning,
                    "Failed!\nCDevice::BurnDiscFromImage(): WARNING! StarBurn_CdvdBurnerGrabber_SetBUP() failed, exception %ld, status %ld, text '%s'\n",
                        m_ExceptionNumber,
                        m_SystemError,
                        m_ExceptionText
                    );
               CLogger::Log(User,lmtInformation,"Enabling BUP failed!");


            }
            else
	    {
	       CLogger::Log(Debug,lmtInformation, "OK!\n" );
               CLogger::Log(User,lmtInformation,"Enabling BUP successfully!");
	    }
        }

        CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::BurnDiscFromImage(): Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );

        //Try setting burn speed
        if (TrySetWriteSpeed(&BurnOptions->Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::BurnDiscFromImage():Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }



        // Check is this test burn, if yes we do not need to send OPC as some recorders do not like sending OPC in
        // simulation mode

        if ( BurnOptions->OPC)
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Sending OPC (Optimum Power Calibration)... " );
            CLogger::Log(User,lmtInformation,"Sending OPC (Optimum Power Calibration)..." );

            // Set flag OPC was successful, it will be reset in case of exception

            l_IsSendOPCSuccessful = TRUE;

            // Try to send OPC
            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SendOPC(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo
                    );

            // Check for success
            if ( m_ExceptionNumber != EN_SUCCESS )
            {
               CLogger::Log(Debug,lmtError, "Failed!\n" );
               CLogger::Log(User,lmtError,"Sending OPC Error!" );

                // Reset the flag OPC was successful

                l_IsSendOPCSuccessful = FALSE;

            }

            // Check was OPC successful

            if ( l_IsSendOPCSuccessful == TRUE )
            {
                CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnDiscFromImage(): Writing the stuff to the CD/DVD disc >>>\n" );
                CLogger::Log(User,lmtInformation,"Sending OPC successfully." );

            }
            else
            {
                CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Writing the stuff to the CD/DVD disc >>>\n" );
            }
        }
        else
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Skipping send OPC (Optimum Power Calibration) in test mode... OK!\n" );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDiscFromImage(): Writing the stuff (simulating) to the CD/DVD disc >>>\n" );
            CLogger::Log(User,lmtInformation,"Writing the stuff (simulating) to the CD/DVD disc" );
        }


	if (BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_RAW_PW ||
            BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_PQ)
	{
		//
		// Prepare DAO disc layout
		//
		RtlZeroMemory( &l_DAODiscLayout, sizeof(l_DAODiscLayout) );

		//
		// Process single file
		//
		l_DAODiscLayout.m__LONG__NumberOfEntries = 1;
		strcpy(l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName,
			ImageFilePath);

                CLogger::Log(Debug,lmtInformation,
                                "CDevice::BurnDiscFromImage(): Processing file <%s>\n" ,
                                l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName
                                );

                CLogger::Log(User,lmtInformation,
                                "Processing file <%s>",
                                l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName
                                 );
	}
	else if (BurnOptions->WriteMethod == WRITE_MODE_SESSION_AT_ONCE)
	{
		//
		// Prepare disc layout
		//
		RtlZeroMemory( &l_DiscLayout, sizeof(l_DiscLayout) );

		//
		// Process single file
		//
		l_DiscLayout.m__LONG__NumberOfEntries = 1;
		strcpy(l_DiscLayout.m__DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName,
			ImageFilePath);

                CLogger::Log(Debug,lmtInformation,
                                "CDevice::BurnDiscFromImage(): Processing file <%s>\n" ,
                                l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName
                                );

                CLogger::Log(User,lmtInformation,
                                "Processing file <%s>",
                                l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[0].m__CHAR__TrackName
                                 );
	}

	//
	// Try to set CD-Text information for the disc
	//
	m_ExceptionNumber =
	StarBurn_CdvdBurnerGrabber_SetCDTextItem(
		m_CdvdBurnerGrabber,
		0,
		DiscTitle,
		DiscDescription,
		(CHAR*)(&m_ExceptionText  ),
		sizeof(m_ExceptionText),
		&m_SystemError
	);

	 // Check for success
            if (m_ExceptionNumber != EN_SUCCESS)
            {
                CLogger::Log(Debug,lmtError,
                    "CDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_SetCDTextItem(0) failed, exception %d, status %d, text '%s'",
                    m_ExceptionNumber,
                    m_SystemError,
                    m_ExceptionText
                    );

                // Get out of here
				
                return false;
            }



	ZeroMemory(&m_ExceptionText, sizeof(m_ExceptionText));
	ZeroMemory(&m_CDBFailureInfo, sizeof(m_CDBFailureInfo));

        m_DeviceStatus = Burning;

	if (BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_RAW_PW)
	{
		
        //
		// Try to burn  CDDA in Disc-At-Once raw P-W mode
		//
		m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_DiscAtOnceRawPWFromFile(
			m_CdvdBurnerGrabber,
			( PCHAR )( &m_ExceptionText ),
			sizeof( m_ExceptionText ),
			&m_SystemError,
			&m_CDBFailureInfo,
			&l_DAODiscLayout,
			FALSE, // TRUE -- MODE2/Form1 vs FALSE -- MODE1
			l_TestWrite,
			TRUE, // Next session allowed
			FALSE, // Do NOT repair broken subchannel
			WRITE_REPORT_DELAY_IN_SECONDS,
			BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
		);

                //
                // Check for success
                //
                if (Canceling != m_DeviceStatus)
                {
                        m_DeviceStatus = NoAction;

                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                            CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_DiscAtOnceRawPWFromFile() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                            CLogger::Log(User,lmtError,"Burning Error!" );
							
                            return false;
                        }
                        else
                        {               
                            SetLastWrittenPercent(100);
                            CLogger::Log(User,lmtInformation,"Burning Complete" );

							
                            return true;
                        }
                }
                else
                {
                        m_DeviceStatus = NoAction;

						
                        return true;
                }
        }
        else if(BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_PQ)
	{
			
			//
       		// Try to burn CDDA in Disc-At-Once PQ mode
       		//
       		m_ExceptionNumber =
       		StarBurn_CdvdBurnerGrabber_DiscAtOncePQFromFile(
       			m_CdvdBurnerGrabber,
       			(char*)( &m_ExceptionText ),
       			sizeof( m_ExceptionText ),
       			&m_SystemError,
       			&m_CDBFailureInfo,
       			&l_DAODiscLayout,
       			FALSE, // TRUE -- MODE2/Form1 vs FALSE -- MODE1
       			l_TestWrite,
       			TRUE,  // Next session allowed
       			FALSE, // Do NOT repair broken subchannel
       			WRITE_REPORT_DELAY_IN_SECONDS,
       			BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
       		);

                //
                // Check for success
                //
                if (Canceling != m_DeviceStatus)
                {
                        m_DeviceStatus = NoAction;

                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                            CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_DiscAtOncePQFromFile() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                            CLogger::Log(User,lmtError,"Burning Error!" );
							
                            return false;
                        }
                        else
                        {
                            SetLastWrittenPercent(100);
                            CLogger::Log(User,lmtInformation,"Burning Complete" );

							
                            return true;
                        }
                }
                else
                {
                    m_DeviceStatus = NoAction;

					
					return true;
                }
        }
	else if (BurnOptions->WriteMethod == WRITE_MODE_SESSION_AT_ONCE)
	{
				
                //
                // Try to write the ISO image to the disc
                //
                m_ExceptionNumber =
		        StarBurn_CdvdBurnerGrabber_SessionAtOnce(
			        m_CdvdBurnerGrabber,
        			(char*)( &m_ExceptionText ),
	        		sizeof( m_ExceptionText ),
		        	&m_SystemError,
			        &m_CDBFailureInfo,
        			&l_DiscLayout,
	        		FALSE, //TRUE
		        	l_TestWrite,
			        TRUE,  // Next session allowed
        			WRITE_REPORT_DELAY_IN_SECONDS,
	        		BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
		        );

      	    // Check for success
                if (Canceling != m_DeviceStatus)
                {
                        m_DeviceStatus = NoAction;

                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                            CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_DiscAtOncePQ[RawPW]FromFile() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                            CLogger::Log(User,lmtError,"Burning Error!" );

							
                            return false;
                        }
                        else
                        {
                            SetLastWrittenPercent(100);
                            CLogger::Log(User,lmtInformation,"Burning Complete" );

							
                            return true;
                        }
                }
                else
                {
                        m_DeviceStatus = NoAction;

						
                        return true;
                }
        }
	else if (BurnOptions->WriteMethod == WRITE_MODE_TRACK_AT_ONCE)
        {
				
                //
                // Try to write the ISO image to the disc
                //
                m_ExceptionNumber =
	        	StarBurn_CdvdBurnerGrabber_TrackAtOnceFromFile(
		        	m_CdvdBurnerGrabber,
			        (char*)( &m_ExceptionText ),
        			sizeof( m_ExceptionText ),
	        		&m_SystemError,
		        	&m_CDBFailureInfo,
			        ImageFilePath,
        			FALSE,	// No CD-ROM XA
	        		l_TestWrite,
		        	TRUE,	// Next session allowed
			        WRITE_REPORT_DELAY_IN_SECONDS,
        			BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
	        	);
		          	    // Check for success
                if (Canceling != m_DeviceStatus)
                {
                        m_DeviceStatus = NoAction;

                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                            CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_DiscAtOncePQ[RawPW]FromFile() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                            CLogger::Log(User,lmtError,"Burning Error!" );

							
                            return false;
                        }
                        else
                        {
                            SetLastWrittenPercent(100);
                        }
                }
                else
                {
                        m_DeviceStatus = NoAction;

						
                        return true;
                }

				if(!BurnOptions->TestWrite)
				{

					CLogger::Log(User,lmtInformation,"Closing session...");

	        		//
					// Try to close the session
        			//
	        		m_ExceptionNumber =
		        		StarBurn_CdvdBurnerGrabber_CloseSession(
						m_CdvdBurnerGrabber,
        				(char*)( &m_ExceptionText ),
	        			sizeof( m_ExceptionText ),
		        		&m_SystemError,
						&m_CDBFailureInfo
        				);

	        		//
					// Check for success
        			//
	        		if ( m_ExceptionNumber != EN_SUCCESS )
					{
        				CLogger::Log(Debug,lmtError,
	        				"\n\nCDevice::BurnDiscFromImage(): StarBurn_CdvdBurnerGrabber_CloseSession() failed, exception %ld, status %ld, text '%s'\n",
		        			m_ExceptionNumber,
			        		m_SystemError,
							m_ExceptionText
        					);

						CLogger::Log(User,lmtError,"Could not close session");
						
						
		        		return false;
        			}
					else
					{
						CLogger::Log(User,lmtInformation,"Session closed");
					}
				}

                CLogger::Log(User,lmtInformation,"Burning Complete" );

				
                return true;
        }

	
    return true;
}

bool CDevice::BurnAudioCD(
                TBurnOptions* pBurnOptions,
                TAudioDiscLayout* pAudioDiscLayout,
                char* pCDTextTitle, /*="Audio CD"*/
                char* pCDTextArtist /*="Unknown Artist"*/
                )
{
	bool bResult(false);
	bResult = BurnAudioCDInternal(
                        pBurnOptions,
                        pAudioDiscLayout,
                        pCDTextTitle,
                        pCDTextArtist) ;
	CleanUp();

	return bResult;
}

bool CDevice::BurnAudioCDInternal(
                TBurnOptions* BurnOptions,
                TAudioDiscLayout* AudioDiscLayout,
                char* pCDTextTitle,
                char* pCDTextArtist)
{

    bool l_IsBUPEnabled;

    bool l_IsBUPSupported;

    bool l_IsSendOPCSuccessful = FALSE;

    bool l_IsTrackAtOnce = FALSE;

    bool l_IsSessionAtOnce = FALSE;

    bool l_IsDiscAtOncePQ = FALSE;

    bool l_IsDiscAtOnceRawPW = FALSE;

    DAO_DISC_LAYOUT l_DAODiscLayout;
    DISC_LAYOUT l_DiscLayout;

	BOOLEAN l_TestWrite = BurnOptions->TestWrite ? TRUE:FALSE;

    CLogger::Log(User,lmtInformation,"Started");

   //Create CdvdBurnerGrabber object if need
    if(!CdvdBurnerGrabberCreate(&DefaultBurnCallbackDispatch, false, true))
	{
		CLogger::Log(User,lmtError,"Started Error!");		
		return 0;
	}

        CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Probing supported write modes... " );
        CLogger::Log(User,lmtInformation,"Probing supported write modes...");

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 (PBOOLEAN)&l_IsTrackAtOnce,
                 (PBOOLEAN)&l_IsSessionAtOnce,
                 (PBOOLEAN)&l_IsDiscAtOncePQ,
                 (PBOOLEAN)&l_IsDiscAtOnceRawPW
                 );

        //
        // Check for success
        //
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );


             CLogger::Log(User,lmtError,"Probing supported write modes Error!");

            //
            // Get out of here
            //
              
			
            return false;
        }


        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnAudioCD(): Track-At-Once: %s, Session-At-Once: %s, Disc-At-Once PQ: %s, Disc-At-Once raw P-W: %s\n",
            l_IsTrackAtOnce ? "Yes" : "No",
            l_IsSessionAtOnce ? "Yes" : "No",
            l_IsDiscAtOncePQ ? "Yes" : "No",
            l_IsDiscAtOnceRawPW ? "Yes" : "No"
            );

        // Check do we have Disc-At-Once PQ or Disc-At-Once raw P-W supported

        CLogger::Log(User,lmtInformation,"Check do we have Disc-At-Once PQ or Disc-At-Once raw P-W supported");

        if (
            ( l_IsDiscAtOncePQ == FALSE ) &&
            ( l_IsDiscAtOnceRawPW == FALSE )
        )
        {

            CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnAudioCD(): Neither Disc-At-Once PQ nor Disc-At-Once raw P-W are unsupported!!!\n" );
            CLogger::Log(User,lmtError,"Disc-At-Once PQ and Disc-At-Once raw P-W  not supported!");

			
            return false;
        }
        if(l_IsDiscAtOncePQ)
        {
             CLogger::Log(User,lmtInformation,"Disc-At-Once PQ supported.");
        }
        if(l_IsDiscAtOnceRawPW)
        {
             CLogger::Log(User,lmtInformation,"Disc-At-Once raw P-W supported.");
        }

         CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Testing unit ready... " );
         CLogger::Log(User,lmtInformation,"Testing unit ready...");

        // Try to test unit ready

        if(!TestUnitReadyInternal())
        {
			CLogger::Log(User,lmtError,"Unit is not ready!");
			
			return false;
        }
        else
        {
			CLogger::Log(User,lmtInformation,"Unit is ready!");
        }

        CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnAudioCD(): Getting BUP (Buffer Underrun Protection) support... " );

        CLogger::Log(User,lmtInformation,"Getting BUP (Buffer Underrun Protection) support...");

        // Try to get BUP here

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                (PBOOLEAN)&l_IsBUPEnabled,
                (PBOOLEAN)&l_IsBUPSupported
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnAudioCD()): StarBurn_CdvdBurnerGrabber_GetBUP() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Getting BUP failed!");

			                  
            return false;
        }

        CLogger::Log(User,lmtInformation,"Getting BUP successfully!");

        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnAudioCD(): BUP (Buffer Underrun Protection) Enabled: %s, Supported: %s\n",
            l_IsBUPEnabled ? "Yes" : "No",
            l_IsBUPSupported ? "Yes" : "No"
            );

        // Check if the BUP supported try to enable it (for now we'll try to enable it ALWAYS)

        if ( /* l__BOOLEAN__IsBUPSupported == */ true )
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Enabling BUP (Buffer Underrun Protection)... " );
            CLogger::Log(User,lmtInformation,"Enabling BUP (Buffer Underrun Protection)...");

            // Try to set BUP status

            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 true
                    );

            // Check for success

            if ( m_ExceptionNumber != EN_SUCCESS )
            {
                CLogger::Log(Debug,lmtWarning,
                    "Failed!\nCDevice::BurnAudioCD(): WARNING! StarBurn_CdvdBurnerGrabber_SetBUP() failed, exception %ld, status %ld, text '%s'\n",
                        m_ExceptionNumber,
                        m_SystemError,
                        m_ExceptionText
                    );
               CLogger::Log(User,lmtInformation,"Enabling BUP failed!");


            }
            else
	    {
	       CLogger::Log(Debug,lmtInformation, "OK!\n" );
               CLogger::Log(User,lmtInformation,"Enabling BUP successfully!");
	    }
        }

        CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::BurnAudioCD(): Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );

        //Try setting burn speed
        if (TrySetWriteSpeed(&BurnOptions->Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::BurnAudioCD():Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }



        // Check is this test burn, if yes we do not need to send OPC as some recorders do not like sending OPC in
        // simulation mode

        if ( BurnOptions->OPC)
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Sending OPC (Optimum Power Calibration)... " );
            CLogger::Log(User,lmtInformation,"Sending OPC (Optimum Power Calibration)..." );

            // Set flag OPC was successful, it will be reset in case of exception

            l_IsSendOPCSuccessful = TRUE;

            // Try to send OPC
            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SendOPC(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo
                    );

            // Check for success
            if ( m_ExceptionNumber != EN_SUCCESS )
            {
               CLogger::Log(Debug,lmtError, "Failed!\n" );
               CLogger::Log(User,lmtError,"Sending OPC Error!" );

                // Reset the flag OPC was successful

                l_IsSendOPCSuccessful = FALSE;

            }

            // Check was OPC successful

            if ( l_IsSendOPCSuccessful == TRUE )
            {
                CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnAudioCD(): Writing the stuff to the CD/DVD disc >>>\n" );
                CLogger::Log(User,lmtInformation,"Sending OPC successfully." );

            }
            else
            {
                CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Writing the stuff to the CD/DVD disc >>>\n" );
            }
        }
        else
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Skipping send OPC (Optimum Power Calibration) in test mode... OK!\n" );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnAudioCD(): Writing the stuff (simulating) to the CD/DVD disc >>>\n" );
            CLogger::Log(User,lmtInformation,"Writing the stuff (simulating) to the CD/DVD disc" );
        }

        // Reset last written percent to zero
        SetLastWrittenPercent(0);

        m_DeviceStatus = Burning;

        switch(BurnOptions->WriteMethod)
        {
        case WRITE_MODE_DISC_AT_ONCE_RAW_PW:
        case WRITE_MODE_DISC_AT_ONCE_PQ:
                {
                        // Prepare DAO disc layout
                        RtlZeroMemory(
                            &l_DAODiscLayout,
                            sizeof(l_DAODiscLayout)
                            );

                        // Filling Disk Layout

                        l_DAODiscLayout.m__LONG__NumberOfEntries = AudioDiscLayout->NumberOfTracks;
                        for ( int i = 0; i<AudioDiscLayout->NumberOfTracks; i++ )
                        {

                            strcpy(l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[i].m__CHAR__TrackName,
                            AudioDiscLayout->AudioTrackLayout[i].TrackName);

                            CLogger::Log(Debug,lmtSubInformation,
                                "CDevice::BurnAudioCD(): Processing file <%s>, %ld file from %ld file(s) total\n",
                                l_DAODiscLayout.m__DAO_DISC_LAYOUT_ENTRY[i].m__CHAR__TrackName,
                                (i+1),
                                (l_DAODiscLayout.m__LONG__NumberOfEntries)
                                );


                            // Try to set CD-Text information for current item
                            m_ExceptionNumber =
                                StarBurn_CdvdBurnerGrabber_SetCDTextItem(
                                      m_CdvdBurnerGrabber,
                                      (char)( i + 1 ),
                                      AudioDiscLayout->AudioTrackLayout[i].TrackCdTextArtist,
                                      AudioDiscLayout->AudioTrackLayout[i].TrackCdTextTitle,
                                      (char* )( &m_ExceptionText ),
                                      sizeof( m_ExceptionText ),
                                      &m_SystemError
                                      );

                            // Check for success
                            if (m_ExceptionNumber != EN_SUCCESS)
                            {
                                CLogger::Log(Debug,lmtError,
                                    "CDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_SetCDTextItem(%d) failed, exception %d, status %d, text '%s'",
                                    (i),
                                    m_ExceptionNumber,
                                    m_SystemError,
                                    m_ExceptionText
                                    );

                                // Get out of here
								
                                return false;
                            }


                        }

                        // Try to set CD-Text information for the disc
                       m_ExceptionNumber =
                            StarBurn_CdvdBurnerGrabber_SetCDTextItem(
                                m_CdvdBurnerGrabber,
                                0,
                                pCDTextTitle,
                                pCDTextArtist,
                                (char*)(&m_ExceptionText  ),
                                sizeof(m_ExceptionText),
                                &m_SystemError
                                );

                        // Check for success
                        if (m_ExceptionNumber != EN_SUCCESS)
                        {
                            CLogger::Log(Debug,lmtError,
                                "CDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_SetCDTextItem(0) failed, exception %d, status %d, text '%s'",
                                    m_ExceptionNumber,
                                    m_SystemError,
                                    m_ExceptionText
                                );

                            // Get out of here
							
                            return false;
                        }

                        // Check do we have Disc-At-Once raw P-W supported (try to burn in DAO96 if both DAO96 and DAO16 are available)

                        if ( BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_RAW_PW )
                        {
							
                            // Try to burn  CDDA (CD digital audio) in Disc-At-Once raw P-W mode
                            m_ExceptionNumber =
                                StarBurn_CdvdBurnerGrabber_DiscAtOnceRawPWFromFile(
                                        m_CdvdBurnerGrabber,
                                        ( PCHAR )( &m_ExceptionText ),
                                        sizeof( m_ExceptionText ),
                                        &m_SystemError,
                                        &m_CDBFailureInfo,
                                        &l_DAODiscLayout,
                                        false, // TRUE -- MODE2/Form1 vs FALSE -- MODE1
                                        l_TestWrite,
                                        false, // TRUE, // Next session allowed
                                        false, // Do NOT repair broken subchannel
                                        WRITE_REPORT_DELAY_IN_SECONDS,
                                        BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
                                        );

                        }
                        else
                        if(BurnOptions->WriteMethod == WRITE_MODE_DISC_AT_ONCE_PQ)
                        {
							
                            // Try to burn CDDA (CD digital audio) in Disc-At-Once PQ mode
                            m_ExceptionNumber =
                                StarBurn_CdvdBurnerGrabber_DiscAtOncePQFromFile(
                                        m_CdvdBurnerGrabber,
                                        (char*)( &m_ExceptionText ),
                                        sizeof( m_ExceptionText ),
                                        &m_SystemError,
                                        &m_CDBFailureInfo,
                                        &l_DAODiscLayout,
                                        false, // TRUE -- MODE2/Form1 vs FALSE -- MODE1
                                        l_TestWrite,
                                        false, // TRUE, // Next session allowed
                                        false, // Do NOT repair broken subchannel
                                        WRITE_REPORT_DELAY_IN_SECONDS,
                                        BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
                                        );
                        }

                        // Check for success
                        if (Canceling != m_DeviceStatus)
                        {
                                m_DeviceStatus = NoAction;

                                if ( m_ExceptionNumber != EN_SUCCESS )
                                {
                                    CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_DiscAtOncePQ[RawPW]FromFile() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                                    CLogger::Log(User,lmtError,"Burning Error!" );
									
                                    return false;
                                }
                                else
                                {
                                    SetLastWrittenPercent(100);
                                    CLogger::Log(User,lmtInformation,"Burning Complete" );
									
                                    return true;
                                }
                        }
                        else
                        {
                                m_DeviceStatus = NoAction;
								
                                return true;
                        }
                }
        case WRITE_MODE_SESSION_AT_ONCE:
                {
			//
			// Prepare SAO disc layout
			//
			RtlZeroMemory( &l_DiscLayout, sizeof(l_DiscLayout) );

			l_DiscLayout.m__LONG__NumberOfEntries = AudioDiscLayout->NumberOfTracks;

			for ( int i = 0; i<AudioDiscLayout->NumberOfTracks; i++ )
			{
				strcpy(l_DiscLayout.m__DISC_LAYOUT_ENTRY[i].m__CHAR__TrackName,
					AudioDiscLayout->AudioTrackLayout[i].TrackName);
				l_DiscLayout.m__DISC_LAYOUT_ENTRY[i].m__BOOLEAN__IsAudio = TRUE;

				CLogger::Log(Debug, lmtSubInformation,
					"DiscBurnerThread: Processing file <%s>, %d file from %d file(s) total\n",
					l_DiscLayout.m__DISC_LAYOUT_ENTRY[i].m__CHAR__TrackName,
					(i+1),
					(l_DiscLayout.m__LONG__NumberOfEntries)
				);			
			}

			
			m_ExceptionNumber =
				StarBurn_CdvdBurnerGrabber_SessionAtOnce(
                    m_CdvdBurnerGrabber,
                    (char*)( &m_ExceptionText ),
                    sizeof( m_ExceptionText ),
                    &m_SystemError,
                    &m_CDBFailureInfo,
					&l_DiscLayout,
					FALSE, //TRUE
					l_TestWrite,
					FALSE,
					WRITE_REPORT_DELAY_IN_SECONDS,
					BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
				);
			
                        //
                        // Check for success
                        //
                        if (Canceling != m_DeviceStatus)
                        {
                                m_DeviceStatus = NoAction;

                                if ( m_ExceptionNumber != EN_SUCCESS )
                                {
                                    CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_SessionAtOnce() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                                    CLogger::Log(User,lmtError,"Burning Error!" );
									
                                    return false;
                                }
                                else
                                {
                                    SetLastWrittenPercent(100);
                                    CLogger::Log(User,lmtInformation,"Burning Complete" );
									
                                    return true;
                                }
                        }
                        else
                        {
                                m_DeviceStatus = NoAction;
								
                                return true;
                        }

                }
        case WRITE_MODE_TRACK_AT_ONCE:
                {
			for ( int i = 0; i<AudioDiscLayout->NumberOfTracks; i++ )
			{
                                SetLastWrittenPercent(0);
                                SetCurrentTrackIndex(i);

				m_ExceptionNumber =
					StarBurn_CdvdBurnerGrabber_TrackAtOnceFromFile(
                                                m_CdvdBurnerGrabber,
                                                (char*)( &m_ExceptionText ),
                                                sizeof( m_ExceptionText ),
                                                &m_SystemError,
                                                &m_CDBFailureInfo,
						(char*)&AudioDiscLayout->AudioTrackLayout[i].TrackName,
						FALSE,						// No CD-ROM XA
						l_TestWrite,
						FALSE,
						WRITE_REPORT_DELAY_IN_SECONDS,
						BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
						);
                                //
                                // Check for success
                                //
                                if (Canceling != m_DeviceStatus)
                                {

                                        if ( m_ExceptionNumber != EN_SUCCESS )
                                        {
                                            CLogger::Log(Debug,lmtError,
                                                "\n\nCDevice::BurnAudioCD(): StarBurn_CdvdBurnerGrabber_TrackAtOnceFromFile() failed, exception %ld, status %ld, text '%s'\n",
                                                m_ExceptionNumber,
                                                m_SystemError,
                                                m_ExceptionText
                                                );

                                            CLogger::Log(User,lmtError,"Burning Error!" );
											
                                            return false;
                                        }
                                }
                                else
                                {
                                        m_DeviceStatus = NoAction;
										
                                        return true;
                                }
			}

                        m_DeviceStatus = NoAction;

			if(!BurnOptions->TestWrite)
			{
                                CLogger::Log(User,lmtInformation,"Closing session... " );

				m_ExceptionNumber =
					StarBurn_CdvdBurnerGrabber_CloseSession(
                                                m_CdvdBurnerGrabber,
                                                (char*)( &m_ExceptionText ),
                                                sizeof( m_ExceptionText ),
                                                &m_SystemError,
                                                &m_CDBFailureInfo
						);
				//
				// Check for success
				//
				if ( m_ExceptionNumber != EN_SUCCESS )
				{
					CLogger::Log(Debug,lmtError,
						"\n\nBurnAudioCD: StarBurn_CdvdBurnerGrabber_CloseSession() failed, exception %ld, status %ld, text '%s'\n",
						m_ExceptionNumber,
						m_SystemError,
						m_ExceptionText
					);

                    CLogger::Log(User,lmtError,"Burning Error!" );
					
					return false;
				}
			}

                        SetLastWrittenPercent(100);
                        CLogger::Log(User,lmtInformation,"Burning Complete" );
						
                        return true;
                }
		default:
			
	       	return false;
        }
}

bool CDevice::BurnVideoCD(TBurnOptions* BurnOptions,
						  char* FilePath,
						  bool IsExclusiveDriveAccess/* = true*/)
{
	bool bResult(false);
	bResult = BurnVideoCDInternal(BurnOptions, FilePath, IsExclusiveDriveAccess);
	CleanUp();

	return bResult;
}

bool CDevice::BurnVideoCDInternal(TBurnOptions* BurnOptions,
								  char* FilePath,
								  bool IsExclusiveDriveAccess/* = true*/)
{
    bool l_IsBUPEnabled;

    bool l_IsBUPSupported;

    bool l_IsSendOPCSuccessful = FALSE;

    BOOLEAN l_TestWrite = BurnOptions->TestWrite ? TRUE:FALSE;

    CLogger::Log(User,lmtInformation,"Started");

   //Create CdvdBurnerGrabber object if need
    if(!CdvdBurnerGrabberCreate(&VideoCDBurnCallbackDispatch, false, IsExclusiveDriveAccess))
                  {
                          CLogger::Log(User,lmtError,"Started Error!");						 
                         return 0;
                  }
        if(!TestUnitReadyInternal())
        {
                  CLogger::Log(User,lmtError,"Unit is not ready!");
				  
                  return false;
        }
        else
        {
                  CLogger::Log(User,lmtInformation,"Unit is ready!");
        }
  // Try to get BUP here

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                (PBOOLEAN)&l_IsBUPEnabled,
                (PBOOLEAN)&l_IsBUPSupported
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnVideoCD()): StarBurn_CdvdBurnerGrabber_GetBUP() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Getting BUP failed!");

			
            return false;
        }

        CLogger::Log(User,lmtInformation,"Getting BUP successfully!");

        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnVideoCD(): BUP (Buffer Underrun Protection) Enabled: %s, Supported: %s\n",
            l_IsBUPEnabled ? "Yes" : "No",
            l_IsBUPSupported ? "Yes" : "No"
            );

        // Check if the BUP supported try to enable it (for now we'll try to enable it ALWAYS)

        if ( /* l__BOOLEAN__IsBUPSupported == */ true )
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnVideoCD(): Enabling BUP (Buffer Underrun Protection)... " );
            CLogger::Log(User,lmtInformation,"Enabling BUP (Buffer Underrun Protection)...");

            // Try to set BUP status

            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 true
                    );

            // Check for success

            if ( m_ExceptionNumber != EN_SUCCESS )
            {
                CLogger::Log(Debug,lmtWarning,
                    "Failed!\nCDevice::BurnVideoCD(): WARNING! StarBurn_CdvdBurnerGrabber_SetBUP() failed, exception %ld, status %ld, text '%s'\n",
                        m_ExceptionNumber,
                        m_SystemError,
                        m_ExceptionText
                    );
               CLogger::Log(User,lmtInformation,"Enabling BUP failed!");


            }
            else
	    {
	       CLogger::Log(Debug,lmtInformation, "OK!\n" );
               CLogger::Log(User,lmtInformation,"Enabling BUP successfully!");
	    }
        }

        CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::BurnVideoCD(): Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );

        //Try setting burn speed
        if (TrySetWriteSpeed(&BurnOptions->Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }



        // Check is this test burn, if yes we do not need to send OPC as some recorders do not like sending OPC in
        // simulation mode

        if ( BurnOptions->OPC)
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnVideoCD(): Sending OPC (Optimum Power Calibration)... " );
            CLogger::Log(User,lmtInformation,"Sending OPC (Optimum Power Calibration)..." );

            // Set flag OPC was successful, it will be reset in case of exception

            l_IsSendOPCSuccessful = TRUE;

            // Try to send OPC
            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SendOPC(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo
                    );

            // Check for success
            if ( m_ExceptionNumber != EN_SUCCESS )
            {
               CLogger::Log(Debug,lmtError, "Failed!\n" );
               CLogger::Log(User,lmtError,"Sending OPC Error!" );

                // Reset the flag OPC was successful

                l_IsSendOPCSuccessful = FALSE;

            }

            // Check was OPC successful

            if ( l_IsSendOPCSuccessful == TRUE )
            {
                CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnVideoCD(): Writing the stuff to the CD/DVD disc >>>\n" );
                CLogger::Log(User,lmtInformation,"Sending OPC successfully." );

            }
            else
            {
                CLogger::Log(Debug,lmtInformation, "CDevice::BurnVideoCD(): Writing the stuff to the CD/DVD disc >>>\n" );
            }
        }
        else
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnVideoCD(): Skipping send OPC (Optimum Power Calibration)\n" );
            CLogger::Log(User,lmtInformation,"Writing the stuff (simulating) to the CD/DVD disc" );
        }


        // Reset last written percent to zero
        SetLastWrittenPercent(0);
        
        //Reset writing tack number
        m_WritingTrack = 0;

        m_DeviceStatus = Burning;

        // Try to write the VideoCD/MPEG1 image to the disc using current directory as temporary

		
        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_VideoCDEx(
                m_CdvdBurnerGrabber,
                (char*)( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                BurnOptions->WriteMethod,
                FilePath,
                "Album VideoCDEx",
                0x0001,
                0x0001,
                l_TestWrite,
                WRITE_REPORT_DELAY_IN_SECONDS,
                BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
                );



        // Check for success
        if (Canceling != m_DeviceStatus)
        {
                m_DeviceStatus = NoAction;


                if ( m_ExceptionNumber != EN_SUCCESS )
                {
                    CLogger::Log(Debug,lmtError,
                                "\n\nCDevice::BurnVideoCD(): StarBurn_CdvdBurnerGrabber_VideoCDEx failed, exception %ld, status %ld, text '%s'\n",
                                m_ExceptionNumber,
                                m_SystemError,
                                m_ExceptionText
                                );

                    CLogger::Log(User,lmtError,"Burning Error!" );

					
                    return false;
                }
                else
                {
                    if(WRITE_MODE_TRACK_AT_ONCE == BurnOptions->WriteMethod)
                    {
                        CLogger::Log(User,lmtInformation,"Closing session... " );

			m_ExceptionNumber =
				StarBurn_CdvdBurnerGrabber_CloseSession(
                                               m_CdvdBurnerGrabber,
                                               (char*)( &m_ExceptionText ),
                                               sizeof( m_ExceptionText ),
                                               &m_SystemError,
                                               &m_CDBFailureInfo
					);
			//
			// Check for success
			//
			if ( m_ExceptionNumber != EN_SUCCESS )
			{
				CLogger::Log(Debug,lmtError,
					"\n\nCDevice::BurnVideoCD(: StarBurn_CdvdBurnerGrabber_CloseSession() failed, exception %ld, status %ld, text '%s'\n",
					m_ExceptionNumber,
					m_SystemError,
					m_ExceptionText
				        );

                                CLogger::Log(User,lmtError,"Closing session error!" );
							
                        	return false;
			}
                        else
                        {
                                CLogger::Log(User,lmtInformation,"Session closed" );
                        }
                    }

                    SetLastWrittenPercent(100);
                    CLogger::Log(User,lmtInformation,"Burning Complete" );
					
                    return true;
                }
        }
        else
        {
                m_DeviceStatus = NoAction;
				
                return true;
        }

}

bool CDevice::BurnSuperVideoCD(TBurnOptions* BurnOptions,
							   char* FilePath,
							   bool IsExclusiveDriveAccess/* = true*/)
{
	bool bResult(false);
	bResult = BurnSuperVideoCDInternal(BurnOptions, FilePath, IsExclusiveDriveAccess);
	CleanUp();

	return bResult;
}

bool CDevice::BurnSuperVideoCDInternal(TBurnOptions* BurnOptions,
									   char* FilePath,
									   bool IsExclusiveDriveAccess/* = true*/)
{
    bool l_IsBUPEnabled;

    bool l_IsBUPSupported;

    bool l_IsSendOPCSuccessful = FALSE;


    BOOLEAN l_TestWrite = BurnOptions->TestWrite ? TRUE:FALSE;

    CLogger::Log(User,lmtInformation,"Started");

   //Create CdvdBurnerGrabber object if need
    if(!CdvdBurnerGrabberCreate(&VideoCDBurnCallbackDispatch, false, IsExclusiveDriveAccess))
                  {
                          CLogger::Log(User,lmtError,"Started Error!");						  
                         return 0;
                  }
        if(!TestUnitReadyInternal())
        {
                  CLogger::Log(User,lmtError,"Unit is not ready!");
				  
                  return false;
        }
        else
        {
                  CLogger::Log(User,lmtInformation,"Unit is ready!");
        }
  // Try to get BUP here

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                (PBOOLEAN)&l_IsBUPEnabled,
                (PBOOLEAN)&l_IsBUPSupported
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnSuperVideoCD()): StarBurn_CdvdBurnerGrabber_GetBUP() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Getting BUP failed!");

			
            return false;
        }

        CLogger::Log(User,lmtInformation,"Getting BUP successfully!");

        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnSuperVideoCD(): BUP (Buffer Underrun Protection) Enabled: %s, Supported: %s\n",
            l_IsBUPEnabled ? "Yes" : "No",
            l_IsBUPSupported ? "Yes" : "No"
            );

        // Check if the BUP supported try to enable it (for now we'll try to enable it ALWAYS)

        if ( /* l__BOOLEAN__IsBUPSupported == */ true )
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnSuperVideoCD(): Enabling BUP (Buffer Underrun Protection)... " );
            CLogger::Log(User,lmtInformation,"Enabling BUP (Buffer Underrun Protection)...");

            // Try to set BUP status

            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 true
                    );

            // Check for success

            if ( m_ExceptionNumber != EN_SUCCESS )
            {
                CLogger::Log(Debug,lmtWarning,
                    "Failed!\nCDevice::BurnSuperVideoCD(): WARNING! StarBurn_CdvdBurnerGrabber_SetBUP() failed, exception %ld, status %ld, text '%s'\n",
                        m_ExceptionNumber,
                        m_SystemError,
                        m_ExceptionText
                    );
               CLogger::Log(User,lmtInformation,"Enabling BUP failed!");


            }
            else
	    {
	       CLogger::Log(Debug,lmtInformation, "OK!\n" );
               CLogger::Log(User,lmtInformation,"Enabling BUP successfully!");
	    }
        }


         CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::BurnSuperVideoCD(): Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );

        //Try setting burn speed
        if (TrySetWriteSpeed(&BurnOptions->Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::BurnSuperVideoCD():Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }

        // Check is this test burn, if yes we do not need to send OPC as some recorders do not like sending OPC in
        // simulation mode

        if ( BurnOptions->OPC)
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnSuperVideoCD(): Sending OPC (Optimum Power Calibration)... " );
            CLogger::Log(User,lmtInformation,"Sending OPC (Optimum Power Calibration)..." );

            // Set flag OPC was successful, it will be reset in case of exception

            l_IsSendOPCSuccessful = TRUE;

            // Try to send OPC
            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SendOPC(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo
                    );

            // Check for success
            if ( m_ExceptionNumber != EN_SUCCESS )
            {
               CLogger::Log(Debug,lmtError, "Failed!\n" );
               CLogger::Log(User,lmtError,"Sending OPC Error!" );

                // Reset the flag OPC was successful

                l_IsSendOPCSuccessful = FALSE;

            }

            // Check was OPC successful

            if ( l_IsSendOPCSuccessful == TRUE )
            {
                CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnSuperVideoCD(): Writing the stuff to the CD/DVD disc >>>\n" );
                CLogger::Log(User,lmtInformation,"Sending OPC successfully." );

            }
            else
            {
                CLogger::Log(Debug,lmtInformation, "CDevice::BurnSuperVideoCD(): Writing the stuff to the CD/DVD disc >>>\n" );
            }
        }
        else
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnSuperVideoCD(): Skipping send OPC (Optimum Power Calibration) in test mode... OK!\n" );
            CLogger::Log(User,lmtInformation,"Writing the stuff (simulating) to the CD/DVD disc" );
        }


        // Reset last written percent to zero
        SetLastWrittenPercent(0);

        //Reset writing tack number
        m_WritingTrack = 0;

        m_DeviceStatus = Burning;

        // Try to write the SuperVideoCD/MPEG2 image to the disc using current directory as temporary

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_SuperVideoCDEx(
                m_CdvdBurnerGrabber,
                (char*)( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                BurnOptions->WriteMethod,
                FilePath,
                "Album VideoCDEx",
                0x0001,
                0x0001,
                l_TestWrite,
                WRITE_REPORT_DELAY_IN_SECONDS,
                BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
                );


        // Check for success
        if (Canceling != m_DeviceStatus)
        {
                m_DeviceStatus = NoAction;


                if ( m_ExceptionNumber != EN_SUCCESS )
                {
                    CLogger::Log(Debug,lmtError,
                                "\n\nCDevice::BurnSuperVideoCD(): StarBurn_CdvdBurnerGrabber_SuperVideoCDEx failed, exception %ld, status %ld, text '%s'\n",
                                m_ExceptionNumber,
                                m_SystemError,
                                m_ExceptionText
                                );

                    CLogger::Log(User,lmtError,"Burning Error!" );
					
                    return false;
                }
                else
                {
                    if(WRITE_MODE_TRACK_AT_ONCE == BurnOptions->WriteMethod)
                    {
                        CLogger::Log(User,lmtInformation,"Closing session... " );

			m_ExceptionNumber =
				StarBurn_CdvdBurnerGrabber_CloseSession(
                                               m_CdvdBurnerGrabber,
                                               (char*)( &m_ExceptionText ),
                                               sizeof( m_ExceptionText ),
                                               &m_SystemError,
                                               &m_CDBFailureInfo
					);
			//
			// Check for success
			//
			if ( m_ExceptionNumber != EN_SUCCESS )
			{
				CLogger::Log(Debug,lmtError,
					"\n\nCDevice::BurnSuperVideoCD(: StarBurn_CdvdBurnerGrabber_CloseSession() failed, exception %ld, status %ld, text '%s'\n",
					m_ExceptionNumber,
					m_SystemError,
					m_ExceptionText
				        );

                                CLogger::Log(User,lmtError,"Closing session error!" );
							
                        	return false;
			}
                        else
                        {
                                CLogger::Log(User,lmtInformation,"Session closed" );
                        }
                    }

                    SetLastWrittenPercent(100);
                    CLogger::Log(User,lmtInformation,"Burning Complete" );
					
                    return true;
                }
        }
        else
        {
                m_DeviceStatus = NoAction;
				
                return true;
        }

}

bool CDevice::BurnDVDVideo(TBurnOptions* BurnOptions,
						   char* FilePath,
						   char* DiskLabel,
						   bool IsExclusiveDriveAccess/* = true*/)
{
	bool bResult(false);
	bResult = BurnDVDVideoInternal(BurnOptions, FilePath, DiskLabel, IsExclusiveDriveAccess);
	CleanUp();

	return bResult;
}

bool CDevice::BurnDVDVideoInternal(TBurnOptions* BurnOptions,
								   char* FilePath,
								   char* DiskLabel,
								   bool IsExclusiveDriveAccess/* = true*/)
{

    bool l_IsBUPEnabled;

    bool l_IsBUPSupported;

    bool l_IsSendOPCSuccessful = FALSE;

    bool l_IsTrackAtOnce = FALSE;

    bool l_IsSessionAtOnce = FALSE;

    bool l_IsDiscAtOncePQ = FALSE;

    bool l_IsDiscAtOnceRawPW = FALSE;

    LARGE_INTEGER l_FileSizeInUCHARs;

    void* l_DVDVideo = NULL; // Initialize with bad pointer

    void* l_ISO9660JolietFileTree = NULL; // Initialize with bad pointer

	BOOLEAN l_TestWrite = BurnOptions->TestWrite ? TRUE:FALSE;

    CLogger::Log(User,lmtInformation,"Started");

        // Try to create DVD-Video file system
        m_ExceptionNumber =
            StarBurn_DVDVideo_Create(
				&l_DVDVideo,
				FilePath,
				TRUE, // FALSE, // TRUE, // TRUE if we want IFO/BUP to be patched and FALSE otherwise
                                 ( PCHAR )( &m_ExceptionText ),
                                 sizeof( m_ExceptionText ),
                                 &m_SystemError,
                                DiskLabel,
				"Sothink",
				"Sothink MovieDVDMaker",
				2006,	        // Year
				10,		// Month
				20,		// Day,
				12,		// Hour
				10,		// Minute
				0,		// Second
				50		// Millisecond
                                );

        // Check for success
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnDVDVideo(): StarBurn_DVDVideo_Create(), exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Creating DVD-Video file system failed!");
			
            return false;
        }


           //Create CdvdBurnerGrabber object if need
        if(!CdvdBurnerGrabberCreate(&DVDVideoBurnCallbackDispatch, false, IsExclusiveDriveAccess))
                  {
                         CLogger::Log(User,lmtError,"Started Error!");						 
                         return 0;
                  }

        CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Testing unit ready... " );
        CLogger::Log(User,lmtInformation,"Testing unit ready...");

        // Try to test unit ready

        if(!TestUnitReadyInternal())
        {
                  CLogger::Log(User,lmtError,"Unit is not ready!");
				  
                  return false;
        }
        else
        {
                  CLogger::Log(User,lmtInformation,"Unit is ready!");
        }

    //Probing supported write modes...

        CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Probing supported write modes... " );
        CLogger::Log(User,lmtInformation,"Probing supported write modes...");

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 (PBOOLEAN)&l_IsTrackAtOnce,
                 (PBOOLEAN)&l_IsSessionAtOnce,
                 (PBOOLEAN)&l_IsDiscAtOncePQ,
                 (PBOOLEAN)&l_IsDiscAtOnceRawPW
                 );

    // Check for success
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnDVDVideo(): StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );


             CLogger::Log(User,lmtError,"Probing supported write modes Error!");

    // Get out of here
			 
             return false;
        }


        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnDVDVideo(): Track-At-Once: %s, Session-At-Once: %s, Disc-At-Once PQ: %s, Disc-At-Once raw P-W: %s\n",
            l_IsTrackAtOnce ? "Yes" : "No",
            l_IsSessionAtOnce ? "Yes" : "No",
            l_IsDiscAtOncePQ ? "Yes" : "No",
            l_IsDiscAtOnceRawPW ? "Yes" : "No"
            );

        // Check do we have Track-At-Once supported

        if ( !l_IsTrackAtOnce )
        {

            CLogger::Log(Debug,lmtError, "\n\nCDevice::BurnDVDVideo(): Track-At-Once is unsupported!!!\n" );
            CLogger::Log(User,lmtError,"Track-At-Once is unsupported!");

            // Get out of here
			
            return false;
        }
        {
            CLogger::Log(User,lmtInformation,"Track-At-Once is supported!");
        }



        // Get size of image we'll store (assume this call cannot fail)

        StarBurn_DVDVideo_GetSizeInUCHARs(
                                     l_DVDVideo,
                                    &l_FileSizeInUCHARs
                                            );
        CLogger::Log(Debug,lmtInformation,
                    "CDevice::BurnDVDVideo(): Burning %I64d UCHARs of DVD-Video image\n",
                    l_FileSizeInUCHARs);

        CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnDVDVideo(): Getting BUP (Buffer Underrun Protection) support... " );

        CLogger::Log(User,lmtInformation,"Getting BUP (Buffer Underrun Protection) support...");

        // Try to get BUP here

        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                (PBOOLEAN)&l_IsBUPEnabled,
                (PBOOLEAN)&l_IsBUPSupported
                );

        // Check for success

        if ( m_ExceptionNumber != EN_SUCCESS )
        {
            CLogger::Log(Debug,lmtError,
                "\n\nCDevice::BurnDVDVideo(): StarBurn_CdvdBurnerGrabber_GetBUP() failed, exception %ld, status %ld, text '%s'\n",
                m_ExceptionNumber,
                m_SystemError,
                m_ExceptionText
                );

            CLogger::Log(User,lmtError,"Getting BUP failed!");
			
			
            return false;
        }

        CLogger::Log(User,lmtInformation,"Getting BUP successfully!");

        CLogger::Log(Debug,lmtInformation,
            "OK!\nCDevice::BurnDVDVideo(): BUP (Buffer Underrun Protection) Enabled: %s, Supported: %s\n",
            l_IsBUPEnabled ? "Yes" : "No",
            l_IsBUPSupported ? "Yes" : "No"
            );

        // Check if the BUP supported try to enable it (for now we'll try to enable it ALWAYS)

        if ( /* l__BOOLEAN__IsBUPSupported == */ true )
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Enabling BUP (Buffer Underrun Protection)... " );
            CLogger::Log(User,lmtInformation,"Enabling BUP (Buffer Underrun Protection)...");

            // Try to set BUP status

            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SetBUP(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 true
                    );

            // Check for success

            if ( m_ExceptionNumber != EN_SUCCESS )
            {
                CLogger::Log(Debug,lmtWarning,
                    "Failed!\nCDevice::BurnDVDVideo(): WARNING! StarBurn_CdvdBurnerGrabber_SetBUP() failed, exception %ld, status %ld, text '%s'\n",
                        m_ExceptionNumber,
                        m_SystemError,
                        m_ExceptionText
                    );
               CLogger::Log(User,lmtInformation,"Enabling BUP failed!");


            }
            else
	    {
	       CLogger::Log(Debug,lmtInformation, "OK!\n" );
               CLogger::Log(User,lmtInformation,"Enabling BUP successfully!");
	    }
        }

          CLogger::Log(Debug,
                    lmtInformation,
                     "CDevice::CDevice::BurnDVDVideo(): Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );
        CLogger::Log(User,
                    lmtInformation,
                     "Setting speed %s (%ld KBps)... ",
                     BurnOptions->Speed.SpeedX,
                     BurnOptions->Speed.SpeedKBps
                     );

        //Try setting burn speed
        if (TrySetWriteSpeed(&BurnOptions->Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting speed %s (%ld KBps) successfully!",
                            BurnOptions->Speed.SpeedX,
                            BurnOptions->Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::BurnDVDVideo():Setting speed failed!");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }

        // Check is this test burn, if yes we do not need to send OPC as some recorders do not like sending OPC in
        // simulation mode

        if ( BurnOptions->OPC)
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Sending OPC (Optimum Power Calibration)... " );
            CLogger::Log(User,lmtInformation,"Sending OPC (Optimum Power Calibration)..." );

            // Set flag OPC was successful, it will be reset in case of exception

            l_IsSendOPCSuccessful = TRUE;

            // Try to send OPC
            m_ExceptionNumber =
                StarBurn_CdvdBurnerGrabber_SendOPC(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo
                    );

            // Check for success
            if ( m_ExceptionNumber != EN_SUCCESS )
            {
               CLogger::Log(Debug,lmtError, "Failed!\n" );
               CLogger::Log(User,lmtError,"Sending OPC Error!" );

                // Reset the flag OPC was successful

                l_IsSendOPCSuccessful = FALSE;

            }

            // Check was OPC successful

            if ( l_IsSendOPCSuccessful == TRUE )
            {
                CLogger::Log(Debug,lmtInformation, "OK!\nCDevice::BurnDVDVideo(): Writing the stuff to the CD/DVD disc >>>\n" );
                CLogger::Log(User,lmtInformation,"Sending OPC successfully." );

            }
            else
            {
                CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Writing the stuff to the CD/DVD disc >>>\n" );
            }
        }
        else
        {
            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Skipping send OPC (Optimum Power Calibration) in test mode... OK!\n" );

            CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Writing the stuff (simulating) to the CD/DVD disc >>>\n" );
            CLogger::Log(User,lmtInformation,"Writing the stuff (simulating) to the CD/DVD disc" );
        }


        // Reset last written percent to zero
        SetLastWrittenPercent(0);


        m_DeviceStatus = Burning;

	// Get pointer to ISO9660/Joliet file tree (assume this call cannot fail)

	StarBurn_DVDVideo_GetTreePointer(
			l_DVDVideo,
			&l_ISO9660JolietFileTree
			);

        //
        // Try to write the generated DVD-Video file system to the optical media
		//
		// ATTENTION! In our sample we'll pass TRUE as "multisession" to make burning process faster (when multisession is
		// disabled and compatible mode is ON we'll burn at least 1GB and it's too long for testing purpose). In a real world
		// application to make resulting DVD-Video disc readable with hardware DVD players you'll need to set "multisession"
		// parameter to FALSE just ALWAYS
        //
        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_TrackAtOnceFromTree(
                m_CdvdBurnerGrabber,
                (char*)( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                l_ISO9660JolietFileTree,
                TRUE, // FALSE
                l_TestWrite,
                FALSE,
                WRITE_REPORT_DELAY_IN_SECONDS,
                BUFFER_STATUS_REPORT_DELAY_IN_SECONDS
                );

		StarBurn_DVDVideo_Destroy(l_DVDVideo);

        // Check for success
        if (Canceling != m_DeviceStatus)
        {
                m_DeviceStatus = NoAction;


                if ( m_ExceptionNumber != EN_SUCCESS )
                {
                    CLogger::Log(Debug,lmtError,
                                "\n\nCDevice::BurnDVDVideo(): StarBurn_CdvdBurnerGrabber_TrackAtOnceFromTree() failed, exception %ld, status %ld, text '%s'\n",
                                m_ExceptionNumber,
                                m_SystemError,
                                m_ExceptionText
                                );

                    CLogger::Log(User,lmtError,"Burning Error!" );
					
                    return false;
                }
                else
                {
					if(!BurnOptions->TestWrite)
					{
                        CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo(): Try to close session by StarBurn_CdvdBurnerGrabber_CloseSession() " );
                        CLogger::Log(User,lmtInformation,"Closing session...");

                        m_ExceptionNumber =
                                StarBurn_CdvdBurnerGrabber_CloseSession(
                                        m_CdvdBurnerGrabber,
                                        (char*)( &m_ExceptionText ),
                                        sizeof( m_ExceptionText ),
                                        &m_SystemError,
                                        &m_CDBFailureInfo
                                            );

                        // Check for success
                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                            CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::BurnDVDVideo(): StarBurn_CdvdBurnerGrabber_CloseSession() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                            CLogger::Log(User,lmtError,"Close Session Error!" );
							
                            return false;
                        }
                        else
                        {
                               CLogger::Log(Debug,lmtInformation, "CDevice::BurnDVDVideo():StarBurn_CdvdBurnerGrabber_CloseSession(): Close session success! " );
                               CLogger::Log(User,lmtInformation,"Closing session success!");
                        }
					}

                    SetLastWrittenPercent(100);
                    CLogger::Log(User,lmtInformation,"Burning Complete!" );
					
                    return true;
                }
        }
        else
        {
                m_DeviceStatus = NoAction;
				
                return true;
        }
}




bool CDevice::TrySetWriteSpeed(TSpeed* Speed)
{
    ULONG l_CurrentReadSpeed;
    ULONG l_MaximumReadSpeed;
    ULONG l_MaximumWriteSpeed;

    //Create CdvdBurnerGrabber object if need
    if(NULL == m_CdvdBurnerGrabber )
          if(!CdvdBurnerGrabberCreate(&Callback, true))
          {
                         m_MediaInfo.MediaSize = 0;						 
                         return false;
          }

	//Try to set current write speed
//----------- BEGIN
//----------- Temporary condition (now setting speed for Blu-Ray media under construction)

    DISC_TYPE l_DiscType = GetMediaType();

	if (l_DiscType == DISC_TYPE_BDR ||
		l_DiscType  == DISC_TYPE_BDRE
		) 
	{
		
		m_ExceptionNumber =
			StarBurn_CdvdBurnerGrabber_SetSpeeds(
			m_CdvdBurnerGrabber,
			( PCHAR )( &m_ExceptionText ),
			sizeof( m_ExceptionText ),
			&m_SystemError,
			&m_CDBFailureInfo,
			CDVD_SPEED_IS_KBPS_MAXIMUM,
			CDVD_SPEED_IS_KBPS_MAXIMUM
			);		
	}
	else
	{
		m_ExceptionNumber =
			StarBurn_CdvdBurnerGrabber_SetSpeeds(
			m_CdvdBurnerGrabber,
			( PCHAR )( &m_ExceptionText ),
			sizeof( m_ExceptionText ),
			&m_SystemError,
			&m_CDBFailureInfo,
			CDVD_SPEED_IS_KBPS_MAXIMUM,
			Speed->SpeedKBps
			);		
	}
//----------- END

        // Check for success
        if (m_ExceptionNumber != EN_SUCCESS )
        {
			 
             return false;
        }

        // Try to get current read/write speeds to show what we'll have
        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetSpeeds(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                &l_CurrentReadSpeed,
                &l_MaximumReadSpeed,
                &Speed->SpeedKBps,
                &l_MaximumWriteSpeed
                );

        // Check for success
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
			
            return false;
        }

		
        return true;
}

bool CDevice::GrabTracks(TGrabTracksInfo* GrabTracksInfo)
{
	bool bResult(false);
	bResult = GrabTracksInternal(GrabTracksInfo);
	CleanUp();

	return bResult;
}
bool CDevice::GrabTracksInternal(TGrabTracksInfo* GrabTracksInfo)
{
        //Check requested track count for grabbing
        if(!GrabTracksInfo->size())
        {
                CLogger::Log(User,lmtError,"No tracks requested for grabbing!");
                CLogger::Log(Debug,lmtError,"No tracks requested for grabbing!");                                
        }

	ZeroMemory(&m_ExceptionText, sizeof(m_ExceptionText));
	ZeroMemory(&m_CDBFailureInfo, sizeof(m_CDBFailureInfo));

        CLogger::Log(User,lmtInformation,"Started");

        //Create CdvdBurnerGrabber object if need
        if(!CdvdBurnerGrabberCreate(&GrabberCallbackDispatch, true, true))
        {
                CLogger::Log(User,lmtError,"Started Error!");				
                return false;
        }



        // Try to test unit ready

        if(!TestUnitReadyInternal())
        {
                  CLogger::Log(User,lmtError,"Unit is not ready!");
				  
                  return false;
        }
        else
        {
                  CLogger::Log(User,lmtInformation,"Unit is ready!");
        }

        CLogger::Log(Debug,lmtInformation, "CDevice::GrabTracks(): Setting maximum supported CD/DVD speeds... \n" );
        CLogger::Log(User,lmtInformation,"Setting maximum supported CD/DVD speeds...");

	//
	//Try to set maximum read speed
	//
        TSpeed l_Speed = {CDVD_SPEED_IS_KBPS_MAXIMUM,"Max speed"};


        if (TrySetReadSpeed(&l_Speed))
        {
                CLogger::Log(User,
                            lmtInformation,
                            "Setting maximum speed (%ld KBps) successfully!",
                            l_Speed.SpeedKBps
                            );

                CLogger::Log(Debug,
                            lmtInformation,
                            "Setting maximum speed (%ld KBps) successfully!\n",
                            l_Speed.SpeedKBps
                            );

        }
        else
        {
                CLogger::Log(Debug,lmtError,"CDevice::GrabTracks():Setting speed failed!\n");
                CLogger::Log(User,lmtError,"Setting speed failed!");
        }

	CLogger::Log(Debug,lmtInformation,"CDevice::TrackGrab():Try to enable fast TOC reading... \n" );
	CLogger::Log(User,lmtInformation,"Trying fast TOC reading...");

	//
	// Try to enable fast TOC reading as it's not critical for WAVs or ISOs to have extra data at the end
	//
	StarBurn_SetFastReadTOC( TRUE );

	//
	// Get current "fast read TOC" property
	//

	if(StarBurn_GetFastReadTOC())
        {
                CLogger::Log(Debug,lmtInformation,"CDevice::TrackGrab():Fast TOC reading enabled\n");
                CLogger::Log(User,lmtInformation,"Fast TOC reading enabled");
        }
        else
        {
                CLogger::Log(Debug,lmtInformation,"CDevice::TrackGrab():Fast TOC reading disabled\n");
                CLogger::Log(User,lmtInformation,"Fast TOC reading disabled");
        }

	//
	// Try to grab tracks
	//

        m_DeviceStatus = Grabbing;

	for (int i=0; i<(int)GrabTracksInfo->size(); ++i)
	{
                CLogger::Log(Debug,lmtInformation,"CDevice::TrackGrab(): Grabbing track: %d , file name: %s \n", (*GrabTracksInfo)[i].Number, (*GrabTracksInfo)[i].FullFileName);
                CLogger::Log(User,lmtInformation,"Grabbing track#%d...",(*GrabTracksInfo)[i].Number);

                SetLastWrittenPercent(0);
                SetCurrentTrackIndex(i);

		m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_GrabTrack(
			m_CdvdBurnerGrabber,
			( PCHAR )( &m_ExceptionText ),
			sizeof( m_ExceptionText ),
			&m_SystemError,
			&m_CDBFailureInfo,
			(*GrabTracksInfo)[i].Number,
			(*GrabTracksInfo)[i].FullFileName,
			NUMBER_OF_READ_RETRIES,
			TRUE, // TRUE - Ignore bad blocks, FALSE - Stop after unrecoverable read errors
			FALSE,
			READ_REPORT_DELAY_IN_SECONDS
		);

                        SetLastWrittenPercent(100);
                        // Check for success
                        if (Canceling != m_DeviceStatus)
                        {
                                if ( m_ExceptionNumber != EN_SUCCESS )
                                {
                                    CLogger::Log(Debug,lmtError,
                                        "\n\nCDevice::TrackGrab(): StarBurn_CdvdBurnerGrabber_GrabTrack() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                        );

                                    CLogger::Log(User,lmtError,"Grabbing Error!" );
									
                                    m_DeviceStatus = NoAction;
                                    return false;
                                }
                        }
                        else
                        {
                                m_DeviceStatus = NoAction;
								
                                return true;
                        }
        }
        CLogger::Log(User,lmtInformation,"Grabbing Complete!" );

	m_DeviceStatus = NoAction;	
        return true;

}

bool CDevice::TrySetReadSpeed(TSpeed* Speed)
{
    ULONG l_MaximumReadSpeed;
    ULONG l_CurrentWriteSpeed;
    ULONG l_MaximumWriteSpeed;

    //Create CdvdBurnerGrabber object if need
    if(NULL == m_CdvdBurnerGrabber )
          if(!CdvdBurnerGrabberCreate(&Callback, true))
          {
                         m_MediaInfo.MediaSize = 0;
                         return false;
          }

   //Try to set current read speed
        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_SetSpeeds(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 Speed->SpeedKBps,
                 CDVD_SPEED_IS_KBPS_MAXIMUM
                );


        // Check for success
        if (m_ExceptionNumber != EN_SUCCESS )
        {
			 
             return false;
        }

        // Try to get current read/write speeds to show what we'll have
        m_ExceptionNumber =
            StarBurn_CdvdBurnerGrabber_GetSpeeds(
                 m_CdvdBurnerGrabber,
                 ( PCHAR )( &m_ExceptionText ),
                 sizeof( m_ExceptionText ),
                 &m_SystemError,
                 &m_CDBFailureInfo,
                 &Speed->SpeedKBps,
                 &l_MaximumReadSpeed,
                 &l_CurrentWriteSpeed,
                 &l_MaximumWriteSpeed
                );

        // Check for success
        if ( m_ExceptionNumber != EN_SUCCESS )
        {
			
            return false;
        }
		
        return true;
}


bool CDevice::RefreshMediaLabel()
{
        char256 l_Label;
        memset(&l_Label,0,sizeof(l_Label));
        switch ( this->GetMediaType() )
                {
                        case DISC_TYPE_NO_MEDIA:
                        {
                                break;
                        }
                        default:
                        {
                                 char root[256] = "";
                                 strcpy(root,(char*)GetDeviceLetter());
                                 strcat(root,"\\");

                                 GetVolumeInformationA(&root[0],&l_Label[0],sizeof(l_Label),NULL,NULL,NULL,NULL,0);

                                 break;
                         }

                 }
        strcpy((char*)&m_MediaInfo.MediaLabel,(char*)&l_Label);
		
        return true;


}



bool CDevice::RefreshMediaSize()
{

	LARGE_INTEGER l_UsedSpaceInUCHARs;
	l_UsedSpaceInUCHARs.QuadPart = 0;

	m_MediaInfo.MediaSize = 0;

    //Create CdvdBurnerGrabber object if need
    if(NULL == m_CdvdBurnerGrabber )
	{
          if(!CdvdBurnerGrabberCreate(&Callback, true))
          {
                         
                         return false;
          }
	}
	
	RefreshMediaType();
	if(m_MediaInfo.MediaType == DISC_TYPE_NO_MEDIA)
	{
		m_MediaInfo.MediaSize = 0;
		
		return true;
	}

	
	  //Try to get disc info
	  m_ExceptionNumber =
			StarBurn_CdvdBurnerGrabber_GetDiscUsedSpace(
			m_CdvdBurnerGrabber,
            (char*)( &m_ExceptionText ),
            sizeof( m_ExceptionText ),
            &m_SystemError,
            &m_CDBFailureInfo,
			&l_UsedSpaceInUCHARs
			);

      // Check for success
      if ( m_ExceptionNumber != EN_SUCCESS )
	  {
	      CLogger::Log(Debug,lmtError,
                          "\nRefreshMediaSize(): StarBurn_CdvdBurnerGrabber_GetDiscUsedSpace() failed, exception %ld, status %ld, text '%s'\n",
                          m_ExceptionNumber,
                          m_SystemError,
                          m_ExceptionText              	
						  );     		
		
		return false;		                
    }

	m_MediaInfo.MediaSize = (double)l_UsedSpaceInUCHARs.QuadPart;

	
	return true;
}

bool CDevice::IsDiscBlank()
{	
	bool bResult(false);
	bResult = IsDiscBlankInternal();
	CleanUp();

	return bResult;
}


bool CDevice::IsDiscBlankInternal()
{	
	BOOLEAN l_IsDiscBlank(FALSE);

	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{			
			return false;
		}
	}

	//
	//Try to get disc blank state
	//
	m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_IsDiscBlank(
			m_CdvdBurnerGrabber,
			(char*)( &m_ExceptionText ),
			sizeof( m_ExceptionText ),
			&m_SystemError,
			&m_CDBFailureInfo,
			&l_IsDiscBlank
			);

    // Check for success
    if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,lmtError,
					"\nCDevice::IsDiscBlank(): StarBurn_CdvdBurnerGrabber_GetDiscInformation() failed, exception %ld, status %ld, text '%s'\n",
					m_ExceptionNumber,
					m_SystemError,
					m_ExceptionText
					); 

		
		return false;		                
    }	
       
	
	return l_IsDiscBlank ? true:false;
}


bool CDevice::IsAudioFileSupported(char* FileName)
{
        EXCEPTION_NUMBER l_ExceptionNumber;
        l_ExceptionNumber =
                StarBurn_IsAudioFileSupported(FileName);
        if (EN_SUCCESS == l_ExceptionNumber)
                return true;
        else
                return false;
};


bool CDevice::RefreshAudioMediaFreeSize()
{
	LARGE_INTEGER l_FreeSpaceInUCHARs;
	LONG l_FreeLBs;

	m_MediaInfo.MediaFreeSize = 0;

	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{
			return false;
		}
	}

	RefreshMediaType();
	if(m_MediaInfo.MediaType == DISC_TYPE_NO_MEDIA)
	{
		m_MediaInfo.MediaFreeSize = 0;
		
		return true;
	}

	//
	//Try to get free disc space
	//
	m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace(
		m_CdvdBurnerGrabber,
		CD_MODE_AUDIO,
		&l_FreeLBs,
		&l_FreeSpaceInUCHARs
		);

	// Check for success
	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,lmtError,
			"\nCDevice::RefreshAudioMediaFreeSize(): StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace() failed, exception %ld\n",
			m_ExceptionNumber
			);
		
		return false;		                
	}

	m_MediaInfo.MediaFreeSize = (double)l_FreeSpaceInUCHARs.QuadPart;

	
	return true;
}

bool CDevice::RefreshVideoMediaFreeSize()
{
	LARGE_INTEGER l_FreeSpaceInUCHARs;
	LONG l_FreeLBs;

	m_MediaInfo.MediaFreeSize = 0;

	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{
			return false;
		}
	}

	RefreshMediaType();
	if(m_MediaInfo.MediaType == DISC_TYPE_NO_MEDIA)
	{
		m_MediaInfo.MediaFreeSize = 0;

		return true;
	}

	//
	//Try to get free disc space
	//
	m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace(
		m_CdvdBurnerGrabber,
		CD_MODE_VIDEO,
		&l_FreeLBs,
		&l_FreeSpaceInUCHARs
		);

	// Check for success
	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,lmtError,
			"\nCDevice::RefreshAudioMediaFreeSize(): StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace() failed, exception %ld\n",
			m_ExceptionNumber
			);

		return false;		                
	}

	m_MediaInfo.MediaFreeSize = (double)l_FreeSpaceInUCHARs.QuadPart;


	return true;
}


bool CDevice::RefreshDataMediaFreeSize()
{
	LARGE_INTEGER l_FreeSpaceInUCHARs;
	LONG l_FreeLBs;

	m_MediaInfo.MediaFreeSize = 0;

	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{			
			return false;
		}
	}

	RefreshMediaType();
	if(m_MediaInfo.MediaType == DISC_TYPE_NO_MEDIA)
	{
		m_MediaInfo.MediaFreeSize = 0;
		
		return true;
	}


	//
	//Try to get free disc space
	//
	m_ExceptionNumber =
		StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace(
		m_CdvdBurnerGrabber,
		CD_MODE_DATA,
		&l_FreeLBs,
		&l_FreeSpaceInUCHARs
		);

	// Check for success
	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,lmtError,
			"\nCDevice::RefreshDataMediaFreeSize(): StarBurn_CdvdBurnerGrabber_GetDiscFreeSpace() failed, exception %ld\n",
			m_ExceptionNumber
			);
		
		return false;		                
	}

	m_MediaInfo.MediaFreeSize = (double)l_FreeSpaceInUCHARs.QuadPart;

	
	return true;
}


bool CDevice::RefreshAudioMediaInfo()
{
	bool bResult(false);
	bResult = RefreshAudioMediaInfoInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshVideoMediaInfo()
{
	bool bResult(false);
	bResult = RefreshVideoMediaInfoInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshAudioMediaInfoInternal()
{
	if(
       RefreshMediaType()  &&
	   RefreshMediaLabel() &&
	   RefreshMediaSize() &&
	   RefreshAudioMediaFreeSize()
	   )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CDevice::RefreshVideoMediaInfoInternal()
{
	if(
		RefreshMediaType()  &&
		RefreshMediaLabel() &&
		RefreshMediaSize() &&
		RefreshVideoMediaFreeSize()
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CDevice::RefreshDataMediaInfo()
{
	bool bResult(false);
	bResult = RefreshDataMediaInfoInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshDataMediaInfoInternal()
{
    //GetTrackInfoInternal(TRACK_NUMBER_INVISIBLE, &m_HiddenTrackInformation);

	if(
      RefreshMediaType() &&
	   RefreshMediaLabel()&&
	   RefreshMediaSize() &&
	   RefreshDataMediaFreeSize()
	   )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CDevice::RefreshDVDFeatures()
{
	bool bResult(false);
	bResult = RefreshDVDFeaturesInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshDVDFeaturesInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{			
			return false;
		}
	}

    //
	// Try to detect Changes left, Region mask and RPC Scheme
	//
        m_DVDFeatures.TypeCode = 0;
        m_DVDFeatures.ChangesLeftVendor = 0;
        m_DVDFeatures.ChangesLeftUser = 0;
        m_DVDFeatures.RegionMask = 0;
        m_DVDFeatures.RPCScheme = 0;
        
	m_ExceptionNumber =
	StarBurn_CdvdBurnerGrabber_GetRPC(
		m_CdvdBurnerGrabber,
		(char*)( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo,
		&(m_DVDFeatures.TypeCode),
		&(m_DVDFeatures.ChangesLeftVendor),
		&(m_DVDFeatures.ChangesLeftUser),
		&(m_DVDFeatures.RegionMask),
		&(m_DVDFeatures.RPCScheme)
		);

	//
	// Check for success
	//
	if (m_ExceptionNumber!= EN_SUCCESS)
	{
		CLogger::Log(Debug, lmtError,
			"\n\n RefreshDVDFeatures():StarBurn_CdvdBurnerGrabber_GetRPC() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
			);
		m_DVDFeatures.Valid = FALSE;
	}
	else
	{
		m_DVDFeatures.Valid = TRUE;
	}

	
	return true;
}

bool CDevice::RefreshReadWriteModes()
{
	bool bResult(false);
	bResult = RefreshReadWriteModesInternal();
	CleanUp();

	return bResult;
}

bool CDevice::RefreshReadWriteModesInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{			
			return false;
		}
	}

	//
	// Try go get supported write modes
	//
	m_ExceptionNumber =
	StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes(
                m_CdvdBurnerGrabber,
                (char*)( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                (PBOOLEAN)&m_ReadWriteModes.IsTAO,
                (PBOOLEAN)&m_ReadWriteModes.IsSAO,
                (PBOOLEAN)&m_ReadWriteModes.IsDAOPQ,
                (PBOOLEAN)&m_ReadWriteModes.IsDAOrawPW

	);

	//
	// Check for success
	//
	if (m_ExceptionNumber!= EN_SUCCESS)
	{
		CLogger::Log(Debug, lmtError,
			"\n\n RefreshReadWriteModes():StarBurn_CdvdBurnerGrabber_ProbeSupportedWriteModes() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
			);

		m_ReadWriteModes.IsTAO = FALSE;
		m_ReadWriteModes.IsDAOPQ = FALSE;
		m_ReadWriteModes.IsSAO = FALSE;
		m_ReadWriteModes.IsDAOrawPW = FALSE;
	}

	//
	// Try go get supported read modes
	//
	m_ExceptionNumber =
	StarBurn_CdvdBurnerGrabber_ProbeSupportedReadModes(
                m_CdvdBurnerGrabber,
                (char*)( &m_ExceptionText ),
                sizeof( m_ExceptionText ),
                &m_SystemError,
                &m_CDBFailureInfo,
                (PBOOLEAN)&m_ReadWriteModes.IsCooked,
                (PBOOLEAN)&m_ReadWriteModes.IsRaw,
                (PBOOLEAN)&m_ReadWriteModes.IsRawPQ,
                (PBOOLEAN)&m_ReadWriteModes.IsRawRawPW,
                (PBOOLEAN)&m_ReadWriteModes.IsPQ,
                (PBOOLEAN)&m_ReadWriteModes.IsRawPW

		);

	//
	// Check for success
	//
	if (m_ExceptionNumber != EN_SUCCESS)
	{
		CLogger::Log(Debug, lmtError,
			"\n\n RefreshReadWriteModes():StarBurn_CdvdBurnerGrabber_ProbeSupportedReadModes() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
			);

		m_ReadWriteModes.IsCooked = FALSE;
		m_ReadWriteModes.IsPQ = FALSE;
		m_ReadWriteModes.IsRaw = FALSE;
		m_ReadWriteModes.IsRawPW = FALSE;
		m_ReadWriteModes.IsRawPQ = FALSE;
		m_ReadWriteModes.IsRawRawPW = FALSE;
	}

	
	return true;
}

bool CDevice::RefreshTOCInfo()
{
	bool bResult(false);
	bResult = RefreshTOCInfoInternal();
	CleanUp();
	return bResult;
}

bool CDevice::RefreshTOCInfoInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
	{
		if(!CdvdBurnerGrabberCreate(&Callback, true))
		{			
			return false;
		}
	}

	StarBurn_SetFastReadTOC(TRUE);
         //Try to get TOC Info
                        m_ExceptionNumber =
                                StarBurn_CdvdBurnerGrabber_GetTOCInformation(
                                 m_CdvdBurnerGrabber,
                                 (char*)( &m_ExceptionText ),
                                 sizeof( m_ExceptionText ),
                                 &m_SystemError,
                                 &m_CDBFailureInfo,
                                 &m_TOCInfo);

                      // Check for success
                        if ( m_ExceptionNumber != EN_SUCCESS )
                        {
                                CLogger::Log(Debug,lmtError,
                                        "\n\nRefreshTOCInfo((): StarBurn_CdvdBurnerGrabber_GetTOCInformation() failed, exception %ld, status %ld, text '%s'\n",
                                        m_ExceptionNumber,
                                        m_SystemError,
                                        m_ExceptionText
                                 );
                                m_MediaInfo.MediaSize = 0;
								
                                return false;
                         }
		
        return true;                         
}

long CDevice::GetAudioFileStreamSizeInSeconds(char* AudioFilePath)
{
        long l_StreamSize;
        unsigned long l_SystemError;
        StarBurn_GetAudioFileStreamSizeInUCHARs(AudioFilePath,&l_StreamSize,&l_SystemError);
        return l_StreamSize/AUDIO_FILE_ONE_SECOND_STREAM_SIZE_IN_UCHARS;
};

long CDevice::GetAudioFileStreamSizeInUCHARs(char* AudioFilePath)
{
        long l_StreamSize;
        unsigned long l_SystemError;
        StarBurn_GetAudioFileStreamSizeInUCHARs(AudioFilePath,&l_StreamSize,&l_SystemError);
        return l_StreamSize;
}


PSCSI_DEVICE_ADDRESS CDevice::GetCurrentSCSIDeviceAddress()  
{
	return &m_SCSIDeviceAddress;
}

char* CDevice::GetCurrentInternalDeviceName()
{
	return m_InternalDeviceName;
}

TDeviceStatus CDevice::GetDeviceStatus()
{
        return m_DeviceStatus;
}

const char1024* CDevice::GetVendorID()
{
        return &m_DeviceInfo.VendorID;
}
const char1024* CDevice::GetProductID()
{
        return &m_DeviceInfo.ProductID;
}
const char1024* CDevice::GetProductRevisionLevel()
{
        return &m_DeviceInfo.ProductRevisionLevel;
}
const char* CDevice::GetDeviceLetter()
{
        return (char*)&m_DeviceInfo.DeviceLetter;
}

const DISC_TYPE CDevice::GetMediaType()
{
        return m_MediaInfo.MediaType;
}

void CDevice::GetMediaType(char* MediaType)
{
	switch (GetMediaType())
	{
	case DISC_TYPE_UNKNOWN:
		strcpy(MediaType,"Unknown");
		break;
	case DISC_TYPE_CDROM:
	case DISC_TYPE_DDCDROM:
		strcpy(MediaType,"CD-ROM");
		break;
	case DISC_TYPE_DDCDR:
	case DISC_TYPE_CDR:
		strcpy(MediaType,"CD-R");
		break;
	case DISC_TYPE_CDRW:
	case DISC_TYPE_DDCDRW:
		strcpy(MediaType,"CD-RW");
		break;
	case DISC_TYPE_DVDROM:
		strcpy(MediaType,"DVD-ROM");
		break;
	case DISC_TYPE_DVDR:
		strcpy(MediaType,"DVD-R");
		break;
        case DISC_TYPE_DVDR_DL:
		strcpy(MediaType,"DVD-R DL");
		break;
	case DISC_TYPE_DVDRAM:
		strcpy(MediaType,"DVD-RAM");
		break;
	case DISC_TYPE_DVDRWRO:
	case DISC_TYPE_DVDRWSR:
		strcpy(MediaType,"DVD-RW");
		break;
	case DISC_TYPE_DVDPLUSRW:
		strcpy(MediaType,"DVD+RW");
		break;
	case DISC_TYPE_DVDPLUSR:
		strcpy(MediaType,"DVD+R");
		break;
	case DISC_TYPE_DVDPLUSR_DL:
		strcpy(MediaType,"DVD+R DL");
		break;
	case DISC_TYPE_BDROM:
		strcpy(MediaType,"BD-ROM");
		break;
	case DISC_TYPE_BDR:
		strcpy(MediaType,"BD-R");
		break;
	case DISC_TYPE_BDRE:
		strcpy(MediaType,"BD-RE");
		break;
	case DISC_TYPE_HDDVDROM:
		strcpy(MediaType,"HD-DVD ROM");
		break;
	case DISC_TYPE_HDDVDR:
		strcpy(MediaType,"HD-DVD R");
		break;
	case DISC_TYPE_HDDVDRW:
		strcpy(MediaType,"HD-DVD RW");
		break;
	case DISC_TYPE_NO_MEDIA:
		strcpy(MediaType,"Empty");
	}
	return;
}

const double CDevice::GetMediaSize()
{
        return m_MediaInfo.MediaSize;
}
const double CDevice::GetMediaFreeSize()
{
        return m_MediaInfo.MediaFreeSize;
}
const char256* CDevice::GetMediaLabel()
{
        return &m_MediaInfo.MediaLabel;
}

void CDevice::GetDVDFeatures(TDVDFeatures* DVDFeatures)
{
        *DVDFeatures = m_DVDFeatures;
        return;
}

//PSTARBURN_TRACK_INFORMATION CDevice::GetHiddenTrackInformation()
//{
//	return &m_HiddenTrackInformation;
//}

unsigned long CDevice::GetBufferSize()
{
        return m_DeviceInfo.BufferSizeInUCHARs;
}

void CDevice::SetLastWrittenPercent(int Percent)
{
        EnterCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
        m_LastWrittenPercent = Percent;
        LeaveCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
}

int CDevice::GetLastWrittenPercent()
{

        EnterCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
        int l_LastWrittenPercent = m_LastWrittenPercent;
        LeaveCriticalSection(&m_CRITICAL_SECTION_LastWrittenPercent);
        return  l_LastWrittenPercent;
}

void CDevice::GetDeviceSupportedMediaFormats(TDeviceSupportedMediaFormats* DeviceSupportedMediaFormats)
{
        *DeviceSupportedMediaFormats = m_DeviceSupportedMediaFormats;
}

void CDevice::GetReadWriteModes(TReadWriteModes* ReadWriteModes)
{
        *ReadWriteModes = m_ReadWriteModes;
}

void CDevice::GetTOCInfo(TOC_INFORMATION* pTOCInfo)
{
        *pTOCInfo = m_TOCInfo;
}


void  CDevice::GetDirSize(
           TCHAR* p__PTCHAR__DirName,
           PLARGE_INTEGER p__PLARGE_INTEGER__DirSize
)
{

    HANDLE l__HANDLE__Find;
    WIN32_FIND_DATA l__WIN32_FIND_DATA;

    if (p__PTCHAR__DirName[_tcslen(p__PTCHAR__DirName) - 1] == '\\')
    {
        p__PTCHAR__DirName[_tcslen(p__PTCHAR__DirName) - 1] = '\0';
    }

    TCHAR l__TCHAR__DirName[1024];

    memset(l__TCHAR__DirName,0,sizeof(l__TCHAR__DirName));

    _stprintf(
        l__TCHAR__DirName,
        _T("%s\\*")    ,
        p__PTCHAR__DirName
        );


    l__HANDLE__Find =
            FindFirstFile(
                l__TCHAR__DirName,
                &l__WIN32_FIND_DATA
                );

    if (l__HANDLE__Find == INVALID_HANDLE_VALUE || l__HANDLE__Find == NULL)
    {
        /*
        LOG(
            "GetDirSize(): error: FindFirstFile() failed for '%s' with error %d\n" ,
            l__CHAR__DirName,
            GetLastError()
            );*/

        return;
    }

    while( FindNextFile(l__HANDLE__Find,&l__WIN32_FIND_DATA) )
    {
        TCHAR l__TCHAR__FilePath[1024];
        memset(l__TCHAR__FilePath,0,sizeof(l__TCHAR__FilePath));
        _stprintf(
            l__TCHAR__FilePath,
            _T("%s\\%s"),
            p__PTCHAR__DirName,
            l__WIN32_FIND_DATA.cFileName
            );

        if ((l__WIN32_FIND_DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  &&
            _tcscmp(l__WIN32_FIND_DATA.cFileName,_T(".."))
            )
        {
            GetDirSize(
                l__TCHAR__FilePath,
                p__PLARGE_INTEGER__DirSize
                );
        }

        else
        {
            HANDLE l__HANDLE__File =
                CreateFile(
                    l__TCHAR__FilePath,
                    GENERIC_READ,
                    FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                    0,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0
                    );

            if (l__HANDLE__File != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER l__LARGE_INTEGER__FileSize;
                GetFileSizeEx(
                    l__HANDLE__File,
                    &l__LARGE_INTEGER__FileSize
                    );

                p__PLARGE_INTEGER__DirSize->QuadPart +=
                    l__LARGE_INTEGER__FileSize.QuadPart;

				CloseHandle(l__HANDLE__File);
            }
			
        }
    }

    FindClose(l__HANDLE__Find);
}

bool operator == (TSpeed first,TSpeed second)
{
        if(first.SpeedKBps == second.SpeedKBps)
        {
                return true;
        }
        else
        {
                return false;
        }
};

SCSI_TRANSPORT CDevice::GetSCSITransport()
{
        return m_SCSITransport;
}

int CDevice::GetCurrentTrackIndex()
{
	EnterCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
        int l_CurrentTrackIndex = m_CurrentTrackIndex;
	LeaveCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
        return l_CurrentTrackIndex;
}

void CDevice::SetCurrentTrackIndex(int Index)
{
	EnterCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
        m_CurrentTrackIndex = Index;
	LeaveCriticalSection(&m_CRITICAL_SECTION_CurrentTrackIndex);
}

//Lock device
bool CDevice::Lock()
{
	bool bResult(false);
	bResult = LockInternal();
	CleanUp();

	return bResult;
}

//Lock device
bool CDevice::LockInternal()
{
	//Create CdvdBurnerGrabber object if need
       if(NULL == m_CdvdBurnerGrabber )
                  if(!CdvdBurnerGrabberCreate(&Callback, true, false))
                         return false;

	m_ExceptionNumber = 
	StarBurn_CdvdBurnerGrabber_Lock( 
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo
		);

	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,
			lmtError,
			"\n\nGetReadSpeeds(): StarBurn_CdvdBurnerGrabber_Lock() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
            );           
		
		return false;
	}

	
	return true;
}
//Release device
bool CDevice::Release()
{
	bool bResult(false);
	bResult = ReleaseInternal();
	CleanUp();

	return bResult;
}

//Release device
bool CDevice::ReleaseInternal()
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true, false))
			return false;

	m_ExceptionNumber = 
	StarBurn_CdvdBurnerGrabber_Release( 
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo
		);

	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,
			lmtError,
			"\n\nGetReadSpeeds(): StarBurn_CdvdBurnerGrabber_Release() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
            );

		
		return false;
	}

	
	return true;
}
//Get device lock
bool CDevice::GetLock(bool &p_IsLocked)
{
	bool bResult(false);
	bResult = GetLockInternal(p_IsLocked);
	CleanUp();

	return bResult;
}

//Get device lock
bool CDevice::GetLockInternal(bool &p_IsLocked)
{
	//Create CdvdBurnerGrabber object if need
	if(NULL == m_CdvdBurnerGrabber )
		if(!CdvdBurnerGrabberCreate(&Callback, true))
			return false;

	BOOLEAN l_IsLockUnLockSupported = FALSE;
	BOOLEAN l_IsLocked = FALSE;
	BOOLEAN l_IsLoadEjectSupported = FALSE;
	UCHAR l_LoadingMechanismType = 0x00;

	// Try to close the session
	m_ExceptionNumber =
	StarBurn_CdvdBurnerGrabber_GetMechanicalOptions(
		m_CdvdBurnerGrabber,
		( PCHAR )( &m_ExceptionText ),
		sizeof( m_ExceptionText ),
		&m_SystemError,
		&m_CDBFailureInfo,
		&l_IsLockUnLockSupported,
		&l_IsLocked,
		&l_IsLoadEjectSupported,
		&l_LoadingMechanismType
		);

	if ( m_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,
			lmtError,
			"\n\nGetReadSpeeds(): StarBurn_CdvdBurnerGrabber_GetMechanicalOptions() failed, exception %ld, status %ld, text '%s'\n",
			m_ExceptionNumber,
			m_SystemError,
			m_ExceptionText
            );    

		
		return false;
	}
	else
	{
		p_IsLocked = TRUE == l_IsLocked;

		
		return true;
	}
}

bool CDevice::GetDeviceSCSIAddress(
								   LONG StarPortDeviceTargetId,
								   PSCSI_DEVICE_ADDRESS p__PSCSI_DEVICE_ADDRESS)
{
	ULONG  l_SystemError;        
	EXCEPTION_NUMBER l_ExceptionNumber;

	l_ExceptionNumber =
		StarBurn_StarPort_GetDeviceSCSIAddress(
		StarPortDeviceTargetId, 
		p__PSCSI_DEVICE_ADDRESS,
		&l_SystemError);

	// Check for success	
	if ( l_ExceptionNumber != EN_SUCCESS )
	{
		CLogger::Log(Debug,lmtError,
			"\n\nCDevice::GetDeviceSCSIAddress()): StarBurn_StarPort_GetDeviceSCSIAddress() failed, exception %ld, status %ld\n",
			l_ExceptionNumber,
			l_SystemError					
			);
		return false;
	}

	return true;
}

bool CDevice::GetDeviceSPTIInternalNameByLetter(const char* DeviceLetter, char* DeviceInternalName)
{
	char d = *DeviceLetter;
	char l_csDeviceName[3] = {d,':','\0'};
	char l_csTarget[512] = {0};
	if(QueryDosDeviceA(l_csDeviceName, l_csTarget, 511) != 0)
	{
		strcpy(DeviceInternalName, &l_csTarget[8]);	
		return true;
	}

	return false;
}

bool CDevice::ShutDownStarBurnDll()
{
	//
	// Uninitalize StarBurn, do not care about execution status
	//
	if (EN_SUCCESS == StarBurn_DownShut())
	{
		return true;
	}
	else
	{
		return false;
	}	
	
}

bool CDevice::IsMediaTypeCD()
{
	switch (GetMediaType())
	{
	case DISC_TYPE_CDROM:
	case DISC_TYPE_DDCDROM:
	case DISC_TYPE_DDCDR:
	case DISC_TYPE_CDR:
	case DISC_TYPE_CDRW:
	case DISC_TYPE_DDCDRW:
		{
			return true;
		}
		break;
	default:
		{
			return false;
		}
		break;
	}	
}

bool CDevice::IsMediaTypeDVD()
{
	switch (GetMediaType())
	{
	case DISC_TYPE_DVDROM:
	case DISC_TYPE_DVDR:
	case DISC_TYPE_DVDR_DL:
	case DISC_TYPE_DVDRAM:
	case DISC_TYPE_DVDRWRO:
	case DISC_TYPE_DVDRWSR:
	case DISC_TYPE_DVDPLUSRW:
	case DISC_TYPE_DVDPLUSR:
	case DISC_TYPE_DVDPLUSR_DL:
		{
			return true;
		}
		break;
	default:
		{
			return false;
		}
		break;
	}
}

bool CDevice::IsMediaTypeBluRay()
{
	switch (GetMediaType())
	{
	case DISC_TYPE_BDROM:
	case DISC_TYPE_BDR:
	case DISC_TYPE_BDRE:
		{
			return true;
		}		
		break;
	default:
		{
			return false;
		}
		break;
	}
}

bool CDevice::IsMediaTypeHDDVD()
{
	switch (GetMediaType())
	{
	case DISC_TYPE_HDDVDROM:
	case DISC_TYPE_HDDVDR:
	case DISC_TYPE_HDDVDRW:
		{
			return true;
		}
		break;
	default:
		{
			return false;
		}
		break;
	}
}

unsigned long CDevice::GetLastSystemError()
{
        return m_SystemError;
};

EXCEPTION_NUMBER CDevice::GetLastExceptionNumber()
{
        return m_ExceptionNumber;
};

void CDevice::GetLastExceptionText(char* p_csExceptionText, unsigned int p_intBufferSizeInUCHARs)
{
        if(strlen(&m_ExceptionText[0]) > p_intBufferSizeInUCHARs)
        {
                strncpy(p_csExceptionText, &m_ExceptionText[0], p_intBufferSizeInUCHARs-1);
                p_csExceptionText[p_intBufferSizeInUCHARs-1] = '\0';
        }
        else
        {
                strcpy(p_csExceptionText, &m_ExceptionText[0]);
        }

        return;
}


void CDevice::GetLastCDBFailureInformation(PCDB_FAILURE_INFORMATION p_pCDBFailureInfo)
{
	if (p_pCDBFailureInfo)
	{
		*p_pCDBFailureInfo = m_CDBFailureInfo;
	}
	
	return;	
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualDevice
CVirtualDevice::CVirtualDevice(char* InternalDeviceName, unsigned int uTargetID):
CDevice(InternalDeviceName)
{	
	m_uTargetID = uTargetID;
}

CVirtualDevice::CVirtualDevice():
CDevice()
{		
}


CVirtualDevice::CVirtualDevice(CDevice& Device, unsigned int uTargetID):
CDevice(Device)
{	
	m_uTargetID = uTargetID;
}


unsigned int CVirtualDevice::GetTargetID()
{
	return m_uTargetID;
}



//---------------------------------------------------------------------------

//CCopressor realization

CCompressor::CCompressor()
{
};

ULONG __stdcall CCompressor::CompressorCallbackDispatch(
	IN ULONG p__ULONG__Reason,
	IN ULONG p__ULONG__Parameter,
	IN PVOID p__PVOID__Context
	)
{
	/*++

Routine Description:

	Our callback dispatch for compressor routines

Arguments:

	Reason of calling,
	Reason-specific parameter value,
	Pointer to context value

Return Value:

    Execution status

--*/

	//
	// Check do we have progress indication
	//
        CCompressor* compressor = (CCompressor*)p__PVOID__Context;

        compressor->CompressorCallback(p__ULONG__Reason, p__ULONG__Parameter);

        if (CompStatCanceling == compressor->GetCompressorStatus())
            {
                return ERROR_SUCCESS+1;
            }
        else
            {
                return ERROR_SUCCESS;
            }

}

void __stdcall CCompressor::CompressorCallback(
	IN ULONG p__ULONG__Reason,
	IN ULONG p__ULONG__Parameter
	)
{
	/*++

Routine Description:

	Our callback for compressor routines

Arguments:

	Reason of calling,
	Reason-specific parameter value,

--*/
	//
	// Check do we have progress indication
	//

	if ( p__ULONG__Reason == STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS )
	{
		//
		// Check did we print this percent already
		//

		m_LastCompressedPercent =(( LONG )( p__ULONG__Parameter ));
	}
}


VOID
__stdcall
CCompressor::AudioCompressor_WaveFileHeaderFormat(
	OUT PWAVE_FILE_HEADER p__PWAVE_FILE_HEADER,
        IN ULONG p__ULONG__DataSizeInUCHARs
        )

/*++

Routine Description:

	Formats WAV file header

Arguments:

	Pointer to WAV file header,
	Data payload size in UCHARs

Return Value:

	Nothing

--*/

{
	//
	// Zero out whole input buffer initially
	//
	ZeroMemory(
		p__PWAVE_FILE_HEADER,
		sizeof( WAVE_FILE_HEADER )
		);

    //
    // Set mandatory fields in the header
    //

    p__PWAVE_FILE_HEADER->m__ULONG__Riff = WAVE_FILE_RIFF_SIGNATURE;

    p__PWAVE_FILE_HEADER->m__ULONG__Wave = WAVE_FILE_WAVE_SIGNATURE;

    p__PWAVE_FILE_HEADER->m__ULONG__FormatTag = WAVE_FILE_TAG_SIGNATURE;

    p__PWAVE_FILE_HEADER->m__ULONG__FormatLength = sizeof( WAVE_FORMAT_CHUNK );

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__USHORT__Format = WAVE_FILE_WAVE_FORMAT;

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__USHORT__Channels = WAVE_FILE_CHANNELS;

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__ULONG__SamplesPerSecond = WAVE_FILE_SAMPLES_PER_SECOND;

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__ULONG__AverageSamplesPerSecond = 
		( WAVE_FILE_SAMPLES_PER_SECOND * WAVE_FILE_CHANNELS * 2 );

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__USHORT__Alignment = WAVE_FILE_ALIGNMENT;

    p__PWAVE_FILE_HEADER->m__WAVE_FORMAT_CHUNK.m__USHORT__BitsPerSample = WAVE_FILE_BITS_PER_SAMPLE;

    p__PWAVE_FILE_HEADER->m__ULONG__DataTag = WAVE_FILE_DATA_SIGNATURE;

    //
    // Set size-dependent fields
    //

    p__PWAVE_FILE_HEADER->m__ULONG__Length = ( p__ULONG__DataSizeInUCHARs + sizeof( WAVE_FILE_HEADER ) - 8 );

    p__PWAVE_FILE_HEADER->m__ULONG__DataLength = p__ULONG__DataSizeInUCHARs;
}



bool
__stdcall
CCompressor::AudioCompressor_UncompressedFileCompress(
        IN TCompressMode Mode,
	IN PCHAR p__PCHAR__SourceUncompressedFileName,
	IN PCHAR p__PCHAR__DestinationCompressedFileName,
	IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
	IN STARBURN_STARWAVE_COMPRESSION p__STARBURN_STARWAVE_COMPRESSION
	)

/*++

Routine Description:

	Compresses uncompressed file

Arguments:

	Pointer to source uncompressed file name,
	Pointer to destination compressed file name,
	Working buffer size in UCHARs (recommeded to have it equal to STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS),
	Pointer to StarWave callback function (to handle progress and cancellation I/O),
	Pointer to context value passed to StarWave callback function,
	Compression mode selected

Return Value:

    Execution status

--*/

{

  ULONG l__ULONG__Status = ERROR_CALL_NOT_IMPLEMENTED; // Assume failure by default

  m_CompressorStatus = CompStatCompressing;

  m_LastCompressedPercent = 0;    

  try
  {
    switch(Mode)
   {
    case Complex:
      {

			PVOID l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT = NULL;

			ULONG l__ULONG__LowSizeInUCHARs = 0; // Low part of file size in UCHARs

			ULONG l__ULONG__LowPositionInUCHARs = 0; // Low part of seeking procedure

			ULONG l__ULONG__LeftToReadSizeInUCHARs = 0;

			ULONG l__ULONG__IoSizeInUCHARs = 0;

			ULONG l__ULONG__UCHARsReaden = 0;

			ULONG l__ULONG__Index = 0;

			ULONG l__ULONG__CompletionPercent = 0;

			HANDLE l__HANDLE__UncompressedFile = INVALID_HANDLE_VALUE;

			PUCHAR l__PUCHAR__DataBuffer = NULL; // Pointer to data buffer initalized with bad value

			//
			// Try to determine do we have supported file for compression
			//
			l__ULONG__Status = StarBurn_StarWave_UncompressedFileSupportedIs( p__PCHAR__SourceUncompressedFileName );

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
                                 CLogger::Log(
                                        Debug,
                                        lmtError,
					"*** %s(): StarWave_UncompressedFileSupportedIs( '%s' ) failed, status %d ( 0x%x )\n",
					__FUNCTION__,
					p__PCHAR__SourceUncompressedFileName,
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Try to open uncompressed file
			//
			l__HANDLE__UncompressedFile =
				CreateFileA(
					p__PCHAR__SourceUncompressedFileName,
					GENERIC_READ,
					FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
					);

			//
			// Check for success
			//
			if ( l__HANDLE__UncompressedFile == INVALID_HANDLE_VALUE )
			{
				//
				// Get execution status
				//
				l__ULONG__Status = GetLastError();

                        CLogger::Log(
                                        Debug,
                                        lmtError,
					"*** %s(): CreateFileA( '%s', GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) failed, status %d ( 0x%x )\n",
					__FUNCTION__,
					p__PCHAR__SourceUncompressedFileName,
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
			}		

			//
			// Try to get uncompressed file size in UCHARs
			//
			l__ULONG__LowSizeInUCHARs =
				GetFileSize(
					l__HANDLE__UncompressedFile, 
					NULL 
					);

			//
			// Check for success
			//
			if ( l__ULONG__LowSizeInUCHARs == INVALID_FILE_SIZE )
			{
				//
				// Get execution status
				//
				l__ULONG__Status = GetLastError();

				CLogger::Log(
				Debug,
				lmtError,
				"*** %s(): GetFileSize( 0x%x, NULL ) failed, status %d ( 0x%x )\n",
				__FUNCTION__,
				l__HANDLE__UncompressedFile,
				l__ULONG__Status,
				l__ULONG__Status
				);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// There is no need to check for correct file size as header was checked already and file is considered to be correct
			//

			//
			// Update file size with the header
			//
			l__ULONG__LowSizeInUCHARs -= sizeof( WAVE_FILE_HEADER );

			//
			// Try to set file pointer in UCHARs
			//
			l__ULONG__LowPositionInUCHARs =
				SetFilePointer(
					l__HANDLE__UncompressedFile,
					sizeof( WAVE_FILE_HEADER ), 
					NULL, 
					FILE_BEGIN
					);
	
			//
			// Check for success
			//
			if ( l__ULONG__LowPositionInUCHARs == INVALID_SET_FILE_POINTER )
			{
				//
				// Get execution status
				//
				l__ULONG__Status = GetLastError();

                        CLogger::Log(
                                        Debug,
                                        lmtError,

					"*** %s(): SetFilePointer( 0x%x, %d, NULL, FILE_BEGIN ) failed, status %d ( 0x%x )\n",
					__FUNCTION__,
					l__HANDLE__UncompressedFile,
					sizeof( WAVE_FILE_HEADER ), 
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
		    }

			//
			// Check do we have correct pointer now
			//
			if ( l__ULONG__LowPositionInUCHARs != sizeof( WAVE_FILE_HEADER ) )
			{
				//
				// Set execution status to bad one
				//
				l__ULONG__Status = ERROR_SEEK;

                        CLogger::Log(
                                        Debug,
                                        lmtError,
 					"*** %s(): SetFilePointer( 0x%x, %d, NULL, FILE_BEGIN ) failed, set position to %d instead of %d, status %d ( 0x%x )\n",
					__FUNCTION__,
					l__HANDLE__UncompressedFile, 
					sizeof( WAVE_FILE_HEADER ),
					l__ULONG__LowPositionInUCHARs,
					sizeof( WAVE_FILE_HEADER ),
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
			}
		
			//
			// Try to allocate memory for data buffer
			//
			l__PUCHAR__DataBuffer =
				(PUCHAR) VirtualAlloc(
					NULL,
					p__ULONG__WorkingBufferSizeInUCHARs,
					MEM_COMMIT,
					PAGE_READWRITE
					);

			//
			// Check for success
			//
			if ( l__PUCHAR__DataBuffer == NULL )
			{
				//
				// Get execution status 
				//
				l__ULONG__Status = GetLastError();

                        CLogger::Log(
                                        Debug,
                                        lmtError,
					"*** %s(): VirtualAlloc( 0x%p, %d, %d, %d ) failed, status %d ( 0x%x )\n",
					__FUNCTION__,
					NULL,
					p__ULONG__WorkingBufferSizeInUCHARs,
					MEM_COMMIT,
					PAGE_READWRITE,
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Try to create compressed file writer object
			//
			l__ULONG__Status =
				StarBurn_StarWave_CompressedFileWriterObjectCreate(
					( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT ),
					p__PCHAR__DestinationCompressedFileName,
					p__STARBURN_STARWAVE_COMPRESSION
					);

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
                        CLogger::Log(
                                        Debug,
                                        lmtError,
					"*** %s(): StarWave_CompressedFileWriterObjectCreate( 0x%p, '%s', %d ) failed, status %d ( 0x%x )\n",
					__FUNCTION__,
					&l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT,
					p__PCHAR__DestinationCompressedFileName,
					p__STARBURN_STARWAVE_COMPRESSION,
					l__ULONG__Status,
					l__ULONG__Status
					);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Keep number of UCHARs we need to read
			//
			l__ULONG__LeftToReadSizeInUCHARs = l__ULONG__LowSizeInUCHARs;

			//
			// Set default I/O size in UCHARs
			//
			l__ULONG__IoSizeInUCHARs = p__ULONG__WorkingBufferSizeInUCHARs;

			//
			// Process every chunk
			//
			for ( l__ULONG__Index = 0; l__ULONG__Index < l__ULONG__LowSizeInUCHARs; l__ULONG__Index += p__ULONG__WorkingBufferSizeInUCHARs )
			{

				//
				// Calculate completion percent in the safe way
				//
				if ( l__ULONG__LowSizeInUCHARs != 0 )
				{
					l__ULONG__CompletionPercent =
						( ULONG )
							( ( ( double )( l__ULONG__LowSizeInUCHARs - l__ULONG__LeftToReadSizeInUCHARs ) * 100 ) / l__ULONG__LowSizeInUCHARs );
				}
				else
				{
					l__ULONG__CompletionPercent = 0;
				}

				//
				// Check do we need to cancel I/O and do some progress indication
				//
				l__ULONG__Status =
					CompressorCallbackDispatch(
						STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
						l__ULONG__CompletionPercent,
						this
						);

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
					//
					// Set execution status (repatch)
					//
					l__ULONG__Status = ERROR_CANCELLED;

                                CLogger::Log(
                                                Debug,
                                                lmtError,
						"*** %s(): ( %d, %d, 0x%p ) failed, status %d ( 0x%x )\n",
						__FUNCTION__,												
						STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
						l__ULONG__CompletionPercent,
						this,
						l__ULONG__Status,
                                                l__ULONG__Status
                                                
                                             );

					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Check do we need to patch I/O size
				//
				if ( l__ULONG__LeftToReadSizeInUCHARs < p__ULONG__WorkingBufferSizeInUCHARs )
				{
					//
					// Replace with new value
					//
					l__ULONG__IoSizeInUCHARs = l__ULONG__LeftToReadSizeInUCHARs;
				}

				//
				// Try to read current chunk
				//
				if (
					ReadFile(
						l__HANDLE__UncompressedFile,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs,
						&l__ULONG__UCHARsReaden,
						NULL
						) == FALSE
				)
				{
					//
					// Get execution status
					//
					l__ULONG__Status = GetLastError();

                                        CLogger::Log(
                                                Debug,
                                                lmtError,
 						"*** %s(): ReadFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed, status %d ( 0x%x )\n",
						__FUNCTION__,
						l__HANDLE__UncompressedFile,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs,
						&l__ULONG__UCHARsReaden,
						l__ULONG__Status,
						l__ULONG__Status
						);

					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Check for size of readen be the same as expected
				//
				if ( l__ULONG__UCHARsReaden != l__ULONG__IoSizeInUCHARs )
				{
					//
					// Set execution status to bad one
					//
					l__ULONG__Status = ERROR_IO_INCOMPLETE;

                                        CLogger::Log(
                                                Debug,
                                                lmtError,
						"*** %s(): ReadFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed, read only %d instead of %d UCHARs, status %d ( 0x%x )\n",
						__FUNCTION__,
						l__HANDLE__UncompressedFile,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs,
						&l__ULONG__UCHARsReaden,
						l__ULONG__UCHARsReaden,
						l__ULONG__IoSizeInUCHARs,
						l__ULONG__Status,
						l__ULONG__Status
						);

					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Try to write current chunk to the compressed file
				//
				l__ULONG__Status =
					StarBurn_StarWave_CompressedFileWriterObjectWrite(
						l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs
						);

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
                                        CLogger::Log(
                                                Debug,
                                                lmtError,
						"*** %s(): StarBurn_StarWave_CompressedFileWriterObjectWrite( 0x%p, 0x%p, %d ) failed, read only %d instead of %d UCHARs, status %d ( 0x%x )\n",
						__FUNCTION__,
						l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs,
						l__ULONG__Status,
						l__ULONG__Status
						);

					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Update left to read
				//
				l__ULONG__LeftToReadSizeInUCHARs -= p__ULONG__WorkingBufferSizeInUCHARs;
			}
	
			//
			// Set execution status to good one
			//
			l__ULONG__Status = ERROR_SUCCESS;

			//
			// Check do we need to free STARWAVE_COMPRESSED_FILE_WRITER_OBJECT
			//
			if ( l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT != NULL )
			{
				//
				// Free allocated memory
				//
				StarBurn_StarWave_CompressedFileWriterObjectDestroy( ( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT ) );
			}

			//
			// Check do we need to close uncompressed file
			//
			if ( l__HANDLE__UncompressedFile != INVALID_HANDLE_VALUE )
			{
				//
				// Close uncompressed file handle
				//
				CloseHandle( l__HANDLE__UncompressedFile );
			}

			//
			// Check do we need to free the buffer
			//
			if ( l__PUCHAR__DataBuffer != NULL )
			{
				//
				// Free allocated memory
				//
				VirtualFree(
					l__PUCHAR__DataBuffer,
					0,
					MEM_RELEASE
					);
			}



    }
    break;
    case Simple:
    {
        // Using high-level StarBurn API compression function
	// Try to uncompress compressed
	//

       	l__ULONG__Status =
		StarBurn_StarWave_UncompressedFileCompress(
			p__PCHAR__SourceUncompressedFileName,
			p__PCHAR__DestinationCompressedFileName,
			STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS,
			CompressorCallbackDispatch,
			this,
                        STARBURN_STARWAVE_COMPRESSION_WMA_VBR_Q98
			);
        if ( l__ULONG__Status != ERROR_SUCCESS )
        {
                throw 0;
        }

    }
   }
  }
  catch(...)
  {
	  //
	  // Get exception status
	  //
	  l__ULONG__Status = GetLastError();

	  CLogger::Log(
		  Debug,
		  lmtError,
		  "*** %s(): Exception caught, status %d ( 0x%x )\n",
		  __FUNCTION__,
		  l__ULONG__Status,
		  l__ULONG__Status
		  );
	  //
	  // Check do we have bad status
	  //
	  if ( l__ULONG__Status != ERROR_SUCCESS )
	  {
		  //
		  // Delete output file
		  //
		  DeleteFileA( p__PCHAR__DestinationCompressedFileName );
	  }
    //
	// Uninitalize StarBurn, do not care about execution status
	//
	StarBurn_DownShut();
  }


  m_LastCompressedPercent = 100;

  if(CompStatCanceling == m_CompressorStatus)
  {
        m_CompressorStatus = CompStatCanceled;
        return false;
  }
  else
  {
        if(ERROR_SUCCESS == l__ULONG__Status)
        {
                m_CompressorStatus = CompStatDone;
                return true;
        }
        else
        {
                m_CompressorStatus = CompStatError;
                return false;
        }
  }


}


bool
__stdcall
CCompressor::AudioCompressor_CompressedFileUncompress(
        IN TCompressMode Mode,
	IN PCHAR p__PCHAR__SourceCompressedFileName,
	IN PCHAR p__PCHAR__DestinationUncompressedFileName,
	IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
	IN BOOLEAN p__BOOLEAN__IsWavHeaderRequired,
	OUT PULONG p__PULONG__UncompressedSizeInUCHARs
	)

/*++

Routine Description:

	Decompresses compressed file

Arguments:

	Pointer to source compressed file name,
	Pointer to destination decompressed file name,
	Working buffer size in UCHARs (recommeded to have it equal to STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS),
	Is WAV header required on output WAV file (in case of FALSE no WAV header would be recorded),
	Pointer to the variable to receive uncompressed stream size in UCHARs,
	Pointer to StarWave callback function (to handle progress and cancellation I/O),
	Pointer to context value passed to StarWave callback function

Return Value:

    Execution status

--*/

{

   ULONG l__ULONG__Status = ERROR_CALL_NOT_IMPLEMENTED; // Assume failure by default

   m_CompressorStatus = CompStatUncompressing;

   m_LastCompressedPercent = 0;

   try
   {   
		switch (Mode)
		{
			case Complex:
			{

				ULONG l__ULONG__SizeInUCHARs = 0; // Initialize file size in UCHARs with zero

				ULONG l__ULONG__LeftToReadSizeInUCHARs = 0; // Initialize left to read size in UCHARs to known value

				ULONG l__ULONG__CompletionPercent = 0; // Initialize completion percent with zero

				PUCHAR l__PUCHAR__Buffer = NULL; // Pointer to I/O buffer initialized with NULL

				ULONG l__ULONG__IoSizeInUCHARs = 0; // Default I/O size

				ULONG l__ULONG__Index = 0; // Processing iterations counter initialized with bad value

				HANDLE l__HANDLE__Output = INVALID_HANDLE_VALUE; // Output file handle initialized with known bad value

				WAVE_FILE_HEADER l__WAVE_FILE_HEADER;

				ULONG l__ULONG__UCHARsWritten = 0; // Written UCHARs counter initialized with zero

				PVOID l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT = NULL;		         

				//
				// Check can we write at passed pointer with size of ULONG
				//
				if(
					IsBadWritePtr(
						p__PULONG__UncompressedSizeInUCHARs,
						sizeof( ULONG )
						) != 0
					)
				{
					//
					// Set execution status to bad one
					//
					l__ULONG__Status = ERROR_INVALID_PARAMETER;

					CLogger::Log(
								Debug,
								lmtError,
								"*** %s(): IsBadWritePtr( 0x%p, %d ) failed, status %d ( 0x%x )\n",
		            			__FUNCTION__,
			           			p__PULONG__UncompressedSizeInUCHARs,
								sizeof( ULONG ),
        						l__ULONG__Status,
        	        			l__ULONG__Status
								);

					throw 0;
				}

				//
				// Try to create compressed file reader object
				//
				l__ULONG__Status =
					StarBurn_StarWave_CompressedFileReaderObjectCreate(
						( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT ),
						p__PCHAR__SourceCompressedFileName
						);
				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
					CLogger::Log(
								Debug,
								lmtError,
								"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectCreate( 0x%p, '%s' ) failed, status %d ( 0x%x )\n",
				       			__FUNCTION__,
								&l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
								p__PCHAR__SourceCompressedFileName,
								l__ULONG__Status,
								l__ULONG__Status
								);
					//
					// Get out of here
					//
					throw 0;
				}

				//
				// Try to determine uncompressed data size
				//
				l__ULONG__Status =
					StarBurn_StarWave_CompressedFileReaderObjectUncompressedSizeGet(
						l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
						p__PULONG__UncompressedSizeInUCHARs
						);

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
					CLogger::Log(
								Debug,
								lmtError,
								"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectUncompressedSizeGet( 0x%p, 0x%p ) failed, status %d ( 0x%x )\n",
                				__FUNCTION__,
		        				l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
				       			p__PULONG__UncompressedSizeInUCHARs,
                				l__ULONG__Status,
		        				l__ULONG__Status
								);
					//
					// Get out of here
					//
					throw 0;
				}

				//
				// Try to seek to begin
				//
				l__ULONG__Status = StarBurn_StarWave_CompressedFileReaderObjectBeginSeek( l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT );

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
					CLogger::Log(
								Debug,
								lmtError,
								"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectBeginSeek( 0x%p ) failed, status %d ( 0x%x )\n",
								__FUNCTION__,
								l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
								l__ULONG__Status,
								l__ULONG__Status
								);

				
					// Get out of here				
					throw 0;
				}

				//
				// Try to allocate memory for I/O buffer
				//
				l__PUCHAR__Buffer = 
					(PUCHAR) VirtualAlloc(
		   	    		NULL,
    					p__ULONG__WorkingBufferSizeInUCHARs,
						MEM_COMMIT,
    					PAGE_READWRITE
						);

				//
				// Check for success
				//
				if ( l__PUCHAR__Buffer == NULL )
				{
					//
					// Get execution status
					//
					l__ULONG__Status = GetLastError();

					CLogger::Log(
								Debug,
								lmtError,
        						"*** %s(): VirtualAlloc( 0x%p, %d, %d, %d ) failed, status %d ( 0x%x )\n",
	        					__FUNCTION__,
		        				NULL,
			        			p__ULONG__WorkingBufferSizeInUCHARs,
				        		MEM_COMMIT,
								PAGE_READWRITE,
        						l__ULONG__Status,
	        					l__ULONG__Status
								);

					//
					// Get out of here
					//
					throw 0;
				}
			
				//
				// Try to create output file
				//
				l__HANDLE__Output =
					CreateFileA(
						p__PCHAR__DestinationUncompressedFileName,
						GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						CREATE_NEW,
						FILE_ATTRIBUTE_NORMAL,
						NULL 
						);

				//
				// Check for success
				//
				if ( l__HANDLE__Output == INVALID_HANDLE_VALUE )
				{
					//
					// Get execution status
					//
					l__ULONG__Status = GetLastError();

					CLogger::Log(
								Debug,
								lmtError,
				       			"*** %s(): CreateFileA( '%s', GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) failed, status %d ( 0x%x )\n",
				       			__FUNCTION__,
								p__PCHAR__DestinationUncompressedFileName,
        						l__ULONG__Status,
	        					l__ULONG__Status
								);
					//
					// Get out of here
					//
					throw 0;
				}

				//
				// Keep uncompressed file size in the local variable
				//
				l__ULONG__SizeInUCHARs = *p__PULONG__UncompressedSizeInUCHARs;

				//
				// Keep a copy of how much we should read
				//
				l__ULONG__LeftToReadSizeInUCHARs = l__ULONG__SizeInUCHARs;

				//
				// Set I/O to default size to size of I/O buffer
				//
				l__ULONG__IoSizeInUCHARs = p__ULONG__WorkingBufferSizeInUCHARs;

				//
				// Check for we need to format and save WAF file header
				//
				if ( p__BOOLEAN__IsWavHeaderRequired == TRUE )
				{
					//
					// Format WAVE file header
					//
					AudioCompressor_WaveFileHeaderFormat(
						&l__WAVE_FILE_HEADER,
						l__ULONG__SizeInUCHARs
						);

					//
					// Try to write the header to the output file
					//
					if (
						WriteFile(
							l__HANDLE__Output,
							&l__WAVE_FILE_HEADER,
							sizeof( l__WAVE_FILE_HEADER ),
							&l__ULONG__UCHARsWritten,
							NULL
							) == FALSE
						)
					{
						//
						// Get execution status
						//
						l__ULONG__Status = GetLastError();

						CLogger::Log(
									Debug,
									lmtError,
									"*** %s(): #1, WriteFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed, status %d ( 0x%x )\n",
									__FUNCTION__,
									l__HANDLE__Output,
									&l__WAVE_FILE_HEADER,
									sizeof( l__WAVE_FILE_HEADER ),
									&l__ULONG__UCHARsWritten,
									l__ULONG__Status,
									l__ULONG__Status
									);

						//
						// Get out of here
						//
						throw 0;
					}

					//
					// Check for written to be exactly the size of we asked
					//
					if ( l__ULONG__UCHARsWritten != sizeof( l__WAVE_FILE_HEADER ) )
					{
						//
						// Set execution status
						//
						l__ULONG__Status = ERROR_IO_INCOMPLETE;

						CLogger::Log(
									Debug,
									lmtError,
									"*** %s(): #1, WriteFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed by writing only %d UCHARs instead of asked %d, status %d ( 0x%x )\n",
									__FUNCTION__,
									l__HANDLE__Output,
									&l__WAVE_FILE_HEADER,
									sizeof( l__WAVE_FILE_HEADER ),
									&l__ULONG__UCHARsWritten,
									l__ULONG__UCHARsWritten,
									sizeof( l__WAVE_FILE_HEADER ),
									l__ULONG__Status,
									l__ULONG__Status
									);
					
						//
						// Get out of here
						//
						throw 0;
					}
				}

				//
				// Process every chunk
				//
				for ( l__ULONG__Index = 0; l__ULONG__Index < l__ULONG__SizeInUCHARs; l__ULONG__Index += p__ULONG__WorkingBufferSizeInUCHARs )
				{
					//
					// Calculate completion percent in the safe way
					//
					if ( l__ULONG__SizeInUCHARs != 0 )
					{
						l__ULONG__CompletionPercent =
							( ULONG )
							( ( ( double )( l__ULONG__SizeInUCHARs - l__ULONG__LeftToReadSizeInUCHARs ) * 100 ) / l__ULONG__SizeInUCHARs );
					}
					else
					{
						l__ULONG__CompletionPercent = 0;
					}

					//
					// Check do we need to cancel I/O and do some progress indication
					//
					l__ULONG__Status =
						CompressorCallbackDispatch(
							STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
							l__ULONG__CompletionPercent,
							this
							);

					//
					// Check for success
					//
					if ( l__ULONG__Status != ERROR_SUCCESS )
					{
						//
						// Set execution status (repatch)
						//
						l__ULONG__Status = ERROR_CANCELLED;


						CLogger::Log(
									Debug,
									lmtError,
									"*** %s(): ( %d, %d, 0x%p ) failed, status %d ( 0x%x )\n",
									__FUNCTION__,						
									STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
									l__ULONG__CompletionPercent,
									this,
									l__ULONG__Status,
									l__ULONG__Status
									);
					
						//
						// Get out of here
						//
						throw 0;
					}

					//
					// Check do we need to patch I/O size
					//
					if ( l__ULONG__LeftToReadSizeInUCHARs < p__ULONG__WorkingBufferSizeInUCHARs )
					{
						//
						// Replace with new value 
						//
						l__ULONG__IoSizeInUCHARs = l__ULONG__LeftToReadSizeInUCHARs;
					}

					//
					// Try to read current chunk
					//
	        		l__ULONG__Status = 
						StarBurn_StarWave_CompressedFileReaderObjectRead(
							l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
							l__PUCHAR__Buffer,
							l__ULONG__IoSizeInUCHARs
							);

					//
					// Check for success
					//
					if ( l__ULONG__Status != ERROR_SUCCESS )
					{
						CLogger::Log(
									Debug,
									lmtError,
				        			"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectRead( 0x%p, 0x%p, %d ) failed, status %d ( 0x%x )\n",
									__FUNCTION__,
									l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT,
									l__PUCHAR__Buffer,
									l__ULONG__IoSizeInUCHARs,
									l__ULONG__Status,
									l__ULONG__Status
									);
						//
						// Get out of here
						//
						throw 0;
       				}

					//
					// Try to write current chunk to the output file
					//
					if (
						WriteFile(
								l__HANDLE__Output,
								l__PUCHAR__Buffer,
								l__ULONG__IoSizeInUCHARs,
								&l__ULONG__UCHARsWritten,
								NULL
								) == FALSE
						)
					{
						//
						// Get execution status
						//
						l__ULONG__Status = GetLastError();

						CLogger::Log(
									Debug,
									lmtError,
									"*** %s(): #2, WriteFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed, status %d ( 0x%x )\n",
									__FUNCTION__,
									l__HANDLE__Output,
									l__PUCHAR__Buffer,
									l__ULONG__IoSizeInUCHARs,
									&l__ULONG__UCHARsWritten,
									l__ULONG__Status,
									l__ULONG__Status
									);

						//
						// Get out of here
						//
						throw 0;
					}

					//
					// Check for written to be exactly the size of we asked
					//
					if ( l__ULONG__UCHARsWritten != l__ULONG__IoSizeInUCHARs )
					{
						//
						// Set execution status
						//
						l__ULONG__Status = ERROR_IO_INCOMPLETE;

						CLogger::Log(
									Debug,
									lmtError,
									"*** %s(): WriteFile( 0x%x, 0x%p, %d, 0x%p, NULL ) failed by writing only %d UCHARs instead of asked %d, status %d ( 0x%x )\n",
									__FUNCTION__,
									l__HANDLE__Output,
									l__PUCHAR__Buffer,
									l__ULONG__IoSizeInUCHARs,
									&l__ULONG__UCHARsWritten,
									l__ULONG__UCHARsWritten,
									l__ULONG__IoSizeInUCHARs,
									l__ULONG__Status,
									l__ULONG__Status
									);

						//
						// Get out of here
						//
						throw 0;
					}

					//
					// Update left to read
					//
					l__ULONG__LeftToReadSizeInUCHARs -= p__ULONG__WorkingBufferSizeInUCHARs;
				}

				l__ULONG__Status = ERROR_SUCCESS;

				//
				// Check do we need to free I/O buffer
				//
				if ( l__PUCHAR__Buffer != NULL )
				{
					//
					// Free allocated memory
					//
					VirtualFree(
						l__PUCHAR__Buffer,
						0,
						MEM_RELEASE
						);
				}
				// 
				// Check do we need to free STARWAVE_COMPRESSION_FILE_OBJECT
				//
				if ( l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT != NULL )
				{
					//
					// Free allocated memory
					//
					StarBurn_StarWave_CompressedFileReaderObjectDestroy( ( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT ) );
				}

				//
				// Check was output file created
				//
				if ( l__HANDLE__Output != INVALID_HANDLE_VALUE )
				{
					//
					// Close output file
					//
					CloseHandle( l__HANDLE__Output );
				}

		}
			break;
			case Simple:
			{
				// Using high-level StarBurn API compression function
				// Try to compress uncompressed
				//

				l__ULONG__Status =
					StarBurn_StarWave_CompressedFileUncompress(
						p__PCHAR__SourceCompressedFileName,
						p__PCHAR__DestinationUncompressedFileName,
						STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS,
						TRUE,
						p__PULONG__UncompressedSizeInUCHARs,
						CompressorCallbackDispatch,
						this
						);
                                 if ( l__ULONG__Status != ERROR_SUCCESS )
                                {
                                        throw 0;
                                }
			}
		}
	}
	catch (...)
	{
		//
		// Check do we have bad status
		//
		if ( l__ULONG__Status != ERROR_SUCCESS )
		{
				//
				// Delete output file 
				//
				DeleteFileA( p__PCHAR__DestinationUncompressedFileName );		
		}
		//
		// Get exception status
		//
		l__ULONG__Status = GetLastError();

		CLogger::Log(
					Debug,
					lmtError,
					"*** %s(): Exception caught, status %d ( 0x%x )\n",
					__FUNCTION__,
					l__ULONG__Status,
					l__ULONG__Status
					);
	}
   m_LastCompressedPercent = 100;
   if(CompStatCanceling == m_CompressorStatus)
   {
        m_CompressorStatus = CompStatCanceled;
        return false;
   }
   else
   {
           if(ERROR_SUCCESS == l__ULONG__Status)
           {
                m_CompressorStatus = CompStatDone;
                return true;
           }
           else
           {
                m_CompressorStatus = CompStatError;
                return false;
           }
   }
}


bool
__stdcall
CCompressor::AudioCompressor_CompressedFileRecompress(
        IN TCompressMode Mode,
	IN PCHAR p__PCHAR__SourceCompressedFileName,
	IN PCHAR p__PCHAR__DestinationRecompressedFileName,
	IN ULONG p__ULONG__WorkingBufferSizeInUCHARs,
	OUT PULONG p__PULONG__UncompressedSizeInUCHARs,
	IN STARBURN_STARWAVE_COMPRESSION p__STARBURN_STARWAVE_COMPRESSION
	)

/*++

Routine Description:

	Compresses compressed file

Arguments:

	Pointer to source compressed file name,
	Pointer to destination compressed file name,
	Working buffer size in UCHARs (recommeded to have it equal to STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS),
	Pointer to the variable to receive uncompressed payload size in UCHARs,
	Pointer to StarWave callback function (to handle progress and cancellation I/O),
	Pointer to context value passed to StarWave callback function,
	Compression mode selected

Return Value:

    Execution status

--*/

{

  ULONG l__ULONG__Status = ERROR_CALL_NOT_IMPLEMENTED; // Assume failure by default

  m_CompressorStatus = CompStatRecompressing;

  m_LastCompressedPercent = 0;

  try
  {
	switch (Mode)
	{
	case Complex:
		{

			PVOID l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source = NULL;

			PVOID l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination = NULL;

			ULONG l__ULONG__LeftToReadSizeInUCHARs = 0;

			ULONG l__ULONG__IoSizeInUCHARs = 0;

			ULONG l__ULONG__Index = 0;

			ULONG l__ULONG__CompletionPercent = 0;

			PUCHAR l__PUCHAR__DataBuffer = NULL; // Pointer to data buffer initalized with bad value


			//
			// There is no need to check for compressed file is it compatible or not with the call to special exported function
			// called StarWave_CompressedFileSupportedIs(...) b/s it does nothing except trying to open the file with the call
			// to StarWave_CompressedFileReaderObjectCreate(...) and deallocating resource. This case is different from the
			// StarWave_UncompressedFileCompress(...) where we really need to verify input file to be the right one
			//

			//
			// Try to create compressed file reader object
			//
			l__ULONG__Status =
				StarBurn_StarWave_CompressedFileReaderObjectCreate(
					( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source ),
					p__PCHAR__SourceCompressedFileName
					);

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
				CLogger::Log(
							Debug,
							lmtError,
							"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectCreate( 0x%p, '%s' ) failed, status %d ( 0x%x )\n",
							__FUNCTION__,
							&l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
							p__PCHAR__SourceCompressedFileName,
							l__ULONG__Status,
							l__ULONG__Status
							);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Try to determine uncompressed data size
			//
			l__ULONG__Status =
				StarBurn_StarWave_CompressedFileReaderObjectUncompressedSizeGet( 
					l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
					p__PULONG__UncompressedSizeInUCHARs
					);

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
				CLogger::Log(
							Debug,
							lmtError,
							"*** %s(): StarWave_CompressedFileReaderObjectUncompressedSizeGet( 0x%p, 0x%p ) failed, status %d ( 0x%x )\n",
							__FUNCTION__,
							l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
							p__PULONG__UncompressedSizeInUCHARs,
							l__ULONG__Status,
							l__ULONG__Status
							);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Try to seek to begin
			//
			l__ULONG__Status = StarBurn_StarWave_CompressedFileReaderObjectBeginSeek( l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source );

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
				CLogger::Log(
							Debug,
							lmtError,
							"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectBeginSeek( 0x%p ) failed, status %d ( 0x%x )\n",
							__FUNCTION__,
							l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
							l__ULONG__Status,
							l__ULONG__Status
							);

				//
				// Get out of here
				//
                throw 0;
			}
		
			//
			// Try to allocate memory for data buffer
			//
			l__PUCHAR__DataBuffer =
				(PUCHAR) VirtualAlloc(
										NULL,
										p__ULONG__WorkingBufferSizeInUCHARs,
										MEM_COMMIT,
										PAGE_READWRITE
										);

			//
			// Check for success
			//
			if ( l__PUCHAR__DataBuffer == NULL )
			{
				//
				// Get execution status
				//
				l__ULONG__Status = GetLastError();

				CLogger::Log(
							Debug,
							lmtError,
							"*** %s(): VirtualAlloc( 0x%p, %d, %d, %d ) failed, status %d ( 0x%x )\n",
							__FUNCTION__,
							NULL,
							p__ULONG__WorkingBufferSizeInUCHARs,
							MEM_COMMIT,
							PAGE_READWRITE,
							l__ULONG__Status,
							l__ULONG__Status
							);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Try to create compressed file writer object
			//
			l__ULONG__Status = 
				StarBurn_StarWave_CompressedFileWriterObjectCreate(
					( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination ),
					p__PCHAR__DestinationRecompressedFileName,
					p__STARBURN_STARWAVE_COMPRESSION
					);

			//
			// Check for success
			//
			if ( l__ULONG__Status != ERROR_SUCCESS )
			{
					CLogger::Log(
								Debug,
								lmtError,
								"*** %s(): StarBurn_StarWave_CompressedFileWriterObjectCreate( 0x%p, '%s', %d ) failed, status %d ( 0x%x )\n",
								__FUNCTION__,
								&l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination,
								p__PCHAR__DestinationRecompressedFileName,
								p__STARBURN_STARWAVE_COMPRESSION,
								l__ULONG__Status,
								l__ULONG__Status
								);

				//
				// Get out of here
				//
                throw 0;
			}

			//
			// Keep number of UCHARs we need to read
			//
			l__ULONG__LeftToReadSizeInUCHARs = *p__PULONG__UncompressedSizeInUCHARs;

			//
			// Set default I/O size in UCHARs
			//
			l__ULONG__IoSizeInUCHARs = p__ULONG__WorkingBufferSizeInUCHARs;

			//
			// Process every chunk
			//
			for ( l__ULONG__Index = 0; l__ULONG__Index < *p__PULONG__UncompressedSizeInUCHARs; l__ULONG__Index += p__ULONG__WorkingBufferSizeInUCHARs )
			{

				//
				// Calculate completion percent in the safe way
				//
				if ( *p__PULONG__UncompressedSizeInUCHARs != 0 )
				{
					l__ULONG__CompletionPercent =
						( ULONG )
							( ( ( double )( *p__PULONG__UncompressedSizeInUCHARs - l__ULONG__LeftToReadSizeInUCHARs ) * 100 ) / *p__PULONG__UncompressedSizeInUCHARs );
				}
				else
				{
					l__ULONG__CompletionPercent = 0;
				}

				//
				// Check do we need to cancel I/O and do some progress indication
				//
				l__ULONG__Status =
					CompressorCallbackDispatch(
						STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
						l__ULONG__CompletionPercent,
						this
						);

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
					//
					// Set execution status (repatch)
					//
					l__ULONG__Status = ERROR_CANCELLED;

                                        CLogger::Log(
                                                Debug,
                                                lmtError,
						"*** %s(): ( %d, %d, 0x%p ) failed, status %d ( 0x%x )\n",
						__FUNCTION__,						
						STARBURN_STARWAVE_CALLBACK_REASON_PROGRESS,
						l__ULONG__CompletionPercent,
						this,
						l__ULONG__Status,
						l__ULONG__Status
						);
			
					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Check do we need to patch I/O size
				//
				if ( l__ULONG__LeftToReadSizeInUCHARs < p__ULONG__WorkingBufferSizeInUCHARs )
				{
					//
					// Replace with new value 
					//
					l__ULONG__IoSizeInUCHARs = l__ULONG__LeftToReadSizeInUCHARs;
				}

				//
				// Try to read current chunk
				//
	        	l__ULONG__Status = 
					StarBurn_StarWave_CompressedFileReaderObjectRead(
						l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs
						);

				//
				// Check for success
				//
		        if ( l__ULONG__Status != ERROR_SUCCESS )
		        {
					CLogger::Log(
						Debug,
						lmtError,
						"*** %s(): StarBurn_StarWave_CompressedFileReaderObjectRead( 0x%p, 0x%p, %d ) failed, status %d ( 0x%x )\n",
						__FUNCTION__,
						l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs,
						l__ULONG__Status,
						l__ULONG__Status
						);

					//
					// Get out of here
					//
                    throw 0;
        		}

				//
				// Try to write current chunk to the compressed file
				//
				l__ULONG__Status =
					StarBurn_StarWave_CompressedFileWriterObjectWrite(
						l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination,
						l__PUCHAR__DataBuffer,
						l__ULONG__IoSizeInUCHARs
						);

				//
				// Check for success
				//
				if ( l__ULONG__Status != ERROR_SUCCESS )
				{
                    CLogger::Log(
                            Debug,
                            lmtError,
							"*** %s(): StarBurn_StarWave_CompressedFileWriterObjectWrite( 0x%p, 0x%p, %d ) failed, read only %d instead of %d UCHARs, status %d ( 0x%x )\n",
							__FUNCTION__,
							l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination,
							l__PUCHAR__DataBuffer,
							l__ULONG__IoSizeInUCHARs,
							l__ULONG__Status,
							l__ULONG__Status
							);

					//
					// Get out of here
					//
                    throw 0;
				}

				//
				// Update left to read
				//
				l__ULONG__LeftToReadSizeInUCHARs -= p__ULONG__WorkingBufferSizeInUCHARs;
			}

			//
			// Set execution status to good one
			//
			l__ULONG__Status = ERROR_SUCCESS;


     		//
			// Check do we need to free STARWAVE_COMPRESSED_FILE_READER_OBJECT
			//
			if ( l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source != NULL )
			{
				//
				// Free allocated memory
				//
				StarBurn_StarWave_CompressedFileReaderObjectDestroy( ( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_READER_OBJECT__Source ) );
			}

			// 
			// Check do we need to free STARWAVE_COMPRESSED_FILE_WRITER_OBJECT
			//
			if ( l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination != NULL )
			{
				//
				// Free allocated memory
				//
				StarBurn_StarWave_CompressedFileWriterObjectDestroy( ( PVOID * )( &l__PSTARWAVE_COMPRESSED_FILE_WRITER_OBJECT__Destination ) );
			}

			//
			// Check do we need to free the buffer
			//
			if ( l__PUCHAR__DataBuffer != NULL )
			{
				//
				// Free allocated memory
				//
				VirtualFree(
					l__PUCHAR__DataBuffer,
					0,
					MEM_RELEASE
					);
			}

        CLogger::Log(
                        User,
                        lmtInformation,
                        "Decompressed file size: %d bytes",
                        p__ULONG__WorkingBufferSizeInUCHARs
                        );

        CLogger::Log(
                        Debug,
                        lmtInformation,
                        "Decompressed file size: %d bytes\n",
                        p__ULONG__WorkingBufferSizeInUCHARs
                        );
     }
    break;
    case Simple:
    {
	// Using high-level StarBurn API compression function
	// Try to recompress compressed
	//

        ULONG l__ULONG__UncompressedSizeInUCHARs = 0;


	l__ULONG__Status =
		StarBurn_StarWave_CompressedFileRecompress(
			p__PCHAR__SourceCompressedFileName,
			p__PCHAR__DestinationRecompressedFileName,
			STARBURN_STARWAVE_IO_BUFFER_SIZE_IN_UCHARS,
			&l__ULONG__UncompressedSizeInUCHARs,
			CompressorCallbackDispatch,
			this,
			STARBURN_STARWAVE_COMPRESSION_WMA_LOSSLESS_VBR_Q100
			);
        if ( l__ULONG__Status != ERROR_SUCCESS )
	{
                throw 0;
        }

        CLogger::Log(
                        User,
                        lmtInformation,
                        "Decompressed file size: %d bytes",
                        l__ULONG__UncompressedSizeInUCHARs
                        );

        CLogger::Log(
                        Debug,
                        lmtInformation,
                        "Decompressed file size: %d bytes\n",
                        l__ULONG__UncompressedSizeInUCHARs
                        );


    }
    }
  }
  catch (...) 
  {
	  //
	  // Check do we have bad status
	  //
	  if ( l__ULONG__Status != ERROR_SUCCESS )
	  {
		  //
		  // Delete output file 
		  //
		  DeleteFileA( p__PCHAR__DestinationRecompressedFileName );
	  }

	  //
	  // Get exception status
	  //
	  l__ULONG__Status = GetLastError();

	  CLogger::Log(
		  Debug,
		  lmtError,
		  "*** %s(): Exception caught, status %d ( 0x%x )\n",
		  __FUNCTION__,
		  l__ULONG__Status,
		  l__ULONG__Status
		  );

  }
    m_LastCompressedPercent = 100;
   if(CompStatCanceling == m_CompressorStatus)
   {
        m_CompressorStatus = CompStatCanceled;
        return false;
   }
   else
   {
           if(ERROR_SUCCESS == l__ULONG__Status)
           {
                m_CompressorStatus = CompStatDone;
                return true;
           }
           else
           {
                m_CompressorStatus = CompStatError;
                return false;
           }
   }
}

void CCompressor::CancelCompressorOperation()
{
        m_CompressorStatus = CompStatCanceling;
}

TCompressorStatus CCompressor::GetCompressorStatus()
{
        return m_CompressorStatus;
}

double CCompressor::GetLastCompressedPercent()
{
        return m_LastCompressedPercent;
}



#ifdef _CBUILDER
	#pragma package(smart_init)
#endif









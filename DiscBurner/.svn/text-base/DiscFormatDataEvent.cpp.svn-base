///////////////////////////////////////////////////////////////////////
// DiscFormatDataEvent.cpp
//
// Wrapper for DDiscFormat2DataEvents Interface Events
//
// Written by Eric Haddan
//

#include "stdafx.h"
#include "DiscFormatDataEvent.h"
#include <afxctl.h>

#include "Burner.h"

// CDiscFormatDataEvent

IMPLEMENT_DYNAMIC(CDiscFormatDataEvent, CCmdTarget)

BEGIN_INTERFACE_MAP(CDiscFormatDataEvent, CCmdTarget)
	INTERFACE_PART(CDiscFormatDataEvent, IID_IDispatch, FormatDataEvents)
	INTERFACE_PART(CDiscFormatDataEvent, IID_DDiscFormat2DataEvents, FormatDataEvents)
END_INTERFACE_MAP()

CDiscFormatDataEvent::CDiscFormatDataEvent()
: m_hWndNotify(NULL)
, m_ptinfo(NULL)
, m_dwCookie(0)
, m_pUnkSink(0)
, m_pUnkSrc(0)
{
}

CDiscFormatDataEvent::~CDiscFormatDataEvent()
{
	if (m_dwCookie && (m_pUnkSrc != NULL) && (m_pUnkSink != NULL))
	{
		AfxConnectionUnadvise(m_pUnkSrc, IID_DDiscFormat2DataEvents, m_pUnkSink,
			TRUE, m_dwCookie);
	}
}


BEGIN_MESSAGE_MAP(CDiscFormatDataEvent, CCmdTarget)
END_MESSAGE_MAP()



// CDiscFormatDataEvent message handlers

ULONG FAR EXPORT CDiscFormatDataEvent::XFormatDataEvents::AddRef()
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		return pThis->ExternalAddRef();
}
ULONG FAR EXPORT CDiscFormatDataEvent::XFormatDataEvents::Release()
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		return pThis->ExternalRelease();
}
STDMETHODIMP CDiscFormatDataEvent::XFormatDataEvents::QueryInterface(REFIID riid,
																	 LPVOID FAR* ppvObj)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}
STDMETHODIMP
CDiscFormatDataEvent::XFormatDataEvents::GetTypeInfoCount(UINT FAR* pctinfo)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		*pctinfo = 1;
	return NOERROR;
}
STDMETHODIMP CDiscFormatDataEvent::XFormatDataEvents::GetTypeInfo(
	UINT itinfo,
	LCID lcid,
	ITypeInfo FAR* FAR* pptinfo)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		*pptinfo = NULL;

	if(itinfo != 0)
		return ResultFromScode(DISP_E_BADINDEX);
	pThis->m_ptinfo->AddRef();
	*pptinfo = pThis->m_ptinfo;
	return NOERROR;
}
STDMETHODIMP CDiscFormatDataEvent::XFormatDataEvents::GetIDsOfNames(
	REFIID riid,
	OLECHAR FAR* FAR* rgszNames,
	UINT cNames,
	LCID lcid,
	DISPID FAR* rgdispid)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)
		return DispGetIDsOfNames(pThis->m_ptinfo, rgszNames, cNames, rgdispid);
}
STDMETHODIMP CDiscFormatDataEvent::XFormatDataEvents::Invoke(
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS FAR* pdispparams,
	VARIANT FAR* pvarResult,
	EXCEPINFO FAR* pexcepinfo,
	UINT FAR* puArgErr)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)

		return DispInvoke(&pThis->m_xFormatDataEvents, pThis->m_ptinfo,
		dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}


///////////////////////////////////////////////////////////////////////
//
// CDiscFormatDataEvent::CreateEventSink
//
// Description:
//			Establishes a link between the CDiscFormatData(IDiscFormat2Data)
//			and the CDiscFormatDataEvent(DDiscFormat2DataEvents) so 
//			CDiscFormatDataEvent can receive Update messages
//
CDiscFormatDataEvent* CDiscFormatDataEvent::CreateEventSink()
{
	// Create the event sink
	CDiscFormatDataEvent* pDiscFormatDataEvent = new CDiscFormatDataEvent();

	pDiscFormatDataEvent->EnableAutomation();
	pDiscFormatDataEvent->ExternalAddRef();

	return pDiscFormatDataEvent;
}

///////////////////////////////////////////////////////////////////////
//
// CDiscFormatDataEvent::ConnectDiscFormatData
//
// Description:
//			Establishes a link between the CDiscFormatData(IDiscFormat2Data)
//			and the CDiscFormatDataEvent(DDiscFormat2DataEvents) so 
//			CDiscFormatDataEvent can receive Update messages
//
BOOL CDiscFormatDataEvent::ConnectDiscFormatData(IDiscFormat2Data* pData)
{
	m_pUnkSink = GetIDispatch(FALSE);
	m_pUnkSrc = pData;

	LPTYPELIB ptlib = NULL;
	HRESULT hr = LoadRegTypeLib(LIBID_IMAPILib2, 
		IMAPILib2_MajorVersion, IMAPILib2_MinorVersion, 
		LOCALE_SYSTEM_DEFAULT, &ptlib);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr = ptlib->GetTypeInfoOfGuid(IID_DDiscFormat2DataEvents, &m_ptinfo);
	ptlib->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	return AfxConnectionAdvise(m_pUnkSrc, IID_DDiscFormat2DataEvents, m_pUnkSink, TRUE, &m_dwCookie);
}


///////////////////////////////////////////////////////////////////////
//
// CDiscFormatDataEvent::Update
//
// Description:
//			Receives update notifications from IDiscFormat2Data
//
STDMETHODIMP_(HRESULT) CDiscFormatDataEvent::XFormatDataEvents::Update(IDispatch* objectDispatch, IDispatch* progressDispatch)
{
	METHOD_PROLOGUE(CDiscFormatDataEvent, FormatDataEvents)

	IDiscFormat2DataEventArgs* args = NULL;
	HRESULT hr = progressDispatch->QueryInterface(IID_PPV_ARGS(&args));

	IDiscFormat2Data* discFormatData = NULL;
	hr = objectDispatch->QueryInterface(IID_PPV_ARGS(&discFormatData));

	DATA_PROGRESS progress;
	hr = args->get_CurrentAction(&progress.action);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		return S_OK;
	}

	if ((progress.action == IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA) ||
		(progress.action == IMAPI_FORMAT2_DATA_WRITE_ACTION_FINALIZATION))
	{
		args->get_ElapsedTime(&progress.lElapsedTime);
		args->get_RemainingTime(&progress.lRemainingTime);
		args->get_TotalTime(&progress.lTotalTime);
	}

	if (pThis->m_hWndNotify != NULL)
	{
		if (::SendMessage(pThis->m_hWndNotify, WM_BURN, BURN_DATA_EVENT, (LPARAM)&progress) == 1)
			discFormatData->CancelWrite();
	}

	return S_OK;
}

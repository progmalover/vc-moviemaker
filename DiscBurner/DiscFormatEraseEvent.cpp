///////////////////////////////////////////////////////////////////////
// DiscFormatEraseEvent.cpp
//
// Wrapper for DDiscFormat2EraseEvents Interface Events
//
// Written by Eric Haddan
//

#include "stdafx.h"
#include "DiscFormatEraseEvent.h"
#include "Converter.h"
#include <afxctl.h>
#include "Burner.h"

// CDiscFormatEraseEvent

IMPLEMENT_DYNAMIC(CDiscFormatEraseEvent, CCmdTarget)

BEGIN_INTERFACE_MAP(CDiscFormatEraseEvent, CCmdTarget)
	INTERFACE_PART(CDiscFormatEraseEvent, IID_IDispatch, FormatEraseEvents)
	INTERFACE_PART(CDiscFormatEraseEvent, IID_DDiscFormat2EraseEvents, FormatEraseEvents)
END_INTERFACE_MAP()

CDiscFormatEraseEvent::CDiscFormatEraseEvent()
: m_hWndNotify(NULL)
, m_ptinfo(NULL)
, m_dwCookie(0)
, m_pUnkSink(0)
, m_pUnkSrc(0)
{
}

CDiscFormatEraseEvent::~CDiscFormatEraseEvent()
{
	if (m_dwCookie && (m_pUnkSrc != NULL) && (m_pUnkSink != NULL))
	{
		AfxConnectionUnadvise(m_pUnkSrc, IID_DDiscFormat2EraseEvents, m_pUnkSink,
			TRUE, m_dwCookie);
	}
}


BEGIN_MESSAGE_MAP(CDiscFormatEraseEvent, CCmdTarget)
END_MESSAGE_MAP()



// CDiscFormatEraseEvent message handlers

ULONG FAR EXPORT CDiscFormatEraseEvent::XFormatEraseEvents::AddRef()
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		return pThis->ExternalAddRef();
}
ULONG FAR EXPORT CDiscFormatEraseEvent::XFormatEraseEvents::Release()
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		return pThis->ExternalRelease();
}
STDMETHODIMP CDiscFormatEraseEvent::XFormatEraseEvents::QueryInterface(REFIID riid,
																	 LPVOID FAR* ppvObj)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}
STDMETHODIMP
CDiscFormatEraseEvent::XFormatEraseEvents::GetTypeInfoCount(UINT FAR* pctinfo)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		*pctinfo = 1;
	return NOERROR;
}
STDMETHODIMP CDiscFormatEraseEvent::XFormatEraseEvents::GetTypeInfo(
	UINT itinfo,
	LCID lcid,
	ITypeInfo FAR* FAR* pptinfo)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		*pptinfo = NULL;

	if(itinfo != 0)
		return ResultFromScode(DISP_E_BADINDEX);
	pThis->m_ptinfo->AddRef();
	*pptinfo = pThis->m_ptinfo;
	return NOERROR;
}
STDMETHODIMP CDiscFormatEraseEvent::XFormatEraseEvents::GetIDsOfNames(
	REFIID riid,
	OLECHAR FAR* FAR* rgszNames,
	UINT cNames,
	LCID lcid,
	DISPID FAR* rgdispid)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)
		return DispGetIDsOfNames(pThis->m_ptinfo, rgszNames, cNames, rgdispid);
}
STDMETHODIMP CDiscFormatEraseEvent::XFormatEraseEvents::Invoke(
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS FAR* pdispparams,
	VARIANT FAR* pvarResult,
	EXCEPINFO FAR* pexcepinfo,
	UINT FAR* puArgErr)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)

		return DispInvoke(&pThis->m_xFormatEraseEvents, pThis->m_ptinfo,
		dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}


///////////////////////////////////////////////////////////////////////
//
// CDiscFormatEraseEvent::CreateEventSink
//
// Description:
//			Establishes a link between the CDiscFormatErase(IDiscFormat2Erase)
//			and the CDiscFormatEraseEvent(DDiscFormat2EraseEvents) so 
//			CDiscFormatEraseEvent can receive Update messages
//
CDiscFormatEraseEvent* CDiscFormatEraseEvent::CreateEventSink()
{
	// Create the event sink
	CDiscFormatEraseEvent* pDiscFormatEraseEvent = new CDiscFormatEraseEvent();

	pDiscFormatEraseEvent->EnableAutomation();
	pDiscFormatEraseEvent->ExternalAddRef();

	return pDiscFormatEraseEvent;
}

///////////////////////////////////////////////////////////////////////
//
// CDiscFormatEraseEvent::ConnectDiscFormatErase
//
// Description:
//			Establishes a link between the CDiscFormatErase(IDiscFormat2Erase)
//			and the CDiscFormatEraseEvent(DDiscFormat2EraseEvents) so 
//			CDiscFormatEraseEvent can receive Update messages
//
BOOL CDiscFormatEraseEvent::ConnectDiscFormatErase(IDiscFormat2Erase* pErase)
{
	m_pUnkSink = GetIDispatch(FALSE);
	m_pUnkSrc = pErase;

	LPTYPELIB ptlib = NULL;
	HRESULT hr = LoadRegTypeLib(LIBID_IMAPILib2, 
		IMAPILib2_MajorVersion, IMAPILib2_MinorVersion, 
		LOCALE_SYSTEM_DEFAULT, &ptlib);
	if (FAILED(hr))
	{
		return FALSE;
	}
	hr = ptlib->GetTypeInfoOfGuid(IID_DDiscFormat2EraseEvents, &m_ptinfo);
	ptlib->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	return AfxConnectionAdvise(m_pUnkSrc, IID_DDiscFormat2EraseEvents, m_pUnkSink, TRUE, &m_dwCookie);
}


///////////////////////////////////////////////////////////////////////
//
// CDiscFormatEraseEvent::Update
//
// Description:
//			Receives update notifications from IDiscFormat2Erase
//
STDMETHODIMP_(HRESULT) CDiscFormatEraseEvent::XFormatEraseEvents::Update(IDispatch* object, LONG elapsedSeconds, LONG estimatedTotalSeconds)
{
	METHOD_PROLOGUE(CDiscFormatEraseEvent, FormatEraseEvents)

	ERASE_PROGRESS progress;
	progress.lElapsedTime = elapsedSeconds;
	progress.lTotalTime = estimatedTotalSeconds;

	CLog::Instance()->AppendLog("Erase progress: %d/%d\r\n", elapsedSeconds, estimatedTotalSeconds);

	if (pThis->m_hWndNotify != NULL)
		if (::SendMessage(pThis->m_hWndNotify, WM_BURN, BURN_ERASE_EVENT, (LPARAM)&progress) == 1)
			AfxThrowUserException();

	return S_OK;
}

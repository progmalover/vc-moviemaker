#pragma once

#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>

struct DATA_PROGRESS
{
	IMAPI_FORMAT2_DATA_WRITE_ACTION action;
	LONG lElapsedTime;
	LONG lRemainingTime;
	LONG lTotalTime;
};

// CDiscFormatDataEvent command target

class CDiscFormatDataEvent : public CCmdTarget
{
	DECLARE_DYNAMIC(CDiscFormatDataEvent)
private:
	LPTYPEINFO  m_ptinfo;           // ITest type information
	DWORD		m_dwCookie;
	LPUNKNOWN	m_pUnkSink;
	LPUNKNOWN	m_pUnkSrc;

public:
	CDiscFormatDataEvent();
	virtual ~CDiscFormatDataEvent();

	static CDiscFormatDataEvent* CreateEventSink();

	HWND m_hWndNotify;
	BOOL ConnectDiscFormatData(IDiscFormat2Data *pData);

    DECLARE_INTERFACE_MAP()

    BEGIN_INTERFACE_PART(FormatDataEvents, DDiscFormat2DataEvents)
		//
		// IDispatch Methods
		//
        STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
        STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
        STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames,
			UINT cNames, LCID lcid, DISPID FAR* rgdispid);
        STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid,
			WORD wFlags, DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
			EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr);
		//
		// DDiscFormat2DataEvents Methods
		//
		STDMETHOD_(HRESULT, Update)(LPDISPATCH, LPDISPATCH);
    END_INTERFACE_PART(FormatDataEvents)

protected:
	DECLARE_MESSAGE_MAP()
};



#pragma once

#include <imapi2.h>
#include <imapi2error.h>
#include <imapi2fs.h>
#include <imapi2fserror.h>


struct ERASE_PROGRESS 
{
	LONG lElapsedTime;
	LONG lTotalTime;
};

// CDiscFormatEraseEvent command target

class CDiscFormatEraseEvent : public CCmdTarget
{
	DECLARE_DYNAMIC(CDiscFormatEraseEvent)
private:
	LPTYPEINFO  m_ptinfo;           // ITest type information
	DWORD		m_dwCookie;
	LPUNKNOWN	m_pUnkSink;
	LPUNKNOWN	m_pUnkSrc;

public:
	CDiscFormatEraseEvent();
	virtual ~CDiscFormatEraseEvent();

	static CDiscFormatEraseEvent* CreateEventSink();

	HWND m_hWndNotify;
	BOOL ConnectDiscFormatErase(IDiscFormat2Erase *pErase);

    DECLARE_INTERFACE_MAP()

    BEGIN_INTERFACE_PART(FormatEraseEvents, DDiscFormat2EraseEvents)
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
		// DDiscFormat2EraseEvents Methods
		//
		STDMETHOD_(HRESULT, Update)(IDispatch* object, LONG elapsedSeconds, LONG estimatedTotalSeconds);
    END_INTERFACE_PART(FormatEraseEvents)

protected:
	DECLARE_MESSAGE_MAP()
};



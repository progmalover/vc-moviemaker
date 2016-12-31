
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <afxpriv.h>
#include <afxinet.h>
#include <sstream>
#include <string>
#include <list>
#include <map>

#include <math.h>
#include <float.h>
#include <afxdlgs.h>

#include "filepath.h"
#include "utils.h"
#include "Observer.h"
#include "Log.h"

#include "..\Regexx\regexx.h"
using namespace regexx;

#define POCO_NO_UNWINDOWS

#include "Poco/Exception.h"

/*
#include "Poco/Net/Net.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/IPAddress.h"
#include "Poco/Net/StreamSocketImpl.h"
#include "Poco/Net/NetException.h"
*/

#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/CDATASection.h"
#include "Poco/DOM/Attr.h"
#include "Poco/DOM/ElementsByTagNameList.h"
#include "Poco/XML/XMLException.h"

using namespace Poco;

/*
using Poco::TimeoutException;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::NameValueCollection;
using Poco::Net::HTTPClientSession;
using Poco::Net::StreamSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;
using Poco::Net::StreamSocketImpl;
using Poco::Net::NetException;
using Poco::Net::HTTPException;
*/

using namespace Poco::XML;

using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMParser;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;
using Poco::XML::InputSource;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::Node;
using Poco::XML::NodeList;
using Poco::XML::NamedNodeMap;
using Poco::XML::CDATASection;

#ifdef _BD

#define CONVERTER_GUID		"{C3440C0A-8250-4e4e-BC6F-DDBCA47E530E}"
#define CONVERTER_CLASS		"CONVERTER_BD_Class"

#elif defined _DVD

#define CONVERTER_GUID		"{A5D3D904-08E7-44fb-9E8A-01F4F48DE615}"
#define CONVERTER_CLASS		"CONVERTER_DVD_Class"

#endif

#define DEFINE_REGISTERED_MESSAGE(message) UINT message = ::RegisterWindowMessage(CONVERTER_GUID":"#message"");



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define  TOOLBAR_BORDER_Y   2
#define  TOOLBAR_BORDER_X   1

#define tstring basic_string<TCHAR>
#define _MENU_TEMPLATE_VIEW

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

#include <sstream>
#include <string>
#include <list>
#include <map>

#include <math.h>
#include <float.h>

#include "filepath.h"
#include "utils.h"

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

// TODO: reference additional headers your program requires here

#include "StdAfx.h"
#include "xmlutils.h"
#include <strstream>

BOOL EncodeString(LPCTSTR pszInput, UINT nInputLen, UINT nInputCodePage, LPSTR *ppszOutput, UINT nOutputCodePage)
{
	UINT len = nInputLen;

	if (len == 0)
	{
		*ppszOutput = new char[1];
		**ppszOutput = 0;
		return TRUE;
	}
	else
	{
		LPWSTR pwsz = new wchar_t[len + 1];
		len = MultiByteToWideChar(nInputCodePage, 0, pszInput, len, pwsz, len + 1);
		if (len == 0)
		{
			delete pwsz;
			return FALSE;
		}

		LPSTR psz = new char[len * 3 + 1];	// for UTF-8
		len = WideCharToMultiByte(nOutputCodePage, 0, pwsz, len, psz, len * 3 + 1, NULL, NULL);
		if (len == 0)
		{
			delete pwsz;
			delete psz;
			return FALSE;
		}

		psz[len] = 0;
		*ppszOutput = psz;
		delete pwsz;

		return TRUE;
	}
}

std::string UTF8Encode(LPCTSTR lpszString)
{
	std::string sEncoded = lpszString;
	if (_tcslen(lpszString) > 0)
	{
		TCHAR *lpszEncoded = NULL;
		if (EncodeString(lpszString, _tcslen(lpszString), CP_ACP, &lpszEncoded, CP_UTF8))
		{
			sEncoded = lpszEncoded;
			delete lpszEncoded;
		}
	}

	return sEncoded;
}

std::string UTF8Decode(LPCTSTR lpszString)
{
	std::string sDecoded = lpszString;
	if (_tcslen(lpszString) > 0)
	{
		TCHAR *lpszDecoded = NULL;
		if (EncodeString(lpszString, _tcslen(lpszString), CP_UTF8, &lpszDecoded, CP_ACP))
		{
			sDecoded = lpszDecoded;
			delete lpszDecoded;
		}
	}

	return sDecoded;
}

std::string XMLEscape(LPCTSTR pszString)
{
	std::string sEncoded;

	int len = _tcslen(pszString);
	for (int i = 0; i < len; i++)
	{
		TCHAR c = pszString[i];
		switch (c)
		{
			case '<':
				sEncoded += "&lt;";
				break;
			case '>':
				sEncoded += "&gt;";
				break;
			case '&':
				sEncoded += "&amp;";
				break;
			case '\'':
				sEncoded += "&apos;";
				break;
			case '\"':
				sEncoded += "&quot;";
				break;
			default:
				sEncoded += c;
				break;
		}
	}

	return sEncoded;
}

std::string GetNodeAttribute(Node *pNode, LPCTSTR lpszName)
{
	std::string sValue = ((Element *)pNode)->getAttribute(lpszName);

	TCHAR *pszValueDecoded;
	if (EncodeString(sValue.c_str(), sValue.length(), CP_UTF8, &pszValueDecoded, CP_ACP))
	{
		sValue = pszValueDecoded;
		delete pszValueDecoded;
	}

	return sValue;
}

int GetNodeAttributeInt(Node *pNode, LPCTSTR lpszName)
{
	std::string sValue = GetNodeAttribute(pNode, lpszName);
	return _ttoi(sValue.c_str());
}

void SetNodeAttribute(Node *pNode, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	TCHAR *lpszValueEncoded = NULL;
	if (EncodeString(lpszValue, _tcslen(lpszValue), CP_ACP, &lpszValueEncoded, CP_UTF8))
	{
		((Element *)pNode)->setAttribute(lpszName, lpszValueEncoded);
		delete lpszValueEncoded;
	}
	else
	{
		((Element *)pNode)->setAttribute(lpszName, lpszValue);
	}
}

void SetNodeAttribute(Node *pNode, LPCTSTR lpszName, int nValue)
{
	TCHAR szValue[10];
	_itot(nValue, szValue, 10);
	
	((Element *)pNode)->setAttribute(lpszName, szValue);
}

void WriteChildNode(Document *pDoc, Element *pNode, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcslen(lpszValue) > 0)
	{
		TCHAR *pszValueEncoded = NULL;
		if (EncodeString(lpszValue, _tcslen(lpszValue), CP_ACP, &pszValueEncoded, CP_UTF8))
		{
			AutoPtr<Element> pChildNode = pDoc->createElement(lpszName);
			AutoPtr<Text> pText = pDoc->createTextNode(pszValueEncoded);
			delete pszValueEncoded;
			pChildNode->appendChild(pText);
			pNode->appendChild(pChildNode);
		}
	}
}

void WriteChildCDATANode(Document *pDoc, Element *pNode, LPCTSTR lpszName, LPCTSTR lpszValue)
{
	size_t len = _tcslen(lpszValue);
	if (len > 0)
	{
		TCHAR *pszValueEncoded = NULL;
		if (EncodeString(lpszValue, len, CP_ACP, &pszValueEncoded, CP_UTF8))
		{
			AutoPtr<Element> pChildNode = pDoc->createElement(lpszName);
			AutoPtr<CDATASection> pText = pDoc->createCDATASection(pszValueEncoded);
			delete pszValueEncoded;
			pChildNode->appendChild(pText);
			pNode->appendChild(pChildNode);
		}
	}
}

void ReadChildNode(Node *pNode, std::string &sValue)
{
	TCHAR *pszValueDecoded;
	std::string s = pNode->firstChild()->nodeValue();
	if (EncodeString(s.c_str(), s.length(), CP_UTF8, &pszValueDecoded, CP_ACP))
		sValue = pszValueDecoded;
	else
		sValue = pNode->firstChild()->nodeValue();
	delete pszValueDecoded;
}

Document *OpenXMLDocument(LPCTSTR lpszFile)
{
	CStdioFile file;
	CFileException e;
	if (!file.Open(lpszFile, CFile::typeBinary | CFile::modeRead | CFile::shareDenyWrite, &e))
		throw FileException(lpszFile);

	// Check if the file is empty.
	ULONGLONG total = file.GetLength();
	if (total == 0)
		throw FileException(lpszFile);

	std::strstream ss;
	try
	{
		char szBuf[4096];
		UINT read = 0;
		while (read < total)
		{
			UINT ret = file.Read(szBuf, 4096);
			read += ret;
			ss.write(szBuf, ret);
		}
	}
	catch (CFileException *e)
	{
		TCHAR szError[256];
		e->GetErrorMessage(szError, 256);
		TRACE("Error: %s.\n", szError);

		e->Delete();
		file.Close();
		throw FileException(lpszFile);
	}

	InputSource src(ss);

	DOMParser parser;
	parser.setFeature(DOMParser::FEATURE_WHITESPACE, false);
	Document *pDoc = parser.parse(&src);

	return pDoc;
}

void SaveXMLDocument(Document *pDoc, LPCTSTR lpszFile)
{
	try
	{
		std::strstream ss;

		DOMWriter writer;
		writer.setNewLine(XMLWriter::NEWLINE_CRLF);
		writer.setOptions(XMLWriter::WRITE_XML_DECLARATION | XMLWriter::PRETTY_PRINT);
		writer.writeNode(ss, pDoc);

		char *pBuf = ss.str();

		CStdioFile file(lpszFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(pBuf, ss.pcount());
		file.Flush();
		file.Close();
	}
	catch (Exception& e)
	{
		throw e;
	}
	catch (CFileException *e)
	{
		e->Delete();
		throw FileException(lpszFile);
	}
}

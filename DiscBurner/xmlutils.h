#pragma once

#include <string>

BOOL EncodeString(LPCTSTR pszInput, UINT nInputLen, UINT nInputCodePage, LPSTR *ppszOutput, UINT nOutputCodePage);
std::string UTF8Encode(LPCTSTR lpszString);
std::string UTF8Encode(LPCTSTR lpszString);
std::string XMLEscape(LPCTSTR pszString);

std::string GetNodeAttribute(Node *pNode, LPCTSTR lpszName);
int GetNodeAttributeInt(Node *pNode, LPCTSTR lpszName);

void SetNodeAttribute(Node *pNode, LPCTSTR lpszName, LPCTSTR lpszValue);
void SetNodeAttribute(Node *pNode, LPCTSTR lpszName, int nValue);

void WriteChildNode(Document *pDoc, Element *pNode, LPCTSTR lpszName, LPCTSTR lpszValue);
void ReadChildNode(Node *pNode, std::string &sValue);
void WriteChildCDATANode(Document *pDoc, Element *pNode, LPCTSTR lpszName, LPCTSTR lpszValue);

Document *OpenXMLDocument(LPCTSTR lpszFile);
void SaveXMLDocument(Document *pDoc, LPCTSTR lpszFile);
#pragma once

#include "shlwapi.h"
#include "io.h"

BOOL IsValidPath(LPCTSTR lpszPath);
BOOL IsValidFileName(LPCTSTR lpszFileName);
BOOL PrepareDirectory(LPCTSTR lpszPath, BOOL bPrompt);
void ReplaceInvalidCharsInFileName(LPTSTR lpszFileName, TCHAR ch);
BOOL CreateDirectoryNest(LPCTSTR lpszPath);
BOOL RemoveReadOnlyAttribute(LPCTSTR lpszName);
BOOL DeleteFile(LPCTSTR lpszName, BOOL bChangeAttrsIfNeed);
BOOL RemoveDirectory(LPCTSTR lpszName, BOOL bForce);
BOOL CopyFile(LPCTSTR lpszExisting, LPCTSTR lpszNew, BOOL bFailedIfExists, BOOL bForce);


////////////////////////////////////////////////////////////
// CString version of shlwapi functions
CString FileStripFileName(LPCTSTR lpszPathName);
CString FileStripPath(LPCTSTR lpszPathName);
CString FileCombinePathName(LPCTSTR lpszPath, LPCTSTR lpszName);
CString GetRelativePath(LPCTSTR lpszFrom, LPCTSTR lpszTo);

// aaa[1].htm, aaa[2].htm
CString RemoveSequenceNumber(LPCTSTR lpszFile);
CString GenerateSequenceNumber(LPCTSTR lpszFile);

// return value is a pointer to a static variable
LPCTSTR GetModuleFilePath();

// Pattern: lpszPath\lpszPrefix[0-9]+.lpszExt
CString GetTempFile(LPCTSTR lpszPath, LPCTSTR lpszPrefix, LPCTSTR lpszExt);

// e.g. C:\Temp\{6AF84971-245C-460f-A034-F40103F7949C}
BOOL CreateTempFolder(LPCTSTR lpszPath, CString &strFolder);

// compare 2 files' content.
// 0 : same contents
// !0: different contents
int CompareFile(LPCSTR lpszFile1, LPCSTR lpszFile2);

CString URLToPath(LPCTSTR lpszUrl);

BOOL IsDirWritable(LPCTSTR lpszPath);

void ReplaceInvalidCharsInFileName(LPTSTR lpszFileName, TCHAR ch);

CString GetAbsolutePath(LPCSTR RelativePath,LPCSTR BaseAbsolutePath);

BOOL ValidPath(LPCSTR path);
INT64 GetFolderSize(LPCTSTR lpszFolder);
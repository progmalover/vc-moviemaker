#include "stdafx.h"
#include "filepath.h"
#include "Utils.h"
#include "Resource.h"
#include <atlbase.h>
#include <sys/stat.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const unsigned char szTokInvalidNameChars[] = {'\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0'};	//"\"*?/:><|";
const unsigned char szTokInvalidPathChars[] = {'/', ':', '*', '?', '"', '<', '>', '|', '\0'};	//"\"*?/:><|";

BOOL PrepareDirectory(LPCTSTR lpszPath, BOOL bPrompt)
{
	if (!PathIsDirectory(lpszPath))
	{
		if (bPrompt)
		{
			CString strPrompt;
			if (AfxMessageBoxEx(MB_ICONQUESTION | MB_YESNO, IDS_PROMPT_CREATE_DIRECTORY1, lpszPath) == IDNO)
				return FALSE;
		}

		if (!IsValidPath(lpszPath))
		{
			::SetLastError(ERROR_DIRECTORY);
			if (bPrompt)
				ReportLastError(IDS_E_CREATE_DIRECTORY1);

			return FALSE;
		}

		if (!CreateDirectoryNest(lpszPath))
		{
			TRACE1("Failed to create directory. GetLastError() == %d\n", GetLastError());
			if (bPrompt)
				ReportLastError(IDS_E_CREATE_DIRECTORY1);

			return FALSE;
		}
	}

	return TRUE;
}

BOOL IsValidPath(LPCTSTR lpszPath)
{
	LPCTSTR p = lpszPath;
	int len = lstrlen(lpszPath);

	// "c:" or "\\"
	if (len < 2)
	{
		return FALSE;
	}
	if (isalpha(*lpszPath) && *(lpszPath + 1) == ':')
	{
		p += 2;
	}
	else if (memcmp(lpszPath, "\\\\", sizeof(TCHAR) * 2) == 0)	// UNC name
	{
		// \\a\b
		if (len < 5)
		{
			return FALSE;
		}
		p += 2;
	}
	else
	{
		return FALSE;
	}
	
	// Check if all characters is in valid range
	LPCTSTR p2 =p;
	while (*p2)
	{
		if ((unsigned char)*p2 ++ < 0x20)
		{
			return FALSE;
		}
	}

	// Check if there's 2 continuous '\'
	if (_mbsstr((const unsigned char *)p, (const unsigned char *)"\\\\"))
	{
		return FALSE;
	}

	// Check if there're invalid characters
	size_t ret = _mbscspn((const unsigned char *)p, szTokInvalidPathChars);
	return (ret >= strlen(p));
}

BOOL IsValidFileName(LPCTSTR lpszFileName)
{
	int len = strlen(lpszFileName);
	if (len == 0)
		return FALSE;

	// Check if there're invalid characters

	int ret = _mbscspn((const unsigned char *)lpszFileName, szTokInvalidNameChars);
	if (ret < len)
		return FALSE;

	return TRUE;
}

void ReplaceInvalidCharsInFileName(LPTSTR lpszFileName, TCHAR ch)
{
	TCHAR *p = lpszFileName;
	while (*p)
	{
		int ret = _mbscspn((const unsigned char *)p, szTokInvalidNameChars);
		if (ret < strlen(p))
		{
			p += ret;
			*p = ch;
			p ++;

			continue;
		}
		break;
	}
}

BOOL CreateDirectoryNest(LPCTSTR lpszPath)
{
	/*
	if (!IsValidPath(lpszPath))
	{
		::SetLastError(ERROR_DIRECTORY);
		return FALSE;
	}

	CAutoPtr<TCHAR> pszTemp(new TCHAR[_tcslen(lpszPath) + 1]);

	unsigned char *p = (unsigned char *)lpszPath;
	unsigned char *pB;

	// Test for UNC name
	if (memcmp(lpszPath, "\\\\", sizeof(TCHAR) * 2) == 0)
		p += 2;

	BOOL bLast = FALSE;
	while (TRUE)
	{
		pB = _mbschr((unsigned char *)p, (unsigned int)'\\');
		if (pB != NULL)
		{
			p = _mbsninc(pB, 1);
			lstrcpyn(pszTemp, lpszPath, (LPCTSTR)pB - lpszPath + 1);
		}
		else
		{
			bLast = TRUE;
			lstrcpy(pszTemp, lpszPath);
		}

		if (!PathIsDirectory(pszTemp))
			if (!CreateDirectory(pszTemp, NULL))
				return FALSE;

		if (bLast)
			break;
	}
	return TRUE;
	*/

	USES_CONVERSION;
	return (SHCreateDirectory(NULL, A2W(lpszPath)) == ERROR_SUCCESS);
}

CString FileStripFileName(LPCTSTR lpszPathName)
{
	TCHAR *psz = new TCHAR[strlen(lpszPathName) + 1];
	if (!psz)
		AfxThrowMemoryException();

	_tcscpy(psz, lpszPathName);
	PathRemoveFileSpec(psz);

	CString strPath = psz;
	delete psz;
	return strPath;
}

CString FileStripPath(LPCTSTR lpszPathName)
{
	TCHAR *psz = new TCHAR[strlen(lpszPathName) + 1];
	if (!psz)
		AfxThrowMemoryException();

	_tcscpy(psz, lpszPathName);
	PathStripPath(psz);

	CString strPath = psz;
	delete psz;
	return strPath;
}

CString FileCombinePathName(LPCTSTR lpszPath, LPCTSTR lpszName)
{
	ASSERT(lpszPath != NULL && lpszName != NULL);
	CString strPathName;
	LPTSTR lpszPathName = strPathName.GetBufferSetLength(max(_MAX_PATH, _tcslen(lpszPath) + 1 + _tcslen(lpszName)));
	::PathCombine(lpszPathName, lpszPath, lpszName);
	strPathName.ReleaseBuffer();
	return strPathName;
}

// NOTE: if any input parameter is a directory, 
// you must add a backslash at the end
CString GetRelativePath(LPCTSTR lpszFrom, LPCTSTR lpszTo)
{
	CString strResult;
	ASSERT(lstrlen(lpszTo) != 0);

	TCHAR szResult[_MAX_PATH + 1];
	if (PathRelativePathTo(
		szResult, 
		lpszTo, 
		0, 
		lpszFrom, 
		0))
	{
		if (memcmp(szResult, ".\\", 2) != 0)
		{
			strResult = szResult;
		}
		else
		{
			strResult = szResult + 2;
		}
	}

	return strResult;
}

BOOL RemoveReadOnlyAttribute(LPCTSTR lpszName)
{
	DWORD dwAttrs = GetFileAttributes(lpszName);
	if (dwAttrs == -1)
		return FALSE;

	return ::SetFileAttributes(lpszName, dwAttrs & ~FILE_ATTRIBUTE_READONLY);
}

BOOL DeleteFile(LPCTSTR lpszName, BOOL bForce)
{
	if (bForce)
		RemoveReadOnlyAttribute(lpszName);

	return ::DeleteFile(lpszName);
}

BOOL RemoveDirectory(LPCTSTR lpszName, BOOL bForce)
{
	if (bForce)
	{
		RemoveReadOnlyAttribute(lpszName);

		size_t len = _tcslen(lpszName);
		CAutoPtr<TCHAR> pszName(new TCHAR[len + 2]);
		_tcscpy(pszName, lpszName);
		pszName[len + 1] = '\0';

		SHFILEOPSTRUCT fo;
		memset(&fo, 0, sizeof(fo));
		fo.hwnd = NULL;
		fo.wFunc = FO_DELETE;
		fo.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
		fo.pFrom = pszName;

		return (SHFileOperation(&fo) == 0);
	}
	else
	{
		return ::RemoveDirectory(lpszName);
	}
}

BOOL CopyFile(LPCTSTR lpszExisting, LPCTSTR lpszNew, BOOL bFailedIfExists, BOOL bForce)
{
	if (bForce && PathFileExists(lpszNew))
		RemoveReadOnlyAttribute(lpszNew);

	for (int i = 0; i < 3; i++)
	{
		if (::CopyFile(lpszExisting, lpszNew, bFailedIfExists))
			return TRUE;
		Sleep(1);
	}
	return FALSE;	
}

CString RemoveSequenceNumber(LPCTSTR lpszFile)
{
	TCHAR szName[_MAX_PATH + 1];
	TCHAR szExt[_MAX_PATH + 1] = "";
	_tcsncpy(szName, lpszFile, _MAX_PATH);
	LPTSTR pExt = PathFindExtension(lpszFile);
	if (pExt)
	{
		_tcsncpy(szExt, pExt, _MAX_PATH);
		szName[pExt - lpszFile] = (TCHAR)'\0';
	}

	// x[n].ext
	int len = _tcslen(szName);
	if (len > 4 && szName[len - 1] == ']')
	{
		int nEndPos = len - 1;		// index of ']'
		int nStartPos = len - 3;	// skip one number at least
		while (nStartPos > 0 && szName[nStartPos] != '[')
			nStartPos--;
		if (nStartPos > 0)
		{
			TCHAR szNum[_MAX_PATH + 1];
			memcpy(szNum, szName + nStartPos, sizeof(TCHAR) * (nEndPos - nStartPos + 1));
			szNum[nEndPos - nStartPos + 1] = (TCHAR)'\0';
			int num;
			if (sscanf(szNum, "[%d]", &num) == 1)
			{
				TRACE1("%d\n", num);
				szName[nStartPos] = (TCHAR)'\0';
				_tcscat(szName, szExt);
				return szName;
			}
		}
	}

	return lpszFile;
}

CString GenerateSequenceNumber(LPCTSTR lpszFile)
{
	if (!::PathFileExists(lpszFile))
		return lpszFile;

	CString strFile = RemoveSequenceNumber(lpszFile);

	TCHAR szName[_MAX_PATH + 1];
	TCHAR szExt[_MAX_PATH + 1] = "";
	_tcsncpy(szName, strFile, _MAX_PATH);
	LPTSTR pExt = PathFindExtension(strFile);
	if (pExt)
	{
		_tcsncpy(szExt, pExt, _MAX_PATH);
		szName[pExt - (LPCTSTR)strFile] = (TCHAR)'\0';
	}

	// x[n].ext
	for (int i = 1; ;i++)
	{
		CString strName;
		strName.Format("%s[%d]%s", szName, i, szExt);
		if (!::PathFileExists(strName))
			return strName;
	}
}

// Note: static variable used
LPCTSTR GetModuleFilePath()
{
	static CString strPath;
	if (strPath.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH + 1];
		::GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
		::PathRemoveFileSpec(szPath);
		strPath = szPath;
	}
	return (LPCTSTR)strPath;
}

int CompareFile(LPCSTR lpszFile1, LPCSTR lpszFile2)
{
	CString Paths[2];
	Paths[0] = lpszFile1;
	Paths[1] = lpszFile2;

	HANDLE FileHandle[2];
	DWORD dwSizeLow[2], dwSizeHigh[2];

	// Attempt to open both copies of the file.
	for (int i = 0; i < 2; i++)
	{
		FileHandle[i] = CreateFile(
						 (LPCSTR) Paths[i], 
						 GENERIC_READ, 
						 FILE_SHARE_READ,
						 NULL, 
						 OPEN_EXISTING, 
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);

		if (FileHandle[i] == INVALID_HANDLE_VALUE)
		{
			if (i == 1) CloseHandle(FileHandle[0]);

			return 1;
		}

		// Determine size of both copies of the file.
		dwSizeLow[i] = GetFileSize(FileHandle[i], 
								   &dwSizeHigh[i]);

		// If the file is larger than 4 GB, exit.
		if (dwSizeHigh[i] != 0)
		  {
			CloseHandle(FileHandle[0]);
			if (i == 1) CloseHandle(FileHandle[1]);
			return 1;
		  }
	}  

	// Determine if the copies are different sizes.
	if (dwSizeLow[0] != dwSizeLow[1]) {
		CloseHandle(FileHandle[0]);
		CloseHandle(FileHandle[1]);
		return 1;
	}

	// At this point, the files are identical in 
	// size.
	const DWORD dwFileSize = dwSizeLow[0];

	// No need to compare empty files
	if (dwFileSize == 0)
		return 0;

	HANDLE Maps[2];
	const void *FileContents[2];
	BOOL bResult = TRUE;

	// Map the files using memory-mapped file I/O.
	for (int i = 0; i < 2; i++)
	{
		Maps[i] = CreateFileMapping(FileHandle[i],
								NULL,
								PAGE_READONLY,
								0,
								dwFileSize,
								NULL);

		if (Maps[i] == NULL)
		  {
			bResult = FALSE;
			break;
		  }

		FileContents[i] = MapViewOfFile(
								Maps[i],
								FILE_MAP_READ,
								0,
								0,
								dwFileSize);

		if (FileContents[i] == NULL)
		  {
			bResult = FALSE;
			break;
		  }
	}

	// Compare the memory-mapped files with memcmp.
	if (bResult){
		bResult = memcmp(FileContents[0], 
						  FileContents[1], 
						  dwFileSize) == 0;
	}

	// Unmap and close the files.
	for (int i = 0; i < 2; i++)
	{
		if (!UnmapViewOfFile(FileContents[i]))
		  {
			bResult = FALSE;
		  }

		if (!CloseHandle(Maps[i]))
		  {
			bResult = FALSE;
		  }

		if (!CloseHandle(FileHandle[i]))
		  {
			bResult = FALSE;
		  }
	}

	return bResult ? 0 : 1;
}

CString URLToPath(LPCTSTR lpszUrl)
{
	CString strPath;

	wchar_t szBuffer[_MAX_PATH];
	USES_CONVERSION;
	DWORD cchResult;
	if (CoInternetParseUrl(
		A2W(lpszUrl), 
		PARSE_PATH_FROM_URL,
		0, 
		szBuffer, 
		_MAX_PATH, 
		&cchResult, 
		0) == S_OK)
	{
		wchar_t szBuffer2[_MAX_PATH];

		if (CoInternetParseUrl(
			szBuffer, 
			PARSE_UNESCAPE,
			0, 
			szBuffer2, 
			_MAX_PATH, 
			&cchResult, 
			0) == S_OK)
		{
			strPath = szBuffer2;
		}
	}

	return strPath;
}

BOOL IsDirWritable(LPCTSTR lpszPath)
{
	// the following code using crt library will not work
	//struct _stat buf;
	//if (_stat(lpszPath, &buf) != 0)
	//	return FALSE;

	//return (buf.st_mode & _S_IWRITE) != 0;

	BOOL ret = FALSE;

	TCHAR szFile[_MAX_PATH + 1];
	if (::GetTempFileName(lpszPath, "~", 0, szFile))
	{
		HANDLE hFile = ::CreateFile(szFile, 0, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			ret = TRUE;
			::CloseHandle(hFile);
		}
		::DeleteFile(szFile);
	}
	return ret;
}

CString GetTempFile(LPCTSTR lpszPath, LPCTSTR lpszPrefix, LPCTSTR lpszExt)
{
	TCHAR szPath[_MAX_PATH + 1];
	memset(szPath, 0, sizeof(TCHAR) * sizeof(szPath));

	CString strName;
	if (lpszPath != NULL)
	{
		_tcsncpy(szPath, lpszPath, _MAX_PATH);
	}
	else
	{
		if (GetTempPath(_MAX_PATH, szPath) == 0)
			_tcsncpy(szPath, ::GetModuleFilePath(), _MAX_PATH);
	}

	::PathAddBackslash(szPath);
	for (int i = 0; i < INT_MAX; i++)
	{
		strName.Format("%s%s%d.%s", szPath, lpszPrefix ? lpszPrefix : "~", i, lpszExt ? lpszExt : "tmp");
		if (!::PathFileExists(strName))
			break;
	}

	return strName;
}

BOOL CreateTempFolder(LPCTSTR lpszPath, CString &strFolder)
{
	TCHAR szPath[_MAX_PATH + 1];
	memset(szPath, 0, sizeof(TCHAR) * sizeof(szPath));
	if (lpszPath != NULL)
	{
		_tcsncpy(szPath, lpszPath, _MAX_PATH);
	}
	else
	{
		if (GetTempPath(_MAX_PATH, szPath) == 0)
			_tcsncpy(szPath, ::GetModuleFilePath(), _MAX_PATH);
	}
	::PathAddBackslash(szPath);

	UUID id;
	RPC_CSTR lpsz = NULL;
	::UuidCreate(&id);
	::UuidToString(&id, &lpsz);

	strFolder.Format("%s{%s}", szPath, lpsz);

	::RpcStringFree(&lpsz);

	if (!::CreateDirectory(strFolder, NULL))
	{
		strFolder.Empty();
		return FALSE;
	}

	return TRUE;
}

INT64 GetFolderSize(LPCTSTR lpszFolder)
{
	INT64 size = 0;

	CString strFind = lpszFolder;
	::PathAddBackslash(strFind.GetBufferSetLength(_MAX_PATH));
	strFind.ReleaseBuffer();
	strFind += "*.*";

	CFileFind ff;
	BOOL ret = ff.FindFile(strFind);

	while (ret)
	{
		ret = ff.FindNextFile();
		if (!ff.IsDots())
		{
			CString strFile = ff.GetFilePath();

			if (::PathIsDirectory(strFile))
			{
				size += GetFolderSize(strFile);
			}
			else
			{
				struct _stat64 fs;
				if (_stati64(strFile, &fs) == 0)
				{
					size += fs.st_size;
				}
				else
				{
					TRACE("_stati64(%s) failed.\r\n", strFile);
				}
			}
		}
	}

	return size;
}

CString GetAbsolutePath(LPCTSTR RelativePath,LPCTSTR BaseAbsolutePath)
{
	TCHAR szCombinePath[MAX_PATH + 1];
	PathCombine(szCombinePath,BaseAbsolutePath,RelativePath);
	return szCombinePath;
}
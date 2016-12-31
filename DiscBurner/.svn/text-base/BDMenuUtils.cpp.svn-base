#include "StdAfx.h"
#include "BDMenuUtils.h"
#ifdef _BD

#define BD_FOLDERS   8
CString s_strBDfolder[BD_FOLDERS]=
{
	"BDMV",
	"BDMV\\AUXDATA",
	"BDMV\\BACKUP",
	"BDMV\\BACKUP\\CLIPINF",
	"BDMV\\BACKUP\\PLAYLIST",
	"BDMV\\CLIPINF",
	"BDMV\\PLAYLIST",
	"BDMV\\STREAM"
};

BOOL CreateBDFolderStructure(LPCTSTR lpszBDFolder)
{
	CString strFolder;
	for (int i= 0;i<BD_FOLDERS;i++)
	{
		strFolder.Format("%s\\%s",lpszBDFolder,s_strBDfolder[i]);
		ASSERT(IsValidPath(strFolder));
		TRACE("Create folder:%s\n",strFolder);
		if(!CreateDirectoryNest(strFolder))
			return FALSE;
	}
	return TRUE;
}

BOOL CopyFolder(LPCTSTR lpszSource,LPCTSTR lpszDst)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	CString strSourcePath = lpszSource;
	PathRemoveBackslash(strSourcePath.GetBuffer());
	strSourcePath.ReleaseBuffer();
	strSourcePath += "\\*";
	CString strFileRemovedName;
	CString strFileSource,strFileDst;
	// Find the first file in the directory.
	hFind = FindFirstFile(strSourcePath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		TRACE ("Invalid file handle. Error is %u.\n", GetLastError());
		return FALSE;
	} 
	else 
	{
		TRACE ("First file name is %s.\n", FindFileData.cFileName);
		// List all the other files in the directory.
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			printf ("Next file name is %s.\n", FindFileData.cFileName);
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TRACE ("  %s   <DIR>\n", FindFileData.cFileName);
			}
			else
			{
				strFileSource.Format("%s\\%s",lpszSource,FindFileData.cFileName);
				ASSERT(PathFileExists(strFileSource));
				strFileDst.Format("%s\\%s",lpszDst,FindFileData.cFileName);
				TRACE ("Copy from \"%s\" to \"%s\".\n", strFileSource,strFileDst);
				if(!CopyFile(strFileSource,strFileDst,FALSE,TRUE))
				{
					FindClose(hFind);
					return FALSE;
				}
			}
		}
	}
	FindClose(hFind);
	return TRUE;
}

BOOL CopyBlurayMenu(LPCTSTR lpszSource,LPCTSTR lpszDst)
{
	CString strSourceFolder,strDstFolder;
	for (int i= 0;i<BD_FOLDERS;i++)
	{
		strSourceFolder.Format("%s\\%s",lpszSource,s_strBDfolder[i]);
		ASSERT(IsValidPath(strSourceFolder));
		strDstFolder.Format("%s\\%s",lpszDst,s_strBDfolder[i]);
		ASSERT(IsValidPath(strDstFolder));

		if(!CopyFolder(strSourceFolder,strDstFolder))
			return FALSE;
	}
	return TRUE;
}

#endif
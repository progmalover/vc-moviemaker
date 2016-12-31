#include "StdAfx.h"
#include "VideoList.h"
#include "FileList.h"
#include "Task.h"

static CString s_strFileExt=".vfl";//video file list
static CString s_strComment="#";   //×¢ÊÍ·ûºÅ


IMPLEMENT_SINGLETON(CVideoList)
CVideoList::CVideoList(void)
{
	m_bReadList = AfxGetApp()->GetProfileInt("Debug", "Enable Video List", TRUE);
}

CVideoList::~CVideoList(void)
{

}

BOOL  CVideoList::Check(CString strFile)
{
	if(m_bReadList)
	{
		strFile.Trim();
		if(!::PathFileExists(strFile))
			return FALSE;
		if(strFile.GetLength()>=s_strFileExt.GetLength())
		{
			CString strExt = strFile.Right(s_strFileExt.GetLength());
			return strExt.CompareNoCase(s_strFileExt)==0;
		}
	}
	return FALSE;
}

BOOL  CVideoList::Read(CString strFile,std::vector<std::string> &list,std::vector<std::string> &failed_list)
{
	ASSERT(Check(strFile));
	if(!::PathFileExists(strFile))
		return FALSE;

	CStdioFile  fo(strFile,CFile::modeRead);
	CString strVideo;
	while (fo.ReadString(strVideo))
	{
		strVideo.Trim();
   
		if(strVideo.IsEmpty()) //Empty
			continue;
    
		if(strVideo.Left(s_strComment.GetLength())==s_strComment)//Comment
		{
			TRACE(strVideo+"\n");
			continue;
		}

		if(::PathFileExists(strVideo))
		{
			list.push_back((LPCTSTR)strVideo);
			TRACE("Add:%s\n",strVideo);
		}
		else
		{	
			failed_list.push_back((LPCTSTR)strVideo);
			TRACE("Add failed:%s\n",strVideo);
		}
	}
	return TRUE;
}

void  CVideoList::Remove(std::vector<std::string> &list,std::string strItem)
{
	//BOOL bItemed = TRUE;
	//while(bItemed)
	//{
	//	bItemed = FALSE;
	//	std::vector<std::string>::iterator it;
	//	for(it = list.begin();it != list.end();it++)
	//	{
	//		if(*it == strItem)
	//		{
	//			list.erase(it);
	//			bItemed = TRUE;
	//			break;
	//		}
	//	}
	//}

	std::vector<std::string>::iterator iter = list.begin();
	while(iter != list.end())
	{
		if (*iter == strItem)
			iter = list.erase(iter);
		else
			++iter;
	}
}

void  CVideoList::Splice(std::vector<std::string> &dst_list,std::vector<std::string> &src_list)
{
	if(src_list.size()>0)
	{
		for(int i = 0;i < src_list.size();i++)
			dst_list.push_back(src_list[i]);
		src_list.clear();
	}
}

BOOL CVideoList::MakeFileList(CString strFileList)
{
	if(!m_bReadList)
		return FALSE;
	FILE *fo = fopen(strFileList,"w");
	if(fo==NULL)
	{
		TRACE("Create file(\"%s\") failed.\r\n",strFileList);
		return FALSE;
	}
	CString strLineEnd = "\r\n";
	for (int i = 0; i < CFileList::Instance()->GetItemCount(); i++)
	{
		if (CFileList::Instance()->GetCheck(i))
		{
			CTask *pTask = (CTask *)CFileList::Instance()->GetItemData(i);
			fwrite(pTask->m_strSourceFile,1,pTask->m_strSourceFile.GetLength(),fo);
			fwrite(strLineEnd,1,strLineEnd.GetLength(),fo);
		}
	}
	fclose(fo);
	::ShellOpenFolder(NULL, strFileList);
	return TRUE;
}
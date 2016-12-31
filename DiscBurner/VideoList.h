#pragma once
//方便测试人员将一些视频编辑为一个文件，直接添加

class CVideoList
{
	
public:
	CVideoList(void);
	~CVideoList(void);
	DECLARE_SINGLETON(CVideoList)

	BOOL  Read(CString strFile,std::vector<std::string> &list,std::vector<std::string> &failed_list);
	BOOL  Check(CString strFile);

	static void  Remove(std::vector<std::string> &list,std::string strItem);
	static void  Splice(std::vector<std::string> &dst_list,std::vector<std::string> &src_list);
	BOOL MakeFileList(CString strFileList);
private:
	BOOL m_bReadList;
};

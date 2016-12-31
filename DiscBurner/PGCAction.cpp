#include "StdAfx.h"
#include "PGCAction.h"

#ifdef _DVD

CString g_strAction[4]=
{
	_T("jump title %d chapter %d;"),
	_T("jump title %d chapter %d;"),
	_T("call menu;"),
	_T("call vmgm menu entry title;")
};

//��Ҫ�Ļ�ȡ�㷨
int GetTotalPGC(int nTotal)
{
	ASSERT(nTotal>=0);
	return min(nTotal,MAX_TITLES);
}

//��������˵������ǰ����ı�Ż�ȡtitle ID[PGC ID],�����ص�ǰtitle������������
//int nTaskIndex[in]  From 0
//int nTotal[in]
//int &nTitleID[out]
int  GetTitleID(int nTask,int nTotal,int &nTitleID,int &nChapterID)
{
	ASSERT(nTask>=0);
	int nChapters = 1;
	if(nTotal<=MAX_TITLES)
	{
		nTitleID = nTask+1;
		nChapterID = 1;
		return 1;
	}
	else
	{
		int nAvg = nTotal/MAX_TITLES;
		if(nTotal%MAX_TITLES!=0)
			nAvg++;
		//ǰx��PGCÿ��PGC x��Task����MAX_TITLES-x��PGCÿ��1��Task
		//x*nAvg + (MAX_TITLES-x)*1 = nTotal
		int nMulitPGCMax = (nTotal-MAX_TITLES)/(nAvg-1);
		if((nTotal-MAX_TITLES)%(nAvg-1)!=0)
			nMulitPGCMax++;

		int nMulitTask = nTotal-(MAX_TITLES-nMulitPGCMax);
		if((nTask+1)<=nMulitTask)
		{
			nTitleID = nTask/nAvg+1;
			nChapterID = nTask%nAvg+1;
			if(nMulitPGCMax != nTitleID)
				return nAvg;
			else//the last
				return nMulitTask-(nTitleID-1)*nAvg;
		}
		else 
		{
			nTitleID = MAX_TITLES-(nTotal-(nTask+1));
			nChapterID =1;
			return 1;
		}
	}
	return -1;
}
#ifdef _DEBUG
void TestGetTitleID()
{
	int totalArray[]={3,5,7,8,9,10,11,12,13,14};
	for(int i=0;i<sizeof(totalArray)/sizeof(int);i++)
	{
		TRACE("-----------\nAll task = %d\n",totalArray[i]);
		for(int j= 0;j<totalArray[i];j++)
		{
			int nChapters,nTitleID,nChapterID;
			nChapters = GetTitleID(j,totalArray[i],nTitleID,nChapterID);
			TRACE("Task = %d,nTitleID = %d,nChapterID = %d,All chapter = %d\n",j,nTitleID,nChapterID,nChapters);
		}
	}
}
#endif


CPGCAction::CPGCAction(void)
{
}

CPGCAction::~CPGCAction(void)
{
}

#endif

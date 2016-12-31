#pragma once

#ifdef _DVD
enum NAVIGATION_ACTION
{
	PLAY_REPEATLY,
	PLAY_NEXT_VIDEO,
	BACK_TO_MENU,
	BACK_FIRST_MENU,
};

extern CString g_strAction[4];
#define MAX_TITLES    99

int GetTotalPGC(int nTotal);
int GetTitleID(int nTask,int nTotal,int &nTitleID,int &nChapterID);
#ifdef _DEBUG
void TestGetTitleID();
#endif

class CPGCAction
{
	
public:
	CPGCAction(void);
	~CPGCAction(void);
};
#endif
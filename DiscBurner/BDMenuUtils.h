#pragma once
#ifdef _BD
enum LAST_TITLE_ACTION{BACK_FIRST_MENU=0,BACK_FIRST_TITLE=1,BACK_FIRST_PLAYBACK=2};
enum RESOURCE_FORMAT{PNG,BINARY};
#define NAVIGATION_ACTION LAST_TITLE_ACTION
#define ENABLE_MENU       0

BOOL CreateBDFolderStructure(LPCTSTR lpszBDFolder);
BOOL CopyBlurayMenu(LPCTSTR lpszSource,LPCTSTR lpszDst);

#endif


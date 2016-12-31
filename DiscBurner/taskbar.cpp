#include "stdafx.h"
#include "taskbar.h"

UINT WM_TASKBARBUTTONCREATED = RegisterWindowMessage(_T("TaskbarButtonCreated"));

CComPtr<ITaskbarList3> g_spTaskbar;
#pragma once

#include "resource.h"
#define  MENU_VIDEO_BITRATE   5000  //K 
#define  MENU_AUDIO_BITRATE    192  //K 
//以上定义在估算产生的菜单文件大小时需要，如果这2个参数被修改，请同时修改FileList.h中的定义

typedef BOOL (* PARSE_OUTPUT_PROC)(LPCTSTR);

std::string GetNodeAttribute(Node *pNode, LPCTSTR lpszName);
BOOL ParseXML(LPCTSTR lpszFile);
BOOL GenerateMenu();
BOOL EncodeMenu();
BOOL ParseFFmpegOutput(LPCTSTR lpszLine);
BOOL ParseSPUMuxOutput(LPCTSTR lpszLine);
BOOL ExecuCommand(LPCTSTR lpszApplication, LPCTSTR lpszParameters, PARSE_OUTPUT_PROC pfnParseOutput);
//#define  _TEST_SPEED

/*
Copyright (c) Rocket Division Software 2001-2008. All rights reserved.

File Name:

	Logger.cpp

Description:

	Implementation of CLogger class.
	class CLogger implement multi level logging support

Author:

	Andery Trubeckoy
*/
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include "Logger.h"
//#include "stdafx.h"

std::vector<TLogImp> CLogger::ImpList;

bool operator== (TLogImp LogImp1,TLogImp LogImp2)
{
        if( LogImp1.LogFunc == LogImp2.LogFunc &&
            LogImp1.LogLevel == LogImp2.LogLevel)
        {
                return true;
        }
        else
        {
                return false;
        }
}
void CLogger::Log
    (
    TLogLevel LogLevel,
    TMsgType MsgType,
    const char *TextMsg,
    ...
    )
{

    char l_TempTextMsg[4096];
    char l_FullTextMsg[32786];
        memset(
        &l_FullTextMsg,
		0,
        sizeof( l_FullTextMsg )
        );


    va_list l_VaListNext;

    // Start processing virtual arguments list

    va_start(
        l_VaListNext,
        TextMsg
        );

    // Format the string

    memset(
        &l_TempTextMsg,
		0,
        sizeof( l_TempTextMsg )
        );

    vsprintf(
        l_TempTextMsg,
        TextMsg,
        l_VaListNext
        );

    strcat((char*)&l_FullTextMsg,(char*)&l_TempTextMsg);
    va_end( l_VaListNext );

    LogByImp(LogLevel,
              MsgType,
              l_FullTextMsg);
}


void CLogger::Reg(DWORD cData, TLogFunc LogFunc,TLogLevel LogLevel)
{
        TLogImp LogImp;

        LogImp .LogFunc = LogFunc;
        LogImp .LogLevel = LogLevel;
		LogImp .cData  = cData;
        for(size_t i=0;i<ImpList.size();i++)
        {
                if(ImpList[i] == LogImp)
                {
                        return;
                }
        };
        ImpList.push_back(LogImp);
}

void CLogger::LogByImp(
                       TLogLevel LogLevel,
                       TMsgType MsgType,
                       const char *Msg
                       )
{
    for(size_t i=0;i<ImpList.size();i++)
    {
        if(LogLevel == ImpList[i].LogLevel)
        {
                TLogFunc LogFunc = (TLogFunc)ImpList[i].LogFunc;
                LogFunc(ImpList[i].cData, MsgType,Msg);
        }
    }
}



void CLogger::ClearImpList()
{
        ImpList.clear();
}

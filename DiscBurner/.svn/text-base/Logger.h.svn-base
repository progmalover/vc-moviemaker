 /*
 Copyright (c) Rocket Division Software 2001-2008. All rights reserved.

File Name:

    Logger.h

Description:

    Interface of CLogger class.
	class CLogger implement multi level logging support

Author:

    Andery Trubeckoy
*/

#ifndef CLoggerH
#define CLoggerH

#include <vector>


// Logger Level Type
enum TLogLevel
{
        Debug,
        User        
};
// Type of Message Type
enum TMsgType
{
        lmtInformation,
        lmtSubInformation,
        lmtWarning,
        lmtError
};
//Type of function which will be registred in Logger
typedef void (*TLogFunc)(DWORD,TMsgType,const char*);

//Log Implement Type
class TLogImp
{
public:
        TLogFunc LogFunc;  //Function which will be registered in Logger
        TLogLevel LogLevel;//Logger level for this function
		DWORD cData;
        friend bool operator== (TLogImp LogImp1,TLogImp LogImp2);
};




class CLogger
{
        public:
//Method which delete all implements
        static void ClearImpList();
//Method which registred implement
        static void Reg(DWORD,TLogFunc,TLogLevel);
//Log Method
        static void Log(
                        TLogLevel,           // Logger Level
                        TMsgType,            //Message Type
                        const char *TextMsg, // Title Message
                        ...
                        );

        private:
        static void LogByImp(
                              TLogLevel,        //Logger Level
                              TMsgType,         //Message Type
                              const char *Msg   //Message
                              );

        static std::vector<TLogImp> ImpList;

};
#endif






#pragma once

class CLog
{
public:
	DECLARE_SINGLETON(CLog)
	enum {LogRelease,logDebug};//Log Level

	CLog(void)
	{
#ifdef _DEBUG
		m_bEnableLogging = AfxGetApp()->GetProfileInt("Options\\General", "Enable Logging", TRUE);
#else
		m_bEnableLogging = AfxGetApp()->GetProfileInt("Options\\General", "Enable Logging", FALSE);
#endif
		m_logLevel = AfxGetApp()->GetProfileInt("Options\\General", "Log Level", LogRelease);

		m_hLogMutex = NULL;

		if (m_bEnableLogging)
			OpenLog();
	}

	virtual ~CLog(void)
	{
	#ifdef _DEBUG
		if (m_bEnableLogging)
			CloseLog();
	#endif
		AfxGetApp()->WriteProfileInt("Options\\General", "Enable Logging", m_bEnableLogging);
	}

	BOOL m_bEnableLogging;
	CFile m_logFile;
	HANDLE m_hLogMutex;
	int    m_logLevel;

	BOOL OpenLog()
	{
		if (m_logFile.m_hFile == INVALID_HANDLE_VALUE)
		{
			CTime time(time(0));
			CString strTime;
#ifdef _DVD
			strTime = time.Format("SOTHINK-MDM-%Y-%m-%d-");
#else
			strTime = time.Format("SOTHINK-HDMM-%Y-%m-%d-");
#endif
			CString strFile = GetTempFile(NULL, strTime, "LOG");

			CFileException e;
			if (!m_logFile.Open(strFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyWrite, &e))
			{
				e.ReportError();
				e.Delete();
				return FALSE;
			}

			m_hLogMutex = ::CreateMutex(NULL, FALSE, NULL);
		}

		return TRUE;
	}

	void CloseLog()
	{
		if (m_logFile.m_hFile != INVALID_HANDLE_VALUE)
		{
			CString strFile = m_logFile.GetFilePath();

			::WaitForSingleObject(m_hLogMutex, INFINITE);
			m_logFile.Close();
			::ReleaseMutex(m_hLogMutex);
			::CloseHandle(m_hLogMutex);
			m_hLogMutex = NULL;

			if (::PathFileExists(strFile))
				::DeleteFile(strFile);
		}
	}

	void ViewLogFile()
	{
		if (m_bEnableLogging)
		{
			CString strFile = m_logFile.GetFilePath();
			if (::PathFileExists(strFile))
				ShellOpenFolder(NULL, strFile, FALSE);
		}
	}

	void AppendLog(LPCTSTR fmt, ...)
	{
		if (!m_bEnableLogging || m_logFile.m_hFile == INVALID_HANDLE_VALUE)
			return;

		if (::WaitForSingleObject(m_hLogMutex, INFINITE) != WAIT_OBJECT_0)
			return;

		CTime time(time(0));
		CString strTime = time.Format("%H:%M:%S ");
		m_logFile.Write(strTime, strTime.GetLength());

		CString strLog;
		va_list args;
		va_start(args, fmt);
		strLog.FormatV(fmt, args);
		va_end (args);

		m_logFile.Write(strLog, strLog.GetLength());

		m_logFile.Flush();

		::ReleaseMutex(m_hLogMutex);
	}

	void AppendLogNoTime(LPCTSTR fmt, ...)
	{
		if (!m_bEnableLogging || m_logFile.m_hFile == INVALID_HANDLE_VALUE)
			return;

		if (::WaitForSingleObject(m_hLogMutex, INFINITE) != WAIT_OBJECT_0)
			return;

		CString strLog;
		va_list args;
		va_start(args, fmt);
		strLog.FormatV(fmt, args);
		va_end (args);

		m_logFile.Write(strLog, strLog.GetLength());

		m_logFile.Flush();

		::ReleaseMutex(m_hLogMutex);
	}

	void AppendLog2(LPCTSTR fmt, ...)
	{
		if(m_logLevel>LogRelease)
		{
			if (!m_bEnableLogging || m_logFile.m_hFile == INVALID_HANDLE_VALUE)
				return;

			if (::WaitForSingleObject(m_hLogMutex, INFINITE) != WAIT_OBJECT_0)
				return;

			CTime time(time(0));
			CString strTime = time.Format("%H:%M:%S ");
			m_logFile.Write(strTime, strTime.GetLength());

			CString strLog;
			va_list args;
			va_start(args, fmt);
			strLog.FormatV(fmt, args);
			va_end (args);

			m_logFile.Write(strLog, strLog.GetLength());

			m_logFile.Flush();

			::ReleaseMutex(m_hLogMutex);
		}
	}

	void AppendLogNoTime2(LPCTSTR fmt, ...)
	{
		if(m_logLevel>LogRelease)
		{
			if (!m_bEnableLogging || m_logFile.m_hFile == INVALID_HANDLE_VALUE)
				return;

			if (::WaitForSingleObject(m_hLogMutex, INFINITE) != WAIT_OBJECT_0)
				return;

			CString strLog;
			va_list args;
			va_start(args, fmt);
			strLog.FormatV(fmt, args);
			va_end (args);

			m_logFile.Write(strLog, strLog.GetLength());

			m_logFile.Flush();

			::ReleaseMutex(m_hLogMutex);
		}
	}
};

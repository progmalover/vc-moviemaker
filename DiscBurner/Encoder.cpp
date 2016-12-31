#include "StdAfx.h"
#include "Converter.h"
#include "Encoder.h"
#include "Task.h"
#include "FileList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEncoder::CEncoder()
{
	m_pTask = NULL;
	m_bOutput = FALSE;
}

CEncoder::~CEncoder(void)
{
}

BOOL CEncoder::Start()
{
	TRACE("%s\n",m_strCommandLine);
	CLog::Instance()->AppendLog("Enter CEncode::Start()\r\n");
	CLog::Instance()->AppendLog("\"%s\" %s\r\n", (LPCTSTR)GetApplicationName(), (LPCTSTR)m_strCommandLine);

	BOOL bResult = FALSE;

	HANDLE hOutputReadTmp, hOutputRead, hOutputWrite;

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (::CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0))
	{
		::DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
			GetCurrentProcess(),
			&hOutputRead, // Address of new handle.
			0,FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS);

		// Close inheritable copies of the handles you do not want to be
		// inherited.
		::CloseHandle(hOutputReadTmp);

		STARTUPINFO si;

		// Set up the start up info struct.
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdOutput = hOutputWrite;
		si.hStdError = hOutputWrite;

		PROCESS_INFORMATION pi;
		memset(&pi, 0, sizeof(pi));

		CString strCommandLine;
		strCommandLine.Format("\"%s\" %s", (LPCTSTR)GetApplicationName(), (LPCTSTR)m_strCommandLine);
		TRACE("***\n%s\n\n", strCommandLine);

		CLog::Instance()->AppendLog("CreateProcess(): ");

		BOOL ret = ::CreateProcess(NULL, strCommandLine.GetBuffer() /* may be modified in UNICODE version */, 
			NULL, NULL, TRUE, CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);

		strCommandLine.ReleaseBuffer();

		CLog::Instance()->AppendLogNoTime("Done\r\n");

		if (!ret)
		{
			m_pTask->Lock();
			m_pTask->m_nStatus = CONV_STATUS_FAILED;
			m_pTask->Unlock();

		#ifdef _DEBUG
			CString strError;
			strError.Format("Failed to execute the encoder: %s", (LPCTSTR)GetLastErrorMessage());
			CFileList::Instance()->MessageBox(strError, NULL, MB_OK | MB_ICONERROR);
		#endif
		}
		else
		{
			CLog::Instance()->AppendLog("WaitForInputIdle(): ");
			::WaitForInputIdle(pi.hProcess, 1000 * 10);
			CLog::Instance()->AppendLogNoTime("Done\r\n");

			HANDLE handles[] = {pi.hProcess, m_pTask->m_hCancelEvent, m_pTask->m_hPauseEvent, m_pTask->m_hResumeEvent};
			BOOL bExitProcess = FALSE;

			CString strLine;
			char szRead[256];

			while (true)
			{
				if (!bExitProcess)
				{
					DWORD ret = ::WaitForMultipleObjects(sizeof(handles) / sizeof(HANDLE), handles, FALSE, 0);
					if (ret == WAIT_OBJECT_0)
					{
						bExitProcess = TRUE;
					}
					else if (ret == WAIT_OBJECT_0 + 1)
					{
						// Wait for the console initialization. If can not stop now, m_pTask->m_hCancelEvent 
						// remains signed so it can be waited in the next loop.
						if (CanStop())
						{
							TRACE("CEncoder::Start(): Conversion is canceled.\n");
							if (::AttachConsole(pi.dwProcessId))
							{
								::GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pi.dwProcessId);
								::FreeConsole();
							}

							ret = ::WaitForSingleObject(pi.hProcess, 1000 * 5);
							if (ret != WAIT_OBJECT_0)
								TerminateProcess(pi.hProcess, -1);

							break;
						}
					}
					else if (ret == WAIT_OBJECT_0 + 2)
					{
						// Wait for the console initialization. If can not stop now, m_pTask->m_hPauseEvent  
						// remains signed so it can be waited in the next loop.
						if (CanPause())
						{
							ASSERT(!m_pTask->m_bPaused);
							::ResetEvent(m_pTask->m_hPauseEvent);
							VERIFY(::SuspendThread(pi.hThread) == 0);
							::SetEvent(m_pTask->m_hPauseEvent2);
						}
					}
					else if (ret == WAIT_OBJECT_0 + 3)
					{
						ASSERT(m_pTask->m_bPaused);
						::ResetEvent(m_pTask->m_hResumeEvent);
						VERIFY(::ResumeThread(pi.hThread) == 1);
						::SetEvent(m_pTask->m_hResumeEvent2);
					}
				}

				DWORD dwRead = 0;

				if (!::PeekNamedPipe(hOutputRead, NULL, 0, NULL, &dwRead, NULL))
					break;

				if (dwRead == 0)
				{
					if (bExitProcess)
					{
						break;
					}
					else
					{
						Sleep(m_bOutput ? 1000 : 100);
						continue;
					}
				}

				if (!::ReadFile(hOutputRead, szRead, 255, &dwRead, NULL) || dwRead == 0)
					break;

				m_bOutput = TRUE;	// We have gotten the console output

				szRead[dwRead] = '\0';
				strLine += szRead;

				while (true)
				{
					int pos = strLine.FindOneOf("\r\n");
					if (pos >= 0)
					{
						while (strLine[pos + 1] == '\r' || strLine[pos + 1] == '\n')
							pos++;

						int len = strLine.GetLength();
						strLine.SetAt(pos, '\0');
						
						CString strTemp = strLine;
						strTemp.TrimRight("\r\n");
						CLog::Instance()->AppendLog2("%s\r\n", strTemp);

						ParseOutput(strLine);

						strLine = strLine.Right(len - (pos + 1));
					}
					else
					{
						break;
					}
				}
			}

			DWORD dwCode;
			GetExitCodeProcess(pi.hProcess, &dwCode);
			CLog::Instance()->AppendLog("Encoder exit code: %d\r\n", dwCode);
			
			m_pTask->Lock();
			/*
			if (m_pTask->IsCanceled())
			{
				m_pTask->m_nStatus = CONV_STATUS_CANCELED;
			}
			else
			{
				m_pTask->m_nStatus = dwCode == 0 ? CONV_STATUS_COMPLETED : CONV_STATUS_FAILED;
			}

			if (m_pTask->m_nStatus == CONV_STATUS_COMPLETED)
				bResult = TRUE;
			*/

			if (m_pTask->IsCanceled())
			{
				m_pTask->m_nStatus = CONV_STATUS_CANCELED;
			}

			if(dwCode==0) bResult = TRUE;

			
			m_pTask->m_nUpdate |= UPDATE_STATUS;
			m_pTask->Unlock();

			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}
	}

	TRACE("Close Pipe\n");
	if (hOutputWrite)
	{
		::CloseHandle(hOutputWrite);
		hOutputWrite = NULL;
	}
	if (hOutputRead)
	{
		::CloseHandle(hOutputRead);
		hOutputRead = NULL;
	}

	CLog::Instance()->AppendLog("Exit CEncoder::Start()\r\n");

	return bResult;
}

BOOL CEncoder::CanPause()
{
	return FALSE;
}

BOOL CEncoder::CanStop()
{
	return TRUE;
}

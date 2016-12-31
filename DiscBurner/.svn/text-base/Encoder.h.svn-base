#pragma once

class CTask;

class CEncoder
{
public:
	CEncoder();
	virtual ~CEncoder(void);

	virtual BOOL Start();

	CTask *m_pTask;
	CString m_strCommandLine;
	BOOL m_bOutput;

protected:

	virtual CString GetApplicationName() = 0;
	virtual BOOL ParseOutput(LPCTSTR lpszLine) = 0;
	virtual BOOL CanPause();
	virtual BOOL CanStop();
};

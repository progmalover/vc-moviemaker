#pragma once
#include "encoder.h"

#ifdef _BD

class CEncoderBlueRay :
	public CEncoder
{
public:
	CEncoderBlueRay(void);
	~CEncoderBlueRay(void);

	virtual BOOL Start();

	CString m_strOutputRoot;
	CString m_strScript;
	CString m_strOutput;
	CString m_strInputFiles;
	CString m_strInputAudios;
	CString m_strMenuFolder;
protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
};

#endif
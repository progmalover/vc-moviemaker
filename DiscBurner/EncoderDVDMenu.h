#pragma once
#include "encoder.h"

#ifdef _DVD

class CEncoderDVDMenu :
	public CEncoder
{
public:
	CEncoderDVDMenu(void);
	~CEncoderDVDMenu(void);

	virtual BOOL Start();

	CString m_strResourceFolder;
	CString m_strOutputFolder;
	CString m_strXMLFile;

protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
};

#endif
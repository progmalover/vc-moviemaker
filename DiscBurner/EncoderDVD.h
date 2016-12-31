#pragma once
#include "encoder.h"

#ifdef _DVD

class CEncoderDVD :
	public CEncoder
{
public:
	CEncoderDVD(void);
	~CEncoderDVD(void);

	virtual BOOL Start();

	INT64 m_nTotalSize;
	CString m_strXMLFile;
	CString m_strOutputFolder;

protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
};

#endif
#pragma once
#include "encoder.h"

#ifdef _BD
class CEncoderBDMenu :
	public CEncoder
{
public:
	CEncoderBDMenu(void);
	~CEncoderBDMenu(void);

	CString m_strOutputFolder;
	CString m_strMenuXML;

	virtual BOOL Start();
protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
	virtual BOOL CanPause(){return FALSE;};
	virtual BOOL CanStop(){return FALSE;};
};
#endif
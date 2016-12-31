#pragma once
#include "encoder.h"

class CEncoderMEncoder :
	public CEncoder
{
public:
	CEncoderMEncoder();
	virtual ~CEncoderMEncoder(void);

protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
};

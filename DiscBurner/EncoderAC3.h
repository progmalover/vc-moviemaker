#pragma once
#include "encoder.h"
#include "Task.h"

#ifdef AC3_AFTEN
class CEncoderAC3 :
	public CEncoder
{
public:
	CEncoderAC3(void);
	~CEncoderAC3(void);
	BOOL Start();
protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
	virtual BOOL CanPause(){return TRUE;};
	virtual BOOL CanStop(){return TRUE;};
};
#endif
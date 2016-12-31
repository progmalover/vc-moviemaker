#pragma once
#include "encoder.h"

class CEncoderFFmpeg :
	public CEncoder
{
public:
	CEncoderFFmpeg();
	virtual ~CEncoderFFmpeg(void);

	BOOL Start();
protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
	virtual BOOL CanPause();
	virtual BOOL CanStop();
	int     m_nConvertedFrames;
};

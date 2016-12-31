#pragma once
#include "encoder.h"
#include "Task.h"

#ifdef _BD
extern  float g_fMKVMergeRatio;

class CMKVMerge :
	public CEncoder
{
public:
	CMKVMerge(void);
	~CMKVMerge(void);

	virtual BOOL Start();

	CString m_strOutputFolder;
	CString m_strInputs;
	CString m_strMKV;
private:
	int m_nProgress;
protected:
	virtual CString GetApplicationName();
	virtual BOOL ParseOutput(LPCTSTR lpszLine);
	virtual BOOL CanPause(){return FALSE;};
	virtual BOOL CanStop(){return FALSE;};
};
#endif
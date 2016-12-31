#include "StdAfx.h"
#include "Task.h"
#include "Converter.h"
#include "ProfileManager.h"
#include <Shlwapi.h>
#include "EncoderAC3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef AC3_AFTEN
BOOL CTask::AudioAtom(vector<taskAtom> *pAtoms)
{
	BOOL bFunctionLimited = !g_bIsActivated;
	double fDuration  = m_fDuration;

	taskAtom atom;
	atom.startframe = -1;
	atom.endframe = -1;
	atom.novideo = TRUE;
	atom.starttime = 0;

	CString strAVSEncoding;
	for (int i = 0; ; i++)
	{
		strAVSEncoding.Format("%s\\Encoding%d.avs", m_strTempFolder, i);
		if (!PathFileExists(strAVSEncoding)) break;
	}	
	atom.avs=strAVSEncoding;

	CProfileItem *pItem;
	CProfileManager::Instance()->GetCurrentProfile(NULL, &pItem);

	CString strSampleRate;
	CProfileParam *pParamSampleRate = (CProfileParam *)pItem->GetProfileParam("asamplerate");
	if (pParamSampleRate == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	pParamSampleRate->GetValue(this, strSampleRate);

	CString strChannels;
	CProfileParam *pParamChannels = (CProfileParam *)pItem->GetProfileParam("achannels");
	if (pParamChannels == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	pParamChannels->GetValue(this, strChannels);

	BOOL bUnknownFps = (fabs(m_mi.m_fFrameRate - 0.0) < FLT_EPSILON);
	float fFramerate = min(MAX_FRAMERATE, max(MIN_FRAMERATE, (bUnknownFps ? DEF_FRAMERATE : m_mi.m_fFrameRate)));

	CString strScript;
	try
	{
		CString strTemplate;
		strTemplate.Format("%s\\Scripts\\AEncoding.avs", GetModuleFilePath());

		CStdioFile file(strTemplate, CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite);
		ULONGLONG len = file.GetLength();
		file.Read(strScript.GetBufferSetLength((int)len), (UINT)len);
		strScript.ReleaseBuffer();

		strScript.Replace("$(modulepath)", GetModuleFilePath());
		strScript.Replace("$(input)", m_strSourceFileAlt.IsEmpty() ? m_strSourceFile : m_strSourceFileAlt);

		CString strFramerate;
		strFramerate.Format("%.3f", fFramerate);
		strScript.Replace("$(vframerate)", strFramerate);

		strScript.Replace("$(seekzero)", bUnknownFps ? "true" : "false");

		strScript.Replace("$(asamplerate)", strSampleRate);
		strScript.Replace("$(achannels)", strChannels);

		strScript.Replace("$(enablevideo)", pItem->m_bHasVideo ? "true" : "false");

		if(m_bTrimmed)
			atom.startframe=(__int64)(m_fTrimStart*fFramerate+0.5);
		else
			atom.startframe=atom.endframe+1;
		atom.endframe = (__int64)(fDuration*fFramerate)+atom.startframe;

		if(m_bTrimmed || bFunctionLimited)
		{
			CString FrameStr;

			FrameStr.Format("%d",(int)(atom.startframe));
			strScript.Replace("$(trimstart)", FrameStr);
			FrameStr.Format("%d",(int)(atom.endframe));
			strScript.Replace("$(trimend)", FrameStr);
		}
		else
		{
			strScript.Replace("$(trimstart)", "0");
			strScript.Replace("$(trimend)", "0");
		}

		if( AfxGetApp()->GetProfileInt("Debug", "File Log", FALSE))
			strScript.Replace("$(baudiolog)", "true");
		else
			strScript.Replace("$(baudiolog)", "false");
		ASSERT(strScript.Find("$(") < 0);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	try
	{
		CStdioFile file(strAVSEncoding, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write(strScript, strScript.GetLength());
		file.Flush();
		file.Close();
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	atom.cmd = CProfileManager::Instance()->GetEncoderCmd(&atom,this,_AUDIO);
	pAtoms->push_back(atom);
	return TRUE;
}
#endif


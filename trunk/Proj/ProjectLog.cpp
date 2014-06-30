#include "StdAfx.h"
#include "ProjectLog.h"
#include "Properties.h"

CProjectLog g_theLogFile;

CProjectLog::CProjectLog(void)
{
}

CProjectLog::~CProjectLog(void)
{
}



/**
*	\remarks	Init the log file
*
*	\todo		-	setup some logging options
*				-	right now we always overwrite log files
*					setup something that backs up the old one,
*					or appends to it
*/
void CProjectLog::Init(void)
{
	if (!CProperties::GetBool(rLogFile))
		return;		//no log file requested

	CRedPath file = CProperties::Get(rLogFile);
	if (file._Is(eEXISTS))
		::DeleteFile(file.GetString());

	CString str;
	str.Format( L"Red BuildIt log created %s", 
				CRedHelperFn::GetTime() );

	g_theLogFile.m_LogFile.WriteIt( &g_theLogFile.m_CFile, file.GetString(), str);

} //CProjectLog::Init()



/**
*	\remarks	Init the log file
*/
void CProjectLog::UnInit(void)
{
	if (g_theLogFile.m_CFile.m_hFile != CFile::hFileNull)
		g_theLogFile.m_CFile.Close();

} //CProjectLog::UnInit()



/**
*	\remarks	Write something
*/
void CProjectLog::Write(__in CString sMsg)
{
	CSingleLock lock(&g_theLogFile.m_CriticalSection);
	lock.Lock();

	g_theLogFile.m_LogFile.WriteIt(&g_theLogFile.m_CFile, sMsg);

	lock.Unlock();

} //CProjectLog::Write()


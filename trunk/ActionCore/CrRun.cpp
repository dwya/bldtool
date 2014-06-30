#include "StdAfx.h"
#include "CrRun.h"

CrRun::CrRun(void)
: m_file(_T(""))
, m_cmdline(_T(""))
{
}

CrRun::~CrRun(void)
{
}



/**
*	\remarks	Init members with data from xml node
*/
bool CrRun::Init(LPCTSTR szName)
{
	CString str;
	str.Format(L"%s[@%s = '%s']", 
				  XPATH_ACTION_RUN, 
				  ATT_NAME, 
				  szName);

	if (!m_Project.Select(str))
	{
		str.Format(L"%s no such action", szName);
		CProjectLog::Write(str);

		return false;

	}

	str		= m_Project.GetAttribute(ATT_FILE);
	m_file	= CProperties::ResolveProperties(str);

	str		= m_Project.GetAttribute(ATT_CMDLINE);
	m_cmdline = CProperties::ResolveProperties(str);

	return true;

} //CrRun::Init()



/**
*	\remarks	Write members to log file and execute
*/
UINT CrRun::Go(void)
{
	CString str;

	str.Format(L"file = %s", m_file);
	CProjectLog::Write(str);

	str.Format(L"cmdline = %s", m_cmdline);
	CProjectLog::Write(str);

	return CRedHelperFn::LaunchExecutableAndWait(m_file, m_cmdline);

} //CrRun::Go()


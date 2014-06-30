#include "StdAfx.h"
#include "CrGet.h"

CrGet::CrGet(void)
{
}

CrGet::~CrGet(void)
{
}


bool CrGet::Init(LPCTSTR szName)
{
	CString str;
	str.Format(L"%s[@%s = '%s']", 
			   XPATH_ACTION_GET, 
			   ATT_NAME, 
			   szName);

	if (!m_Project.Select(str))
	{
		str.Format(L"%s no such action", szName);
		CProjectLog::Write(str);

		return false;

	}

	m_name = szName;

	m_type = m_Project.GetAttribute(ATT_TYPE);

	str		= m_Project.GetAttribute(ATT_SRC);
	m_src	= CProperties::ResolveProperties(str);

	str		= m_Project.GetAttribute(ATT_DEST);
	m_dest	= CProperties::ResolveProperties(str);

	str			= m_Project.GetAttribute(ATT_OPTIONS);
	m_options	= CProperties::ResolveProperties(str);

	str = m_Project.GetAttribute(ATT_CREDITS);
	GetCredits(str, &m_Credits);

	return true;

} //CrGet::Init()


UINT CrGet::Go(void)
{
	//call pl script

	AfxMessageBox(L"CrGet::Go()");

	return ERROR_SUCCESS;

} //CrGet::Go


#include "StdAfx.h"
#include "CrPut.h"

CrPut::CrPut(void)
: m_type(_T(""))
, m_src(_T(""))
, m_dest(_T(""))
, m_options(_T(""))
{
	m_Credits.sUser = L"";
	m_Credits.sPass = L"";
	m_Credits.sServer = L"";
}


CrPut::~CrPut(void)
{
}


bool CrPut::Init(LPCTSTR szName)
{
	CString str;
	str.Format(L"%s[@%s = '%s']", 
			   XPATH_ACTION_PUT, 
			   ATT_NAME, 
			   szName);

	if (!m_Project.Select(str))
	{
		str.Format(L"%s no such action", szName);
		CProjectLog::Write(str);

		return false;

	}

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

} //CrPut::Init()


UINT CrPut::Go(void)
{
	//call pl script

	AfxMessageBox(L"CrPut::Go()");

	return ERROR_SUCCESS;

} //CrPut::Go


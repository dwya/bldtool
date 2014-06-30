#include "StdAfx.h"
#include "CrFile.h"

CrFile::CrFile(void)
{
}

CrFile::~CrFile(void)
{
}

bool CrFile::Init(LPCTSTR szName)
{
	CString str;
	str.Format(L"%s[@%s = '%s']", 
			   XPATH_ACTION_FILE, 
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

	return true;

} //CrFile::Init()


UINT CrFile::Go(void)
{
	//call pl script

	AfxMessageBox(L"CrFile::Go()");

	return ERROR_SUCCESS;

} //CrFile::Go



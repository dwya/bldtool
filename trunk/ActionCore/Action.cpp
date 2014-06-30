#include "StdAfx.h"
#include "Action.h"



CAction::CAction(void)
{
}

CAction::~CAction(void)
{
}


/**
*	\remarks	Populates rCREDITS with Credits node attributes
*
*	\arg	LPCTSTR szCredits	(name of Credits node)
*	\arg	rCREDITS* rCredits
*
*	\retval	true if Credits node was found
*
*	\details	-	if return is true, rCredits structure is populated
*
*/
bool CAction::GetCredits(__in LPCTSTR szCredits, __inout rCREDITS* rCredits)
{
	CString str;
	str.Format(L"%s[@%s = '%s']", 
			   XPATH_CREDITS, 
			   ATT_NAME, 
			   szCredits);

	if (!m_Project.Select(str))
		return false;

	rCredits->sUser		= m_Project.GetAttribute(ATT_USER);
	rCredits->sPass		= m_Project.GetAttribute(ATT_PASS);
	rCredits->sServer	= m_Project.GetAttribute(ATT_SERVER);

	return true;

} //CAction::GetCredits()


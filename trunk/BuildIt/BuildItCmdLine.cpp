// BuildItCmdLine.cpp : implementation file
//

#include "stdafx.h"
#include "BuildItCmdLine.h"


// CBuildItCmdLine

CBuildItCmdLine::CBuildItCmdLine()
: m_strCurrentFlag(_T(""))
, m_strFullCmdLine(_T(""))
{
}

CBuildItCmdLine::~CBuildItCmdLine()
{
}


// CBuildItCmdLine member functions

/**
*	\remarks	Parses command line parameter
*
*	\retval		true (always, no need for anything else)
*
*	\details	-	log file;
					/l "C:\logfile.txt"
					/log "C:\logfile.txt"

				-	every thing else is take as a property
					MyProperty=My value
*
*/
bool CBuildItCmdLine::ParseParam(const TCHAR* pszParam, bool bFlag, bool bLast)
{
	CString strParam = pszParam;
	CString strTemp;

	if (bFlag)
	{
		// pszParam will be just the flag name
		strParam.MakeLower();

		// new flag -- clear the current one
		m_strCurrentFlag = L"";
		
		////////////////////////////////////////////////////////////
		//	[log file]
		if ( L"log" == strParam ||
			 L"l" == strParam )
		{
			m_strCurrentFlag = L"log";	//next param should be log file
		}
		
		////////////////////////////////////////////////////////////
		//	unknown flag
		if ( (L"?" == strParam) ||
			 (L"help" == strParam) )
		{
			//nothing hooked up yet
		}

		if (L"showmsg" == strParam)
			CProperties::Set(rShowDisplayMsg, true);

	}

	else
	{
		if (L"logfile" == m_strCurrentFlag)
		{
			CProperties::Set(rLogFile, strParam);
		}

		if (L"" == m_strCurrentFlag)
		{
			// setting a a property
			CString strProp		= L"";
			CString strValue	= L"";

			INT iStart = strParam.Find(L"=");
			if (iStart > 1)
			{
				strProp	= strParam.Left(iStart);
				strValue = strParam.Mid(iStart + 1);

				if (strProp != L"" && strValue != L"")
				{
					CProperties::Set(strProp, strValue);
				}

			} //if (iStart > 1)
		}//checking ... m_strCurrentFlag

		m_strCurrentFlag = L"";	//reset for next loop

	} //if (bFlag)

	return true;

} //CBuildItCmdLine::ParseParam()



/**
*	\remarks	Provides a way to build the full command line so, we
*				can write it to the log file
*/
void CBuildItCmdLine::AppendToFullCmdLine(LPCTSTR szValue)
{
	if (L"" == m_strFullCmdLine)
		m_strFullCmdLine = szValue;
	else
		m_strFullCmdLine.AppendFormat(L" %s", szValue);

}

/**
*	\remarks	Returns full command line
*/
CString CBuildItCmdLine::GetFullCmdLine(void)
{
	return m_strFullCmdLine;
}


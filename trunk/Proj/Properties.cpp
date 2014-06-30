#include "StdAfx.h"
#include "Properties.h"
#include "ProjectCreator.h"
#include "ProjDefines.h"

CComPtr<IrBldProperties> g_pProperties;

CProperties::CProperties(void)
{
}

CProperties::~CProperties(void)
{
}


/**
*	\remarks	Init the properties
*/
bool CProperties::Init(void)
{
	if (g_pProperties.p)
		return true;

	bool b = false;

	if (S_OK == ::CoCreateInstance(CLSID_rBldProperties, NULL, 
								   CLSCTX_INPROC_SERVER, IID_IrBldProperties,
								   (void**)&g_pProperties))
	{
		CString sId;
		sId.Format(ROTID_PROPERTIES, CProjectCreator::GetProjectId());

		LONG lResult = 0;
		g_pProperties->Init(sId.AllocSysString(), &lResult);
	
		if (S_OK == lResult)
			b = true;

	}

	return b;

} //CProperties::Init()


void CProperties::UnInit(void)
{
	if (g_pProperties.p)
	{
		g_pProperties->UnInit();
	}

	g_pProperties = NULL;

} //CProperties::UnInit()


/**
*	\remarks	Get as string
*/
CString CProperties::Get(LPCTSTR szProperty)
{
	CString sReturn = L"";
	
	if (Init())
	{
		BSTR bstr;
		g_pProperties->GetProperty(::SysAllocString(szProperty), &bstr);

		sReturn = bstr;
		::SysFreeString(bstr);
	}

	return sReturn;

}



/**
*	\remarks	Get as int
*/
int CProperties::GetInt(LPCTSTR szProperty)
{
	return ::_tstoi(CProperties::Get(szProperty));
}



/**
*	\remarks	Get as bool 0 || empty is false, else true
*/
bool CProperties::GetBool(LPCTSTR szProperty)
{
	CString str = CProperties::Get(szProperty);

	if (EMPTY == str || ZERO == str)
		return false;

	return true;

}



/**
*	\remarks	Set as string
*/
void CProperties::Set(LPCTSTR szProperty, LPCTSTR szValue)
{
	if (Init())
	{
		//VARIANT_BOOL bSuccess = VARIANT_FALSE;

		g_pProperties->SetProperty(::SysAllocString(szProperty), 
								   ::SysAllocString(szValue));
	}

}



/**
*	\remarks	Set as int
*/
void CProperties::Set(LPCTSTR szProperty, int iValue)
{
	CString str;
	str.Format(L"%d");
	CProperties::Set(szProperty, str);
}


/**
*	\remarks	Set as bool false = "0", true = "1"
*/
void CProperties::Set(LPCTSTR szProperty, bool bValue)
{
	CString str = ZERO;

	if (bValue)
		str = ONE;

	CProperties::Set(szProperty, str);
}


/**
* \arg	[in]	LPCTSTR szCondition	(condition to evaluate)
*
* \retval bool (true/false)
*
* \remarks	Evaluate simple property conditions
*
* \details
*	-	Supports comparing simple property conditions example: REMOVE=ALL
*	-	To invert the condtion use ! (for NOT example: !REMOVE=ALL)
*	-	Single property conditions are treated as true if the property
*		has any value example: REMOVE  -- is true if property REMOVE
*		has any value at all
*	-	NOTE:	- The value for should not have quotes around it
*				Correct:	REMOVE=ALL 
*				In-correct:	REMOVE='ALL'
*	-	If szCondition is an empty string, EvaluateCondition() returns true
*
* \todo
*	FUTRUE:	-	Add support for complex conditions
*				example:	((NOT This) AND (THISPROPERTY='A string value'))
*/
bool CProperties::EvaluateCondition(LPCTSTR szCondition)
{
	CString sCondition = szCondition;

	bool bInvert = false;
	if (L'!' == sCondition[0])
	{
		bInvert = true;
	}

	sCondition.TrimLeft(L'!');

	if ( (L"" == sCondition) ||
		 (L"1" == sCondition) )
	{
		if (!bInvert)
		{
			return true; //no condition, is treated as true
		}
		else
		{
			return false;
		}
	}

	bool bReturn		= false;

	CString sProp		= L"";
	CString sValue		= L"";
	CString sPropValue	= L"";

	int iPos = sCondition.Find(_T('='));
	if (iPos > 0)
	{
		sProp	= sCondition.Left(iPos);
		sValue	= sCondition.Mid(iPos + 1);
		sValue.Trim();
		sProp.Trim();

		sPropValue = CProperties::Get(sProp);

		if (sPropValue.CompareNoCase(sValue) == 0)
		{
			bReturn = true;
		}
	}
	else
	{
		//no equal (=) sign, just see if the property is defined
		if (_T("") != CProperties::Get(sCondition))
		{
			bReturn = true;
		}

	} //if (iPos > 0)

	if (bInvert)
	{
		bReturn = !bReturn;
	}

	return bReturn;

} //CUtlXProperty::EvaluateCondition



/**
*	\remarks	Resolves properties found in string
*
*	\arg	__in LPCTSTR szString
*
*	\retval	CString (szString with resolved properties)
*
*	\details	-	searches szString for [property] and replaces it with it's value
*
*	\todo	-	Expand environment variables like %path%
*/
CString CProperties::ResolveProperties(LPCTSTR szString)
{
	CString sPlaceholder, sProp, sValue;
	CString sReturn(szString);

	int iStart	= 0;
	int iEnd	= 0;
	int iLength	= 1;
	bool bDone = false;
	while (!bDone)
	{
		iStart	= sReturn.Find(L"[") + 1;
		iEnd	= sReturn.Find(L"]");
		if (!iStart)
		{
			bDone = true;
			break;
		}
		
		iLength	= iEnd - iStart;
		sProp = sReturn.Mid(iStart, iLength);

		sPlaceholder.Format(L"[%s]", sProp);

		sValue = CProperties::Get(sProp);
		
		sReturn.Replace(sPlaceholder, sValue);
	}

	return sReturn;

} //CProperties::ResolveProperties()



/**
*	\remarks	Init standard 'RED' properties avaliable to all builds
*
*	\todo	-	Bring environment variables into properties
*/
UINT CProperties::InitEvironmentProperties(void)
{
	CRedPath path;

	//set folder paths
	if (path._SetPath(CSIDL_COMMON_ADMINTOOLS))
		CProperties::Set(L"AdminToolsFolder", path.GetString());

	if (path._SetPath(CSIDL_APPDATA))
		CProperties::Set(L"AppDataFolder", path.GetString());

	if (path._SetPath(CSIDL_COMMON_APPDATA))
		CProperties::Set(L"CommonAppDataFolder", path.GetString());

	if (path._SetPath(CSIDL_PROGRAM_FILES_COMMON))
		CProperties::Set(L"CommonFilesFolder", path.GetString());

	if (path._SetPath(CSIDL_COMMON_DESKTOPDIRECTORY))
		CProperties::Set(L"DesktopFolder", path.GetString());

	if (path._SetPath(CSIDL_FAVORITES))
		CProperties::Set(L"FavoritesFolder", path.GetString());

	if (path._SetPath(CSIDL_FONTS))
		CProperties::Set(L"FontsFolder", path.GetString());

	if (path._SetPath(CSIDL_LOCAL_APPDATA))
		CProperties::Set(L"LocalAppDataFolder", path.GetString());

	if (path._SetPath(CSIDL_MYPICTURES))
		CProperties::Set(L"MyPicturesFolder", path.GetString());

	if (path._SetPath(CSIDL_PERSONAL))
		CProperties::Set(L"PersonalFolder", path.GetString());

	if (path._SetPath(CSIDL_PROGRAM_FILES))
		CProperties::Set(L"ProgramFilesFolder", path.GetString());

	if (path._SetPath(CSIDL_COMMON_PROGRAMS))
		CProperties::Set(L"ProgramMenuFolder", path.GetString());

	if (path._SetPath(CSIDL_SENDTO))
		CProperties::Set(L"SendToFolder", path.GetString());
	
	if (path._SetPath(CSIDL_COMMON_STARTMENU))
		CProperties::Set(L"StartMenuFolder", path.GetString());

	if (path._SetPath(CSIDL_COMMON_STARTUP))
		CProperties::Set(L"StartupFolder", path.GetString());

	if (path._SetPath(CSIDL_SYSTEM))
		CProperties::Set(L"SystemFolder", path.GetString());

	if (path._SetPath(CSIDL_COMMON_TEMPLATES))
		CProperties::Set(L"TemplateFolder", path.GetString());

	if (path._SetPath(CSIDL_WINDOWS))
		CProperties::Set(L"WindowsFolder", path.GetString());
	
	if (path._SetPathTempFolder())
		CProperties::Set(L"TempFolder", path.GetString());

	//set script Interpreter properties
	/*
	Interpreter.pl
	Interpreter.bat
	Interpreter.cmd
	Interpreter.vbs
	*/

	return ERROR_SUCCESS;	//included for furture use, no need
							//to fail anything, the properties is
							//either set (usable) or not which may
							//fail an action

} //CProperties::InitEvironmentProperties()


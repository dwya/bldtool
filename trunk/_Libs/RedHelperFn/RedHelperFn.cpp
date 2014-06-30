#include "StdAfx.h"
#include "RedHelperFn.h"
#include "RedPath.h"

CRedHelperFn::CRedHelperFn(void)
{
}

CRedHelperFn::~CRedHelperFn(void)
{
}

CString CRedHelperFn::CreateGUID(void)
{
	CString sReturn	=	_T("00000000-0000-0000-0000-000000000000");
	GUID pGuid		=	{0x00};
	unsigned short* pStrUuid = NULL;

	if (CoCreateGuid(&pGuid) == S_OK)
	{
		if (UuidToString(&pGuid, &pStrUuid) == RPC_S_OK)
		{
			sReturn = (TCHAR*)(pStrUuid);
			sReturn.MakeUpper();
			RpcStringFree(&pStrUuid);
		}

	} // create GUID

	return sReturn;

}

/**
* \arg [in] DWORD dwErrCode
* \arg [in]	LPCTSTR szModule = L"" (optional)
*
* \retval CString (error text associated with dwErrCode
*
* \remarks
*	-	If szModule is a path to a .dll, we try to get the
*		text from it.  Otherwise GetErrorMsg() falls back
*		to ::FormatMessage
*
* \todo	- Future: localize according to install language (instead of OS)
*		- Future: see if you can add MUI support
*
*/
CString CRedHelperFn::GetErrorMsg(DWORD dwErrCode, LPCTSTR szModule /*= L""*/ )
{
	CString sReturn = L"";
	LPVOID lpMsgBuf;
	DWORD dwResult = ERROR_SUCCESS;

	CString sModule = szModule;
	if (L"" != sModule)
	{
		//look in the module for the string
		HMODULE hDLL = ::LoadLibrary(sModule);

		if (hDLL)
		{
			sReturn.LoadStringW(hDLL, dwErrCode);

			::FreeLibrary(hDLL);
		}
	}

	if (L"" == sReturn)
	{
		//use system error message
		if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM | 
							FORMAT_MESSAGE_IGNORE_INSERTS, 
							NULL, 
							dwErrCode, 
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
							(LPTSTR) &lpMsgBuf, 
							0, 
							NULL))
		{
			sReturn.Format(L"%s", lpMsgBuf);
			LocalFree(lpMsgBuf);
		}
	}

	if (L"" == sReturn)
	{
		//still no error code, just return the number
		sReturn.Format(L"%d", dwErrCode);
	}
		
	return sReturn;

} //static CRedHelperFn::GetErrorMsg


/**
*	\arg CString strFormat
*
*	\retval	Formatted time string
*
*	\todo
*	- move format into a define? finialize what you want to do here
*	- same function is in CUtlLogIt
*
*/
CString CRedHelperFn::GetTime(CString strFormat /*= L"%#H:%#M:%#S:"*/)
{
	CTime t = CTime::GetCurrentTime();
	return t.Format(strFormat);

} //CRedHelperFn::GetTime()


/**
*	\arg __in LPCTSTR szExe	(must be full path)
*	\arg __in LPCTSTR szCmdLine
*
*	\retval	value from executable or error from CreateProcess
*
*	\details	-	Uses CreateProcess and wait for the application
*					to finish
*
*/
UINT CRedHelperFn::LaunchExecutableAndWait( __in LPCTSTR szExe,
											__in LPCTSTR szCmdLine )
{
	UINT uiReturn = ERROR_SUCCESS;

	CRedPath exe_dir = szExe;
	exe_dir._LopOffLastItem();

	CString sCmd = szCmdLine;
	sCmd.TrimLeft();

	//full command line to be passed
	CString strCmdLine;
	strCmdLine.Format(L"\"%s\" %s", szExe, sCmd);
	
	STARTUPINFO	siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	::memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	::memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);
	
	if(::CreateProcess(NULL, strCmdLine.GetBuffer(), 0, 0, FALSE, CREATE_DEFAULT_ERROR_MODE, 0, 
							exe_dir.GetString(), &siStartupInfo, &piProcessInfo))
	{
		strCmdLine.ReleaseBuffer();

		::WaitForSingleObject(piProcessInfo.hProcess, INFINITE);

		DWORD dwResult = 0;
		::GetExitCodeProcess(piProcessInfo.hProcess, &dwResult);
		if (IDOK == dwResult)
			dwResult = ERROR_SUCCESS;	//we treat IDOK (1) the same as ERROR_SUCCESS (0)

		uiReturn = (UINT) dwResult;

		::CloseHandle(piProcessInfo.hThread);
		::CloseHandle(piProcessInfo.hProcess);
	}
	else
	{
		uiReturn = (UINT)::GetLastError();
	}

	return uiReturn;

} //CRedHelperFn::LaunchExecutableAndWait()


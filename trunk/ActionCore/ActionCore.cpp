// ActionCore.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ActionCore.h"

#include "CrRun.h"
#include "CrPut.h"
#include "CrFile.h"
#include "CrGet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
	}

	return nRetCode;
}


/**
*	\remarks
*/
UINT __stdcall run_get(LPCTSTR szName)
{
	/*
	
	.\Script\Get.pl "type" "src" "dest" "options" "User" "Pass"

	-- these ones are options, script will handle them
	*options
	*user		
	*pass

	-- Script	0	- success
				<>	- http://msdn.microsoft.com/en-us/library/ms681382(VS.85).aspx
					- becomes error code

	*/

	CrGet get;
	
	if (get.Init(szName))
		return get.Go();

	return ERROR_INVALID_FUNCTION;

} //UINT __stdcall run_get(LPCTSTR szName)



/**
*	\remarks
*/
UINT __stdcall run_put(LPCTSTR szName)
{
	CrPut put;
	
	if (put.Init(szName))
		return put.Go();

	return ERROR_INVALID_FUNCTION;

} //UINT __stdcall run_put(LPCTSTR szName)



/**
*	\remarks
*/
UINT __stdcall run_file(LPCTSTR szName)
{
	/*
	
	.\Script\File.pl "type" "src" "dest" "options"

	-- these ones are options, script will handle them
	*options
	*dest might not be there (delete file)

	-- Script	0	- success
				<>	- http://msdn.microsoft.com/en-us/library/ms681382(VS.85).aspx
					- becomes error code

	*/

	CrFile file;
	
	if (file.Init(szName))
		return file.Go();

	return ERROR_INVALID_FUNCTION;

} //UINT __stdcall run_file(LPCTSTR szName)



/**
*	\remarks	Not in use yet
*/
UINT __stdcall run_test(LPCTSTR szName)
{
	return ERROR_SUCCESS;

} //UINT __stdcall run_test(LPCTSTR szName)



/**
*	\remarks	Action type="run"
*/
UINT __stdcall run_run(LPCTSTR szName)
{
	CrRun run;
	
	if (run.Init(szName))
		return run.Go();

	return ERROR_INVALID_FUNCTION;

} //UINT __stdcall run_run(LPCTSTR szName)



/**
*	\remarks	Not in use yet
*/
UINT __stdcall run_config(LPCTSTR szName)
{
	return ERROR_SUCCESS;

} //UINT __stdcall run_config(LPCTSTR szName)



/**
*	\remarks
*/
UINT __stdcall run_notify(LPCTSTR szName)
{
	//not implemented yet

	return ERROR_SUCCESS;

} //UINT __stdcall run_notify(LPCTSTR szName)



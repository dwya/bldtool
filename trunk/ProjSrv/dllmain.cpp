// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ProjSrv_i.h"
#include "dllmain.h"

CProjSrvModule _AtlModule;

class CProjSrvApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CProjSrvApp, CWinApp)
END_MESSAGE_MAP()

CProjSrvApp theApp;

BOOL CProjSrvApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CProjSrvApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

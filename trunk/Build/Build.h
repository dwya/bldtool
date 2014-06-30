// Build.h : main header file for the Build DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "BuildDefines.h"
#include "BldActionManagerDlg.h"


typedef UINT (__stdcall *RUNBUILD)(void);

// CBuildApp
// See Build.cpp for the implementation of this class
//

class CBuildApp : public CWinApp
{
public:
	CBuildApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

public:
	//exported function that starts the build
	static UINT RunBuild(void);
	
	UINT IsBuildRunning(void);
	CString m_sMutexName;
	HANDLE m_hMutex;
	virtual int ExitInstance();
};

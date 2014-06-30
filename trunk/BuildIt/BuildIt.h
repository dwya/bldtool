// BuildIt.h : main header file for the PROJECT_NAME application
//

#pragma once

#include "BuildItCmdLine.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBuildItApp:
// See BuildIt.cpp for the implementation of this class
//

/**	\class CBuildItApp
*
*	\brief	BuildIt.exe application
*
*	\details
*	-	Entry point for build
*	-	Creates xml objects, parses command line, and starts build.dll
*
*/
class CBuildItApp : public CWinApp
{
public:
	CBuildItApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	UINT InitProject(void);
	bool ParseCommandLine(CBuildItCmdLine& rCmdInfo);

private:
	UINT m_uiReturn;

}; //class CBuildItApp


extern CBuildItApp theApp;
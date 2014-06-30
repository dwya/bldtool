// BuildIt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BuildIt.h"
#include "BuildItDlg.h"

#include "..\build\build.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBuildItApp

BEGIN_MESSAGE_MAP(CBuildItApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBuildItApp construction

CBuildItApp::CBuildItApp()
: m_uiReturn(ERROR_INVALID_FUNCTION)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBuildItApp object

CBuildItApp theApp;


// CBuildItApp initialization


/**
*	\remarks	-	BuildIt.exe is a stub that loads the project file,
*					parses the command line and calls Build.dll
*
*	\todo
	-	icon
	-	manifest file
	-	product versioning (resources)
	-	help file (command line and project file)
		-	document what attributes are 'resolveable' in the attributes
	-	function that post messages out to an external ui handler
	-	add progress plus, cancel && pause buttons
	-	command line
		- help
		- sequence xml
		- sequence name
		- sequence index (start at)
		- sequence index(s) (run 1, 5, 7)
		- turn off an action type (like; /noget /noput /nonotify)
	-	CProjectLog::Init() move log file into, ProjSrv so it can be exported to scripts
	-	If build.dll doesn't load then, we get a com error when uninit is called, not a big issue but should be fixed
*/
BOOL CBuildItApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CString str, strTemp;

	m_uiReturn = InitProject();

	if (ERROR_SUCCESS != m_uiReturn)
	{
		strTemp.LoadString(IDS_NO_XML_FILE);
		str.Format(strTemp, CRedHelperFn::GetErrorMsg(m_uiReturn));
		AfxMessageBox(str);

		return FALSE;
	}

	//now that we have proj, we have properties
	//parse the rest of the command line
	CBuildItCmdLine cmdInfo;
	if (!ParseCommandLine(cmdInfo))
	{
		str.LoadString(IDS_NO_CMDLINE_PARSE);
		AfxMessageBox(str);

		m_uiReturn = ERROR_INVALID_FUNCTION;
		return FALSE;
	}

	CProjectLog::Init();
	str.Format(L"command line: %s",
			   cmdInfo.GetFullCmdLine());
	CProjectLog::Write(str);

	//Load build.dll and run
	HMODULE hDll = ::AfxLoadLibrary(BUILD_DLL);
	if (hDll)
	{
		RUNBUILD pfn = (RUNBUILD)::GetProcAddress(hDll, "RunBuild");
		if (pfn)
			m_uiReturn = pfn();

		::AfxFreeLibrary(hDll);
	}
	else
	{
		m_uiReturn = ::GetLastError();
		strTemp.LoadString(IDS_NO_BUILD_DLL);
		str.Format(strTemp, CRedHelperFn::GetErrorMsg(m_uiReturn));
		AfxMessageBox(str);
	}

	CProjectCreator::SaveUm();
	CProjectCreator::UnInit();

	return FALSE;	//program done,
					//no need for dialog or modal loop

} //CBuildItApp::InitInstance()


/**
*	\remarks	Init proj.dll
*
*	\details	-	The .xml file should be the first command line parameter
*					after the name of the (BuildIt.exe MyXml.xml)
*
*	\todo	-	CProjectCreator::Init() may need to be tweaked as additional command lines are added
*			-	Determine if __targv[0] is always the BuildIt.exe, can it be something different if
*				BuildIt.exe is called via a script or CreateProcess()?
*
*/
UINT CBuildItApp::InitProject(void)
{
	if (__argc <= 1)
		return ERROR_FILE_NOT_FOUND;

	CRedPath buildIt = __targv[0];	//full path to buildit.exe
	CRedPath xmlFile = __targv[1];	//xml file, assume full path

	buildIt._LopOffLastItem();

	if (!xmlFile._Is(eEXISTS))
	{
		//see if it is in the same folder
		xmlFile = buildIt._GetPathAsFolder();
		xmlFile._AddPath(__targv[1]);

		if (!xmlFile._Is(eEXISTS))
		{
			return ERROR_FILE_NOT_FOUND;
		}
	}

	//create project structures
	UINT uiReturn = (UINT) CProjectCreator::Init(xmlFile.GetString());

	if (S_OK == uiReturn)
	{
		CRedPath path = xmlFile;
		path._LopOffLastItem();

		CProperties::Set(rProductFolder, buildIt._GetPathAsFolder());
		CProperties::Set(rProjectFolder, path._GetPathAsFolder());
	}

	return uiReturn;
	

} //CBuildItApp::InitProject()


int CBuildItApp::ExitInstance()
{
	CWinApp::ExitInstance();

	return m_uiReturn;

}



/**
*	\remarks	Loops the command line parameters
*
*/
bool CBuildItApp::ParseCommandLine(CBuildItCmdLine& rCmdInfo)
{
	bool bReturn = true;

	//BuildIt.exe terminates after CBuildItApp::InitProject() if __argc < 2
	rCmdInfo.AppendToFullCmdLine(__targv[0]);	//buildit.exe
	rCmdInfo.AppendToFullCmdLine(__targv[1]);	//xml file

	//loop starts on second parameter, the first one is
	//the .xml file (already determined in InitProject())
	for (int i = 2; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];

		rCmdInfo.AppendToFullCmdLine(pszParam);

		bool bFlag = FALSE;
		bool bLast = ((i + 1) == __argc);
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			bFlag = TRUE;
			++pszParam;
		}
		
		if (!rCmdInfo.ParseParam(pszParam, bFlag, bLast))
		{
			bReturn = false;
			break;
		}
	}

	return bReturn;

} //CBuildItApp::ParseCommandLine


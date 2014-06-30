// Build.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Build.h"

#include "BldSequencer.h"
#include "BldActionManagerDlgDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CBuildApp

BEGIN_MESSAGE_MAP(CBuildApp, CWinApp)
END_MESSAGE_MAP()


// CBuildApp construction

CBuildApp::CBuildApp()
: m_sMutexName(_T(""))
, m_hMutex(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBuildApp object

CBuildApp theApp;


// CBuildApp initialization

BOOL CBuildApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
 
} //CBuildApp::InitInstance()



/**
*	\remarks	Entry point for build
*
*	\details	-	Initates the sequencer
*				-	proj.dll (project files) must be initalized
*					before calling
*/
UINT CBuildApp::RunBuild(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hrInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	DWORD dwReturn = theApp.IsBuildRunning();
	if (ERROR_SUCCESS != dwReturn)
		return dwReturn;

	CProperties::InitEvironmentProperties();

	//create 'thread manager' dialog
	CBldActionManagerDlg *pDlg = new CBldActionManagerDlg();
	pDlg->Init();
	pDlg->Create(IDD_THREADMANAGER_DLG);
	
	//create a sequencer and run it
	CBldSequencer sequencer;
	sequencer.Init( pDlg->GetSafeHwnd(), 
					pDlg->GetActionsCompleteHandle(),
					pDlg->GetThreadMapFullHandle() );

	CWinThread *pSequenceThread = ::AfxBeginThread( CBldSequencer::RunSequencer, 
													&sequencer, 0, 0, 
													CREATE_SUSPENDED, 0 );
	pSequenceThread->m_bAutoDelete = false;
	pSequenceThread->ResumeThread();

	//waiting for sequencer to finish
	pDlg->RunModalLoop();

	//get return code from sequencer
	dwReturn = STILL_ACTIVE;
	do
	{
		::Sleep(0);
		::GetExitCodeThread(pSequenceThread->m_hThread, &dwReturn);

	} while (STILL_ACTIVE == dwReturn);

	//clean up
	delete pSequenceThread;
	delete pDlg;

	if (S_OK == hrInit)
		::CoUninitialize();

	return (UINT)dwReturn;

} //static CBuildApp::RunBuild()


/**
*	\remarks	Set a mutex for the currently running build
*
*	\details	-	Mutex name is based on data in project file
*					this allows us to build many builds at once,
*					but only one instance of a build
*/
UINT CBuildApp::IsBuildRunning(void)
{
	CProject xml;
	
	xml.Select(XPATH_ROOT);
	CString str = xml.GetAttribute(ATT_NAME);

	str.Replace(L" ", L"");
	str.Trim();

	if (L"" == str)
	{
		str = L"Bld";
		xml.SetAttribute(ATT_NAME, str);
	}

	m_sMutexName.Format(L"%s-%s", BUILD_MUTEX, str);

	m_hMutex = ::CreateMutex(NULL, FALSE, BUILD_MUTEX);

	UINT uiReturn = ::GetLastError();

	if (!m_hMutex || ERROR_ALREADY_EXISTS == uiReturn)
	{
		str.Format(L"cannot create exists %s : %s", 
					BUILD_MUTEX,
					CRedHelperFn::GetErrorMsg(uiReturn));

		CProjectLog::Write(str);
	}
	else
	{
		uiReturn = ERROR_SUCCESS;
	}

	return uiReturn;

} //CBuildApp::IsBuildRunning()


int CBuildApp::ExitInstance()
{
	if (m_hMutex)
	{
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	return CWinApp::ExitInstance();
}

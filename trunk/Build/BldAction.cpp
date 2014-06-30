#include "StdAfx.h"
#include "BldAction.h"
#include "BldActionDllManager.h"
#include "BldActionManagerDlgDefines.h"

CBldAction::CBldAction(void)
: m_strName(_T(""))
, m_strActionName(_T(""))
, m_strType(_T(""))
, m_strCondition(_T(""))
, m_strFailAction(_T(""))
, m_bWait(true)
, m_iRetryAttempts(0)
, m_strTagName(_T(""))
, m_strEntryPoint(_T(""))
, m_strScript(_T(""))
, m_strCmdLine(_T(""))
, m_strInterpreter(_T(""))
{
}

CBldAction::~CBldAction(void)
{
	if (m_WaitEvent)
		delete m_WaitEvent;
}



/**
*	\remarks	-	Entry point for thread
*
*	\retval		-	IDOK			(no error, nothing was done, condition false)
*				-	ERROR_SUCCESS	(did action successfully)
*
*/
UINT CBldAction::RunAction(LPVOID pParam)
{
	UINT uiReturn = ERROR_SUCCESS;

	CBldAction* pAction = (CBldAction*)pParam;
	if (!pAction)
	{
		ASSERT(0);		//no action was passed?
		return IDOK;
	}

	HRESULT hrInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	pAction->Starting();

	if (pAction->EvaluateCondition())
	{
		uiReturn = pAction->Execute();
	}


	pAction->Done();

	delete pAction;	//pAction is created by the sequencer,
					//it might not wait for the thread so we
					//delete it here (before the thread ends)

	if (S_OK == hrInit)
		::CoUninitialize();

	return uiReturn;

} //static CBldAction::RunAction()



/**
*	\remarks	-	Called to initalize members
*
*	\arg		__out HANDLE &hWait	(allow sync with the 'sequencer')
*
*	\details	-	This is the only function called from the 'sequencer' thread
*				-	Any action defaults are set here
*				-	Do not pass any non-thread safe objects to CBldAction::Init()
*						-	NOTE: CProject is thread safe
*/
void CBldAction::Init(HWND hwndManager, HANDLE hSequencerReady, CProject XmlNode, HANDLE &hWait)
{
	m_hwndManager		= hwndManager;
	m_hSequencerReady	= hSequencerReady;

	//XmlNode is on the Sequence/Action node
	m_strName		= XmlNode.GetAttribute(ATT_NAME);
	m_strCondition	= XmlNode.GetAttribute(ATT_CONDITION);
	m_strFailAction	= XmlNode.GetAttribute(ATT_FAILACTION);
	m_strFailAction.MakeLower();
	if (L"" == m_strFailAction)
		m_strFailAction = L"retry";

	if (L"0" == XmlNode.GetAttribute(ATT_WAIT))
		m_bWait = false;
	else
		m_bWait = true;

	//Now select the action to get the rest of the data
	CString sQuery;
	sQuery.Format(L"%s[@%s=\"%s\"]",
				  XPATH_ACTIONS_ACTION, 
				  ATT_NAME, 
				  m_strName);
	
	CProject xmlActions;
	if (xmlActions.Select(sQuery))
	{
		m_strType = xmlActions.GetAttribute(ATT_TYPE);
		m_strType.MakeLower();

		//possible sub-method for clarity, you need to
		//populate script with whatever subactiion
		CString strScript;
		if (L"svn" == m_strType)
			strScript	= L"[scriptfolder]svn.pl";

		else if (L"ftp" == m_strType)
			strScript	= L"[dllfolder]ftp.dll";
		
		else
			strScript	= xmlActions.GetAttribute(ATT_SCRIPT);	

		m_strScript		= CProperties::ResolveProperties(strScript);

		CRedPath path = m_strScript;
		//make sure type is correct according to script
		sQuery = path.GetFileExtension();
		sQuery.MakeLower();
		if (L".exe" == sQuery ||
			L".cmd" == sQuery ||
			L".bat" == sQuery)
		{
			m_strType		= L"exe";
			m_strCmdLine	= CProperties::ResolveProperties(xmlActions.GetAttribute(ATT_CMDLINE));
		}
		else if (L".dll" == sQuery)
		{
			m_strType		= L"dll";
			m_strEntryPoint	= xmlActions.GetAttribute(ATT_ENTRYPOINT);
		}
		else
		{
			m_strType = L"script";
			sQuery.Format(L"Interpreter%s", path.GetFileExtension());
			m_strInterpreter = CProperties::ResolveProperties(CProperties::Get(sQuery));
		}
		
		

	}

	m_WaitEvent = new CEvent(FALSE, FALSE);
	hWait = m_WaitEvent->m_hObject;
	
} //CBldAction::Init()


/**
*	\remarks	-	Called to signal action is starting, it registers the thread
*					with the 'action manager dlg'
*/
void CBldAction::Starting(void)
{
	CWinThread* thisThread = ::AfxGetThread();
	
	m_strActionName.Format(L"%s.%d", m_strName, thisThread->m_nThreadID);

	CString sMsg;
	sMsg.Format(L"\nstart: %s", CRedHelperFn::GetTime());
	WriteToLog(sMsg);

	::SendMessage(m_hwndManager, THREAD_CONTROL_MSG, eActionStart, (LPARAM)thisThread);	
	::WaitForSingleObject(m_hSequencerReady, INFINITE);

	if (!m_bWait)
		m_WaitEvent->SetEvent();

} //CBldAction::Starting()



/**
*	\remarks	-	Called to signal the action (thread) is complete, it unregisters
*					the thread with the action manager dialog
*/
void CBldAction::Done(void)
{
	CString sMsg;
	sMsg.Format(L"end time : %s", CRedHelperFn::GetTime());
	WriteToLog(sMsg);

	CWinThread* thisThread = ::AfxGetThread();
	::PostMessage(m_hwndManager, THREAD_CONTROL_MSG, eActionFinish, (LPARAM)thisThread);

	if (m_bWait)
		m_WaitEvent->SetEvent();

} //CBldAction::Done()



/**
*	/remarks	
*
*	/retval		-	ERROR_SUCCESS	(sequencer keeps going)
*				-	<anything else>	(error code for build, sequencer stops)
*
*	/details	-	failaction = ignore
*						-	return ERROR_SUCCESS
*
*				-	failaction = retry
*						-	retry MAX_RETRY_ATTEMPTS times, then return
*							success || fail
*
*				-	failaction = quit
*						-	return error code directly
*
*	/note		-	modules loaded here are freed when the sequence ends
*
*/
UINT CBldAction::ExecuteDll(void)
{
	USES_CONVERSION;

	CString sMsg;
	UINT uiReturn = IDOK;

	LPCSTR szFn = T2A(m_strEntryPoint);

	HMODULE hDll = CBldActionDllManager::LoadDll(m_strScript);
	if (hDll)
	{
		ACTION_DLL_RUN pfn = (ACTION_DLL_RUN)::GetProcAddress(hDll, szFn);
		if (pfn)
		{
			uiReturn = pfn(static_cast<LPCTSTR>(m_strName));

			sMsg.Format(L"action library: '%s' : attempt : %d", 
						m_strScript, 
						m_iRetryAttempts);
			WriteToLog(sMsg);
		}

	}

	sMsg.Format(L"return value : %d : %s", 
				uiReturn,
				CRedHelperFn::GetErrorMsg(uiReturn));
	WriteToLog(sMsg);

	return uiReturn;

} //CBldAction::RunDll()



/**
*	\remarks	-	Evaluates the condition to determine if action needs to do anything
*
*	\todo		-	Add complex conditions like (this) AND (that)
*				-	Export EvaluateCondition() to scripts
*/
bool CBldAction::EvaluateCondition(void)
{
	bool bReturn = CProperties::EvaluateCondition(m_strCondition);

	CString sMsg;
	sMsg.Format(L"condition is: %d", (int)bReturn);
	WriteToLog(sMsg);

	if ((true == bReturn) && 
		(L"" == m_strName || L"" == m_strType))
	{
		sMsg.Format(L"name = '%s' type = '%s' can't complete action",
					m_strName, 
					m_strType);
		WriteToLog(sMsg);
		
		bReturn = false;

	}

	return bReturn;

} //CBldAction::EvaluateCondition()



/**
*	\remarks	-	Write to the log file
*
*	\details	-	Each line is post-fixed with an identifier so,
*					we know which action thread it came from
*
*	\todo		-	Edit this one to turn on || off different levels of logging
*
*/
void CBldAction::WriteToLog(LPCTSTR szMsg)
{
	CString sMsg;
	sMsg.Format(L"%s : %s", m_strActionName, szMsg);

	CProjectLog::Write(sMsg);

} //CBldAction::WriteToLog()


/**
*	/remarks	
*
*	/retval		-	ERROR_SUCCESS	(sequencer keeps going)
*				-	<anything else>	(error code for build, sequencer stops)
*
*	/details	-	failaction = ignore
*						-	return ERROR_SUCCESS
*
*				-	failaction = retry
*						-	retry MAX_RETRY_ATTEMPTS times, then return
*							success || fail
*
*				-	failaction = quit
*						-	return error code directly
*
*	/note		-	modules loaded here are freed when the sequence ends
*
*/
UINT CBldAction::Execute(void)
{
	CString sMsg;
	UINT uiReturn = IDOK;

	if (L"exe" == m_strType)
		uiReturn = ExecuteExe();
		
	else if (L"dll" == m_strType)
		uiReturn = ExecuteDll();
	
	else if (L"script" == m_strType)
		uiReturn = ExecuteScript();
	
	else
	{
		sMsg.Format(L"%s unknown type, no action performed", m_strName);
		WriteToLog(sMsg);
	}

	sMsg.Format(L"return value : %d : %s", 
				uiReturn,
				CRedHelperFn::GetErrorMsg(uiReturn));
	WriteToLog(sMsg);
	
	m_iRetryAttempts++;
	if (m_iRetryAttempts > MAX_RETRY_ATTEMPTS)
	{
		m_strFailAction = "quit";
		sMsg.Format(L"max attempts reached failaction moving to: %s", m_strFailAction);
		WriteToLog(sMsg);
	}

	if (ERROR_SUCCESS != uiReturn)
	{
		sMsg.Format(L"failaction : %s", m_strFailAction);
		WriteToLog(sMsg);

		if ("ignore" == m_strFailAction)
			uiReturn = ERROR_SUCCESS;

		if ("retry" == m_strFailAction)
			uiReturn = Execute();
		
		//if ("quit" == m_strFailAction)
		//	just take return code directly

	}

	return uiReturn;

} //CBldAction::Execute()


/**
*	\remarks	-	Execute exe with command line 
*/
UINT CBldAction::ExecuteExe(void)
{
	CString str;

	str.Format(L"file = %s", m_strScript);
	CProjectLog::Write(str);

	str.Format(L"cmdline = %s", m_strCmdLine);
	CProjectLog::Write(str);

	return CRedHelperFn::LaunchExecutableAndWait(m_strScript, m_strCmdLine);

} //CBldAction::ExecuteExe()


/**
*	\remarks	-	Execute a script
*
*	\todo	-	validate the interpreter exists and is valid, write log message if it is not
*			-	find a way to always hide message boxes and ui from LaunchExecutableAndWait()
*				-	example; file not found message
*
*/
UINT CBldAction::ExecuteScript(void)
{
	CString str;

	m_strCmdLine.Format(L"\"%s\" \"%s\" \"%s\"", 
						m_strScript, 
						CProjectCreator::GetProjectId(), 
						m_strName);

	str.Format(L"cmdline = %s", m_strCmdLine);
	CProjectLog::Write(str);

	return CRedHelperFn::LaunchExecutableAndWait(m_strInterpreter, m_strCmdLine);

} //CBldAction::ExecuteScript

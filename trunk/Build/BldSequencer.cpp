#include "StdAfx.h"

#include "BldSequencer.h"
#include "BldAction.h"
#include "BldActionDllManager.h"
#include "BldActionManagerDlgDefines.h"

CBldSequencer::CBldSequencer(void)
: m_hWnd(NULL)
, m_hThreadsComplete(NULL)
, m_hThreadMapCount(NULL)
, m_brShowDisplayMsg(false)
{
}

CBldSequencer::~CBldSequencer(void)
{
}


/**
*	\remarks	Loops the sequence are routes each action
*
*	\details	- 'Main' entry point for sequence
*
*	\todo	-	Make CProjSequence class, the sequencer will
*				create a sequence object and run it from there
*			-	100% sure if the sequencer is getting the right return code
*				possible sequencing issue
*/
UINT CBldSequencer::RunSequencer(LPVOID pParam)
{
	CBldSequencer* pSequencer = (CBldSequencer*)pParam;
	if (!pSequencer)
	{
		ASSERT(0);	//no sequencer passed
		return IDOK;
	}

	UINT uiReturn = IDOK;

	//new thread start, initialize com
	HRESULT hrInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	CProject nodes(XML_SEQUENCE);
	long lActionCnt = nodes.Select(XPATH_SEQUENCE_ACTION);

	pSequencer->Starting(lActionCnt);

	//lActionCnt = 0;
	if (lActionCnt <= 0)
	{
		nodes.UnInit();
		pSequencer->Done();
		
		return IDOK;	//nothing to do, no actions in sequence
	}

//	CProject nodes(XML_SEQUENCE);

	CString str;
	do
	{
		str = nodes.GetAttribute(L"name");
		pSequencer->RouteIt(nodes);
		
		uiReturn = pSequencer->GetStatus();
		if (ERROR_SUCCESS != uiReturn)
		{
			break;	//don't do anymore actions, quit sequencer
		}
	
	//} while (0);
	} while(nodes.CounterNext());

	nodes.UnInit();
	pSequencer->Done();
	

	if (S_OK == hrInit)
		::CoUninitialize();

	return uiReturn;

} //static CBldSequencer::RunSequencer()



/**
*	\remarks	Call to initalize sequencer
*
*	\details	-	Called by main build thread (the one the dialog is on)
*/
void CBldSequencer::Init(HWND hwndManagerDlg, HANDLE hWaitHandle, HANDLE hThreadMapCount)
{
	m_hWnd				= hwndManagerDlg;
	m_hThreadsComplete	= hWaitHandle;
	m_hThreadMapCount	= hThreadMapCount;
	m_brShowDisplayMsg	= CProperties::GetBool(rShowDisplayMsg);

} //CBldSequencer::Init



/**
*	\remarks	Call to initate start of sequence
*
*	\details	-	Waits for CBldActionManagerDlg to be ready
*				-	Tells dlg how many actions are in the sequence
*/
void CBldSequencer::Starting(long lNumberOfActions)
{
	WriteToLog(L"==================================================================================");
	CString sMsg;
	sMsg.Format(L"start : %s : actions to process : %d \n", 
				CRedHelperFn::GetTime(),
				lNumberOfActions);
	WriteToLog(sMsg);

	::SendMessage(m_hWnd, THREAD_CONTROL_MSG, eSequenceStart, lNumberOfActions);

} //CBldSequencer::Starting()



/**
*	\remarks	Finishes the sequence
*/
void CBldSequencer::Done(void)
{
	//sequence is finished but, there may be threads (actions) still
	//running, wait here for them to finish
	//::WaitForSingleObject(m_hThreadsComplete, INFINITE);
	
	CString sMsg;
	sMsg.Format(L"\nend : %s", CRedHelperFn::GetTime());
	WriteToLog(sMsg);
	WriteToLog(L"==================================================================================\n\n");

	//CBldActionDllManager::UnLoadDlls();

	::PostMessage(m_hWnd, THREAD_CONTROL_MSG, eSequenceFinish, 0);

} //CBldSequencer::Done()



/**
*	\remarks	-	Write to the log file
*
*	\details	-	Each line is post-fixed with an identifier so,
*					we know which action thread it came from
*
*	\todo		-	Edit this one to turn on || off different levels of logging
*
*/
void CBldSequencer::WriteToLog(LPCTSTR szMsg)
{
	CString sMsg;
	sMsg.Format(L"Sequencer : %s", szMsg);

	CProjectLog::Write(sMsg);

} //CBldSequencer::WriteToLog()


/**
*	\remarks	Sends the action to the correct action handler
*
*	\details	-	'special actions' performed by the sequencer
*				-	all others get routed to an action<type>.dll
*/
void CBldSequencer::RouteIt(CProject XmlNode)
{
	CString strTemp;
	CString str = XmlNode.GetAttribute(ATT_TYPE);
	str.MakeLower();

	if (L"setproperty" == str)
	{
		strTemp = XmlNode.GetAttribute(ATT_VALUE);
		str		= CProperties::ResolveProperties(strTemp);
		CProperties::Set(XmlNode.GetAttribute(ATT_NAME), str);
	}
	if (L"msg" == str)
	{
		strTemp = XmlNode.GetAttribute(ATT_VALUE);
		str		= CProperties::ResolveProperties(strTemp);
		if ( XmlNode.GetAttributeBool(ATT_DISPLAY) &&
			 m_brShowDisplayMsg)
		{
			::MessageBox(NULL, str, L"rBuidSequencer", MB_OK);
		}

		WriteToLog(str);
	}
	else
	{
		//thread the action, above are any actions
		//that shouldn't be threaded

		RunBldAction(XmlNode);
	}

	return;

} //CBldSequencer::RouteIt()



/**
*	\remarks	Creates and call the 'build action'
*
*	\remarks	-	Each one is created on a different thread
*				-	The 'build action' send messages back to the
*					CBldActionManagerDlg informing it of progress / status
*/
void CBldSequencer::RunBldAction(CProject Xml)
{
	int x = 0;
	//wait for the thread map to be ready (incase it's full)
	::WaitForSingleObject(m_hThreadMapCount, INFINITE);

	CEvent* pEvent = new CEvent(FALSE, FALSE);

	CBldAction *pAction = NULL;
	CWinThread *pThread = NULL;

	//new CBldAction created here is deleted by CBldAction::RunAction() when the thread completes
	pAction = new CBldAction();

	//bool bWait = false;
	HANDLE m_hWait = NULL;
	pAction->Init(m_hWnd,
				  pEvent->m_hObject,
				  Xml,
				  m_hWait);

	//new CWinThread created here is deleted by CBuildActionManagerDlg
	//when the thread finishes using (THREAD_CONTROL_MSG, eActionFinish)
	pThread = ::AfxBeginThread(CBldAction::RunAction, 
							   pAction, 0, 0, CREATE_SUSPENDED, 0);

	pThread->m_bAutoDelete = false;
	pThread->ResumeThread();

	pEvent->SetEvent();

	//bWait is populated by pAction->Init
	//if (bWait)
	::WaitForSingleObject(m_hWait, INFINITE);

	delete pEvent;

} //CBldSequencer::RunBldAction()


/**
*	\remarks	-	Asks the CBldActionManagerDlg for the return code of the build
*
*	\details	-	Each thread (action) sends it's return to the dialog
*/
UINT CBldSequencer::GetStatus(void)
{
	return ::SendMessage(m_hWnd, THREAD_CONTROL_MSG, eGetStatus, 0);

} //CBldSequencer::GetStatus()

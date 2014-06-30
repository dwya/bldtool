// BldActionManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Build.h"
#include "BldActionManagerDlg.h"


// CBldActionManagerDlg dialog

IMPLEMENT_DYNAMIC(CBldActionManagerDlg, CDialog)

CBldActionManagerDlg::CBldActionManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBldActionManagerDlg::IDD, pParent)
	, m_lNumberOfActionsToRun(0)
	, m_ActionsCompleteEvent(NULL)
	, m_ManagedThreadMap(NULL)
	, m_lNumberOfActionsCompleted(0)
	, m_uiReturnStatus(ERROR_SUCCESS)
{

}

CBldActionManagerDlg::~CBldActionManagerDlg()
{
	if (m_ActionsCompleteEvent)
		delete m_ActionsCompleteEvent;

	if (m_ThreadMapFullEvent)
		delete m_ThreadMapFullEvent;

	if (m_ManagedThreadMap)
		delete m_ManagedThreadMap;
}

void CBldActionManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBldActionManagerDlg, CDialog)
		ON_REGISTERED_MESSAGE(THREAD_CONTROL_MSG, OnThreadControlMsg)

END_MESSAGE_MAP()


// CBldActionManagerDlg message handlers

/**
*	\remarks	THREAD_CONTROL_MSG message handler
*
*/
afx_msg LRESULT CBldActionManagerDlg::OnThreadControlMsg(WPARAM wParam, LPARAM lParam)
{
	switch ((THREAD_CONTROL_MSG_FN)wParam)
	{
	case eSequenceStart:
		{
			m_lNumberOfActionsToRun = (long)lParam;
			if (!m_lNumberOfActionsToRun)
				m_ActionsCompleteEvent->SetEvent();

		}
		break;

	case eSequenceFinish:
		{
			EndModalLoop(ERROR_SUCCESS);
		}
		break;

	case eActionStart:
		{
			return ManagedThreadMapAdd((CWinThread*) lParam);
		}
		break;

	case eActionFinish:
		{
			return ManagedThreadMapRemove((CWinThread*) lParam);
		}
		break;

	case eGetStatus:
		{
			return m_uiReturnStatus;
		}
		break;

	default:
		{
			ASSERT(0);	//wParam is incorrect
		}
		break;

	}

	return S_OK;

}



/**
*	\remarks	Initialize internal members
*/
void CBldActionManagerDlg::Init(void)
{
	m_ManagedThreadMap		= new CMapStringToPtr (MAX_RUNNING_THREADS);
	m_ActionsCompleteEvent	= new CEvent(FALSE, FALSE);
	m_ThreadMapFullEvent	= new CEvent(FALSE, TRUE);

	//map is NOT full, tell sequencer it can create threads
	m_ThreadMapFullEvent->SetEvent();

} //CBldActionManagerDlg::Init()



/**
*	\remarks	Adds a thread to the map
*
*	\details	Theads are added (key) thread id (pointer) pointer to CWinThread
*
*/
LRESULT CBldActionManagerDlg::ManagedThreadMapAdd(CWinThread* pThread)
{
	if (!m_ManagedThreadMap)
	{
		ASSERT(0);			//OnInit() must be called first,
							//no map created
		return S_FALSE;
	}

	if (!pThread)
		return S_FALSE;

	CString sThreadID;
	sThreadID.Format(L"%d", pThread->m_nThreadID);

	m_ManagedThreadMap->SetAt(static_cast<LPCTSTR>(sThreadID), pThread);

	if (m_ManagedThreadMap->GetCount() >= MAX_RUNNING_THREADS)
	{
		m_ThreadMapFullEvent->ResetEvent();		//instructs sequencer to not create
												//anymore threads, map is full
	}

	return S_OK;

} //CBldActionManagerDlg::AddToManagedThreadMap()



/**
*	\remarks	Removes thread from map
*
*	\details	-	Removes thread
*				-	Cleans up CWinThread pointer
*				-	Uses thread's return code to set the build status (success or error)
*
*/
LRESULT CBldActionManagerDlg::ManagedThreadMapRemove(CWinThread* pThread)
{
	if (!pThread)
	{
		ASSERT(0);	//no thread?
		return S_FALSE;
	}

	//get map key
	CString sId;
	sId.Format(L"%d", pThread->m_nThreadID);

	//get thread return code
	DWORD dwReturn = STILL_ACTIVE;
	do
	{
		::Sleep(0);
		::GetExitCodeThread(pThread->m_hThread, &dwReturn);

	} while (STILL_ACTIVE == dwReturn);

	//remove from map and clean up
	m_ManagedThreadMap->RemoveKey(sId);
	delete pThread;

	if (m_ManagedThreadMap->GetCount() < MAX_RUNNING_THREADS)
	{
		m_ThreadMapFullEvent->SetEvent();		//map is NOT full, tell sequencer it can create threads
	}

	//process return code, accept first fail only
	if (m_uiReturnStatus == ERROR_SUCCESS)
		m_uiReturnStatus = (UINT)dwReturn;

	if (ERROR_SUCCESS != m_uiReturnStatus)
		m_lNumberOfActionsCompleted = m_lNumberOfActionsToRun;	//terminate build

	m_lNumberOfActionsCompleted++;
	if (m_lNumberOfActionsCompleted >= m_lNumberOfActionsToRun)
	{
		m_ActionsCompleteEvent->SetEvent();		//all threads are finished
	}

	return S_OK;

} //CBldActionManagerDlg::RemoveFromManagedThreadMap()



/**
*	\remarks	Returns sync handle to determine when all actions are complete
*/
HANDLE CBldActionManagerDlg::GetActionsCompleteHandle(void)
{
	return m_ActionsCompleteEvent->m_hObject;
}


/**
*	\remarks	Returns sync handle to determine if map is full
*/
HANDLE CBldActionManagerDlg::GetThreadMapFullHandle(void)
{
	return m_ThreadMapFullEvent->m_hObject;
}






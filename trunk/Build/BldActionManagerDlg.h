#pragma once

#include "BldActionManagerDlgDefines.h"

// CBldActionManagerDlg dialog


/**	\class CBldActionManagerDlg
*
*	\brief		-	Tracks threaded actions through a map
*
*	\details	-	Pointers to threads are stored in a CMapStringToPtr
*					(key) thread id (value) pointer to CWinThread
*				-	Theads send messages here via THREAD_CONTROL_MSG message
*					see: "BldActionManagerDlgDefines.h"
*				-	This class is responisble for managing the threads
*					('thread manager dialog')
*
*/
class CBldActionManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CBldActionManagerDlg)

public:
	CBldActionManagerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBldActionManagerDlg();

// Dialog Data
	enum { IDD = IDD_THREADMANAGER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void Init(void);
	HANDLE GetActionsCompleteHandle(void);
	HANDLE GetThreadMapFullHandle(void);
	
protected:
	afx_msg LRESULT OnThreadControlMsg(WPARAM wParam, LPARAM lParam);
	LRESULT ManagedThreadMapAdd(CWinThread* pThread);
	LRESULT ManagedThreadMapRemove(CWinThread* pThread);

private:
	CMapStringToPtr* m_ManagedThreadMap;	//tracks 'action' threads
	long m_lNumberOfActionsToRun;			//number of actions scheduled
	long m_lNumberOfActionsCompleted;		//number of actions already completed
	UINT m_uiReturnStatus;					//return code at current sequence
	CEvent* m_ActionsCompleteEvent;			//notifies when m_lNumberOfActionsToRun == m_lNumberOfActionsCompleted
	CEvent* m_ThreadMapFullEvent;			//notifies if m_ManagedThreadMap is full or has room for more

}; //class CBldActionManagerDlg


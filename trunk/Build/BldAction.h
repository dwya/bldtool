#pragma once

//Action*.dll signature
typedef UINT (__stdcall *ACTION_DLL_RUN)(__in LPCTSTR szName);

//max. number of retry attempts RunActionDll() will try
#define	MAX_RETRY_ATTEMPTS	10



/**	\class CBldAction
*
*	\brief		-	Represents an action sequence
*
*	\details	-	Actions are initalized by CBldSequencer::RunSequencer()
*				-	Each action runs on a separate thread
*				-	The CBldActionManagerDlg tracks the threads through
*					window message THREAD_CONTROL_MSG
*
*/
class CBldAction
{
public:
	CBldAction(void);
	~CBldAction(void);
	
	//worker function, using AfxCreateThread()
	static UINT RunAction(LPVOID pParam);

	//call to initalize members
	void Init(__in HWND hwndManager, 
			  __in HANDLE hSequencerReady, 
			  __in CProject XmlNode, 
			  __out HANDLE &hWait);
	

protected:
	void Starting(void);
	void WaitForSequencerReady(void);
	void Done(void);
	void WriteToLog(LPCTSTR szMsg);
	bool EvaluateCondition(void);
	UINT RunDll(void);
	UINT Execute(void);
	UINT ExecuteExe(void);
	UINT ExecuteDll(void);
	UINT ExecuteScript(void);
	

private:
	HWND m_hwndManager;			//CBldActionManagerDlg handle
	HANDLE m_hSequencerReady;	//CEvent handle, notifies the Action the sequencer is ready
	CEvent *m_WaitEvent;		//CEvent tells releases calling thread

	CString m_strName;			//ATT_NAME attribute
	CString m_strType;			//ATT_TYPE attribute
	CString m_strActionName;	//[m_strName].<ThreadID>
	CString m_strCondition;		//ATT_CONDITION		(default true)
	CString m_strFailAction;	//ATT_FAILACTION	(default retry)
	bool m_bWait;				//ATT_WAIT			(default true)
	int m_iRetryAttempts;		//number of attempts tried

public:
	CString m_strTagName;
	CString m_strEntryPoint;
	CString m_strScript;
	CString m_strCmdLine;


	CString m_strInterpreter;
}; //class CBldAction



#pragma once


/**	\class CBldSequencer
*
*	\brief		-	Processes the sequence of actions
*
*	\details	-	
*
*/
class CBldSequencer
{
public:
	CBldSequencer(void);
	~CBldSequencer(void);
	
	//main loop (entry point)
	static UINT RunSequencer(LPVOID pParam);

	//get object ready for use, must be called before CBldSequencer::RunSequencer()
	void Init(HWND hwndManagerDlg, 
			  HANDLE hWaitHandle, 
			  HANDLE hThreadMapCount);
protected:
	void Starting(long lNumberOfActions);
	void Done(void);
	void WriteToLog(LPCTSTR szMsg);
	void RouteIt(CProject XmlNode);
	void RunBldAction(CProject Xml);
	UINT GetStatus(void);

private:
	HWND m_hWnd;					//handle to CBldActionManagerDlg
	HANDLE m_hThreadsComplete;		//event CBldActionManagerDlg sets when threads are complete
	HANDLE m_hThreadMapCount;		//event handle signals when CBldActionManagerDlg's map is full
	bool m_brShowDisplayMsg;
}; //class CBldSequencer


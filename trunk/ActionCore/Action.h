#pragma once



struct rCREDITS
{
	CString sUser;
	CString sPass;
	CString sServer;

}; //struct rCREDITS


/**	\class CAction
*
*	\brief CAction (ActionCore.dll) is the base class for our core actions
*
*	\details
*	-	override Init() and Go() in the base class
*/
class CAction
{
public:
	CAction(void);
	~CAction(void);

	virtual bool Init(__in LPCTSTR szName)	{ return true; }
	virtual UINT Go(void)	{ return ERROR_SUCCESS; }

	bool GetCredits(__in LPCTSTR szCredits, __inout rCREDITS* rCredits);

protected:
	CProject m_Project;

}; //class CAction



#pragma once


/**	\class CBuildItCmdLine
*
*	\brief		-	Command line handler
*
*/
class CBuildItCmdLine
{
public:
	CBuildItCmdLine();
	virtual ~CBuildItCmdLine();
	bool ParseParam(const TCHAR* pszParam, bool bFlag, bool bLast);
	void AppendToFullCmdLine(__in LPCTSTR szValue);
	CString GetFullCmdLine(void);


private:
	CString m_strCurrentFlag;
	CString m_strFullCmdLine;

}; //class CBuildItCmdLine



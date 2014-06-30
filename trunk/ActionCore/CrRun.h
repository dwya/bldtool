#pragma once

#include "action.h"


/**	\class CrRun
*
*	\brief CrRun (ActionCore.dll) runs an executable action
*
*/
class CrRun: public CAction
{
public:
	CrRun(void);
	~CrRun(void);

	virtual bool Init(__in LPCTSTR szName);
	virtual UINT Go(void);

private:
	CString m_file;
	CString m_cmdline;

}; //class CrRun


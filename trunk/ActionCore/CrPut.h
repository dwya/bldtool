#pragma once

#include "action.h"

	/*
	
	.\Script\Put.pl "type" "src" "dest" "options" "User" "Pass"

	-- these ones are options, script will handle them
	*options
	*user		
	*pass

	*** add a way to support wildcards (file names only is fine)

	-- Script	0	- success
				<>	- http://msdn.microsoft.com/en-us/library/ms681382(VS.85).aspx
					- becomes error code

	*/
/**	\class CrPut
*
*	\brief CrPut (ActionCore.dll) action to send files to source control
*
*/
class CrPut: public CAction
{
public:
	CrPut(void);
	~CrPut(void);

	virtual bool Init(__in LPCTSTR szName);
	virtual UINT Go(void);

private:
	CString m_type;
	CString m_src;
	CString m_dest;
	CString m_options;
	rCREDITS m_Credits;

}; //class CrPut




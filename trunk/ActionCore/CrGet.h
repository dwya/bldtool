#pragma once
#include "action.h"

class CrGet :
	public CAction
{
public:
	CrGet(void);
	~CrGet(void);

	virtual bool Init(__in LPCTSTR szName);
	virtual UINT Go(void);

private:
	CString m_name;
	CString m_type;
	CString m_src;
	CString m_dest;
	CString m_options;
	rCREDITS m_Credits;

};

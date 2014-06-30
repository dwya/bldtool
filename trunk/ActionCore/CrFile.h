#pragma once
#include "action.h"

class CrFile :
	public CAction
{
public:
	CrFile(void);
	~CrFile(void);

	virtual bool Init(__in LPCTSTR szName);
	virtual UINT Go(void);

private:
	CString m_name;
	CString m_type;
	CString m_src;
	CString m_dest;
	CString m_options;

};

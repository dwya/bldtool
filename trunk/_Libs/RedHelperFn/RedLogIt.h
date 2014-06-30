#pragma once

class CRedLogIt
{
public:
	CRedLogIt(void);
	~CRedLogIt(void);

public:
	static bool CRedLogIt::WriteIt(CFile* pFile, LPCTSTR szFile, LPCTSTR szText);
	static bool CRedLogIt::WriteIt(CFile* pFile, LPCTSTR szText);

}; //class CRedLogIt

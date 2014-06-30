#include "StdAfx.h"
#include "RedLogIt.h"

CRedLogIt::CRedLogIt(void)
{
}

CRedLogIt::~CRedLogIt(void)
{
}


bool CRedLogIt::WriteIt(CFile* pFile, LPCTSTR szFile, LPCTSTR szText)
{
	if (pFile->Open(szFile, 
					CFile::modeCreate|
					CFile::modeNoTruncate|
					CFile::modeWrite|
					CFile::shareDenyNone) > 0 )
	{
		return CRedLogIt::WriteIt(pFile, szText);
	}

	return false;

} //CRedLogIt::WriteIt(szFile)


bool CRedLogIt::WriteIt(CFile* pFile, LPCTSTR szText)
{
	bool b = false;

	if (pFile->m_hFile != CFile::hFileNull)
	{
		CString sData = szText;
		sData.AppendFormat(L"\r\n");

		int x = sData.GetLength();
		x = x * (sizeof(TCHAR));

		pFile->SeekToEnd();
		pFile->Write(sData, x);

		pFile->Flush();

		b = true;
	}

	return b;

} //CRedLogIt::WriteIt(pFile)


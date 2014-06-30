#pragma once

class CRedHelperFn
{
public:
	CRedHelperFn(void);
	~CRedHelperFn(void);
	
	static CString CreateGUID(void);
	static CString GetErrorMsg(DWORD dwErrCode, LPCTSTR szModule = L"");
	static CString GetTime(CString strFormat = L"%#H:%#M:%#S:");
	static UINT LaunchExecutableAndWait(__in LPCTSTR szExe,
										__in LPCTSTR szCmdLine);
};

#pragma once

class CRunningObjects
{
public:
	CRunningObjects(void);
	~CRunningObjects(void);

	static HRESULT RegisterIt(LPCTSTR szId, IUnknown* pUnknown,  DWORD& dwRegistration);
	static HRESULT UnRegisterIt(DWORD& dwRegistration);
	static HRESULT GetIt(LPCTSTR szId, IUnknown** pUnknown);
	
}; //class CRunningObjects



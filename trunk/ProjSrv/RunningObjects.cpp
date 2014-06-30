#include "StdAfx.h"
#include "RunningObjects.h"

CRunningObjects::CRunningObjects(void)
{
}

CRunningObjects::~CRunningObjects(void)
{
}

/**
*	\todo	- test
*			- Registering a second object with the same moniker, or re-registering the same object with the same moniker, creates a second entry in the ROT. In this case, Register returns MK_S_MONIKERALREADYREGISTERED. Each call to Register must be matched by a call to IRunningObjectTable::Revoke because even duplicate entries have different pdwRegister identifiers. A problem with duplicate registrations is that there is no way to determine which object will be returned if the moniker is specified in a subsequent call to IRunningObjectTable::IsRunning.
*			- build ID More or change LPCTSTR szId to bstr
*			- is ROTFLAGS_REGISTRATIONKEEPSALIVE right?
*			- not sure if IUnknown is being passed and registred correctly
*/
HRESULT CRunningObjects::RegisterIt(LPCTSTR szId, IUnknown* pUnknown, DWORD& dwRegistration)
{
	HRESULT hr	= S_OK;
	CString sId	= szId;

	CComPtr<IRunningObjectTable> spTable;

	hr = GetRunningObjectTable(0, &spTable);
	if ( SUCCEEDED(hr) )
	{
		CComPtr<IMoniker> spMoniker;
		hr = CreateItemMoniker( NULL, 
								sId.AllocSysString(), 
								&spMoniker );

		if ( SUCCEEDED(hr) )
		{
			hr = spTable->Register( ROTFLAGS_REGISTRATIONKEEPSALIVE, 
									pUnknown, 
									spMoniker, 
									&dwRegistration );

			spMoniker.Release();

		} //create Moniker
	} //GetRunningObjectTable()

	return hr;

} //CRunningObjects::RegisterIt()



/**
*	\todo	- test
*			- mimic parms for RegisterIt()
*			- not sure if IMoniker should be released, try both ways
*			- should LPCTSTR szId be a bstr?
*/
HRESULT CRunningObjects::GetIt(LPCTSTR szId, IUnknown** pUnknown)
{
	CComPtr<IRunningObjectTable> spTable;
	
	CString strName = szId;

	CComPtr<IBindCtx> spContext = NULL;
	HRESULT hr = ::CreateBindCtx( 0, &spContext ); /* Create a bind context */
	if (!spContext)
		return NULL;

	hr = spContext->GetRunningObjectTable(&spTable);

	if (SUCCEEDED(hr))
	{
		CComPtr<IMoniker> spMoniker;
		hr = CreateItemMoniker( NULL, 
								strName.AllocSysString(), 
								&spMoniker );
			
		if ( SUCCEEDED(hr) )
		{
			spTable->GetObjectW(spMoniker, pUnknown);

			spMoniker.Release();

		} //CreateItemMoniker()
	} //GetRunningObjectTable()

	spContext.Release();

	return hr;

} //CRunningObjects::GetIt()


/**
*	\todo	- test
*/
HRESULT CRunningObjects::UnRegisterIt(DWORD& dwRegistration)
{
	if (!dwRegistration)
		return S_OK;	//object is not registered, nothing to do

	
	CComPtr<IRunningObjectTable> spTable;
	HRESULT hr = GetRunningObjectTable(0, &spTable);
	
	if ( SUCCEEDED(hr) )
	{
		hr = spTable->Revoke(dwRegistration);
		dwRegistration = 0;
	}

	return hr;

} //CRunningObjects::UnRegisterIt()



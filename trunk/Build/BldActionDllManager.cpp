#include "StdAfx.h"
#include "BldActionDllManager.h"

CBldActionDllManager g_theDllManager;

CBldActionDllManager::CBldActionDllManager(void)
: m_DllMap(NULL)
, m_iLoaded(0)
{
}

CBldActionDllManager::~CBldActionDllManager(void)
{
	if (g_theDllManager.m_DllMap)
		delete g_theDllManager.m_DllMap;
}


/**
*	\remarks	Loads a .dll / stores the HMODULE in a map
*
*	\details	-	When a module (Action*.dll) is loaded it is stored in a map
*				-	Subsequent loads return the mapped pointer, instead of calling
*					LoadLibrary()
*				-	Modules stay loaded until CBldActionDllManager::UnLoadDlls()
*					is called (when the sequence is complete CBldSequencer::Done())
*/
HMODULE CBldActionDllManager::LoadDll(LPCTSTR lpszModuleName)
{
	//create map if not loaded
	if (!g_theDllManager.m_DllMap)
		g_theDllManager.m_DllMap = new CMapStringToPtr (MAX_LOADED_DLLS);

	//if no map || map is full, just use ::AfxLoadLibrary
	if (!g_theDllManager.m_DllMap ||
		 g_theDllManager.m_iLoaded >= MAX_LOADED_DLLS)
	{
		return ::AfxLoadLibrary(lpszModuleName);
	}

	HMODULE hDll = NULL;
	CString sDll = lpszModuleName;
	sDll.MakeLower();

	//if already in map, return module
	if (g_theDllManager.m_DllMap->Lookup(sDll, (void*&)hDll))
		return hDll;

	//not in map, load it and add it to map
	hDll = ::AfxLoadLibrary(sDll);
	if (hDll)
	{
		g_theDllManager.m_DllMap->SetAt(static_cast<LPCTSTR>(sDll), hDll);
		g_theDllManager.m_iLoaded++;
	}

	return hDll;

} //static CBldActionDllManager::LoadDll()


/**
*	\remarks	Unloads all the modules in the map
*
*/
void CBldActionDllManager::UnLoadDlls(void)
{
	if (!g_theDllManager.m_DllMap)
		return;

	CString sKey;
	HMODULE hDll = NULL;
	POSITION pos = g_theDllManager.m_DllMap->GetStartPosition();

	while(pos != NULL)
	{
		g_theDllManager.m_DllMap->GetNextAssoc(pos, sKey,(void*&) hDll);
		::AfxFreeLibrary(hDll);
	}
} //static CBldActionDllManager::UnLoadDlls()


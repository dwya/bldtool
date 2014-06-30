#pragma once

//number of .dlls that can be stored in the map, once
//the map is full, the module will be loaded via AfxLoadLibrary()
//each time CBldActionDllManager::LoadDll() is called -- also,
//the module will be unloaded when the process ends (not when UnLoadDlls() is called)
#define MAX_LOADED_DLLS		10


/**	\class CBldActionDllManager
*
*	\brief		-	Stores loaded action*.dlls in a map
*
*	\details	-	Pointers to threads are stored in a CMapStringToPtr
*					(key) module name (value) HMODULE
*				-	Once the .dll is loaded, we return the mapped HMODULE
*					instead of recalling AfxLoadLibrary()
*
*/
class CBldActionDllManager
{
public:
	CBldActionDllManager(void);
	~CBldActionDllManager(void);

	static HMODULE LoadDll(LPCTSTR lpszModuleName);
	static void UnLoadDlls(void);

private:
	CMapStringToPtr* m_DllMap;	//HMODULE map
	int m_iLoaded;				//number of mapped HMODULE

}; //class CBldActionDllManager


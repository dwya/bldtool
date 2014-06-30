// dllmain.h : Declaration of module class.

class CProjSrvModule : public CAtlDllModuleT< CProjSrvModule >
{
public :
	DECLARE_LIBID(LIBID_ProjSrvLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROJSRV, "{CF8B904D-74F8-4F58-8EA1-716FE71F68CE}")
};

extern class CProjSrvModule _AtlModule;

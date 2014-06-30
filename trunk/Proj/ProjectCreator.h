#pragma once

#ifdef PROJ_EXPORTS
#define PROJ_API __declspec(dllexport)
#else
#define PROJ_API __declspec(dllimport)
#endif

#include "..\ProjSrv\ProjSrv_i.h"
#include "..\ProjSrv\ProjSrvXmlMarkupDefines.h"


class PROJ_API CProjectCreator
{
public:
	CProjectCreator(void);
	~CProjectCreator(void);

public:
	static UINT Init(CRedPath projectFile);
	static void SaveUm(void);
	static void UnInit(void);
	static CString GetProjectId(void);

protected:
	UINT InitProject(CRedPath projectFile);
	UINT InitSequence(void);
	UINT InitProperties(void);
	UINT ErrorIt(HRESULT hrError);	

private:
	CComPtr<IrXmlFile> m_pProject;
	CComPtr<IrXmlFile> m_pProperties;
	CComPtr<IrXmlFile> m_pSequence;
	CRedPath m_projectFile;
	CString m_strProjectId;
	bool m_bInitCalled;
	HRESULT m_hrInit;

}; //class CProjectCreator




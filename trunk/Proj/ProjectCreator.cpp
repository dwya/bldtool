#include "StdAfx.h"
#include "ProjectCreator.h"
#include "ProjDefines.h"
#include "Properties.h"

CProjectCreator g_theProjectCreator;	//project creator object, only one per/build process


CProjectCreator::CProjectCreator(void)
: m_strProjectId(_T(""))
, m_bInitCalled(false)
, m_hrInit(S_FALSE)
{
}

CProjectCreator::~CProjectCreator(void)
{
	UnInit();
}


/**
*	\remarks
*/
UINT CProjectCreator::Init(CRedPath projectFile)
{
	if (g_theProjectCreator.m_bInitCalled)
		return E_FAIL;	//already called, nothing to do

	g_theProjectCreator.m_bInitCalled = true;

	g_theProjectCreator.m_hrInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	UINT uiResult = g_theProjectCreator.InitProject(projectFile);
	if (ERROR_SUCCESS != uiResult)
		return uiResult;

	uiResult = g_theProjectCreator.InitProperties();
	if (ERROR_SUCCESS != uiResult)
		return uiResult;

	uiResult = g_theProjectCreator.InitSequence();
	if (ERROR_SUCCESS != uiResult)
		return uiResult;

	return uiResult;

} //CProjectCreator::Init()


/**
*	\remarks	Clean up
*/
void CProjectCreator::UnInit(void)
{
	if (!g_theProjectCreator.m_bInitCalled)
		return;

	if (g_theProjectCreator.m_pProject)
	{
		g_theProjectCreator.m_pProject->UnInit();
	}

	if (g_theProjectCreator.m_pSequence)
	{
		g_theProjectCreator.m_pSequence->UnInit();
	}
	
	if (g_theProjectCreator.m_pProperties)
	{
		CProperties::UnInit();
		g_theProjectCreator.m_pProperties->UnInit();
	}

	//make sure com is uninitialized last, not before
	//the project creator objects
	if (S_OK == g_theProjectCreator.m_hrInit)
		::CoUninitialize();

	g_theProjectCreator.m_pProject		= NULL;
	g_theProjectCreator.m_pProperties	= NULL;
	g_theProjectCreator.m_pSequence		= NULL;
	g_theProjectCreator.m_bInitCalled	= false;
	g_theProjectCreator.m_hrInit		= S_FALSE;


} //CProjectCreator::UnInit


/**
*	\remarks	Save the project files out to file
*
*	\todo	-	Remove hard coded paths
*			-	Combine them back into one
*/
void CProjectCreator::SaveUm(void)
{
	if (!g_theProjectCreator.m_bInitCalled)
		return;

	long lResult = 0;
	VARIANT_BOOL bOverwrite = VARIANT_TRUE;

	CString sFile;

	if (g_theProjectCreator.m_pProject)
	{
		sFile = L"D:\\_BuildSystem\\trunk\\BuildEngine\\Main\\Project.xml";
		g_theProjectCreator.m_pProject->SaveIt( sFile.AllocSysString(), 
												bOverwrite, 
												&lResult );
	}

	if (g_theProjectCreator.m_pSequence)
	{
		sFile = L"D:\\_BuildSystem\\trunk\\BuildEngine\\Main\\Sequence.xml";
		g_theProjectCreator.m_pSequence->SaveIt( sFile.AllocSysString(), 
												 bOverwrite, 
												 &lResult );
	}
	
	if (g_theProjectCreator.m_pProperties)
	{
		sFile = L"D:\\_BuildSystem\\trunk\\BuildEngine\\Main\\Properties.xml";
		g_theProjectCreator.m_pProperties->SaveIt( sFile.AllocSysString(),
												   bOverwrite, 
												   &lResult );
	}

} //CProjectCreator::SaveUm()


UINT CProjectCreator::InitProject(CRedPath projectFile)
{
	LONG lResult = 1;	//default to fail;
	
	HRESULT hr = ::CoCreateInstance(CLSID_rXmlFile, NULL, 
									CLSCTX_INPROC_SERVER, IID_IrXmlFile,
									(void**)&m_pProject);
	if (S_OK != hr)
		return ErrorIt(hr);
	
	m_pProject->Init(&lResult);
	if (S_OK != lResult)
		return ErrorIt(lResult);

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	m_pProject->CreateIt(CComBSTR(projectFile.GetString()), &bSuccess);
	if (VARIANT_FALSE == bSuccess)
		return ErrorIt(111111);	//add custom error about can't create .xml

	m_strProjectId.Format(ROTID_BUILDIT, CRedHelperFn::CreateGUID());

	CString sId;
	sId.Format(ROTID_PROJECT, m_strProjectId);

	m_pProject->RegisterIt(sId.AllocSysString(), &lResult);
	if (S_OK != lResult)
		return ErrorIt(lResult);

	m_projectFile = projectFile;

	return ERROR_SUCCESS;

}

UINT CProjectCreator::InitProperties(void)
{
	LONG lResult = 1;	//default to fail;
	
	HRESULT hr = ::CoCreateInstance(CLSID_rXmlFile, NULL, 
									CLSCTX_INPROC_SERVER, IID_IrXmlFile,
									(void**)&m_pProperties);
	if (S_OK != hr)
		return ErrorIt(hr);
	
	m_pProperties->Init(&lResult);

	CString str;
	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	m_pProject->GetNodeCount(XPATH_PROPERTIES, &lResult);
	
	if (lResult > 0)
	{
		BSTR bstr;
		m_pProject->GetXmlFragment(XPATH_PROPERTIES, &bstr);
		str.Format(	L"<%s>%s</%s>", NODE_ROOT, bstr, NODE_ROOT);
		::SysFreeString(bstr);

		m_pProject->RemoveAll(XPATH_PROPERTIES, &bSuccess);
	}
	else
	{
		str.Format(	L"<%s><%s><%s name=\"Red.Default\" value=\"created\" /></%s></%s>",
					NODE_ROOT,
					NODE_PROPERIES,
					NODE_PROPERTY,
					NODE_PROPERIES,
					NODE_ROOT);
	}

	m_pProperties->CreateIt(str.AllocSysString(), &bSuccess);
	if (VARIANT_FALSE == bSuccess)
		return ErrorIt(111111);	//add custom error about can't create .xml

	CString sId;
	sId.Format(ROTID_PROPERTIES, m_strProjectId);

	m_pProperties->RegisterIt(sId.AllocSysString(), &lResult);
	if (S_OK != lResult)
		return ErrorIt(lResult);

	return ERROR_SUCCESS;

}


UINT CProjectCreator::InitSequence(void)
{
	LONG lResult = 1;	//default to fail;
	
	HRESULT hr = ::CoCreateInstance(CLSID_rXmlFile, NULL, 
									CLSCTX_INPROC_SERVER, IID_IrXmlFile,
									(void**)&m_pSequence);
	if (S_OK != hr)
		return ErrorIt(hr);
	
	m_pSequence->Init(&lResult);

	CString str;
	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	m_pProject->GetNodeCount(XPATH_SEQUENCE, &lResult);
	
	if (lResult > 0)
	{
		BSTR bstr;
		m_pProject->GetXmlFragment(XPATH_SEQUENCE, &bstr);
		str.Format(	L"<%s>%s</%s>", NODE_ROOT, bstr, NODE_ROOT);
		::SysFreeString(bstr);

		m_pProject->RemoveAll(XPATH_SEQUENCE, &bSuccess);
	}

	m_pSequence->CreateIt(str.AllocSysString(), &bSuccess);
	if (VARIANT_FALSE == bSuccess)
		return ErrorIt(111111);	//add custom error about can't create .xml

	CString sId;
	sId.Format(ROTID_SEQUENCE, m_strProjectId);

	m_pSequence->RegisterIt(sId.AllocSysString(), &lResult);
	if (S_OK != lResult)
		return ErrorIt(lResult);

	return ERROR_SUCCESS;

}

UINT CProjectCreator::ErrorIt(HRESULT hrError)
{
	//figure out error and return;
	//messagebox of some sort
	return (UINT)hrError;
}


CString CProjectCreator::GetProjectId(void)
{
	return g_theProjectCreator.m_strProjectId;
}

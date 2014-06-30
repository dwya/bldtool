// rBldProperties.cpp : Implementation of CrBldProperties

#include "stdafx.h"
#include "rBldProperties.h"
#include "RunningObjects.h"
#include "ProjSrvXmlMarkupDefines.h"


// CrBldProperties


/**
*	\remarks
*/
STDMETHODIMP CrBldProperties::Init(BSTR bstrProjectId, LONG* lResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComPtr<IUnknown> Unknown;
	HRESULT hr = CRunningObjects::GetIt(bstrProjectId, (IUnknown**)&Unknown);

	if (Unknown.p)
	{
		hr = Unknown->QueryInterface(IID_IrXmlFile, (void**)&m_pXml);
		
		//CComPtr<IrXmlFile> iFileIt;
		//hr = Unknown->QueryInterface(IID_IrXmlFile, (void**)&m_pXml);
		//hr = Unknown->QueryInterface(IID_IrXmlFile, (void**)&iFileIt);
		//m_pXml.Attach(iFileIt);
	}

//	m_pXml.Attach();

	*lResult = (LONG)hr;

	return hr;

} //CrBldProperties::Init()


/**
*	\remarks
*/
STDMETHODIMP CrBldProperties::GetProperty(BSTR bstrName, BSTR* bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrValue = SysAllocString(L"");

	if (!m_pXml)
		return E_FAIL;


	CString sQuery;
	sQuery.Format(L"%s[@%s = \"%s\"]", 
				  XPATH_PROPERTY, 
				  ATT_NAME, 
				  bstrName);

	m_pXml->GetAttribute(sQuery.AllocSysString(), 
						 CComBSTR(ATT_VALUE),
						 0,
						 bstrValue);

	return S_OK;

} //CrBldProperties::GetProperty()


/**
*	\remarks
 [out,retval] VARIANT_BOOL* bSuccess
 VARIANT_BOOL* bSuccess

*/
STDMETHODIMP CrBldProperties::SetProperty(BSTR bstrName, BSTR bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	if (!m_pXml)
		return E_FAIL;

	CString sQuery;
	sQuery.Format(L"%s[@%s = \"%s\"]", 
				  XPATH_PROPERTY, 
				  ATT_NAME, 
				  bstrName);

	bSuccess = VARIANT_FALSE;
	m_pXml->SetAttribute(sQuery.AllocSysString(), 
						 CComBSTR(ATT_VALUE), 
						 bstrValue,
						 0,
						 &bSuccess);

	if (VARIANT_TRUE == bSuccess)
		return S_OK;

	//Property doesn't exist, select * property and add a new one
	LONG lSuccess = 0;
	m_pXml->AddNodeToEndOfList( CComBSTR(XPATH_PROPERTY),
								CComBSTR(NODE_PROPERTY),
								&lSuccess );
	if (lSuccess > 0)
	{
		sQuery.Format(L"%s[not (@%s)]", 
					  XPATH_PROPERTY, 
					  ATT_NAME);

		m_pXml->SetAttribute(sQuery.AllocSysString(), 
							 CComBSTR(ATT_VALUE),
							 bstrValue,
							 0,
							 &bSuccess);

		m_pXml->SetAttribute(sQuery.AllocSysString(), 
							 CComBSTR(ATT_NAME),
							 bstrName,
							 0,
							 &bSuccess);

	} //if (lSuccess > 0)

	return S_OK;

} //CrBldProperties::SetProperty()


/**
*	\remarks
*/
STDMETHODIMP CrBldProperties::UnInit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_pXml.p)
		m_pXml.Release();

		//m_pXml.Detach();
		//m_pXml.Release();

	return S_OK;

} //CrBldProperties::UnInit()

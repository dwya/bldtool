// rBldProjectXml.cpp : Implementation of CrBldProjectXml

#include "stdafx.h"
#include "rBldProjectXml.h"
#include "RunningObjects.h"


// CrBldProjectXml


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::Init(BSTR bstrProjectId, LONG* lResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComPtr<IUnknown> Unknown;
	HRESULT hr = CRunningObjects::GetIt(bstrProjectId, (IUnknown**)&Unknown);

	if (Unknown.p)
		hr = Unknown->QueryInterface(IID_IrXmlFile, (void**)&m_pXml);

	*lResult = (LONG)hr;

	return hr;

} //CrBldProjectXml::Init()



/**
*	\remarks
*
*	\todo	-	Do we need to add any references to the project file?  Should we track how many 
*				CrBldProjectXml are using it?
*			-	If we release here do we release the main object or just the reference to it?
*/
STDMETHODIMP CrBldProjectXml::UnInit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_pXml.p)
		m_pXml.Release();

	//if (m_bstrXPath)
	//	SysFreeString(m_bstrXPath);
	if (L"" != m_strXPath)
		m_strXPath = L"";

	return S_OK;

} //CrBldProjectXml::UnInit()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::Select(BSTR bstrXPath, LONG* lNodeCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//m_bstrXPath = bstrXPath;
	//m_bstrXPath = SysAllocString(bstrXPath);
	m_strXPath = CString(bstrXPath);
	m_lCounter	= 0;

	return GetNodeCount(lNodeCount);

} //CrBldProjectXml::Select()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::GetNodeCount(LONG* lNodeCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lNodeCount = 0;
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->GetNodeCount(m_strXPath.AllocSysString(), lNodeCount);

} //CrBldProjectXml::GetNodeCount()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::GetAttribute(BSTR bstrAttribute, BSTR* bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrValue = SysAllocString(L"");
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->GetAttribute(m_strXPath.AllocSysString(), bstrAttribute, m_lCounter, bstrValue);

} //CrBldProjectXml::GetAttribute


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::GetParentAttribute(BSTR bstrAttribute, BSTR* bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrValue = SysAllocString(L"");
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->GetParentAttribute(m_strXPath.AllocSysString(), bstrAttribute, m_lCounter, bstrValue);

} //CrBldProjectXml::GetParentAttribute()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::SetAttribute(BSTR bstrAttribute, BSTR bstrValue, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->SetAttribute(m_strXPath.AllocSysString(), bstrAttribute, bstrValue, m_lCounter, bSuccess);

} //CrBldProjectXml::SetAttribute()


/**
*	remarks
*/
STDMETHODIMP CrBldProjectXml::RemoveAll(VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->RemoveAll(m_strXPath.AllocSysString(), bSuccess);

} //CrBldProjectXml::RemoveAll()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::AddNodeToEndOfList(BSTR bstrTagName, LONG* lNewPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lNewPosition = 0;
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->AddNodeToEndOfList(m_strXPath.AllocSysString(), bstrTagName, lNewPosition);

} //CrBldProjectXml::AddNodeToEndOfList()



/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::AddNodeAsChild(BSTR bstrTagName, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->AddNodeAsChild(m_strXPath.AllocSysString(), bstrTagName, m_lCounter, bSuccess);

} //CrBldProjectXml::AddNodeAsChild()



/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::GetXmlFragment(BSTR* bstrStringFragment)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrStringFragment = SysAllocString(L"");
	if (!m_pXml)
		return E_FAIL;

	return m_pXml->GetXmlFragment(m_strXPath.AllocSysString(), bstrStringFragment);

} //CrBldProjectXml::GetXmlFragment()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::CounterNext(LONG* lCounter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LONG lReturn = 0;

	if (m_pXml.p)
	{
		m_lCounter++;
		
		LONG lCnt = 0;
		m_pXml->GetNodeCount(m_strXPath.AllocSysString(), &lCnt);

		if (lCnt > m_lCounter)
			lReturn = m_lCounter;

	}

	*lCounter = lReturn;

	return S_OK;

} //CrBldProjectXml::CounterNext()


/**
*	\remarks
*/
STDMETHODIMP CrBldProjectXml::CounterLast(LONG* lCounter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LONG lReturn = 0;

	if (m_pXml.p)
	{
		m_lCounter--;

		if (m_lCounter > 0)
			lReturn = m_lCounter;
	}

	*lCounter = lReturn;

	return S_OK;

} //CrBldProjectXml::CounterLast

STDMETHODIMP CrBldProjectXml::GetXPath(BSTR* bstrXPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*bstrXPath = m_strXPath.AllocSysString();

	return S_OK;
}

STDMETHODIMP CrBldProjectXml::GetCounter(LONG* lCounter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lCounter = m_lCounter;

	return S_OK;
}

STDMETHODIMP CrBldProjectXml::SetPos(BSTR bstrXPath, LONG lCounter)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//m_bstrXPath = SysAllocString(bstrXPath);
	m_strXPath = CString(bstrXPath);
	m_lCounter = lCounter;

	return S_OK;
}

STDMETHODIMP CrBldProjectXml::GetTagName(BSTR* bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pXml->GetTagName(m_strXPath.AllocSysString(), m_lCounter, bstrName);

	return S_OK;
}


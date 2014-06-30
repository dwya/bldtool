// rXmlFile.cpp : Implementation of CrXmlFile

#include "stdafx.h"
#include "rXmlFile.h"
#include "RunningObjects.h"


// CrXmlFile


/**
*	\remarks
*
*	\todo	-	Move message to string table
*/
STDMETHODIMP CrXmlFile::Init(LONG* lResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//UnInit();	//incase we are already in use

	HRESULT hr = m_pXMLDOM.CoCreateInstance(CLSID_DOMDocument);
	if (S_OK != hr)
		::AfxMessageBox(L"CrXmlFile::Init() -> ProjSrv.dll couldn't instance IID_IXMLDOMDocument2");

	*lResult = (LONG)hr;
	return S_OK;

} //CrXmlFile::Init()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::UnInit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (NULL != m_pXMLDOM.p)
	{
		m_pXMLDOM.Release();
		CRunningObjects::UnRegisterIt(m_dwRegistration);
		//m_pXMLDOM.p = NULL;
	}

	return S_OK;

} //CrXmlFile::UnInit()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::CreateIt(BSTR bstrXml, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;

	if (NULL == m_pXMLDOM.p)
	{
		return E_FAIL;	//Initialize() not called
	}

	CComVariant vXMLFile(bstrXml);

	m_pXMLDOM->put_async(VARIANT_FALSE);
	m_pXMLDOM->put_validateOnParse(VARIANT_FALSE);
	m_pXMLDOM->put_resolveExternals(VARIANT_FALSE);

	HRESULT hr = m_pXMLDOM->load(vXMLFile, bSuccess);
	if (S_OK != hr)
	{
		//see if it is a blob instead of a file
		hr = m_pXMLDOM->loadXML(vXMLFile.bstrVal, bSuccess);
		
	}

	return S_OK;

} //CrXmlFile::CreateIt()



/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::SaveIt(BSTR bstrFile, VARIANT_BOOL bOverwrite, LONG* lResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lResult = (LONG)E_FAIL;

	if (!m_pXMLDOM.p)
	{
		return E_FAIL;	//Initialize() not called
	}

	//make sure existing files are not read-only
	CFileFind oFind;
	if (oFind.FindFile(bstrFile))
	{
		if (VARIANT_TRUE == bOverwrite)
		{
			CFileStatus oStatus;
			oStatus.m_attribute = 0x00;
			CFile::SetStatus(bstrFile, oStatus);
		}
		else
		{
			*lResult = ERROR_FILE_EXISTS;
			return ERROR_FILE_EXISTS;
		}
	}

	CComVariant vXMLFile(bstrFile);
	HRESULT hr = m_pXMLDOM->save(vXMLFile);

	*lResult = (LONG)hr;

	return hr;

} //CrXmlFile::SaveIt()



/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::GetNodeCount(BSTR bstrXPath, LONG* lNodes)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lNodes = 0;

	if (!m_pXMLDOM.p)
		return 0;

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if (S_OK == hr)
		pNodeList->get_length(lNodes);

	FreeLock(locker);

	return hr;

} //CrXmlFile::GetNodeCount()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::GetParentAttribute(BSTR bstrXPath, BSTR bstrAttribute, LONG lIndex, BSTR* bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrValue = SysAllocString(L"");

	if (!m_pXMLDOM.p)
		return E_FAIL;	//Initialize() not called

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if (NULL != pNodeList.p)
	{
		CComPtr<IXMLDOMNode>	pNode;
		CComPtr<IXMLDOMNode>	pNodeParent;
		CComPtr<IXMLDOMElement>	pElement;

		CComVariant v;

		LONG lNodeCount = 0;

		hr = pNodeList->get_length(&lNodeCount);
		if ((lNodeCount > 0) && (lIndex <= lNodeCount))
		{
			pNodeList->get_item(lIndex, &pNode);
			if (NULL != pNode)
			{
				pNode->get_parentNode(&pNodeParent);
				if (NULL != pNodeParent)
				{
					pElement = pNodeParent;
					if (NULL != pElement.p)
					{
						pElement->getAttribute(bstrAttribute, &v);
						if (VT_BSTR == v.vt)
						{
							*bstrValue = v.bstrVal;
						}
					}//if (NULL != pElement.p)
				}//if (NULL != pNodeParent)
			}//if (NULL != pNode)
		}//found at least one node
	}

	FreeLock(locker);

	return S_OK;

} //CrXmlFile::GetParentAttribute()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::GetAttribute(BSTR bstrXPath, BSTR bstrAttribute, LONG lIndex, BSTR* bstrValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrValue = SysAllocString(L"");

	if (!m_pXMLDOM.p)
		return E_FAIL;		//Initialize() not called

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if (pNodeList.p)
	{
		CComPtr<IXMLDOMNode>	pNode;
		CComPtr<IXMLDOMElement>	pElement;
		CComVariant v;

		LONG lNodeCount = 0;

		pNodeList->get_length(&lNodeCount);
		if ((lNodeCount > 0) && (lIndex <= lNodeCount))
		{
			pNodeList->get_item(lIndex, &pNode);
			
			pElement = pNode;
			if (NULL != pElement.p)
			{
				hr = pElement->getAttribute(bstrAttribute, &v);
				if (S_OK == hr)
				{
					if (VT_BSTR == v.vt)
					{
						*bstrValue = v.bstrVal;
					}

				}//getAttribute()
			}//NULL != pElement.p
		}//found at least one node and the index is not too big
	}

	FreeLock(locker);

	return S_OK;

} //CrXmlFile::GetAttribute()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::SetAttribute(BSTR bstrXPath, BSTR bstrAttribute, BSTR bstrValue, LONG lIndex, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXMLDOM.p)
		return false;	//Initialize() not called

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if (pNodeList.p)
	{
		CComPtr<IXMLDOMNode>	pNode;
		CComPtr<IXMLDOMElement>	pElement;

		CComVariant v(bstrValue);

		LONG lNodeCount = 0;

		pNodeList->get_length(&lNodeCount);
		if ((lNodeCount > 0) && (lIndex <= lNodeCount))
		{
			pNodeList->get_item(lIndex, &pNode);
			
			pElement = pNode;
			if (NULL != pElement.p)
			{
				if (S_OK == pElement->setAttribute(bstrAttribute, v))
				{
					*bSuccess = VARIANT_TRUE;
				}
			}//NULL != pElement.p
		}//found at least one node and index is not too big
	}

	FreeLock(locker);

	return S_OK;

} //CrXmlFile::SetAttribute()



/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::RemoveAll(BSTR bstrXPath, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXMLDOM.p)
		return E_FAIL;	//Initialize() not called

	CSingleLock* locker = GetLock();

	HRESULT hr = E_FAIL;

	CComPtr<IXMLDOMNodeList>	pNodeList;
	CComPtr<IXMLDOMSelection>	pSelection;

	if (S_OK == m_pXMLDOM->selectNodes(bstrXPath, &pNodeList))
	{
		pNodeList->QueryInterface(IID_IXMLDOMSelection ,(void**)&pSelection );
	
		if (NULL != pSelection.p)
		{
			hr = pSelection->removeAll();
			if (S_OK == hr)
				*bSuccess = VARIANT_TRUE;
		}
	}

	FreeLock(locker);

	return hr;

} //CrXmlFile::RemoveAll()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::AddNodeToEndOfList(BSTR bstrXPath, BSTR bstrTagName, LONG* lPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*lPosition = 0;

	if (!m_pXMLDOM.p)
		return E_FAIL;	//Initialize() not called


	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if ((NULL == pNodeList.p) || (S_OK != hr))
	{
		return S_OK;
	}

	CComPtr<IXMLDOMNode>		pNode;
	CComPtr<IXMLDOMNode>		pNodeNew;
	CComPtr<IXMLDOMNode>		pNodeParent;
	CComPtr<IXMLDOMNode>		pNodeNewResult;

	long lReturn = 0;
	long lNodes	= 0;
	long lIndex = 0;

	pNodeList->get_length(&lNodes);
    if (lNodes > 0)
	{
		lIndex = lNodes - 1;

		//select last node in list
		pNodeList->get_item(lIndex, &pNode);
		if (NULL != pNode.p)
		{
			pNode->get_parentNode(&pNodeParent);

            m_pXMLDOM->createNode(CComVariant(NODE_ELEMENT), bstrTagName, NULL, &pNodeNew);
			
			if ((NULL != pNodeNew.p) && (NULL != pNodeParent.p))
			{
				pNodeParent->appendChild(pNodeNew, &pNodeNewResult);
				if (NULL != pNodeNewResult.p)
				{
					GetNodeCount(bstrXPath, lPosition);
					*lPosition = *lPosition - 1;
				}
			} //createNode(...)
		} //get_item(...)
	} //lNodes > 0

	return S_OK;

} //CrXmlFile::AddNodeToEndOfList()


/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::AddNodeAsChild(BSTR bstrXPath, BSTR bstrTagName, LONG lIndex, VARIANT_BOOL* bSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bSuccess = VARIANT_FALSE;
	if (!m_pXMLDOM.p)
		return E_FAIL;	//Initialize() not called

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if ((NULL == pNodeList.p) || (S_OK != hr))
	{
		return S_OK;
	}

	CComPtr<IXMLDOMNode>		pNode;
	CComPtr<IXMLDOMNode>		pNodeNew;
	CComPtr<IXMLDOMNode>		pNodeNewResult;

	long lNodes	= 0;

	pNodeList->get_length(&lNodes);
    if (lNodes > 0 && lIndex <= lNodes)
	{
		//pNodeList->get_item((lIndex - 1), &pNode);
		pNodeList->get_item(lIndex, &pNode);
		if (NULL != pNode.p)
		{
            m_pXMLDOM->createNode(CComVariant(NODE_ELEMENT), bstrTagName, NULL, &pNodeNew);
			
			if (NULL != pNodeNew.p)
			{
				pNode->appendChild(pNodeNew, &pNodeNewResult);
				if (NULL != pNodeNewResult.p)
				{
					*bSuccess = VARIANT_TRUE;
				}
			} //createNode(...)
		} //get_item(...)
	} //lNodes > 0

	return S_OK;

} //CrXmlFile::AddNodeAsChild()



/**
*	\remarks
*/
STDMETHODIMP CrXmlFile::GetXmlFragment(BSTR bstrXPath, BSTR* bstrXmlFragment)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrXmlFragment = SysAllocString(L"");

	if (!m_pXMLDOM.p)
		return NULL;	//Initialize() not called

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNode> pNode = NULL;
	HRESULT hr = m_pXMLDOM->selectSingleNode(bstrXPath, &pNode);
	if (S_OK == hr)
	{
		pNode->get_xml(bstrXmlFragment);
	}

	FreeLock(locker);

	return S_OK;

} //CrXmlFile::GetXmlFragment()


STDMETHODIMP CrXmlFile::GetTagName(BSTR bstrXPath, LONG lIndex, BSTR* bstrTagName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*bstrTagName = SysAllocString(L"");

	if (!m_pXMLDOM.p)
		return E_FAIL;		//Initialize() not called

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(bstrXPath, &pNodeList);
	if (pNodeList.p)
	{
		CComPtr<IXMLDOMNode>	pNode;
		CComPtr<IXMLDOMElement>	pElement;
		CComVariant v;

		LONG lNodeCount = 0;

		pNodeList->get_length(&lNodeCount);
		if ((lNodeCount > 0) && (lIndex <= lNodeCount))
		{
			pNodeList->get_item(lIndex, &pNode);
			pNode->get_nodeName(bstrTagName);
			//pNode->get_nodeTypeString(bstrTagName);

		}//found at least one node and the index is not too big
	}

	FreeLock(locker);

	return S_OK;

}


/**
*	\remarks
*
*	\todo	-	if already exists, return a more accurate error code
*/
STDMETHODIMP CrXmlFile::RegisterIt(BSTR bstrId, LONG* lResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComPtr<IUnknown> Unknown;
	CRunningObjects::GetIt(bstrId, (IUnknown**)&Unknown);
	if (Unknown.p)
	{
		*lResult = (LONG)ERROR_FILE_EXISTS;
		return ERROR_FILE_EXISTS;
	}

	HRESULT hr = CRunningObjects::RegisterIt(bstrId, 
											(IUnknown*)this, 
											 m_dwRegistration);
	*lResult = (LONG)hr;

	return hr;

} //CrXmlFile::RegisterIt()


/**
*	\remarks
*/
CSingleLock* CrXmlFile::GetLock(void)
{
	CSingleLock* lock = new CSingleLock(&m_CriticalSection);
	lock->Lock();
	return lock;

} //CrXmlFile::GetLock()


/**
*	\remarks
*/
void CrXmlFile::FreeLock(CSingleLock* lock)
{
	if (NULL != lock)
	{
		lock->Unlock();
		delete lock;
	}

} //CrXmlFile::FreeLock()



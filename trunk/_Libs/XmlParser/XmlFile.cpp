// XMLFile.cpp : implementation file
//

#include "stdafx.h"
#include "XMLFile.h"


// CXmlFile

CXmlFile::CXmlFile()
: m_bThreadSafe(false)
, m_bInitialized(false)
, m_pXMLDOM(NULL)
{
}

CXmlFile::~CXmlFile()
{
	UnInitialize();
}

// CXmlFile member functions

CSingleLock* CXmlFile::GetLock(void)
{
	if (m_bThreadSafe)
	{
		CSingleLock* lock = new CSingleLock(&m_CriticalSection);
		lock->Lock();
		return lock;
	}

	return NULL;

}

void CXmlFile::FreeLock(CSingleLock* lock)
{
	if (NULL != lock)
	{
		lock->Unlock();
		delete lock;
	}
}


/*	CXmlFile::Initialize	**********************************

Returns:	HRESULT <value from ::CoCreateInstance()>

-	Initalizes COM and the pointer fo the xml doc (m_pXMLDOM)
-	Application Verifier Tool (Windows Vista) detects crash in
	COM if we call this code from the constructor CXmlFile()
-	Member must be called before any xml other methods can be used
**************************************************************/
HRESULT CXmlFile::Initialize(void)
{
	if (NULL != m_pXMLDOM.p)
		return ERROR_ALREADY_EXISTS;

	::CoInitialize(NULL);

	HRESULT hr = m_pXMLDOM.CoCreateInstance(CLSID_DOMDocument);
	if (S_OK != hr)
	{
		::AfxMessageBox(L"CXmlFile() -> couldn't instance IID_IXMLDOMDocument2");
	}

	return hr;

} //CXmlFile::Initialize()


void CXmlFile::UnInitialize(void)
{
	if (NULL != m_pXMLDOM.p)
	{
		m_pXMLDOM.Release();
		::CoUninitialize();
	}

} //CXmlFile::UnInitialize


/*************************************************************
CXmlFile::CreateXML

Args:	[IN]	LPCTSTR szFileOrXMLBlob
		-	full path to file name or xml blob (example <RootNode />

Returns:
	Value from IXMLDOMDocument::load or IXMLDOMDocument::loadXML,
	depending on szFileOrXMLBlob
*************************************************************/
HRESULT CXmlFile::CreateXML(LPCTSTR szFileOrXMLBlob, bool bThreadSafe /* = false*/)
{
	if (NULL == m_pXMLDOM.p)
	{
		return E_FAIL;	//Initialize() not called
	}

	m_bThreadSafe = bThreadSafe;

	CComVariant vXMLFile(szFileOrXMLBlob);

	m_pXMLDOM->put_async(VARIANT_FALSE);
	m_pXMLDOM->put_validateOnParse(VARIANT_FALSE);
	m_pXMLDOM->put_resolveExternals(VARIANT_FALSE);

	VARIANT_BOOL bSuccess = FALSE;	
	HRESULT hr = m_pXMLDOM->load(vXMLFile, &bSuccess);
	if (S_OK != hr)
	{
		//see if it is a blob instead of a file
		hr = m_pXMLDOM->loadXML(vXMLFile.bstrVal, &bSuccess);
		
	}

	if (S_OK == hr)
		m_bInitialized = true;

	return hr;

} //CXML::CreateXML



/*************************************************************
CXmlFile::SaveXML

Args:	[IN]	LPCTSTR szFile
		-	location to save file (full path)

Returns:
	Value from IXMLDOMDocument::save
*************************************************************/
HRESULT CXmlFile::Saveit(LPCTSTR szFile, bool bOverwriteIfExists /* = true*/)
{
	if (!m_pXMLDOM.p)
	{
		return E_FAIL;	//Initialize() not called
	}

	//make sure existing files are not read-only
	CFileFind oFind;
	if (oFind.FindFile(szFile))
	{
		if (bOverwriteIfExists)
		{
			CFileStatus oStatus;
			oStatus.m_attribute = 0x00;
			CFile::SetStatus(szFile, oStatus);
		}
		else
		{
			return ERROR_FILE_EXISTS;
		}
	}

	CComVariant vXMLFile(szFile);
	return m_pXMLDOM->save(vXMLFile);

} //CXmlFile::SaveXML



/*************************************************************
CXmlFile::GetNodeCount

Args:	[IN]	CString strXPath

Returns:
	Number of nodes selected with the XPath
*************************************************************/
long CXmlFile::GetNodeCount(CString strXPath)
{
	if (!m_pXMLDOM.p)
	{
		return 0;	//Initialize() not called
	}

	CSingleLock* locker = GetLock();

	long lReturn = 0;

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
	if (S_OK == hr)
	{
		long lNodes = 0;
		if (S_OK == pNodeList->get_length(&lNodes))
		{
			lReturn = lNodes;
		}

	}

	FreeLock(locker);

	return lReturn;

} //CXmlFile::GetNodeCount



/*************************************************************
CXmlFile::GetAttribute

Args:	[IN]	CString strXPath
		[IN]	CString strAttribute
		[IN]	long lIndex
		- lIndex is the number of nodes to count forward
		before getting the attribute's value

Returns:
	blank string "" if no such attribute, or the attribute's value
*************************************************************/
CString CXmlFile::GetAttribute(CString strXPath, CString strAttribute, long lIndex)
{
	if (!m_pXMLDOM.p)
	{
		return L"";	//Initialize() not called
	}

	CSingleLock* locker = GetLock();

	CString strReturn = _T("");

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
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
				hr = pElement->getAttribute(strAttribute.AllocSysString(), &v);
				if (S_OK == hr)
				{
					if (VT_BSTR == v.vt)
					{
						strReturn = v.bstrVal;
					}

				}//getAttribute()
			}//NULL != pElement.p
		}//found at least one node and the index is not too big
	}

	FreeLock(locker);

	return strReturn;

} //CXmlFile::GetAttribute



/*************************************************************
CXmlFile::GetParentAttribute

Args:	[IN]	CString strXPath
		[IN]	CString strAttribute
		[IN]	long lIndex
		- lIndex is the number of nodes to count forward
		before getting the attribute's value

Returns:
	blank string "" if no such attribute, or the attribute's value
*************************************************************/
CString CXmlFile::GetParentAttribute(CString strXPath, CString strAttribute, long lIndex)
{
	if (!m_pXMLDOM.p)
	{
		return L"";	//Initialize() not called
	}

	CSingleLock* locker = GetLock();

	CString strReturn = _T("");

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
	if (NULL == pNodeList.p)
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
						pElement->getAttribute(strAttribute.AllocSysString(), &v);
						if (VT_BSTR == v.vt)
						{
							strReturn = v.bstrVal;
						}
					}//if (NULL != pElement.p)
				}//if (NULL != pNodeParent)
			}//if (NULL != pNode)
		}//found at least one node
	}

	FreeLock(locker);

	return strReturn;

} //CXmlFile::GetParentAttribute



/*************************************************************
CXmlFile::SetAttribute

Args:	[IN]	CString strXPath
		[IN]	CString strAttribute
		[IN]	CString strValue
		[IN]	long lIndex

Returns: true if attribute was successfully set, else false
*************************************************************/
bool CXmlFile::SetAttribute(CString strXPath, CString strAttribute, CString strValue, long lIndex)
{
	if (!m_pXMLDOM.p)
	{
		return false;	//Initialize() not called
	}

	CSingleLock* locker = GetLock();

	bool bReturn = false;

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
	if (pNodeList.p)
	{
		CComPtr<IXMLDOMNode>	pNode;
		CComPtr<IXMLDOMElement>	pElement;

		CComVariant v(strValue);

		LONG lNodeCount = 0;

		pNodeList->get_length(&lNodeCount);
		if ((lNodeCount > 0) && (lIndex <= lNodeCount))
		{
			pNodeList->get_item(lIndex, &pNode);
			
			pElement = pNode;
			if (NULL != pElement.p)
			{
				if (S_OK == pElement->setAttribute(strAttribute.AllocSysString(), v))
				{
					bReturn = true;
				}
			}//NULL != pElement.p
		}//found at least one node and index is not too big
	}

	FreeLock(locker);

	return bReturn;

} //CXmlFile::SetAttribute



/*************************************************************
CXmlFile::RemoveAll

Args:	[IN]	CString strXPath
		- deletes all nodes selected by the XPath
Returns:
	value from IXMLDOMSelection::removeAll()
*************************************************************/
HRESULT CXmlFile::RemoveAll(CString strXPath)
{
	if (!m_pXMLDOM.p)
	{
		return E_FAIL;	//Initialize() not called
	}

	CSingleLock* locker = GetLock();

	HRESULT hr = E_FAIL;

	CComPtr<IXMLDOMNodeList>	pNodeList;
	CComPtr<IXMLDOMSelection>	pSelection;

	if (S_OK == m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList))
	{
		pNodeList->QueryInterface(IID_IXMLDOMSelection ,(void**)&pSelection );
	
		if (NULL != pSelection.p)
		{
			hr = pSelection->removeAll();
		}
	}

	FreeLock(locker);

	return hr;

} //CXmlFile::RemoveAll



/*************************************************************
CXmlFile::AddNodeToEndOfList

Args:	[IN]	CString strXPath
		[IN]	CString strTagName
		- node name to add

Returns:	long
	- if successful the position the counter needs to be at to select the
	new node, else 0

Notes:
	->	adds strTagName as a new node in the current XPath
	->	if the current XPath will not select the new node
		then you will need to change the XPath before you can add
		attributes to it
	->	example:
		strXPath is:	//Root/Nodes	<- selects all Nodes
		strTagname is:	MyNewNode		<- node is added as //Root/MyNewNode
											which is not in the current XPath
*************************************************************/
long CXmlFile::AddNodeToEndOfList(CString strXPath, CString strTagName)
{
	if (!m_pXMLDOM.p)
	{
		return 0;	//Initialize() not called
	}

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
	if ((NULL == pNodeList.p) || (S_OK != hr))
	{
		return 0;
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

            m_pXMLDOM->createNode(CComVariant(NODE_ELEMENT), 
									strTagName.AllocSysString(), NULL, &pNodeNew);
			
			if ((NULL != pNodeNew.p) && (NULL != pNodeParent.p))
			{
				pNodeParent->appendChild(pNodeNew, &pNodeNewResult);
				if (NULL != pNodeNewResult.p)
				{
					lReturn = (GetNodeCount(strXPath.AllocSysString()) - 1);
				}
			} //createNode(...)
		} //get_item(...)
	} //lNodes > 0

	return lReturn;

} //CXmlFile::AddNodeToEndOfList



/*************************************************************
CXmlFile::AddNodeAsChild

Args:	[IN]	CString strXPath
		[IN]	CString strTagName
		[IN]	long lIndex

Returns: true / false

Notes:
	->	same as AddNodeToEndOfList however the node is added as a child
	->	Example:
		strXPath is:	//Root/Nodes	<- selects all Nodes
		strTagname is:	MyNewNode		<- node is added as //Root/Nodes/MyNewNode
											which is not in the current XPath
*************************************************************/
bool CXmlFile::AddNodeAsChild(CString strXPath, CString strTagName, long lIndex)
{
	if (!m_pXMLDOM.p)
	{
		return false;	//Initialize() not called
	}

	bool bReturn = false;

	CComPtr<IXMLDOMNodeList>	pNodeList;
	HRESULT hr = m_pXMLDOM->selectNodes(strXPath.AllocSysString(), &pNodeList);
	if ((NULL == pNodeList.p) || (S_OK != hr))
	{
		return bReturn;	//TODO - handle error?
	}

	CComPtr<IXMLDOMNode>		pNode;
	CComPtr<IXMLDOMNode>		pNodeNew;
	CComPtr<IXMLDOMNode>		pNodeNewResult;

	long lNodes	= 0;

	pNodeList->get_length(&lNodes);
    if (lNodes > 0 && lIndex <= lNodes)
	{
		pNodeList->get_item(lIndex, &pNode);
		if (NULL != pNode.p)
		{
            m_pXMLDOM->createNode(CComVariant(NODE_ELEMENT), 
									strTagName.AllocSysString(), NULL, &pNodeNew);
			
			if (NULL != pNodeNew.p)
			{
				pNode->appendChild(pNodeNew, &pNodeNewResult);
				if (NULL != pNodeNewResult.p)
				{
					bReturn = true;
				}
			} //createNode(...)
		} //get_item(...)
	} //lNodes > 0

	return bReturn;

} //CXML::AddNodeAsChild



/*************************************************************
CXmlFile::GetXmlFragment

Args:	[IN]	CString strXPath

Returns:
	The node and all children as a CString, only single
	node can be selected
*************************************************************/
CString CXmlFile::GetXmlFragment(CString strXPath)
{
	if (!m_pXMLDOM.p)
	{
		return NULL;	//Initialize() not called
	}

	CString sReturn = L"";

	CSingleLock* locker = GetLock();

	CComPtr<IXMLDOMNode> pNode = NULL;
	HRESULT hr = m_pXMLDOM->selectSingleNode(strXPath.AllocSysString(), &pNode);
	if (S_OK == hr)
	{
		CComBSTR bstr;
		if (S_OK == pNode->get_xml(&bstr))
		{
			sReturn = bstr;
		}
	}

	FreeLock(locker);

	return sReturn;

} //CXmlFile::GetNodeCount


#include "StdAfx.h"
#include "Project.h"
#include "ProjectCreator.h"
#include "ProjDefines.h"

CProject::CProject(void)
: m_strProjectFilter(XML_PROJECT)
, m_bInitCalled(false)
, m_bCoInitializeCalled(false)
{
}


CProject::CProject(LPCTSTR szProjectFilter)
: m_strProjectFilter(szProjectFilter)
, m_bInitCalled(false)
, m_bCoInitializeCalled(false)
{
	Init(m_strProjectFilter);
}


CProject::CProject(CProject& proj)
{
	*this = proj;
}


CProject& CProject::operator=(CProject& proj)
{
	m_strProjectFilter = proj.m_strProjectFilter;
	m_bstrXPath = proj.m_bstrXPath;
	m_bInitCalled = false;

	CComBSTR bstr;
	long lCnt = 0;

	proj.m_pXml->GetXPath(&bstr);
	proj.m_pXml->GetCounter(&lCnt);

	//counter
	Init(m_strProjectFilter);

	m_pXml->SetPos(bstr, lCnt);

	return *this;
}

CProject::~CProject(void)
{
	UnInit(); //-	won't work from here, UnInit()
	//				needs to be called from the same location (thread)
	//				that called Init()

	//if (m_bCoInitializeCalled)
		//::CoUninitialize();

}


UINT CProject::Init(LPCTSTR szProjectFilter)
{
	HRESULT hr = ERROR_SUCCESS;
	m_strProjectFilter = szProjectFilter;

	//UnInit();
	if (m_bInitCalled)
		return ERROR_SUCCESS;

	m_bInitCalled = true;

	if (!m_pXml.p)
	{
		hr = ::CoCreateInstance(CLSID_rBldProjectXml, NULL, 
								CLSCTX_INPROC_SERVER, IID_IrBldProjectXml,
								(void**)&m_pXml);
	}

	if (m_pXml.p)
	{
		CString sId;
		sId.Format(L"%s.%s", CProjectCreator::GetProjectId(), m_strProjectFilter);

		m_pXml->Init(sId.AllocSysString(), (LONG*)&hr);

	}

	return (UINT)hr;

} //CProject::Init


bool CProject::Verify(void)
{
	if (m_pXml.p)
		return true;

	bool b = false;

	if (S_OK == Init(m_strProjectFilter))
		b = true;

	/*
#if _DEBUG
	if (!b)
	{
		ASSERT(0); //no IrBldProjectXml, was Init() called?
	}
#endif
	*/

	return b;
		
}

void CProject::UnInit(void)
{
	if (m_bInitCalled)
	{
		if (m_pXml.p)
		{
			m_pXml->UnInit();
			m_pXml.Release();
		}
	}

	m_bInitCalled = false;
	m_pXml = NULL;

} //CProject::UnInit()



/**
*	\remarks
*/
LONG CProject::Select(CString sXPath)
{
	if (!Verify())
		return 0;

//	m_bstrXPath = sXPath.AllocSysString();
//	m_lCounter	= 0;

	LONG lReturn = 0;
	m_pXml->Select(sXPath.AllocSysString(), &lReturn);

	return lReturn;

} //CProject::Select()


/**
*	\remarks
*/
LONG CProject::GetNodeCount()
{
	if (!Verify())
		return 0;

	LONG lReturn = 0;
	m_pXml->GetNodeCount(&lReturn);

	return lReturn;

} //CProject::GetNodeCount()


/**
*	\remarks
*/
CString CProject::GetAttribute(CString sAttribute)
{
	if (!Verify())
		return L"";

	CString sReturn;
	CComBSTR bstr;

	m_pXml->GetAttribute(sAttribute.AllocSysString(), &bstr);

	sReturn = bstr;

	return sReturn;

} //CProject::GetAttribute


/**
*	\todo	- project defines ZERO and BLANK
*/
bool CProject::GetAttributeBool(CString sAttribute)
{
	bool b = true;

	CString s = GetAttribute(sAttribute);

	if (L"" == s || L"0" == s)
	{
		b = false;
	}

	return b;

} //CProject::GetAttributeBool

int CProject::GetAttributeInt(CString sAttribute)
{
	return 0;//::_tstoi(CProperties::Get(szProperty));

} //CProject::GetAttributeInt


/**
*	\remarks
*/
CString CProject::GetParentAttribute(CString sAttribute)
{
	if (!Verify())
		return L"";

	CString sReturn = L"";
	CComBSTR bstr;

	m_pXml->GetParentAttribute(sAttribute.AllocSysString(), &bstr);

	sReturn = bstr;

	return CString(bstr);


} //CProject::GetParentAttribute()


/**
*	\remarks
*/
bool CProject::SetAttribute(CString sAttribute, CString sValue)
{
	if (!Verify())
		return false;

	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	m_pXml->SetAttribute(sAttribute.AllocSysString(), 
						 sValue.AllocSysString(),
						 &bSuccess);

	if (VARIANT_TRUE == bSuccess)
		return true;

	return false;

} //CProject::SetAttribute()


/**
*	remarks
*/
bool CProject::RemoveAll(void)
{
	if (!Verify())
		return false;

	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	m_pXml->RemoveAll(&bSuccess);

	if (VARIANT_TRUE == bSuccess)
		return true;

	return false;

} //CProject::RemoveAll()


/**
*	\remarks
*/
LONG CProject::AddNodeToEndOfList(CString sTagName)
{
	if (!Verify())
		return 0;

	LONG lReturn = 0;
	m_pXml->AddNodeToEndOfList(sTagName.AllocSysString(),
							   &lReturn);

	return lReturn;

} //CProject::AddNodeToEndOfList()



/**
*	\remarks
*/
bool CProject::AddNodeAsChild(CString sTagName)
{
	if (!Verify())
		return false;

	VARIANT_BOOL bSuccess = VARIANT_FALSE;

	m_pXml->AddNodeAsChild(sTagName.AllocSysString(), 
						   &bSuccess);

	if (VARIANT_TRUE == bSuccess)
		return true;

	return false;

} //CProject::AddNodeAsChild()



/**
*	\remarks
*/
CString CProject::GetXmlFragment(void)
{
	if (!Verify())
		return L"";

	CComBSTR bstr;
	m_pXml->GetXmlFragment(&bstr);

	return CString(bstr);

} //CProject::GetXmlFragment()


/**
*	\remarks
*/
LONG CProject::CounterNext(void)
{
	if (!Verify())
		return 0;

	LONG lReturn = 0;
	m_pXml->CounterNext(&lReturn);

	return lReturn;

} //CProject::CounterNext()


/**
*	\remarks
*/
LONG CProject::CounterLast(void)
{
	if (!Verify())
		return 0;

	LONG lReturn = 0;
	m_pXml->CounterLast(&lReturn);

	return lReturn;	

} //CProject::CounterLast


CString CProject::GetTagName(void)
{
	BSTR bstr;
	m_pXml->GetTagName(&bstr);

	CString sReturn(bstr);
	return sReturn;

}

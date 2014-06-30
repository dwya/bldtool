// rBldProjectXml.h : Declaration of the CrBldProjectXml

#pragma once
#include "resource.h"       // main symbols

#include "ProjSrv_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CrBldProjectXml

class ATL_NO_VTABLE CrBldProjectXml :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CrBldProjectXml, &CLSID_rBldProjectXml>,
	public IDispatchImpl<IrBldProjectXml, &IID_IrBldProjectXml, &LIBID_ProjSrvLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CrBldProjectXml()
		: m_lCounter(0)
		, m_strXPath(L"")
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RBLDPROJECTXML)


BEGIN_COM_MAP(CrBldProjectXml)
	COM_INTERFACE_ENTRY(IrBldProjectXml)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(Init)(BSTR bstrProjectId, LONG* lResult);
	STDMETHOD(UnInit)(void);
	STDMETHOD(Select)(BSTR bstrXPath, LONG* lNodeCount);
	STDMETHOD(GetNodeCount)(LONG* lNodeCount);
	STDMETHOD(GetAttribute)(BSTR bstrAttribute, BSTR* bstrValue);
	STDMETHOD(GetParentAttribute)(BSTR bstrAttribute, BSTR* bstrValue);
	STDMETHOD(SetAttribute)(BSTR bstrAttribute, BSTR bstrValue, VARIANT_BOOL* bSuccess);
	STDMETHOD(RemoveAll)(VARIANT_BOOL* bSuccess);
	STDMETHOD(AddNodeToEndOfList)(BSTR bstrTagName, LONG* lNewPosition);
	STDMETHOD(AddNodeAsChild)(BSTR bstrTagName, VARIANT_BOOL* bSuccess);
	STDMETHOD(GetXmlFragment)(BSTR* bstrStringFragment);
	STDMETHOD(GetXPath)(BSTR* bstrXPath);
	STDMETHOD(GetCounter)(LONG* lCounter);
	STDMETHOD(SetPos)(BSTR bstrXPath, LONG lCounter);
	STDMETHOD(GetTagName)(BSTR* bstrName);
	STDMETHOD(CounterNext)(LONG* lCounter);
	STDMETHOD(CounterLast)(LONG* lCounter);

private:
	CComPtr<IrXmlFile> m_pXml;
	BSTR m_bstrXPath;
	CString m_strXPath;
	LONG m_lCounter;

};

OBJECT_ENTRY_AUTO(__uuidof(rBldProjectXml), CrBldProjectXml)

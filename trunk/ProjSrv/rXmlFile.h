// rXmlFile.h : Declaration of the CrXmlFile

#pragma once
#include "resource.h"       // main symbols

#include "ProjSrv_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CrXmlFile

class ATL_NO_VTABLE CrXmlFile :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CrXmlFile, &CLSID_rXmlFile>,
	public IDispatchImpl<IrXmlFile, &IID_IrXmlFile, &LIBID_ProjSrvLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CrXmlFile()
		: m_dwRegistration(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RXMLFILE)


BEGIN_COM_MAP(CrXmlFile)
	COM_INTERFACE_ENTRY(IrXmlFile)
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
	STDMETHOD(Init)(LONG* lResult);
	STDMETHOD(UnInit)(void);
	STDMETHOD(CreateIt)(BSTR bstrXml, VARIANT_BOOL* bSuccess);
	STDMETHOD(SaveIt)(BSTR bstrFile, VARIANT_BOOL bOverwrite, LONG* lResult);
	STDMETHOD(GetNodeCount)(BSTR bstrXPath, LONG* lNodes);
	STDMETHOD(GetParentAttribute)(BSTR bstrXPath, BSTR bstrAttribute, LONG lIndex, BSTR* bstrValue);
	STDMETHOD(GetAttribute)(BSTR bstrXPath, BSTR bstrAttribute, LONG lIndex, BSTR* bstrValue);
	STDMETHOD(SetAttribute)(BSTR bstrXPath, BSTR bstrAttribute, BSTR bstrValue, LONG lIndex, VARIANT_BOOL* bSuccess);
	STDMETHOD(RemoveAll)(BSTR bstrXPath, VARIANT_BOOL* bSuccess);
	STDMETHOD(AddNodeToEndOfList)(BSTR bstrXPath, BSTR bstrTagName, LONG* lPosition);
	STDMETHOD(AddNodeAsChild)(BSTR bstrXPath, BSTR bstrTagName, LONG lIndex, VARIANT_BOOL* bSuccess);
	STDMETHOD(GetXmlFragment)(BSTR bstrXPath, BSTR* bstrXmlFragment);
	STDMETHOD(RegisterIt)(BSTR bstrId, LONG* lResult);
	STDMETHOD(GetTagName)(BSTR bstrXPath, LONG lIndex, BSTR* bstrTagName);

protected:
	CSingleLock* GetLock(void);
	void FreeLock(CSingleLock* lock);

private:
	CComPtr<IXMLDOMDocument> m_pXMLDOM;
	CCriticalSection m_CriticalSection;
	DWORD m_dwRegistration;

};

OBJECT_ENTRY_AUTO(__uuidof(rXmlFile), CrXmlFile)

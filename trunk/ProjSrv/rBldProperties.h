// rBldProperties.h : Declaration of the CrBldProperties

#pragma once
#include "resource.h"       // main symbols

#include "ProjSrv_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CrBldProperties

class ATL_NO_VTABLE CrBldProperties :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CrBldProperties, &CLSID_rBldProperties>,
	public IDispatchImpl<IrBldProperties, &IID_IrBldProperties, &LIBID_ProjSrvLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CrBldProperties()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RBLDPROPERTIES)


BEGIN_COM_MAP(CrBldProperties)
	COM_INTERFACE_ENTRY(IrBldProperties)
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
	STDMETHOD(GetProperty)(BSTR bstrName, BSTR* bstrValue);
	STDMETHOD(SetProperty)(BSTR bstrName, BSTR bstrValue);
	STDMETHOD(UnInit)(void);

private:
	CComPtr<IrXmlFile> m_pXml;

};

OBJECT_ENTRY_AUTO(__uuidof(rBldProperties), CrBldProperties)

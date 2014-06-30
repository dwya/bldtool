#pragma once

#ifdef PROJ_EXPORTS
#define PROJ_API __declspec(dllexport)
#else
#define PROJ_API __declspec(dllimport)
#endif

#include "..\ProjSrv\ProjSrv_i.h"

class PROJ_API CProject
{
public:
	CProject(void);
	CProject(LPCTSTR szProjectFilter);
	CProject(CProject &proj);
	~CProject(void);

	bool Verify(void);

private:

	CComPtr<IrBldProjectXml> m_pXml;
	bool m_bCoInitializeCalled;

public:
	CString m_strProjectFilter;

	UINT Init(LPCTSTR szProjectFilter);
	bool m_bInitCalled;
	void UnInit(void);
	CComBSTR m_bstrXPath;

	bool CProject::RemoveAll(void);
	LONG CProject::AddNodeToEndOfList(CString sTagName);
	bool CProject::AddNodeAsChild(CString sTagName);
	CString CProject::GetXmlFragment(void);
	LONG CProject::CounterNext(void);
	LONG CProject::CounterLast(void);
	LONG CProject::Select(CString sXPath);
	LONG CProject::GetNodeCount();
	CString CProject::GetAttribute(CString sAttribute);
	bool CProject::GetAttributeBool(CString sAttribute);
	int CProject::GetAttributeInt(CString sAttribute);
	CString CProject::GetParentAttribute(CString sAttribute);
	bool CProject::SetAttribute(CString sAttribute, CString sValue);
	CString CProject::GetTagName(void);
	CProject& operator=(CProject& proj);

}; //class CProject

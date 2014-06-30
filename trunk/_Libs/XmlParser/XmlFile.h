#pragma once

#include <afxmt.h>				//MFC thread synchronization support

/**	\class CXmlFile
*
*	\brief is a xml file wrapper, direct access to XML DOM
*
*	\details
*	-	not thread safe, CXmlParser will add thread safe calls based
*		on CXmlFile::IsMarkedThreadSafe
*/
class CXmlFile
{
public:
	CXmlFile();
	virtual ~CXmlFile();

	HRESULT Initialize(void);
	void UnInitialize(void);

	HRESULT CreateXML(LPCTSTR szFileOrXMLBlob, bool bThreadSafe = false);
	HRESULT Saveit(LPCTSTR szFile, bool bOverwriteIfExists = true);
	long GetNodeCount(CString strXPath);
	CString GetParentAttribute(CString strXPath, CString strAttribute, long lIndex);
	CString GetAttribute(CString sXPath, CString strAttribute, long lIndex);
	bool SetAttribute(CString strXPath, CString strAttribute, CString strValue, long lIndex);
	HRESULT RemoveAll(CString strXPath);
	long AddNodeToEndOfList(CString strXPath, CString strTagName);
	bool AddNodeAsChild(CString strXPath, CString strTagName, long lIndex);

	CString GetXmlFragment(CString strXPath);

	bool IsInitialized()		{ return m_bInitialized; }
	bool IsMarkedThreadSafe()	{ return m_bThreadSafe; }

protected:
	CSingleLock* CXmlFile::GetLock(void);
	void CXmlFile::FreeLock(CSingleLock* lock);

private:
	CComPtr<IXMLDOMDocument> m_pXMLDOM;
	CCriticalSection m_CriticalSection;
	bool m_bThreadSafe;
	bool m_bInitialized;

}; //class _CXmlFile


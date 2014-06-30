#pragma once

#include "xmlfile.h"

/**	\class CXmlParser
*
*	\brief handles XPath and counter for current nodes
*
*	\details
*	- previously CUtlX from SetupUtlX.lib
*/
class CXmlParser
{
public:
	CXmlParser(CXmlFile *Xml);
	CXmlParser(CXmlParser *p);
	CXmlParser();
	~CXmlParser(void);

	void Init(CXmlFile *Xml);

	long GetNodeCount(void);
	CString GetParentAttribute(CString strAttribute)			{ return m_Xml->GetParentAttribute(m_sXPath, strAttribute, m_lCounter); }

	CString GetAttribute(CString strAttribute)					{ return m_Xml->GetAttribute(m_sXPath, strAttribute, m_lCounter); }
	bool GetAttributeBool(CString strAttribute);
	int GetAttributeInt(CString strAttribute);

	bool SetAttribute(CString strAttribute, LPCTSTR strValue)	{ return m_Xml->SetAttribute(m_sXPath, strAttribute, strValue, m_lCounter); }
	bool SetAttribute(CString strAttribute, int iValue);
	bool SetAttribute(CString strAttribute, bool bValue);

	HRESULT RemoveAll(void)										{ return m_Xml->RemoveAll(m_sXPath); }
	long AddNodeToEndOfList(CString strTagName);
	bool AddNodeAsChild(CString strTagName)						{ return m_Xml->AddNodeAsChild(m_sXPath, strTagName, m_lCounter); }

	long GetCounter(void)	{ return m_lCounter; }
	void CounterReset(void)	{ m_lCounter = 0; }
	long CounterNext(void);
	long CounterLast(void);

	long Select(CString szXPath);


private:
	CXmlFile *m_Xml;
	long m_lCounter;
	CString m_sXPath;

};	//class CXmlParser


typedef CXmlParser CXml;




#include "StdAfx.h"
#include "XmlParser.h"


CXmlParser::CXmlParser()
{
	Init(NULL);
}


CXmlParser::CXmlParser(CXmlFile *Xml)
{
	Init(Xml);
}


CXmlParser::CXmlParser(CXmlParser *p)
{
	m_lCounter = p->m_lCounter;
	m_sXPath = p->m_sXPath;
	m_Xml = p->m_Xml;
}


CXmlParser::~CXmlParser(void)
{
}


void CXmlParser::Init(CXmlFile *Xml)
{
	m_lCounter = 0;
	m_sXPath = _T("");
	m_Xml = Xml;
}


bool CXmlParser::GetAttributeBool(CString strAttribute)
{
	bool b = true;

	CString s = GetAttribute(strAttribute);

	if (L"" == s || L"0" == s)
	{
		b = false;
	}

	return b;

}

int CXmlParser::GetAttributeInt(CString strAttribute)
{
	return ::_ttoi(GetAttribute(strAttribute));
}


bool CXmlParser::SetAttribute(CString strAttribute, int iValue)
{
	CString str;
	str.Format(L"%d", iValue);

	return SetAttribute(strAttribute, str);
}

bool CXmlParser::SetAttribute(CString strAttribute, bool bValue)
{
	if (true == bValue)
	{
		return SetAttribute(strAttribute, L"1");
	}

	return SetAttribute(strAttribute, L"0");

}

long CXmlParser::CounterNext(void)
{
	m_lCounter++;

	if (m_Xml->GetNodeCount(m_sXPath) <= m_lCounter)
	{
		return 0;
	}

	return m_lCounter;

}


long CXmlParser::CounterLast(void)
{
	m_lCounter--;

	if (m_lCounter < 0)
	{
		return 0;	//can't have counter less than 0
	}

	return m_lCounter;

}

long CXmlParser::Select(CString szXPath)
{
	m_sXPath = szXPath;
	CounterReset();
	return m_Xml->GetNodeCount(m_sXPath);
}


long CXmlParser::AddNodeToEndOfList(CString strTagName)
{
	m_lCounter = -1;

	long lResult = m_Xml->AddNodeToEndOfList(m_sXPath, strTagName);

	if (lResult)
	{
		m_lCounter = lResult;
		return true;
	}

	return false;

}


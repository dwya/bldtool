#pragma once

#ifdef PROJ_EXPORTS
#define PROJ_API __declspec(dllexport)
#else
#define PROJ_API __declspec(dllimport)
#endif



/**	\class CProperties
*
*	\brief		-	Xml of currently running properties (our property bag)
*
*	\details	-	Properties are moved into it's own parsing object
*					for speed.  The smaller the xml is the faster access.
*				-	There can only be on set of properties per/running instance
*					so, all functions of this class are static
*
*/
class PROJ_API CProperties
{
public:
	CProperties(void);
	~CProperties(void);

	static bool Init(void);
	static void UnInit(void);
	static UINT InitEvironmentProperties(void);

	static void SaveIt(void);

	static CString Get(__in LPCTSTR szProperty);
	static int GetInt(__in LPCTSTR szProperty);
	static bool GetBool(__in LPCTSTR szProperty);

	static void Set(__in LPCTSTR szProperty, __in LPCTSTR szValue);
	static void Set(__in LPCTSTR szProperty, __in int iValue);
	static void Set(__in LPCTSTR szProperty, __in bool bValue);
	
	static bool EvaluateCondition(__in LPCTSTR szCondition);

	static CString ResolveProperties(__in LPCTSTR szString);

}; //class CProperties


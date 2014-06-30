#pragma once


#ifdef PROJ_EXPORTS
#define PROJ_API __declspec(dllexport)
#else
#define PROJ_API __declspec(dllimport)
#endif

#include <afxmt.h>
#include "..\_Libs\RedHelperFn\RedHelperFnIncludeIt.h"


/**	\class CProjectLog
*
*	\brief		-	The running instances' log file
*
*	\details	-	There can only be one log file so, all functions
*					in this class are static
*				-	class is thread-safe
*
*/
class PROJ_API CProjectLog
{
public:
	CProjectLog(void);
	~CProjectLog(void);

	static void Init(void);
	static void UnInit(void);
	static void Write(__in CString sMsg);

private:
	CRedLogIt m_LogFile;
	CFile m_CFile;
	CCriticalSection m_CriticalSection;

}; //class CProjectLog


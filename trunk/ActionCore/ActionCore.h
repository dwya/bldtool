// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ACTIONCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ACTIONCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ACTIONCORE_EXPORTS
#define ACTIONCORE_API __declspec(dllexport)
#else
#define ACTIONCORE_API __declspec(dllimport)
#endif

// This class is exported from the ActionCore.dll
class ACTIONCORE_API CActionCore {
public:
	CActionCore(void);
	// TODO: add your methods here.
};

extern ACTIONCORE_API int nActionCore;

ACTIONCORE_API int fnActionCore(void);

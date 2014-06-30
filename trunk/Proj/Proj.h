// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PROJ_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PROJ_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PROJ_EXPORTS
#define PROJ_API __declspec(dllexport)
#else
#define PROJ_API __declspec(dllimport)
#endif



// This class is exported from the Proj.dll
class PROJ_API CProj {
public:
	CProj(void);
	// TODO: add your methods here.
};

extern PROJ_API int nProj;

PROJ_API int fnProj(void);

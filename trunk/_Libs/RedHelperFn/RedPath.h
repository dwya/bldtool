#pragma once


enum IS_PATH
{
	eURL,		//determine if the string starts with http
	eEXISTS,	//determine if the path exists
	eVALID,		//determine if path starts with Letter: or double backslash
	eEXE		//determine the file ends with .exe
};

enum ICA_VERSION_INFO
{
	ePRODUCT,	//Get Product Info from GetVersionInfo()
	eFILE,		//Get File Info from GetVersionInfo()
	eMAJOR,		//Get Major version	X.0.0.0
	eMINOR,		//Get Minor version	0.X.0.0
	eBUILD,		//Get Build version	0.0.X.0
	eQFE		//Get QFE version	0.0.0.X
};

enum ICA_VERSION_COMPARE
{
	eERROR,
	eLESS_THAN,
	eGREATER_THAN,
	eEQUAL
};

typedef HRESULT (STDAPICALLTYPE *fnDllRegisterServer)(void);


/** \class CRedPath
*
*	\brief Extends CStringT class, helper functions for file/folder paths
*
*	\details
*	- also defined as CUtlPath
*/
class CRedPath: public CString
{
public:
	CRedPath();
	CRedPath(LPCTSTR szPath);
	CRedPath(CRedPath& oPath);
	virtual ~CRedPath();

public:
	//returns the item's name (C:\Folder\Sub-Folder\) would return Sub-Folder
	CString _GetItemName(void);
	//returns the string with a backslash on the end
	CString _GetPathAsFolder(void);
	//returns the string with no backslash on the end
	CString _GetPathAsFile(void);
	//adds to the path, backslashes are automatically accounted for
	void _AddPath(LPCTSTR szFormat, ...);
	//concatenate oPath onto the current string
	void _AddPath(CRedPath oPath);
	//returns a string representing the folder the item is in
	CString _GetContainingFolder(void);
	//recursively creates directories
	DWORD _CreateDirectory(LPCTSTR szFolder = L"");
	// Recursively deletes empty folders (szFolder and down the folder structure not up)
	void _DeleteEmptyDirectories(LPCTSTR szFolder);
	//determine something about the string see: enum IS_PATH
	bool _Is(IS_PATH eIsWhat);
	//remove the last item from the string
	void _LopOffLastItem(void);
	//removes white space in folder path -- 'C:\ My Path  \' becomes 'C:\My Path\'
	void _TrimFolderNamesInPath(void);
	//delete all files from current path (folder will not be removed)
	void _DeleteAllFiles(bool bRecursive=false);
	//delete all files from a specified path (folder will not be removed)
	void _DeleteAllFiles(LPCTSTR szFolder, bool bRecursive);
	//returns the number of subfolders contained in a specific folder
	UINT CountSubfolders(void);
	//returns a comma-delimited list of all the subfolder names
	CString GetSubfolders(void);
	//returns the shortname for a file (NOTE: file must exist on system)
	CString _GetShortName(void);
	//returns the root drive or share
	CString _GetRoot(void);
	//Gets version information for a file
	DWORD GetVersionInfo(ICA_VERSION_INFO eVersion, DWORD &dwError, ICA_VERSION_INFO eProductOrFile = eFILE);
	//Gets version string
	CString GetVersionInfoString(DWORD &dwError, ICA_VERSION_INFO eProductOrFile = eFILE);
	//Compares 2 version strings
	static ICA_VERSION_COMPARE CompareVersionStrings(LPCTSTR szVersion1, LPCTSTR szVersion2);
	//Set CRedPath object to the path of the user's TempFolder
	bool _SetPathTempFolder(void);
	//Uses SHGetFolderPath() to set the object
	bool _SetPath(int nFolder);
	//Attempts to self-register szDLL using DllRegisterServer
	HRESULT RegisterDLL(LPCTSTR szDLL = L"");
	//copy all files in a folder
	static bool CRedPath::CopyAllFiles(CRedPath SourceFolder, 
											 CRedPath DestFolder, 
											 bool bRecursive = false, 
											 BOOL bFailIfExists = FALSE);
	//returns the file extension example: .txt
	CString GetFileExtension(void);

	CRedPath& operator=(const CString& sPath);
	CRedPath& operator=(CRedPath& oPath);


protected:
	void CopyMembers(CRedPath& oPath);
	DWORD GetVersionInit();
	void SetString( __in_opt PCXSTR pszSrc );

private:
	VS_FIXEDFILEINFO m_vsFileInfo;

}; //class CRedPath


#define CUtlPath CRedPath


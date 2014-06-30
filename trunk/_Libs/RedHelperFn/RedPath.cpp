// RedPath.cpp : implementation file
//

#include "stdafx.h"
#include "RedPath.h"


CRedPath::CRedPath()
{
	m_vsFileInfo.dwSignature = 0;
}

CRedPath::CRedPath(LPCTSTR szPath)
{
	m_vsFileInfo.dwSignature = 0;
	SetString(szPath);
}

CRedPath::CRedPath(CRedPath& oPath)
{
	CopyMembers(oPath);
}

CRedPath::~CRedPath()
{
}

void CRedPath::SetString( __in_opt PCXSTR pszSrc )
{
	m_vsFileInfo.dwSignature = 0;
	CString::SetString(pszSrc);
}


/**
*	\remarks	copy constructor
*/
void CRedPath::CopyMembers(CRedPath& oPath)
{
	if (oPath.m_vsFileInfo.dwSignature == VS_FFI_SIGNATURE)
	{
		m_vsFileInfo = oPath.m_vsFileInfo;
	//	::CopyMemory( &m_vsFileInfo, (LPVOID)oPath.m_vsFileInfo, sizeof( VS_FIXEDFILEINFO ) );
	}
	else
	{
		oPath.m_vsFileInfo.dwSignature = 0;
	}

	SetString(oPath.GetString());
	
} //CRedPath::CopyMembers()


// CRedPath member functions

/**
* \arg
*
* \retval CString {value of CRedPath WITHOUT a last backslash (\\)}
*
* \remarks
* CRedPath string is not changed
*/
CString CRedPath::_GetPathAsFile(void)
{
	//make sure there is no trailing backslash
	CString strTemp = GetString();
	strTemp.TrimRight(L"\\");
	return strTemp;
}


/**
* \arg
*
* \retval CString {value of CRedPath WITH a last backslash (\\)}
*
* \remarks
* CRedPath string is not changed
*/
CString CRedPath::_GetPathAsFolder(void)
{
	//make sure there IS a trailing backslash
	CString strTemp = GetString();
	strTemp.TrimRight(L"\\");
	strTemp.AppendChar(L'\\');
	return strTemp;
}


/**
* \arg LPCTSTR szFormat
*
* \remarks
* Appends szFormat to the existing value and formates the string as a path
* CRedPath string is changed
* \code
* CString sMyString = L"My String";
* CRedPath myPath = L"C:\\TestIt"; 
*
* myPath._AddPath(L"Add To Path\\%s\\Here", sMyString);
* myPath is == L"C:\\TestIt\\Add To Path\\My String\\Here";
* \endcode
*/
void CRedPath::_AddPath(LPCTSTR szFormat, ...)
{
	CString strRoot = GetString();
	strRoot.TrimRight(_T("\\"));

	CString strAppend = L"";
	
	va_list args;
	va_start(args, szFormat);

	strAppend.FormatV(szFormat, args);
	va_end(args);

	strAppend.TrimLeft(L"\\");

	if (L"" == strRoot)
	{
		SetString(strAppend);
	}
	else
	{
		Format(L"%s\\%s", strRoot, strAppend);
	}
}


/**
* \arg CRedPath oPath
*
* \remarks
* concatenate another CRedPath to the end
* CRedPath string is changed
* - NOTE: the caller needs to remove the drive letter before calling
*
* \code
* CRedPath myPath1 = L"C:\\Path 1"; 
* CRedPath myPath2 = L"C:\\Path 2"; 
*
* myPath1._AddPath(myPath2);
* myPath1 is == L"C:\\Path 1\\C:\\Path 2";
* \endcode
*/
void CRedPath::_AddPath(CRedPath oPath)
{
	CString strRoot = GetString();
	strRoot.TrimRight(L"\\");

	CString strAppend = oPath.GetString();
	strAppend.TrimLeft(L"\\");

	Format(L"%s\\%s", strRoot, strAppend);
}


/**
* \arg
*
* \retval CString representing the containing folder
*
* \remarks
* CRedPath string is not changed
*
* \code
* CRedPath myPath = L"C:\\Folder\\SubFolder\\"; 
*
* // myPath._GetContainingFolder(); == L"C:\\Folder\\";
* \endcode
*/
CString CRedPath::_GetContainingFolder(void)
{
	CString strReturn = L"";
	CString strTemp = GetString();
	strTemp.TrimRight(L'\\');

	int iPos = strTemp.ReverseFind(L'\\') + 1;
	if (iPos > 0)
	{
		strReturn = strTemp.Mid(0, iPos);
	}

	return strReturn;
}



/**
* \arg
*
* \remarks
* removes the last item from the string
* CRedPath string is changed
*
* \code
* CRedPath myPath1 = L"C:\\Folder\\SubFolder\\"; 
* CRedPath myPath2 = L"C:\\Folder\\SubFolder\\My File.txt"; 
*
* myPath1._LopOffLastItem();
* myPath2._LopOffLastItem();
*
* // myPath1 == L"C:\\Folder\\";
* // myPath1 == L"C:\\Folder\\SubFolder\\";
* \endcode
*/
void CRedPath::_LopOffLastItem(void)
{
	CString strPath = _GetPathAsFile();
	CString strTemp = strPath.Mid(0, strPath.ReverseFind(L'\\'));
	SetString(strTemp);
}


/**
* \arg [in] LPCTSTR szFolder
*
* \remarks
* recursively deletes empty folders only from the system
* CRedPath string is not changed
*
* \code
* CRedPath myPath1 = L"C:\\Folder\\SubFolder\\"; 
*
* myPath1._DeleteEmptyDirectories(myPath._GetPathAsFolder())
*
* // function attempts to delete any empty folders in
* // C:\\Folder\\SubFolder\\ -- "SubFolder\\" is not removed
*
* \endcode
*/
void CRedPath::_DeleteEmptyDirectories(LPCTSTR szFolder)
{
	CRedPath oFolderSearch = szFolder;

	if (!oFolderSearch._Is(eVALID))
	{
		return;
	}

	oFolderSearch._AddPath(L"*.*");
	
	CFileFind oFind;
	if (!oFind.FindFile(oFolderSearch.GetString()))
	{
		return;		//didn't find any matching file(s)
	}

	CString sFile;
	int iFiles = 0;
	
	do
	{
		iFiles = oFind.FindNextFile();

		if (!oFind.IsDots())
		{
			sFile = oFind.GetFilePath();

			if (oFind.IsDirectory())
			{
				//this is a sub-folder, recall function to attempt delete
				_DeleteEmptyDirectories(sFile);
			}

			sFile = L"";

		} // if (!oFind.IsDots())

	} while (iFiles > 0);

	oFind.Close();

	//all done, try to delete this folder
	oFolderSearch._LopOffLastItem();
	::RemoveDirectory(oFolderSearch._GetPathAsFile());

} //CRedPath::_DeleteEmptyDirectories


/**
* \arg [in] LPCTSTR szFolder
*
* \remarks
* recursively creates a folder
* CRedPath string is not changed
*
* \code
* CRedPath myPath1 = L"C:\\Folder\\SubFolder1\\SubFolder2"; 
*
* myPath1._CreateDirectory(myPath1._GetPathAsFolder())
*
* // if the system only contains C:\\
* // the _CreateDirectory will create the full path 
* // C:\\Folder\\SubFolder1\\SubFolder2
*
* \endcode
*/
DWORD CRedPath::_CreateDirectory(LPCTSTR szFolder)
{
	CString sFolder	= szFolder;
	if (L"" == sFolder)
	{
		//sFolder = GetString();
		sFolder = _GetPathAsFolder();
	}

	CString sPath	= L"";
	DWORD dwReturn	= ERROR_SUCCESS;
	int iStart		= 1;

	while(iStart > 0)
	{
		iStart = sFolder.Find(L"\\", iStart);
		iStart++;
		sPath = sFolder.Left(iStart);
		if (L"" != sPath)
		{
			::CreateDirectory(sPath, NULL);
			dwReturn = ::GetLastError();
		}
	} //while(iStart > 0)

	return dwReturn;

} //CRedPath::_CreateDirectory


/**
* \arg [in] CString
*
* \retval CRedPath
*
* \remarks
* copy constructor
*
*/
CRedPath& CRedPath::operator=(const CString& sPath)
{
	SetString(sPath.GetString());
	return *this;
}


/**
* \arg [in] CRedPath
*
* \retval CRedPath
*
* \remarks
* copy constructor
*
*/
CRedPath& CRedPath::operator=(CRedPath& oPath)
{
	CopyMembers(oPath);
	return *this;
}


/**
* \arg [in] IS_PATH eIsWhat
*
* \retval (true/false)
*
* \remarks
* - determines if the CRedPath string is something
* \code
* - eIsWhat can be;
* eURL		//determine if the string starts with http
* eEXISTS	//determine if the path exists
* eVALID	//determine if path starts with Letter: or double backslash
* eEXE		//determine the file ends with .exe
* \endcode
*
* \code
* CRedPath myPath1 = L"C:\\Folder\\SubFolder1\\SubFolder2"; 
* if (myPath1._Is(eEXISTS))
* {
*	//path exists ...
* }
* \endcode
*
* \todo
* FUTURE: add eREADABLE, eWRITEABLE, eIS_NETWORKDRIVE
*/
bool CRedPath::_Is(IS_PATH eIsWhat)
{
	bool bReturn = false;

	CString sTemp = GetString();
	if ( (L"" == sTemp) ||
		 (sTemp.IsEmpty()) )
	{
		return false;	//String is empty, doesn't matter what
						//we are looking for, it is false
	}

	switch(eIsWhat)
	{
	case eURL:
		{
			sTemp.MakeLower();
			if (L"http" == sTemp.Left(4))
			{
				bReturn = true;
			}
			break;
		} //case eURL:

	case eEXISTS:
		{
			if (_Is(eVALID))
			{
				sTemp = this->_GetPathAsFile();
				CFileFind oFind;
			
				if (oFind.FindFile(sTemp))
				{
					bReturn = true;		//path found
					oFind.Close();
				}
				else
				{
					//didn't find path, see if we are suppose to
					//be looking for a folder -- append *.* to the
					//end so we look for all files in a folder path
					sTemp.Append(L"\\*.*");
					if (oFind.FindFile(sTemp))
					{
						bReturn = true;	//path found
						oFind.Close();
					}
				}
			} //if Is(eVALID)

			break;
		} //case eEXISTS:

	case eVALID:
		{
			//just make sure it starts with either
			//<Letter>: or a double backslash
			TCHAR szChar1 = sTemp.GetAt(0);
			TCHAR szChar2 = sTemp.GetAt(1);
			if (L"" != sTemp)
			{
				if (_istalpha(szChar1) &&
					L':' == szChar2)
				{
					bReturn = true;
				}
				else if (L'\\' == szChar1 &&
						 L'\\' == szChar2)
				{
					bReturn = true;
				}
			}

			break;

		} //case eVALID:

	case eEXE:
		{
			sTemp = GetString();
			sTemp.MakeLower();

			if (sTemp.Right(4) == L".exe")
			{
				bReturn = true;
			}
		}

		break;	//case eEXE:

	} //switch(eIsWhat)

	return bReturn;

} //CRedPath::_Is


/**
* \arg bool bRecursive
*
* \remarks
* - If the string represeted by CRedPath is a valid folder,
* then the function removes all files
* - set bRecursive to true to delete files in sub-folders, the
* default is false
*
*/
void CRedPath::_DeleteAllFiles(bool bRecursive)
{
	CString sPath = _GetPathAsFolder();
	_DeleteAllFiles(sPath, bRecursive);
}

void CRedPath::_DeleteAllFiles(LPCTSTR szFolder, bool bRecursive)
{
	CRedPath oFolder	= szFolder;

	CFileFind oFind;
	
	CFileStatus oStatus;
	oStatus.m_attribute = 0x00;	//normal attributes

	CString sFile = _T("");

	CString sFolder = oFolder.GetString();
	if (L"" == sFolder)
	{
		return;
	}

	oFolder._AddPath(L"*.*");

	if (oFind.FindFile(oFolder.GetString()))
	{
		int iFiles = 0;

		do
		{
			iFiles = oFind.FindNextFile();

			if (!oFind.IsDots())
			{
				sFile = oFind.GetFilePath();

				if (oFind.IsDirectory())
				{
					if (true == bRecursive)
					{
						_DeleteAllFiles(sFile, true);
					}
				}
				else
				{
					CFile::SetStatus(sFile, oStatus);

					if (!::DeleteFile(sFile))
					{
						//log something if you want
					}

				} // is a directory

				sFile = L"";

			} // if (!oFind.IsDots())

		} while (iFiles > 0);

		oFind.Close();

	} // if (oFind.FindFile(sFile))

	//remove the folder
	::RemoveDirectory(szFolder);

} //CRedPath::_DeleteAllFiles

/**
* \todo
* FUTURE: document GetSubfolders()
*/
CString CRedPath::GetSubfolders(void)
{
	CString sFolder			= GetString();
	CRedPath oFolder		= GetString();
	CString csFolderlist = L"", csFolderName = L"";

	CFileFind oFind;

	CString sFile = _T("");

	if (L"" == sFolder)
	{
		return csFolderlist;
	}

	if (!oFolder._Is(eVALID))
	{
		return csFolderlist;
	}

	sFolder.Append(L"*.*");
	oFolder._AddPath(L"*.*");

	if (oFind.FindFile(oFolder._GetPathAsFile()))
	{
		int iFiles = 0;

		do
		{
			iFiles = oFind.FindNextFile();

			if (!oFind.IsDots())
			{
				sFile = oFind.GetFilePath();

				if (oFind.IsDirectory())
				{
					csFolderName = sFile.Mid(sFile.ReverseFind (L'\\')+1);
					if (csFolderlist == L"")
						csFolderlist = csFolderName;
					else
						csFolderlist = csFolderlist + L"," + csFolderName;
				} // is a directory
				csFolderName = L"";
				sFile = L"";

			} // if (!oFind.IsDots())

		} while (iFiles > 0);

		oFind.Close();

	} // if (oFind.FindFile(sFile))

	return csFolderlist;

} //CRedPath::GetSubfolders

/**
* \todo
* FUTURE: document CountSubfolders()
*/
UINT CRedPath::CountSubfolders(void)
{
	CString sFolder		= GetString();
	CRedPath oFolder	= GetString();
	UINT uiFolderCount	= 0;

	CFileFind oFind;

	CString sFile = _T("");

	if (L"" == sFolder)
	{
		return -1;
	}

	if (!oFolder._Is(eVALID))
	{
		return -1;
	}

	sFolder.Append(L"*.*");
	oFolder._AddPath(L"*.*");

	if (oFind.FindFile(oFolder._GetPathAsFile()))
	{
		int iFiles = 0;

		do
		{
			iFiles = oFind.FindNextFile();

			if (!oFind.IsDots())
			{
				sFile = oFind.GetFilePath();

				if (oFind.IsDirectory())
				{
					uiFolderCount++;
				} // is a directory

				sFile = L"";

			} // if (!oFind.IsDots())

		} while (iFiles > 0);

		oFind.Close();

	} // if (oFind.FindFile(sFile))
	
	return uiFolderCount;

} //CRedPath::CountSubfolders



/**
* \arg
*
* \retval CString (the name of folder or file)
*
* \remarks
* \code
* CRedPath oPath1 = L"C:\\My Folder\\My File.txt
* CRedPath oPath2 = L"C:\\My Folder\\Sub-folder\\
* CString str1 = oPath1._GetItemName()
* CString str2 = oPath2._GetItemName()
* \\str1 is == My File.txt
* \\str2 is == Sub-folder
* \endcode
*/
CString CRedPath::_GetItemName(void)
{
	CString sReturn = L"";

	CString sTemp = _GetPathAsFile();

	int iPos = sTemp.ReverseFind(L'\\');

	if (iPos > 0)
	{
		sReturn = sTemp.Mid(iPos + 1);
	}
	else
	{
		sReturn = GetString();
	}

	return sReturn;

} //CRedPath::_GetItemName



/**
* \arg
*
* \retval CString (drive letter or share)
*
* \remarks
* - returns the root drive or share name, with a last backslash
*/
CString CRedPath::_GetRoot(void)
{
	CString str = _GetPathAsFile();
	CString strReturn = L"";

	//start at the 2nd char in case this is a
	//UNC path
	int iPos = 2;
	iPos = str.Find(L'\\', iPos);
	
	if (iPos >= 0)
	{
		iPos++;
		strReturn = str.Left(iPos);
	}
	else
	{
		//already a root path
		strReturn = str;
	}

	//make sure we are always returning with a last backslash
	CRedPath oReturn = strReturn;

	return oReturn._GetPathAsFolder();

} //CRedPath::_GetRoot


/**
* \arg
*
* \remarks
* used to remove empty spaces around folder/file paths
*
* \code
* CRedPath myPath = L"C:\ My Folder\ Path \"
*
* //paths with leading or trialing spaces are invalid
* myPath._TrimFolderNamesInPath()
*
* //myPath is == L"C:\My Folder\Path\"
* \endcode
*/
void CRedPath::_TrimFolderNamesInPath(void)
{
	CString s = GetString();
	CString s1, s2;

	INT iStart = 0;
	do
	{
		s1 = s.Tokenize(L"\\", iStart);
		if (L"" == s1)
		{
			break;
		}

		s1.Trim();
		if (L"" != s1)
		{
			s2.AppendFormat(L"%s\\", s1);
		}

	} while (iStart != -1);

	SetString(s2);

	if (!_Is(eVALID))
	{
		//add double slashes to the front it is a UNC path
		s2.Format(L"\\\\%s", GetString());
		SetString(s2);
	}

} //CRedPath::_TrimFolderNamesInPath


/**
* \arg
*
* \retval CString (the short path)
*
* \remarks
* - NOTE: The path must exist on the system
*
* \code
* CRedPath PathExists = L"C:\\Test Folder\\Test File.txt"
* CRedPath PathNOTExists = L"C:\\Test Folder\\Test File No Such File.txt"
*
*	\\PathExists._GetShortName() returns -> L"C\\Test~1\\Test~1.txt"
*	\\PathNOTExists._GetShortName() returns -> L""
* \endcode
*
* \todo
* FUTURE: make return short path even if the path doesn't exist
*/
CString CRedPath::_GetShortName(void)
{
	CString str = L"";

	TCHAR szPath[MAX_PATH] = {NULL};

	if (GetShortPathName(GetString(), szPath, MAX_PATH))
	{
		str = szPath;
	}

	return str;

}



/**
* \arg
*
* \retval DWORD (ERROR_SUCCESS if success) or an error code
*
* \remarks
* - attempts to populate m_vsFileInfo with a file's version information
* - called internally see: GetVersionInfo()
*/
DWORD CRedPath::GetVersionInit(void)
{
	if (VS_FFI_SIGNATURE == m_vsFileInfo.dwSignature)
	{
		return ERROR_SUCCESS;	//already initalized
	}

	CString str = GetString();
	if (L"" == str)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	DWORD dwHandle	= 0;
	DWORD dwSize	= ::GetFileVersionInfoSize(str, &dwHandle);
	if (dwSize <= 0)
	{
		return ::GetLastError();
	}

	UINT uiResult = ERROR_SUCCESS;
	BYTE *pData = new BYTE[dwSize];

	if (pData)
	{
		uiResult = ::GetFileVersionInfo(str, 0, dwSize, pData);
		if (uiResult > 0)
		{
			LPVOID lpVSFFI = NULL;
			UINT uiSize = 0;
			uiResult = ::VerQueryValue(pData, L"\\", (LPVOID*)&lpVSFFI, &uiSize);
			if (uiResult > 0)
			{
				::CopyMemory( &m_vsFileInfo, lpVSFFI, sizeof( VS_FIXEDFILEINFO ) );
			}

		}

		delete [] pData;
	}
	else
	{
		return ERROR_OUTOFMEMORY;
	}

	if (uiResult <= 0)
	{
		uiResult = ::GetLastError();
	}

	if (uiResult > 0)
	{
		uiResult = ERROR_SUCCESS;
	}

	return uiResult;

} //CRedPath::GetVersionInfo()



/**
* \arg	[in]	ICA_VERSION_INFO eVersion	- eMAJOR for Major version	X.0.0.0
*											- eMINOR for Minor version	0.X.0.0
*											- eBUILD for Build version	0.0.X.0
*											- eQFE for HotFix version 0.0.0.X
*		[out]	DWORD &dwError	- ERROR_SUCCESS if function was successfull
*								- else an error code
*		[in]	ICA_VERSION_INFO eProductOrFile - use ePRODUCT or eFILE (depending if you want product or file version info)
*
* \retval DWORD (version information)
*
* \remarks
* - Used to retreive version information
*
* \code
//My.dll version is 2.0.11.25
DWORD dwResult = 0;
CRedPath myFile = L"C:\\Test Folder\\My.dll"

DWORD dwMajorVersion = myFile.GetVersionInfo(eMAJOR, dwResult);
if (ERROR_SUCCESS == dwResult)
{
	//dwMajorVersion is 2
}

* \endcode
*/
DWORD CRedPath::GetVersionInfo(ICA_VERSION_INFO eVersion, DWORD &dwError, ICA_VERSION_INFO eProductOrFile)
{
	//initalize VS_FIXEDFILEINFO structure
	dwError = GetVersionInit();
	if (ERROR_SUCCESS != dwError)
	{
		return 0;
	}

	switch(eVersion)
	{
	case eMAJOR:
		{
			if (ePRODUCT == eProductOrFile)
				return HIWORD(m_vsFileInfo.dwProductVersionMS);
			else
				return HIWORD(m_vsFileInfo.dwFileVersionMS);
		}
		break;

	case eMINOR:
		{
			if (ePRODUCT == eProductOrFile)
				return LOWORD(m_vsFileInfo.dwProductVersionMS);
			else
				return LOWORD(m_vsFileInfo.dwFileVersionMS);
		}
		break;

	case eBUILD:
		{
			if (ePRODUCT == eProductOrFile)
				return HIWORD(m_vsFileInfo.dwProductVersionLS);
			else
				return HIWORD(m_vsFileInfo.dwFileVersionLS);
		}
		break;

	case eQFE:
		{
			if (ePRODUCT == eProductOrFile)
				return LOWORD(m_vsFileInfo.dwProductVersionLS);
			else
				return LOWORD(m_vsFileInfo.dwFileVersionLS);
		}
		break;

	default:
		{
			dwError	= ERROR_INVALID_PARAMETER;
		}
		break;

	} //switch(eVersion)

	return 0;

} //CRedPath::GetVersionInfo


/**
* \arg	[out]	DWORD &dwError	- ERROR_SUCCESS if function was successfull
*								- else an error code
*		[in]	ICA_VERSION_INFO eProductOrFile - use ePRODUCT or eFILE (depending if you want product or file version info)
*
* \retval CString (version string  XX.XX.XX.XX)
*
* \remarks
*
* \code
//My.dll version is 2.0.11.25
DWORD dwResult = 0;
CRedPath myFile = L"C:\\Test Folder\\My.dll"

CString str = myFile.GetVersionInfo(eMAJOR, dwResult);
if (ERROR_SUCCESS == dwResult)
{
	//str is 2.0.11.25
}

* \endcode
*/
CString CRedPath::GetVersionInfoString(DWORD &dwError, ICA_VERSION_INFO eProductOrFile)
{
	CString strReturn = L"0.0.0.0";

	//initalize VS_FIXEDFILEINFO structure
	dwError = GetVersionInit();
	if (ERROR_SUCCESS != dwError)
	{
		return strReturn;
	}
	
	strReturn.Format(L"%d.%d.%d.%d",
					 GetVersionInfo(eMAJOR, dwError, eProductOrFile), 
					 GetVersionInfo(eMINOR, dwError, eProductOrFile), 
					 GetVersionInfo(eBUILD, dwError, eProductOrFile), 
					 GetVersionInfo(eQFE, dwError, eProductOrFile));

	return strReturn;

} //CRedPath::GetVersionInfoString


/**
* \arg	[in]	LPCTSTR strVersion1	(version string)
*		[in]	LPCTSTR strVersion2	(version string to compare)
*
* \retval ICA_VERSION_COMPARE
	- eERROR		(strVersion1 cannot be compared against strVersion2)
	- eLESS_THAN	(strVersion1 < strVersion2)
	- eGREATER_THAN (strVersion1 > strVersion2)
	- eEQUAL		(strVersion1 == strVersion2)
*
* \remarks
*
*/
ICA_VERSION_COMPARE CRedPath::CompareVersionStrings(LPCTSTR szVersion1, LPCTSTR szVersion2)
{
	ICA_VERSION_COMPARE eResult = eERROR;

	// parse strVersion1 into numeric values
	int iMajor1 = 0;
	int iMinor1 = 0;
	int iBuild1 = 0;
	int iQFE1 = 0;

	INT iStart = 0;
	int iCnt = 0;

	CString strVersion = szVersion1;
	CString str = L"";
	do
	{
		iCnt++;
		str = strVersion.Tokenize(L".", iStart);

		if (L"" == str)
			break;

		if (1 == iCnt)
			iMajor1 = ::_ttoi(str);
		else if (2 == iCnt)
			iMinor1 = ::_ttoi(str);
		else if (3 == iCnt)
			iBuild1 = ::_ttoi(str);
		else if (4 == iCnt)
			iQFE1 = ::_ttoi(str);

	} while(iStart != -1);

	// parse strVersion2 into numeric values
	int iMajor2 = 0;
	int iMinor2 = 0;
	int iBuild2 = 0;
	int iQFE2 = 0;

	iStart = 0;
	iCnt = 0;

	strVersion = szVersion2;
	str = L"";
	do
	{
		iCnt++;
		str = strVersion.Tokenize(L".", iStart);

		if (L"" == str)
			break;

		if (1 == iCnt)
			iMajor2 = ::_ttoi(str);
		else if (2 == iCnt)
			iMinor2 = ::_ttoi(str);
		else if (3 == iCnt)
			iBuild2 = ::_ttoi(str);
		else if (4 == iCnt)
			iQFE2 = ::_ttoi(str);

	} while(iStart != -1);

	//compare them -- major version
	if (iMajor1 < iMajor2)
		return eLESS_THAN;
	else if (iMajor1 > iMajor2)
		return eGREATER_THAN;

	//else major version numbers match, compare minor
	if (iMinor1 < iMinor2)
		return eLESS_THAN;
	else if (iMinor1 > iMinor2)
		return eGREATER_THAN;

	//else major AND minor version numbers match, compare build
	if (iBuild1 < iBuild2)
		return eLESS_THAN;
	else if (iBuild1 > iBuild2)
		return eGREATER_THAN;

	//else major, minor AND build version numbers match, compare QFE
	if (iQFE1 < iQFE2)
		return eLESS_THAN;
	else if (iQFE1 > iQFE2)
		return eGREATER_THAN;

	
	return eEQUAL;	//all match

} //CRedPath::CompareVersionStrings()



/**
* \arg	[in]	int nFolder (see SHGetFolderPath()'s nFolder for values 
*				http://msdn.microsoft.com/en-us/library/bb762181(VS.85).aspx
*
* \retval	true / false (true if the successful)
*
* \remarks
* - Sets the object to the value of SHGetFolderPath
* - No change to the object if _SetPath fails
*
*/
bool CRedPath::_SetPath(int nFolder)
{
	bool b = false;

	TCHAR szPath[MAX_PATH] = {NULL};

	if (SUCCEEDED(SHGetFolderPath(NULL, 
								  nFolder, 
								  NULL, 
								  0, 
								  szPath)))
	{
		CString str = szPath;
		str.TrimRight(L"\\");
		str.AppendChar(L'\\');

		SetString(str);
		b = true;
	}

	return b;

} //CRedPath::SetPath



/**
* \arg
*
* \retval	true / false (true if the successful)
*
* \remarks
* - Sets to object to the [TempFolder]
* - No change to the object if _SetPath fails
*
*/
bool CRedPath::_SetPathTempFolder(void)
{
	bool b = false;

	TCHAR szPath[MAX_PATH] = {NULL};

	if (GetTempPath(MAX_PATH, szPath) > 0)
	{
		CString str = szPath;
		str.TrimRight(L"\\");
		str.AppendChar(L'\\');

		SetString(str);
		b = true;
	}

	return b;

} //CRedPath::_TempFolder


/**
* \arg		LPCTSTR szDLL (.dll to register)
*
* \retval	HRESULT from DllRegisterServer
*
* \remarks
* - attempts to call DllRegisterServer on szDLL
*/
HRESULT CRedPath::RegisterDLL(LPCTSTR szDLL)
{
	HRESULT hr = E_NOTIMPL;

	CString sDll = szDLL;
	if (L"" == sDll)
	{
		sDll = GetString();
	}

	HMODULE hDLL = ::LoadLibrary(sDll);

	if (hDLL)
	{
		fnDllRegisterServer pfn = (fnDllRegisterServer) ::GetProcAddress(hDLL, "DllRegisterServer");

		if (pfn)
		{
			hr = pfn();
		}

		::FreeLibrary(hDLL);

	} //if (hDLL)

	return hr;

} //CRedPath::RegisterDLL


/**
* \arg		CRedPath SourceFolder
* \arg		CRedPath DestFolder
* \arg		bool bRecursive		(copy sub-folders)
* \arg		bool bFailIfExists	(overwrite)
*
* \retval	bool (true means files were copied)
*
*/
bool CRedPath::CopyAllFiles(CRedPath SourceFolder, 
								  CRedPath DestFolder, 
								  bool bRecursive /* = false */, 
								  BOOL bFailIfExists /* = FALSE */)
{
	bool bDestFolderCreated = false;

	bool bReturn = true;

	CFileFind oFind;

	if ( (L"" == SourceFolder.GetString()) || 
		 (L"" == DestFolder.GetString()) )
	{
		return false;		//no folder
	}

	CRedPath SourceFiles, DestFile;

	CString sFileName;

	SourceFiles = SourceFolder.GetString();
	SourceFiles._AddPath(L"*.*");

	if (oFind.FindFile(SourceFiles.GetString()))
	{
		int iFiles = 0;

		do
		{
			iFiles = oFind.FindNextFile();

			if (!oFind.IsDots())
			{
				
				sFileName = oFind.GetFileName();

				if (oFind.IsDirectory())
				{
					if (true == bRecursive)
					{
						CRedPath NewSource = oFind.GetFilePath();
						
						CRedPath NewDest	 = DestFolder;
						NewDest._AddPath(sFileName);

						if (!CopyAllFiles(NewSource, NewDest, true))
						{
							bReturn = false;
							iFiles = 0;
							break;
						}
					}
				}
				else
				{
					if (!bDestFolderCreated)
					{
						DestFolder._CreateDirectory();
						bDestFolderCreated = true;
					}

					DestFile = DestFolder;
					DestFile._AddPath(sFileName);

					if (!CopyFile(oFind.GetFilePath(), DestFile.GetString(), bFailIfExists))
					{
						bReturn = false;
						iFiles = 0;
						break;
					}
				}

				sFileName = L"";

			} // if (!oFind.IsDots())

		} while (iFiles > 0);

		oFind.Close();

	} // if (oFind.FindFile(sFiles))

	return bReturn;

} //CRedPath::CopyAllFiles


/**
*	\remarks	-	returns the file extension example; .txt
*
*/
CString CRedPath::GetFileExtension(void)
{
	CString str = GetString();
	
	int pos = str.ReverseFind(L'.');
	int len = str.GetLength();

	return str.Right(len - pos);

} //CRedPath::GetFileExtension()


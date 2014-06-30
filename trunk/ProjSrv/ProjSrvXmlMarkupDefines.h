#pragma once

/**
*	\remarks	-	ProjSrv\\ProjSrvXmlMarkupDeines.h
*/


/**
*	\remarks	-	node names
*/
#define NODE_ROOT				L"BuildIt"
#define NODE_SEQUENCE			L"Sequence"
#define NODE_ACTIONS			L"Actions"
#define NODE_ACTION				L"Action"
#define NODE_PROPERIES			L"Properties"
#define NODE_PROPERTY			L"Property"


/**
*	\remarks	-	xpaths
*/
#define XPATH_ROOT				L"//BuildIt"
#define XPATH_SEQUENCE			L"//BuildIt/Sequence"
#define XPATH_SEQUENCE_ACTION	L"//BuildIt/Sequence/Action"
#define XPATH_PROPERTIES		L"//BuildIt/Properties"
#define XPATH_PROPERTY			L"//BuildIt/Properties/Property"
#define XPATH_ACTIONS_ACTION	L"//BuildIt/Actions/Action"


/**
*	\remarks	-	attributes
*/
#define ATT_CMDLINE		L"cmdline"
#define ATT_CONDITION	L"condition"
#define ATT_DISPLAY		L"display"
#define ATT_ENTRYPOINT	L"entrypoint"
#define ATT_FAILACTION	L"failaction"
#define	ATT_NAME		L"name"
#define ATT_PROPERTY	L"property"
#define ATT_SCRIPT		L"script"
#define ATT_TYPE		L"type"
#define ATT_WAIT		L"wait"
#define	ATT_VALUE		L"value"


/**
*	\remarks	-	string to bool handleling, EMPTY or ZERO we concider it false, all other values are true
*/
#define	EMPTY	L""		//false property
#define	ZERO	L"0"	//false property
#define ONE		L"1"	//true property (1 or any other value)


/**
*	\remarks	-	properties defined by build engine
*/
#define rLogFile			L"rLogFile"				//full path and file name for log file
#define rProductFolder		L"rProductFolder"		//folder BuildIt.exe is running in
#define rProjectFolder		L"rProjectFolder"		//folder <project>.Xml is from
#define rShowDisplayMsg		L"rShowDisplayMsg"		//if true, the sequencer shows message boxes

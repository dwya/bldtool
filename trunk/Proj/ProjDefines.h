#pragma once

/* ROT object names */
#define ROTID_BUILDIT		L"BuildIt.%s"	//%s replaced with runtime GUID
#define ROTID_PROJECT		L"%s.Project"	//%s replaced with ROTID_BUILDIT (BuildIt.GUID)
#define ROTID_PROPERTIES	L"%s.Properties"
#define ROTID_SEQUENCE		L"%s.Sequence"

/* object filter names (must match ROTID, without %s.) */
#define XML_PROJECT			L"Project"
#define XML_PROPERTIES		L"Properties"
#define XML_SEQUENCE		L"Sequence"
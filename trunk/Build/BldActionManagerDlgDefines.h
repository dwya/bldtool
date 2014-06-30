#pragma once

#include "BuildDefines.h"

#define THREAD_CONTROL_REGISTERED_MSG	L"rOnThreadControlMsg-"BUILD_GUID

//thread control message
static const UINT THREAD_CONTROL_MSG = ::RegisterWindowMessage(THREAD_CONTROL_REGISTERED_MSG);


//wParam values for THREAD_CONTROL_MSG (see: OnThreadControlMsg())
enum THREAD_CONTROL_MSG_FN
{
	eSequenceStart,
	eSequenceFinish,
	eGetStatus,
	eActionStart,
	eActionFinish

}; //enum THREAD_CONTROL_MSG_FN


#define MAX_RUNNING_THREADS	50	//maxium of 50 simultaneous actions



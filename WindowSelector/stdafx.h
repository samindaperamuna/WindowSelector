// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// Application headers
#include <iostream>
#include <string>
#include <oleacc.h>
#include <oleAuto.h>
#include <time.h>
#include <ExDisp.h>
#include <atlbase.h>
#include <atlcom.h>
#include <UIAutomation.h>

#pragma warning(disable: 4192)
#pragma warning(disable: 4146)
#import <mshtml.tlb>
#import <shdocvw.dll>
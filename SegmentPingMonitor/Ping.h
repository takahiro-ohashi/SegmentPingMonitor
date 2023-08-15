#pragma once
#include "pch.h"
#include <afxwin.h>
#include "ChildView.h"
#include <windows.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <icmpapi.h>

// Need to link with Iplhlapi.lib
#pragma comment(lib, "IPHLPAPI.lib")
class Ping
{
public:
	//int validateAddress(TCHAR* address);
	int gui_ping(CChildView* pView, long row);
};
#pragma once
#ifndef __THREADMAKER
#define __THREADMAKER
#include <afxwin.h>
#include "pch.h"
#include "CThread.h"
#include "CHildView.h"
#include <iostream>
#include <string>
#include <algorithm>
//#include "CThread.h"
//#include "MultiThreadPingView.h"
//#include "ThreadMaker.h"
//#include "PingWorker.h"


class ThreadMaker : public CThread 
{
public:
	CChildView* pView;
	int id;
	ThreadMaker(int i);
	ThreadMaker(LPVOID viewPointer, int n);
	virtual DWORD ThreadFunc();
};
#endif

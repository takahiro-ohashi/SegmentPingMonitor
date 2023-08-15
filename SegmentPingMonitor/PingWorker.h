#pragma once
#include <afxwin.h>
#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "CThread.h"
#include "ChildView.h"
//#include "CThread.h"
//#include "MultiThreadPingView.h"

class PingWorker :public CThread
{
	int id;
	CChildView* pView;
public:
	PingWorker(int n);
	PingWorker(LPVOID viewPointer, int n);
	virtual DWORD ThreadFunc();
};
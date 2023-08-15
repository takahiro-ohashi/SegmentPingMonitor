#include <afxwin.h>
#include "pch.h"
//#include "CThread.h"
//#include "MultiThreadPingView.h"
#include "vector"
#include "set"
#include "regex"
#include "cmath"
#include "PingWorker.h"
#include "Ping.h"

PingWorker::PingWorker(int n) : CThread()
{
	interval(0);
	id = n;
}
PingWorker::PingWorker(LPVOID viewPointer, int n) : CThread(viewPointer) 
{
	interval(0);
	id = n;
}
DWORD PingWorker::ThreadFunc()
{
	pView = (CChildView*)view;
	bool result = false;
	Ping pin;
	pin.gui_ping(pView, id);
	return 0;
}
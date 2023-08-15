#pragma once
//#include <winnt.h>
//#include <minwindef.h>
#ifndef __CTHREAD
#define __CTHREAD
//#include "StdAfx.h"
//void _ThreadFunc(LPVOID* param);
#include <afxwin.h>

class CThread
{
public:
	HANDLE threadHandle;
	LPVOID view;
	LPVOID arg;
	CWinThread* thread;
	int running;
	int stime;
	int stopreq;
	int suspension;
	CThread();
	CThread(LPVOID viewPointer);
	void setArg(LPVOID pArg) {
		arg = pArg;
	}
	virtual ~CThread();
	virtual DWORD ThreadFunc();
	virtual void start();
	virtual void stop();
	virtual void kill();

	void setDialog(LPVOID viewPointer) {
		view = viewPointer;
	}
	void interval(int t) { stime = t; }
	int interval() { return stime; }
	void priority(int n);
	int priority();
	void suspend();
	void resume();
	void sleep(int n);
};
class CThreads
{
public:
	DWORD NoThreadsMax;
	DWORD NoThreads;
	CThread** threads;
	CThreads();
	CThreads(int n);
	~CThreads();
	void freeThreads();
	void setNumber(int n);
	void add(CThread* thread);
	void start();
	void wait();
};
#endif // !__CTHREAD

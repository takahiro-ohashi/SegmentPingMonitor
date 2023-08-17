#include <afxwin.h>
#include "pch.h"
#include <vector>
#include <set>
#include <regex>
#include <cmath>
//#include "CThread.h"
//#include "MultiThreadPingView.h"
#include "ThreadMaker.h"
#include "PingWorker.h"

ThreadMaker::ThreadMaker(int n) : CThread()
{
	interval(0);
	id = n;
}
ThreadMaker::ThreadMaker(LPVOID viewPointer, int n) : CThread(viewPointer)
{
	interval(0);
	id = n;
}
extern HINSTANCE hDLL1;
extern HINSTANCE hDLL2;

DWORD ThreadMaker::ThreadFunc()
{
	pView = (CChildView*)view;

	// = LoadLibrary(_T("Iphlpapi.dll"));
	hDLL1 = LoadLibrary(_T("Iphlpapi.dll"));
	hDLL2 = LoadLibrary(_T("Ws2_32.dll"));

	bool result = false;
	//CListCtrl* pListCtrl;
	//pListCtrl = &pView->GetListCtrl();
	TCHAR tszText[256];
	memset(&tszText, 0, sizeof(TCHAR) * 256);
	//int RowCount = pListCtrl->GetItemCount();
	int RowCount;// = 253;
	pView->initRow();
	//RowCount = 16;
	//RowCount = 1; //Pingデバッグ用
	RowCount = 60;  //ギリギリを攻めるとき
	//RowCount = 100; //ダメそう
	CThreads work;
	std::vector<CThread*> thread_vector(RowCount);
	for (int i = 0; i < RowCount; i++)
	{
		thread_vector[i] = new PingWorker((LPVOID)pView, i);

	}
	work.setNumber(RowCount);
	for (int i = 0; i < RowCount; i++)
	{
		work.add(thread_vector[i]);
	}
	for (int i = 0; i < RowCount; i++) 
	{
		thread_vector[i]->priority(1);
	}
	work.start();
	work.wait();
	for (int i = 0; i < RowCount; i++) 
	{
		delete thread_vector[i];
	}
	pView->PostMessageA(WM_CUSTOM_FINISH, 0, 0);

	FreeLibrary(hDLL2);
	FreeLibrary(hDLL1);

	return 0;
}
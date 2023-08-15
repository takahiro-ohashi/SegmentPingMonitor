#include "pch.h"
#include "CThread.h"
#include "resource.h"

int ThreadPriority[5] = {
 THREAD_PRIORITY_LOWEST,
 THREAD_PRIORITY_BELOW_NORMAL,
 THREAD_PRIORITY_NORMAL,
 THREAD_PRIORITY_ABOVE_NORMAL,
 THREAD_PRIORITY_HIGHEST
};

void __ThreadFunc(LPVOID* param) {
	((CThread*)param)->ThreadFunc();
}
CThread::CThread()
{
	threadHandle = NULL;
	running = 0;
	stopreq = 0;
	suspension = 0;
	thread = nullptr;
}
CThread::CThread(LPVOID viewpointer) {
	view = viewpointer;
	threadHandle = NULL;
	running = 0;
	stopreq = 0;
	suspension = 0;
	thread = nullptr;

	if (thread != nullptr) {
		delete thread;
	}
}
CThread::~CThread()
{
	if (threadHandle != NULL)
	{
		TerminateThread(threadHandle, 0);
		threadHandle = NULL;
		running = 0;
	}
}
DWORD CThread::ThreadFunc()
{
	while (stopreq == 0)
	{
		if (stime > 0) Sleep(stime);
	}
	CloseHandle(threadHandle);
	threadHandle = NULL;
	return 0;
}
void CThread::sleep(int n) 
{
	if (threadHandle != NULL) 
	{
		sleep(n);
	}
}
void CThread::start()
{
	if (threadHandle == NULL) {
		thread = AfxBeginThread((AFX_THREADPROC)__ThreadFunc, (LPVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		threadHandle = thread->m_hThread;
		running = 1;
		stopreq = 0;
	}
}
void CThread::stop()
{
	if (threadHandle != NULL) 
	{
		stopreq = 1;
	}
}
void CThread::kill()
{
	if (threadHandle != NULL) 
	{
		TerminateThread(threadHandle, 0);
		threadHandle = NULL;
		running = 0;
		if ((unsigned int)thread != 0xcdcdcdcdcdcdcdcd && ((unsigned int)thread != 0xbaadf00d))
		{
			delete thread;
		}
	}
}
void CThread::priority(int n) 
{
	if ((threadHandle != NULL) && (1 <= n) && (n <= 5)) 
	{
		SetThreadPriority(threadHandle, ThreadPriority[n - 1]);
	}
}
int CThread::priority()
{
	if (threadHandle != NULL) 
	{
		return GetThreadPriority(threadHandle) + 1;
	}
	else
	{
		return (int)0;
	}
}
void CThread::suspend()
{
	if ((threadHandle != NULL) && (suspension == 0)) {
		SuspendThread(threadHandle);
	}
	suspension = 0;
}
void CThread::resume()
{
	if ((threadHandle != NULL) && (suspension == 1)){
		thread->ResumeThread();
	}
	suspension = 0;
}
CThreads::CThreads()
{
	NoThreadsMax = 0;
	NoThreads = 0;
	threads = nullptr;
}
CThreads::CThreads(int n) 
{
	NoThreadsMax = n;
	NoThreads = 0;
	threads = (CThread**)malloc(sizeof(CThread*) * n);
}
CThreads::~CThreads() 
{
	freeThreads();
}
void CThreads::freeThreads()
{
	if (threads != nullptr) 
	{
		free(threads);
	}
}
void CThreads::setNumber(int n) 
{
	NoThreadsMax = n;
	NoThreads = 0;
	threads = (CThread**)malloc(sizeof(CThread*) * n);
}
void CThreads::add(CThread* thread)
{
	if (NoThreads < NoThreadsMax) {
		threads[NoThreads] = thread;
	}
	NoThreads++;
}
void CThreads::start() 
{
	DWORD n;
	for (n = 0; n < NoThreads; n++) {
		threads[n]->start();
	}
	for (n = 0; n < NoThreads; n++) {
		threads[n]->thread->ResumeThread();
	}
}
void CThreads::wait()
{
	DWORD n;
	HANDLE* threadHandles;
	threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * NoThreads);
	for (n = 0; n < NoThreads; n++) {
		threadHandles[n] = threads[n]->thread->m_hThread;
	}
	WaitForMultipleObjects(NoThreads, threadHandles, TRUE, INFINITE);
	free(threadHandles);
}
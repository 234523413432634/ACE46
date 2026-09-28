// AtumThread.cpp: implementation of the CAtumThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AtumThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadProc(LPVOID pParam)
{
	return ((CAtumThread*)pParam)->Run();
}

CAtumThread::CAtumThread()
{
	m_bRunning			= FALSE;
	// Run() loops on this in the two threads that have one, and nothing ever
	// initialised it - so each of those loops was reading uninitialised memory
	// to decide whether to keep going.
	m_bThreadMustStop	= FALSE;
	m_hThread			= NULL;
	m_dThreadID			= 0;
}

HANDLE CAtumThread::CreateThread(DWORD dwCreattionFlags)
{
	m_bRunning = TRUE;
	m_hThread = ::CreateThread(NULL, 0, ThreadProc, (LPVOID)this, dwCreattionFlags, &m_dThreadID);
	return m_hThread;
}

CAtumThread::~CAtumThread()
{
	if(NULL == m_hThread)
	{
		return;						// CreateThread() was never called
	}

	DWORD dExitCode;
	if(m_bRunning)
	{
		GetExitCodeThread(m_hThread, &dExitCode);
		TerminateThread(m_hThread, dExitCode);
	}
	CloseHandle(m_hThread);
}

VOID CAtumThread::Resume()
{
	SuspendThread(m_hThread);
}

VOID CAtumThread::Suspend()
{
	ResumeThread(m_hThread);
}

VOID CAtumThread::Priority(int pri)
{
	SetThreadPriority(m_hThread, pri);
}

DWORD CAtumThread::Run()
{
	m_bRunning = FALSE;
	return 0;
}

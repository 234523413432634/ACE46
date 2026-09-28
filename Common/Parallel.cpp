///////////////////////////////////////////////////////////////////////////////
//  Parallel.cpp : see Parallel.h
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>
#include "Parallel.h"

namespace
{
	const int COUNT_MAX_PARALLEL_THREAD = 64;

	struct SParallelJob
	{
		PFN_ATUM_PARALLEL_WORK	pfnWork;
		void					*pContext;
		volatile LONG			lNextIndex;
		int						nCount;
	};

	unsigned __stdcall ParallelWorker(void *i_pParameter)
	{
		SParallelJob *pJob = (SParallelJob*)i_pParameter;
		for (;;)
		{
			// InterlockedIncrement returns the new value, so subtract one to get
			// the index this thread claimed.
			const int nIndex = (int)InterlockedIncrement(&pJob->lNextIndex) - 1;
			if (nIndex >= pJob->nCount)
			{
				break;
			}
			pJob->pfnWork(nIndex, pJob->pContext);
		}
		return 0;
	}
}

int AtumGetWorkerThreadCount()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	int nCount = (int)systemInfo.dwNumberOfProcessors;
	if (nCount < 1)						{ nCount = 1; }
	if (nCount > COUNT_MAX_PARALLEL_THREAD)	{ nCount = COUNT_MAX_PARALLEL_THREAD; }
	return nCount;
}

void AtumParallelFor(int i_nCount, PFN_ATUM_PARALLEL_WORK i_pfnWork, void *i_pContext,
					 int i_nMaxThreads/*=0*/)
{
	if (i_nCount <= 0 || NULL == i_pfnWork)
	{
		return;
	}

	int nThreads = (i_nMaxThreads > 0) ? i_nMaxThreads : AtumGetWorkerThreadCount();
	if (nThreads > i_nCount)			{ nThreads = i_nCount; }
	if (nThreads > COUNT_MAX_PARALLEL_THREAD)	{ nThreads = COUNT_MAX_PARALLEL_THREAD; }

	SParallelJob job;
	job.pfnWork		= i_pfnWork;
	job.pContext	= i_pContext;
	job.lNextIndex	= 0;
	job.nCount		= i_nCount;

	if (nThreads <= 1)
	{
		ParallelWorker(&job);
		return;
	}

	HANDLE	arrThread[COUNT_MAX_PARALLEL_THREAD];
	int		nStarted = 0;
	for (int i = 0; i < nThreads - 1; i++)
	{	// _beginthreadex rather than CreateThread: the work calls into the CRT
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ParallelWorker, &job, 0, NULL);
		if (NULL == hThread)
		{
			break;
		}
		arrThread[nStarted++] = hThread;
	}

	// The calling thread takes part too, so a failure to spawn any worker at all
	// simply degrades to running everything here.
	ParallelWorker(&job);

	for (int i = 0; i < nStarted; i++)
	{
		WaitForSingleObject(arrThread[i], INFINITE);
		CloseHandle(arrThread[i]);
	}
}

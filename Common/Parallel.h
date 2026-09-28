///////////////////////////////////////////////////////////////////////////////
//  Parallel.h : run an independent piece of work over a range of indices
//
//  Used by the start-up paths that touch thousands of resources - hashing
//  .\map\Res-Obj, loading the map files - where each item is independent of
//  the others.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_PARALLEL_H_
#define _ATUM_PARALLEL_H_

typedef void (*PFN_ATUM_PARALLEL_WORK)(int i_nIndex, void *i_pContext);

// i_nMaxThreads <= 0 means "one per logical processor", capped so that a
// machine with a very high core count does not spawn more threads than there
// is work.
void AtumParallelFor(int i_nCount, PFN_ATUM_PARALLEL_WORK i_pfnWork, void *i_pContext,
					 int i_nMaxThreads = 0);

// Number of logical processors, as AtumParallelFor would use.
int AtumGetWorkerThreadCount();

#endif	// _ATUM_PARALLEL_H_

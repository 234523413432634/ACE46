///////////////////////////////////////////////////////////////////////////////
//  LowMemory.cpp : see LowMemory.h
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "LowMemory.h"

///////////////////////////////////////////////////////////////////////////////
// AtumLowReserveVirtual

void *AtumLowReserveVirtual(size_t i_nSize)
{
#if !defined(_M_X64)

	return VirtualAlloc(NULL, i_nSize, MEM_RESERVE, PAGE_READWRITE);

#else	// _M_X64

	// VirtualAlloc2() (Windows 10 1803 / Server 2019) takes a hard upper bound
	// for the reservation, which is exactly what is needed here.
	typedef PVOID (WINAPI *PFN_VirtualAlloc2)(HANDLE, PVOID, SIZE_T, ULONG, ULONG,
											  MEM_EXTENDED_PARAMETER*, ULONG);
	static PFN_VirtualAlloc2 s_pfnVirtualAlloc2 = (PFN_VirtualAlloc2)GetProcAddress(
		GetModuleHandleA("kernelbase.dll"), "VirtualAlloc2");

	if (NULL != s_pfnVirtualAlloc2)
	{
		MEM_ADDRESS_REQUIREMENTS	addressRequirements;
		MEM_EXTENDED_PARAMETER		extendedParameter;

		memset(&addressRequirements, 0x00, sizeof(addressRequirements));
		memset(&extendedParameter, 0x00, sizeof(extendedParameter));

		addressRequirements.HighestEndingAddress = (PVOID)(ULONG_PTR)0xFFFFFFFFull;
		extendedParameter.Type		= MemExtendedParameterAddressRequirements;
		extendedParameter.Pointer	= &addressRequirements;

		void *pReserved = s_pfnVirtualAlloc2(GetCurrentProcess(), NULL, i_nSize,
											 MEM_RESERVE, PAGE_READWRITE,
											 &extendedParameter, 1);
		if (NULL != pReserved)
		{
			return pReserved;
		}
	}

	// Older systems: walk the low 4 GB looking for a free region big enough.
	SYSTEM_INFO	systemInfo;
	GetSystemInfo(&systemInfo);

	const ULONG_PTR	ulGranularity	= systemInfo.dwAllocationGranularity ? systemInfo.dwAllocationGranularity : 0x10000;
	const ULONG_PTR	ulLimit			= (ULONG_PTR)0x100000000ull;
	ULONG_PTR		ulAddress		= ulGranularity;

	while (ulAddress + i_nSize <= ulLimit)
	{
		MEMORY_BASIC_INFORMATION	memoryInfo;
		if (0 == VirtualQuery((LPCVOID)ulAddress, &memoryInfo, sizeof(memoryInfo)))
		{
			break;
		}

		if (MEM_FREE == memoryInfo.State && memoryInfo.RegionSize >= i_nSize)
		{
			void *pReserved = VirtualAlloc((LPVOID)ulAddress, i_nSize, MEM_RESERVE, PAGE_READWRITE);
			if (NULL != pReserved)
			{
				return pReserved;
			}
		}

		ULONG_PTR ulNext = (ULONG_PTR)memoryInfo.BaseAddress + memoryInfo.RegionSize;
		ulNext = (ulNext + ulGranularity - 1) & ~(ulGranularity - 1);
		if (ulNext <= ulAddress)
		{	// no forward progress, the address space is exhausted
			break;
		}
		ulAddress = ulNext;
	}

	return NULL;

#endif	// _M_X64
}


#if defined(_M_X64)

///////////////////////////////////////////////////////////////////////////////
// Pooled allocator over the low 4 GB.
///////////////////////////////////////////////////////////////////////////////

namespace
{
	const size_t	SIZE_LOW_CHUNK		= 8 * 1024 * 1024;	// one reservation
	const size_t	SIZE_LOW_ALIGNMENT	= 16;

	struct SLowChunk
	{
		char		*pBase;
		size_t		nSize;
		size_t		nUsed;
		SLowChunk	*pNext;
	};

	struct SLowClass
	{
		size_t		nBlockSize;		// header included
		void		*pFreeList;
		SLowClass	*pNext;
	};

	struct SLowHeader					// 16 bytes, keeps the payload 16 byte aligned
	{
		size_t		nBlockSize;
		size_t		nReserved;
	};

	struct SLowLock
	{
		CRITICAL_SECTION cs;
		SLowLock()	{ InitializeCriticalSection(&cs); }
		~SLowLock()	{ DeleteCriticalSection(&cs); }
	};

	SLowLock &LowLock()
	{	// function local static: the C++ runtime makes the initialisation
		// thread safe, so no separate init call is needed.
		static SLowLock s_lock;
		return s_lock;
	}

	SLowChunk	*g_pLowChunks	= NULL;
	SLowClass	*g_pLowClasses	= NULL;

	SLowChunk *LowChunkWithRoom(size_t i_nSize)
	{
		SLowChunk *pChunk = g_pLowChunks;
		while (NULL != pChunk)
		{
			if (pChunk->nSize - pChunk->nUsed >= i_nSize)
			{
				return pChunk;
			}
			pChunk = pChunk->pNext;
		}

		size_t nChunkSize = SIZE_LOW_CHUNK;
		if (i_nSize > nChunkSize)
		{
			nChunkSize = (i_nSize + 0xFFFF) & ~(size_t)0xFFFF;
		}

		void *pBase = AtumLowReserveVirtual(nChunkSize);
		if (NULL == pBase)
		{
			return NULL;
		}
		if (NULL == VirtualAlloc(pBase, nChunkSize, MEM_COMMIT, PAGE_READWRITE))
		{
			VirtualFree(pBase, 0, MEM_RELEASE);
			return NULL;
		}

		pChunk = (SLowChunk*)malloc(sizeof(SLowChunk));
		if (NULL == pChunk)
		{
			VirtualFree(pBase, 0, MEM_RELEASE);
			return NULL;
		}
		pChunk->pBase	= (char*)pBase;
		pChunk->nSize	= nChunkSize;
		pChunk->nUsed	= 0;
		pChunk->pNext	= g_pLowChunks;
		g_pLowChunks	= pChunk;
		return pChunk;
	}

	SLowClass *LowClassFor(size_t i_nBlockSize)
	{
		SLowClass *pClass = g_pLowClasses;
		while (NULL != pClass)
		{
			if (pClass->nBlockSize == i_nBlockSize)
			{
				return pClass;
			}
			pClass = pClass->pNext;
		}

		pClass = (SLowClass*)malloc(sizeof(SLowClass));
		if (NULL == pClass)
		{
			return NULL;
		}
		pClass->nBlockSize	= i_nBlockSize;
		pClass->pFreeList	= NULL;
		pClass->pNext		= g_pLowClasses;
		g_pLowClasses		= pClass;
		return pClass;
	}
}

void *AtumLowAlloc(size_t i_nSize)
{
	const size_t nBlockSize = (i_nSize + sizeof(SLowHeader) + SIZE_LOW_ALIGNMENT - 1)
							  & ~(size_t)(SIZE_LOW_ALIGNMENT - 1);

	EnterCriticalSection(&LowLock().cs);

	void		*pBlock	= NULL;
	SLowClass	*pClass	= LowClassFor(nBlockSize);
	if (NULL != pClass)
	{
		if (NULL != pClass->pFreeList)
		{
			pBlock				= pClass->pFreeList;
			pClass->pFreeList	= *(void**)pBlock;
		}
		else
		{
			SLowChunk *pChunk = LowChunkWithRoom(nBlockSize);
			if (NULL != pChunk)
			{
				pBlock			= pChunk->pBase + pChunk->nUsed;
				pChunk->nUsed	+= nBlockSize;
			}
		}
	}

	LeaveCriticalSection(&LowLock().cs);

	if (NULL == pBlock)
	{
		return NULL;
	}

	SLowHeader *pHeader	= (SLowHeader*)pBlock;
	pHeader->nBlockSize	= nBlockSize;
	pHeader->nReserved	= 0;
	return (char*)pBlock + sizeof(SLowHeader);
}

void AtumLowFree(void *i_pMemory)
{
	if (NULL == i_pMemory)
	{
		return;
	}

	SLowHeader	*pHeader	= (SLowHeader*)((char*)i_pMemory - sizeof(SLowHeader));
	const size_t nBlockSize	= pHeader->nBlockSize;

	EnterCriticalSection(&LowLock().cs);

	SLowClass *pClass = LowClassFor(nBlockSize);
	if (NULL != pClass)
	{	// the freed block itself holds the free list link
		*(void**)pHeader	= pClass->pFreeList;
		pClass->pFreeList	= pHeader;
	}

	LeaveCriticalSection(&LowLock().cs);
}

#else	// !_M_X64

void *AtumLowAlloc(size_t i_nSize)
{
	return malloc(i_nSize);
}

void AtumLowFree(void *i_pMemory)
{
	free(i_pMemory);
}

#endif	// _M_X64

///////////////////////////////////////////////////////////////////////////////
// AtumLowCheckAddress

void AtumLowCheckAddress(const void *i_pAddress, const char *i_szWhere)
{
#if defined(_M_X64)
	if (0 == ((ULONG_PTR)i_pAddress >> 32))
	{
		return;
	}

	static LONG s_lReported = 0;
	if (0 != InterlockedExchange(&s_lReported, 1))
	{
		return;
	}

	char szMessage[256];
	sprintf_s(szMessage, sizeof(szMessage),
		"[ERROR] %s: %p does not fit in the 32 bit protocol field - the object "
		"was not allocated from the low 4 GB (see LowMemory.h)\r\n",
		i_szWhere ? i_szWhere : "?", i_pAddress);
	OutputDebugStringA(szMessage);

	if (IsDebuggerPresent())
	{
		DebugBreak();
	}
#else
	(void)i_pAddress;
	(void)i_szWhere;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Handle registry behind CAtumHandle32.  Handles are dense indices into a table
// of addresses; index 0 is reserved for NULL.  Entries are never removed - the
// registry only ever holds pointers to data loaded once at start-up.

#if defined(_M_X64)

namespace
{
	const unsigned int	COUNT_MAX_HANDLE32 = 64 * 1024;

	const void	*g_arrHandle32[COUNT_MAX_HANDLE32] = { NULL };
	unsigned int g_uiHandle32Count = 1;			// 0 == NULL
}

unsigned int AtumHandle32Acquire(const void *i_pObject)
{
	if (NULL == i_pObject)
	{
		return 0;
	}

	EnterCriticalSection(&LowLock().cs);

	unsigned int uiHandle = 0;
	for (unsigned int ui = 1; ui < g_uiHandle32Count; ui++)
	{
		if (g_arrHandle32[ui] == i_pObject)
		{
			uiHandle = ui;
			break;
		}
	}
	if (0 == uiHandle && g_uiHandle32Count < COUNT_MAX_HANDLE32)
	{
		uiHandle					= g_uiHandle32Count++;
		g_arrHandle32[uiHandle]		= i_pObject;
	}

	LeaveCriticalSection(&LowLock().cs);

	if (0 == uiHandle)
	{
		OutputDebugStringA("[ERROR] AtumHandle32Acquire: registry full (see LowMemory.h)\r\n");
	}
	return uiHandle;
}

void *AtumHandle32Resolve(unsigned int i_uiHandle)
{
	if (i_uiHandle >= COUNT_MAX_HANDLE32)
	{
		return NULL;
	}
	return (void*)g_arrHandle32[i_uiHandle];
}

#else	// !_M_X64

unsigned int AtumHandle32Acquire(const void *)	{ return 0; }
void *AtumHandle32Resolve(unsigned int)			{ return NULL; }

#endif	// _M_X64

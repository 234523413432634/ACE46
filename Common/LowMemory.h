///////////////////////////////////////////////////////////////////////////////
//  LowMemory.h : keeping 32-bit-addressable objects addressable on x64
//
//  The protocol identifies several kinds of object by a 32 bit value that is
//  simply the object's address, so those fields can be neither widened nor
//  moved. ATUM_LOW_MEMORY_OBJECT() keeps such objects below 4 GB and
//  CAtumPtr32<T> is four bytes on both platforms; on Win32 both collapse to a
//  plain pointer.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_LOW_MEMORY_H_
#define _ATUM_LOW_MEMORY_H_

#include <stddef.h>

// Reserves (MEM_RESERVE, PAGE_READWRITE) i_nSize bytes.  On x64 the region is
// guaranteed to end below 4 GB; NULL is returned when no such region is free.
void *AtumLowReserveVirtual(size_t i_nSize);

// General purpose allocation from the low 4 GB.
void *AtumLowAlloc(size_t i_nSize);
void  AtumLowFree(void *i_pMemory);

// Complains (once, to the debugger) when an address that is about to be stored
// in a 32 bit field does not fit.  Nothing in the server should ever hit this;
// it is the backstop that turns a silent wire-format corruption into something
// visible in a debug log.
void AtumLowCheckAddress(const void *i_pAddress, const char *i_szWhere);

// Registry behind CAtumHandle32, for pointers that cannot be moved into the
// low 4 GB (objects owned by an STL container, string literals, ...).
unsigned int  AtumHandle32Acquire(const void *i_pObject);
void         *AtumHandle32Resolve(unsigned int i_uiHandle);

///////////////////////////////////////////////////////////////////////////////
// CAtumPtr32<T> - a pointer member that is always 4 bytes wide.
//
// Used for the pointers embedded in wire structures that refer to objects held
// in the low 4 GB.
///////////////////////////////////////////////////////////////////////////////
template <class T>
struct CAtumPtr32
{
#if defined(_M_X64)

	unsigned int	m_uiAddress;

	operator T*() const					{ return (T*)(size_t)m_uiAddress; }
	T *operator->() const				{ return (T*)(size_t)m_uiAddress; }
	T &operator*() const				{ return *(T*)(size_t)m_uiAddress; }

	// The stored 32 bit value.
	unsigned int GetRaw32() const		{ return m_uiAddress; }

	CAtumPtr32 &operator=(T *i_pObject)
	{
		AtumLowCheckAddress(i_pObject, "CAtumPtr32");
		m_uiAddress = (unsigned int)(size_t)i_pObject;
		return *this;
	}

#else	// !_M_X64

	T				*m_pObject;

	operator T*() const					{ return m_pObject; }
	T *operator->() const				{ return m_pObject; }
	T &operator*() const				{ return *m_pObject; }

	unsigned int GetRaw32() const		{ return (unsigned int)(size_t)m_pObject; }

	CAtumPtr32 &operator=(T *i_pObject)	{ m_pObject = i_pObject; return *this; }

#endif	// _M_X64
};

///////////////////////////////////////////////////////////////////////////////
// CAtumHandle32<T> - a 4 byte reference to an object that lives wherever the
// normal allocator put it.
///////////////////////////////////////////////////////////////////////////////
template <class T>
struct CAtumHandle32
{
#if defined(_M_X64)

	unsigned int	m_uiHandle;

	operator T*() const					{ return (T*)AtumHandle32Resolve(m_uiHandle); }
	T *operator->() const				{ return (T*)AtumHandle32Resolve(m_uiHandle); }
	T &operator*() const				{ return *(T*)AtumHandle32Resolve(m_uiHandle); }

	CAtumHandle32 &operator=(T *i_pObject)
	{
		m_uiHandle = AtumHandle32Acquire(i_pObject);
		return *this;
	}

#else	// !_M_X64

	T				*m_pObject;

	operator T*() const						{ return m_pObject; }
	T *operator->() const					{ return m_pObject; }
	T &operator*() const					{ return *m_pObject; }

	CAtumHandle32 &operator=(T *i_pObject)	{ m_pObject = i_pObject; return *this; }

#endif	// _M_X64
};

///////////////////////////////////////////////////////////////////////////////
// Place in the body of a class whose address is handed to the client - or kept
// in any other 32 bit field - to force its instances into the low 4 GB.
#if defined(_M_X64)

#define ATUM_LOW_MEMORY_OBJECT()										\
	void *operator new(size_t i_nSize)			{ return AtumLowAlloc(i_nSize); }	\
	void *operator new[](size_t i_nSize)		{ return AtumLowAlloc(i_nSize); }	\
	void *operator new(size_t, void *i_pWhere)	{ return i_pWhere; }				\
	void operator delete(void *i_pMemory)		{ AtumLowFree(i_pMemory); }			\
	void operator delete[](void *i_pMemory)		{ AtumLowFree(i_pMemory); }			\
	void operator delete(void *, void *)		{ }

#else	// !_M_X64

#define ATUM_LOW_MEMORY_OBJECT()

#endif	// _M_X64

#endif	// _ATUM_LOW_MEMORY_H_

// MeshInitThread.h: interface for the CMeshInitThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHINITTHREAD_H__3AA93703_BFE3_41AA_A5AC_17064AB096BB__INCLUDED_)
#define AFX_MESHINITTHREAD_H__3AA93703_BFE3_41AA_A5AC_17064AB096BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AtumThread.h"
#include "AtumApplication.h"

#define GAMEDATA_LOADING_TIME 1

///////////////////////////////////////////////////////////////////////////////
//  Reads meshes off the main thread, so that a model coming into view does not
//  cost a frame.
///////////////////////////////////////////////////////////////////////////////

#define COUNT_MESH_LOADER		4

class CMeshInitThread : public CAtumThread
{
private:
	queue<structLoadingGameInfo*>	m_queLoadingGameInfo;
	CRITICAL_SECTION				m_csQueue;
	HANDLE							m_hWorkAvailable;
	HANDLE							m_arrExtraThread[COUNT_MESH_LOADER - 1];
	int								m_nExtraThread;

public:
	CMeshInitThread();
	virtual ~CMeshInitThread();
	virtual DWORD Run();

	// Starts every loader thread, this one included.  Replaces the bare
	// CAtumThread::CreateThread() call the application used to make.
	void CreateLoaderThreads();

	void QuePushGameData( structLoadingGameInfo* GameInfo );
	// NULL when there is nothing waiting.  The caller owns what it gets back.
	structLoadingGameInfo* QuePopGameData();

	void CreateGameData( structLoadingGameInfo* GameInfo );
};

#endif // !defined(AFX_MESHINITTHREAD_H__3AA93703_BFE3_41AA_A5AC_17064AB096BB__INCLUDED_)

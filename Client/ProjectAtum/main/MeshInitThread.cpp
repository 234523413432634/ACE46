// MeshInitThread.cpp: implementation of the CMeshInitThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeshInitThread.h"
#include "ShuttleChild.h"
#include "INFGameMain.h"
#include "GameDataLast.h"
#include "Parallel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshInitThread::CMeshInitThread()
{
	InitializeCriticalSection(&m_csQueue);
	// A semaphore rather than an event: one release per queued mesh, so any
	// idle loader picks it up and no wake-up is lost when several arrive at once.
	m_hWorkAvailable	= CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	m_nExtraThread		= 0;
	memset(m_arrExtraThread, 0x00, sizeof(m_arrExtraThread));
}

CMeshInitThread::~CMeshInitThread()
{
	m_bThreadMustStop = TRUE;
	if(NULL != m_hWorkAvailable)
	{	// wake everyone so they see the flag rather than being terminated
		ReleaseSemaphore(m_hWorkAvailable, COUNT_MESH_LOADER, NULL);
	}
	for(int i = 0; i < m_nExtraThread; i++)
	{
		WaitForSingleObject(m_arrExtraThread[i], 2000);
		CloseHandle(m_arrExtraThread[i]);
	}
	m_nExtraThread = 0;

	// ~CAtumThread takes care of the first thread.  Anything still queued was
	// never going to be loaded.
	while(structLoadingGameInfo* pInfo = QuePopGameData())
	{
		SAFE_DELETE(pInfo);
	}
	if(NULL != m_hWorkAvailable)
	{
		CloseHandle(m_hWorkAvailable);
	}
	DeleteCriticalSection(&m_csQueue);
}

void CMeshInitThread::CreateLoaderThreads()
{
	CreateThread();

	// One per core up to COUNT_MESH_LOADER: on a machine with two cores there
	// is nothing to gain from four readers competing with the render thread.
	int nWanted = AtumGetWorkerThreadCount();
	if(nWanted > COUNT_MESH_LOADER)		{ nWanted = COUNT_MESH_LOADER; }

	for(int i = 0; i < nWanted - 1; i++)
	{
		HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
		if(NULL == hThread)
		{
			break;						// one loader is enough to work with
		}
		m_arrExtraThread[m_nExtraThread++] = hThread;
	}
}

void CMeshInitThread::QuePushGameData( structLoadingGameInfo* GameInfo )
{
	EnterCriticalSection(&m_csQueue);
	m_queLoadingGameInfo.push(GameInfo);
	LeaveCriticalSection(&m_csQueue);
	ReleaseSemaphore(m_hWorkAvailable, 1, NULL);
}

structLoadingGameInfo* CMeshInitThread::QuePopGameData()
{
	structLoadingGameInfo* pInfo = NULL;
	EnterCriticalSection(&m_csQueue);
	if(!m_queLoadingGameInfo.empty())
	{
		pInfo = m_queLoadingGameInfo.front();
		m_queLoadingGameInfo.pop();
	}
	LeaveCriticalSection(&m_csQueue);
	return pInfo;
}

DWORD CMeshInitThread::Run()
{
	m_bRunning = TRUE;
	while (!m_bThreadMustStop)	// 2015-07-08 Future, added proper Shutdown of threads
	{
		// The timeout is only a safety net; a push wakes this immediately.
		WaitForSingleObject(m_hWorkAvailable, 200);
		if(m_bThreadMustStop)
		{
			break;
		}

		structLoadingGameInfo* pInfo = QuePopGameData();
		if(NULL != pInfo)
		{
			CreateGameData(pInfo);
			SAFE_DELETE(pInfo);		// nothing downstream keeps it
		}
	}
	m_bRunning = FALSE;

	return 0;
}

void CMeshInitThread::CreateGameData( structLoadingGameInfo* GameInfo )
{
	char strPath[MAX_PATH];
	CGameData* pMeshData = new CGameData();
	int LoadingType;

	if(GameInfo->MeshType == _EFFECT_TYPE)
	{
		LoadingType = IDS_DIRECTORY_EFFECT;
	}
	else
	{
		LoadingType = IDS_DIRECTORY_OBJECT;
	}

	g_pD3dApp->LoadPath( strPath, LoadingType, GameInfo->MeshName );
	if(pMeshData->SetFile( strPath,FALSE, NULL, 0 ))
	{
		CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();
		if(COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER) && LoadingType == IDS_DIRECTORY_OBJECT)
		{
			// 관리자만 스트링을 찍는다.
			char buf[16];
			wsprintf(buf,"%08d",atoi( GameInfo->MeshName ));
			if(pMeshData->Find(buf) == NULL)
			{
				DBGOUT("Resource File Error(%d)\n",atoi(buf));	//리소스 파일 에러
				char ErrorMsgMissionList[256];
				wsprintf(ErrorMsgMissionList, "Resource File Error(%d)", atoi( GameInfo->MeshName ));
				if(g_pGameMain)
				{
					g_pGameMain->CreateChatChild_OperationMode(ErrorMsgMissionList, COLOR_ERROR);
					SAFE_DELETE( pMeshData );
					return;
				}
			}
		}
		auto LoadingData = new structLoadingGameData;

		LoadingData->MeshIndex		= atoi( GameInfo->MeshName );
		LoadingData->MeshType		= GameInfo->MeshType;
		LoadingData->pGameData		= pMeshData;
		LoadingData->Step			= _RESOURCE_LOADING_START;
		LoadingData->Text_Cnt		= 0;
		LoadingData->pSkinnedMesh	= GameInfo->pSkinnedMesh;
		LoadingData->LoadingPriority = GameInfo->LoadingPriority;

		EnterCriticalSection(&g_pD3dApp->m_cs);
		g_pD3dApp->vecPushGameData( LoadingData );
		LeaveCriticalSection(&g_pD3dApp->m_cs);
	}
	else
	{
		SAFE_DELETE( pMeshData );
	}
}

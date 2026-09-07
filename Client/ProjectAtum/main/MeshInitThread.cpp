// MeshInitThread.cpp: implementation of the CMeshInitThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeshInitThread.h"
#include "ShuttleChild.h"
#include "INFGameMain.h"
#include "GameDataLast.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshInitThread::CMeshInitThread()
{
	
}

CMeshInitThread::~CMeshInitThread()
{

}

DWORD CMeshInitThread::Run()
{
	// 2014-08-18 by ymjoo 싱글스레드로 변경
#ifndef C_GAME_SINGLE_THREAD_YMJOO
 	while( 1 )
 	{
#endif
	// END 2014-08-18 by ymjoo 싱글스레드로 변경
		if( !m_queLoadingGameInfo.empty() )
		{
			// 2015-03-17 Mesh 로딩시 할당한 변수 삭제 코드 추가
// 			CreateGameData( m_queLoadingGameInfo.front() );
// 			QuePopGameData();
			structLoadingGameInfo* LoadingGameInfo = m_queLoadingGameInfo.front();
			CreateGameData( LoadingGameInfo);
			QuePopGameData();
			SAFE_DELETE(LoadingGameInfo);
			// end 2015-03-17 Mesh 로딩시 할당한 변수 삭제 코드 추가
		}		
		Sleep( GAMEDATA_LOADING_TIME );
	// 2014-08-18 by ymjoo 싱글스레드로 변경
#ifndef C_GAME_SINGLE_THREAD_YMJOO
	}
#endif
	// END 2014-08-18 by ymjoo 싱글스레드로 변경
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
		structLoadingGameData* LoadingData = new structLoadingGameData;
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


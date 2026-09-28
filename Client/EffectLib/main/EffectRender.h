// EffectRender.h: interface for the CEffectRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTRENDER_H__7B0E8CE2_67C8_4A53_97BE_F7589D2FFB62__INCLUDED_)
#define AFX_EFFECTRENDER_H__7B0E8CE2_67C8_4A53_97BE_F7589D2FFB62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AtumNode.h"
#include "effect.h"

#define TEX_EFFECT_NUM 100
#define OBJ_EFFECT_NUM 300

class CObjectAni;
class CSpriteAni;
class CParticleSystem;
class CParticle;
class CObjectAni;
class CTraceAni;
class CEffectPlane;
class CSkinnedMesh;
class CGameData;
class CCharacterInfo;
class CAppEffectData;

// 2007-11-08 by bhsohn 인벤 이펙트 관련 처리
class CEffectInfo;
typedef struct
{
	int nWindowInvenIdx;
	char chEffectName[32];
} structInvenParticleInfo;

// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
typedef struct
{
	int LoadingPriority;
	char chEffectName[32];
} LoadingPriorityInfo;
//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경

class CEffectRender : public CAtumNode  
{
public:
	CEffectRender();
	virtual ~CEffectRender();

	void Render();
//	void RenderSun();
//	void RenderCloud();
	void RenderCharacterInfo(CCharacterInfo* pChar, BOOL bAlpha = FALSE, int nAlphaValue = SKILL_OBJECT_ALPHA_NONE);
	void ObjectAniRender(CObjectAni* pEffect, BOOL bAlpha = FALSE, int nAlphaValue = SKILL_OBJECT_ALPHA_NONE);
	void SpriteAniRender(CSpriteAni* pEffect);
	void ParticleSystemRender(CParticleSystem* pEffect);
	int ParticleRender(CParticleSystem* pParticleSystem, CParticle* p,D3DXVECTOR3 vAxis,int nOldTextureIndex);
//	void ParticleRender(CParticleSystem* pEffect);
	void ObjectParticleRender(CObjectAni* pEffect, CParticle* pParticle);
	void TraceAniRender( CTraceAni* pEffect );
	void EffectPlaneRender( CEffectPlane *pEffect );
	void RenderZEnable();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	void Tick(float fElapsedTime);

	///////////////////////////////////////////////////////////////////////////
	//  The answers LoadTexture() has already given
	//
	//  The batch asks it once per particle and once per trail segment, only to
	//  work out which run each one belongs in - seven or eight thousand times a
	//  frame in a heavy scene, and always for the same handful of names.
	///////////////////////////////////////////////////////////////////////////
	enum
	{
		TEXTURE_CACHE_SIZE	= 256,
		TEXTURE_CACHE_MISS	= -2		// -1 is a real answer: no such texture
	};

	struct TextureCacheEntry
	{
		char	szName[24];				// the names are char[20] where they are declared
		int		nIndex;
		DWORD	dwGeneration;
	};

	int  TextureCacheGet(const char* i_szName);
	void TextureCachePut(const char* i_szName, int i_nIndex);

	TextureCacheEntry		m_arrTextureCache[TEXTURE_CACHE_SIZE];
	DWORD					m_dwTextureGeneration;

	int GetEmptyTextureIndex();
	int LoadTexture(char* strName);
//	int GetEmptyObjectIndex();
//	int LoadObject(char* strName);
	// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
	//CSkinnedMesh* LoadObject(char* strName);
	CSkinnedMesh* LoadObject(char* strName, int LoadingPriority = _NOTHING_PRIORITY);
	//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경
	
	void LoadObjectToMap(char* strName);
	DataHeader* FindEffectInfo(char* strName);
	DataHeader* FindObjectInfo(char* strName);
	DWORD LoadEffect(char* strName, DWORD dwEffectType, char* pEffect);
	// by dhkwon, 030923
	BOOL AddFontTexture(char* strText, LPDIRECT3DTEXTURE9 pTexture );
	BOOL DeleteFontTexture(char* strText);

	void DevideZBufferEnableEffect();
	void DevideCharacterEffect(CCharacterInfo* pChar, BOOL bAlpha = FALSE, int nAlphaValue = SKILL_OBJECT_ALPHA_NONE);

	BOOL CheckAlphaRender(CAppEffectData* pEffect, DWORD dwType);

	// 2007-11-08 by bhsohn 인벤 이펙트 관련 처리
	void ParticleAlphaRender(D3DXVECTOR3 posPaticlePos);	
	int InvenParticleRender(CParticleSystem* pParticleSystem, CParticle* p, D3DXVECTOR3 vAxis, int nOldTextureIndex, float fUnitScaling, D3DXMATRIX* pmatPaticlePos, D3DXMATRIX* pmatShttlePos);
	void RenderParticleInvenVector(int nMatIndex, D3DXMATRIX matShuttlePos, D3DXMATRIX matPos, float fUnitScaling);
	void ResetContentInvneParticle();
	void AddInvenPaticleName(int nInvenIdx, char* pEffectName);
	CEffectInfo* GetEffectInfo(char* pEffectName, int nWindowInvenIdx);
	CEffectInfo* GetCharInfo_To_Effect(CCharacterInfo* pChar, char* pEffectName,int nWindowInvenIdx);
	CEffectInfo* GetObjEffectInfo(char* pObjName);

	// 2012-07-13 by isshin 아템미리보기 인첸트이펙스 적용
	void RenderParticleEnemyItemVector(int nMatIndex, D3DXMATRIX matShuttlePos, D3DXMATRIX matPos, float fUnitScaling, UID32_t TargetCharcterUID);
	void ResetContentEnemyItemParticle();
	void AddEnemyItemPaticleName(int nInvenIdx, char* pEffectName);
	CEffectInfo* GetEnemyCharInfo_To_Effect(CCharacterInfo* pChar, char* pEffectName,int nWindowInvenIdx);
	CEffectInfo* GetEnemyEffectInfo(char* pEffectName, int nWindowInvenIdx, UID32_t TargetCharcterUID);
	// end 2012-07-13 by isshin 아템미리보기 인첸트이펙스 적용

	LPDIRECT3DVERTEXBUFFER9	m_pVB1;
	LPDIRECT3DVERTEXBUFFER9 m_pVB2[2];			
	LPDIRECT3DVERTEXBUFFER9 m_pVB4[4];			
	LPDIRECT3DVERTEXBUFFER9 m_pVB8[8];			
	LPDIRECT3DVERTEXBUFFER9 m_pVB16[16];

	///////////////////////////////////////////////////////////////////////////
	//  The sprite particles of RenderZEnable(), gathered into one draw each
	//
	//  A particle used to be a whole pipeline of its own: a world transform, a
	//  material, a light, a stream binding and a DrawPrimitive of two triangles,
	//  four thousand times over in a heavy frame.
	///////////////////////////////////////////////////////////////////////////
	enum { PARTICLE_BATCH_QUAD_MAX = 8192 };	// the heaviest frame seen held ~4,300

	// What a run has in common.  Two particles can share a draw only if all of
	// it matches, because these are what the per particle loop used to set.
	struct ParticleBatchState
	{
		LPDIRECT3DBASETEXTURE9	pTexture;
		DWORD					dwSrcBlend;
		DWORD					dwDestBlend;
		BOOL					bZbufferEnable;
		BOOL					bZWriteEnable;
	};

	// FALSE when the buffers could not be made, and the particles are drawn one
	// at a time exactly as they always were.
	BOOL ParticleBatchReady() const
	{
		return (NULL != m_pParticleBatchVB && NULL != m_pParticleBatchIB
				&& NULL != m_pParticleBatchVertex && NULL != m_pParticleBatchRunOf);
	}
	// How many runs either batch may hold at once.
	enum { EFFECT_BATCH_RUN_MAX = 64 };

	// Why a flush happened, which is the whole of what decides whether any of
	// this can be made to gather better.  Counted, not guessed at.
	enum
	{
		EFFECT_BATCH_WHY_OTHER,			// something not gathered is about to draw
		EFFECT_BATCH_WHY_ORDERED,		// what is held cannot be drawn out of turn
		EFFECT_BATCH_WHY_FULL,			// no room for another run or another item
		EFFECT_BATCH_WHY_END,			// the list is finished
		EFFECT_BATCH_WHY_COUNT
	};

	void EffectBatchFlush(int i_nWhy);
	void ParticleBatchAdd(CParticle* p);
	void ParticleBatchFlush();
	LPDIRECT3DBASETEXTURE9 ParticleBatchTexture(CParticleSystem* pParticleSystem, CParticle* p);
	void ParticleWorldMatrix(CParticleSystem* pParticleSystem, CParticle* p,
							 const D3DXVECTOR3& vAxis, D3DXMATRIX* o_pMatrix);

	LPDIRECT3DVERTEXBUFFER9	m_pParticleBatchVB;			// dynamic, written a run at a time
	LPDIRECT3DINDEXBUFFER9	m_pParticleBatchIB;			// six indices a quad, built once
	SPRITE_VERTEX*			m_pParticleBatchVertex;		// built here, in the order walked, before the copy in
	SPRITE_VERTEX			m_arrParticleQuad[4];		// the corners of m_pVB1, kept to transform
	ParticleBatchState		m_arrParticleBatchRun[EFFECT_BATCH_RUN_MAX];
	UINT					m_arrParticleBatchRunCount[EFFECT_BATCH_RUN_MAX];
	UINT					m_nParticleBatchRuns;
	BYTE*					m_pParticleBatchRunOf;		// which run each gathered quad joins
	UINT					m_nParticleBatchQuadMax;
	UINT					m_nParticleBatchQuadCount;	// quads gathered, over all the runs
	UINT					m_nParticleBatchVertexAt;	// where in the buffer the next lot goes

	///////////////////////////////////////////////////////////////////////////
	//  The trail segments of RenderZEnable(), the same way
	//
	//  A segment was a draw of its own too, and there are more of them than there
	//  are particles: a trail is m_nNumberOfTrace of them and every rocket in the
	//  sky has one.
	///////////////////////////////////////////////////////////////////////////
	enum { EFFECT_PLANE_BATCH_MAX = 2048 };		// the heaviest frame seen held ~2,500

	// What a run of segments has in common.
	struct EffectPlaneBatchState
	{
		LPDIRECT3DBASETEXTURE9	pTexture;
		BOOL					bAlphaBlendEnable;
		DWORD					dwSrcBlend;
		DWORD					dwDestBlend;
		BOOL					bZbufferEnable;
		BOOL					bZWriteEnable;
	};

	BOOL EffectPlaneBatchReady() const
	{
		return (NULL != m_pEffectPlaneBatchVB && NULL != m_pEffectPlaneBatchIB
				&& NULL != m_ppEffectPlaneBatch && NULL != m_pEffectPlaneBatchRunOf);
	}
	void EffectPlaneBatchAdd(CEffectPlane* pEffect);
	void EffectPlaneBatchFlush();
	LPDIRECT3DBASETEXTURE9 EffectPlaneBatchTexture(CEffectPlane* pEffect);

	LPDIRECT3DVERTEXBUFFER9	m_pEffectPlaneBatchVB;
	LPDIRECT3DINDEXBUFFER9	m_pEffectPlaneBatchIB;
	CEffectPlane**			m_ppEffectPlaneBatch;		// the run in hand, in the order it was walked
	UINT					m_nEffectPlaneBatchMax;
	UINT					m_nEffectPlaneBatchCount;
	UINT					m_nEffectPlaneBatchVertexAt;
	EffectPlaneBatchState	m_arrEffectPlaneBatchRun[EFFECT_BATCH_RUN_MAX];
	UINT					m_arrEffectPlaneBatchRunCount[EFFECT_BATCH_RUN_MAX];
	UINT					m_nEffectPlaneBatchRuns;
	BYTE*					m_pEffectPlaneBatchRunOf;

	// TRUE while something held cannot be drawn out of its turn, in which case
	// it is the only thing held and the next thing to gather flushes it first.
	BOOL					m_bEffectBatchOrdered;

	// What both batches did, so that a run which is not batching shows up as a
	// number rather than a suspicion.  A line a minute, beside the D3D one.
	void EffectBatchReport();

	unsigned __int64		m_nParticleBatchQuads;
	unsigned __int64		m_nParticleBatchDraws;
	unsigned __int64		m_nEffectPlaneBatchPlanes;
	unsigned __int64		m_nEffectPlaneBatchDraws;
	unsigned __int64		m_nEffectBatchFlushes;
	unsigned __int64		m_arrEffectBatchWhy[EFFECT_BATCH_WHY_COUNT];

	// Of the things gathered, how many could be drawn out of their turn - and
	// of the runs opened, how many were opened by nothing but a texture, which
	// is what says whether putting the textures in one sheet would merge them.
	unsigned __int64		m_nParticleBatchIndependent;
	unsigned __int64		m_nParticleBatchRunsTextureOnly;
	unsigned __int64		m_nEffectPlaneBatchIndependent;
	unsigned __int64		m_nEffectPlaneBatchRunsTextureOnly;

	float					m_fTextureCheckTime;
	CGameData*				m_pTexEffectData;
	CGameData*				m_pEffectData;
	CGameData*				m_pObjectData;
	LPDIRECT3DTEXTURE9		m_pTexture[TEX_EFFECT_NUM];
	int						m_nTextureRenderCount[TEX_EFFECT_NUM];
	//CGameData* pObjEffectData[];
//	CSkinnedMesh*			m_pObjEffectMesh[OBJ_EFFECT_NUM];
	map<string, int>		m_mapTexNameToIndex;
//	map<string, int>		m_mapObjNameToIndex;
	map<string, CSkinnedMesh*>	m_mapObjNameToMesh;
	vector<Effect*>			m_vecZEnableEffect;
	vector<string>			m_vecLoadObj;
	// 2009. 11. 23 by jskim 리소스 로딩 구조 변경
	vector<LoadingPriorityInfo>	m_vecLoadingPriority;		
	//end 2009. 11. 23 by jskim 리소스 로딩 구조 변경

	// by dhkwon, 030923
	map<string, LPDIRECT3DTEXTURE9> m_mapTextToTexture;
	map<string, int> m_mapTextRenderCount;

	BOOL					m_bZBufferTemp;
	D3DLIGHT9				m_light2;

	int						m_nParticleEffectCountPerSecond;
	int						m_nSpriteEffectCountPerSecond;
	int						m_nObjectEffectCountPerSecond;
	int						m_nTraceEffectCountPerSecond;

private:
	vector<structInvenParticleInfo>			m_vecInvenParticleInfo;
	vector<structInvenParticleInfo>			m_vecEnemyItemParticleInfo;		// 2012-07-13 by isshin 아템미리보기 인첸트이펙스 적용
};

#endif // !defined(AFX_EFFECTRENDER_H__7B0E8CE2_67C8_4A53_97BE_F7589D2FFB62__INCLUDED_)

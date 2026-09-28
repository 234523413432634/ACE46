///////////////////////////////////////////////////////////////////////////////
//  ShadowTest.cpp : see ShadowTest.h
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShadowTest.h"

#include "AtumApplication.h"
#include "SceneData.h"
#include "Background.h"
#include "QuadGround.h"
#include "Camera.h"
#include "Frustum.h"
#include "ShuttleChild.h"
#include "UnitRender.h"		// scripted units
#include "SkinnedMesh.h"
#include "AtumDatabase.h"
#include "dxutil.h"

#ifdef _SHADOW_MAP
#include "ShadowMap.h"
#endif

#include <stdio.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
//  Defaults
//////////////////////////////////////////////////////////////////////

// A window rather than full screen: a full-screen client that stops responding
// takes the desktop with it, and a scripted run has nobody watching it.
#define SHADOW_TEST_DEFAULT_WIDTH		1280
#define SHADOW_TEST_DEFAULT_HEIGHT		720

// How long the harness will wait for the mesh loader before it gives up and
// measures whatever arrived.
#define SHADOW_TEST_WARMUP_FRAMES		120
#define SHADOW_TEST_WARMUP_MAX_FRAMES	4000

// How many frames get measured, and how far the camera turns between two of
// them.
#define SHADOW_TEST_SWEEP_FRAMES		8
#define SHADOW_TEST_YAW_STEP			0.05f

#define SHADOW_TEST_OUTPUT				"Debug-ShadowTest"
#define SHADOW_TEST_RESULT				"shadowtest-result.txt"

// The clock the harness hands the animation code, in place of the wall clock.
#define SHADOW_TEST_FRAME_STEP			0.0166667f

// Where a scripted unit is held in its animation.
#define SHADOW_TEST_UNIT_POSE_TIME		120.0f

// Both to the debugger and to a file.
//
// A scripted run has nobody watching it and no debugger attached, and the
// client's own stdout is full of the quest parser, so a line that only went to
// OutputDebugString would be lost exactly when it was needed.
#define SHADOW_TEST_LOG		"old\\shadowtest.log"

static char s_szLogPath[MAX_PATH] = { 0 };

static void TestSay(const char* i_szFormat, ...)
{
	char szLine[1024];
	va_list args;
	va_start(args, i_szFormat);
	_vsnprintf_s(szLine, sizeof(szLine), _TRUNCATE, i_szFormat, args);
	va_end(args);

	OutputDebugStringA(szLine);

	if(0 != s_szLogPath[0])
	{
		FILE* pf = NULL;
		if(0 == fopen_s(&pf, s_szLogPath, "at") && NULL != pf)
		{
			fputs(szLine, pf);
			fclose(pf);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//  Construction and configuration
//////////////////////////////////////////////////////////////////////

CShadowTest::CShadowTest()
	: m_nProbes(0),
	  m_nProbe(0),
	  m_eStage(STAGE_START),
	  m_nLoadStep(0),
	  m_nStageFrame(0),
	  m_nSettledFrames(0),
	  m_nWidth(SHADOW_TEST_DEFAULT_WIDTH),
	  m_nHeight(SHADOW_TEST_DEFAULT_HEIGHT),
	  m_nWindowMode(ATUM_WINDOW_MODE_WINDOWED),
	  m_nWarmupFrames(SHADOW_TEST_WARMUP_FRAMES),
	  m_nWarmupMaxFrames(SHADOW_TEST_WARMUP_MAX_FRAMES),
	  m_nSweepFrames(SHADOW_TEST_SWEEP_FRAMES),
	  m_fYawStepDegrees(SHADOW_TEST_YAW_STEP),
	  m_fPitchStepDegrees(0.0f),
	  m_nExperiment(0),
	  m_nShadowMode(1),
	  m_nBenchmarkFrames(0),
	  // The client's flight camera - CAMERA_NEAR_IN_FLIGHT.
	  m_fNearPlane(6.0f),
	  m_fFarPlane(100000.0f),
	  m_nBenchStartTicks(0),
	  m_nBenchCasterDraws(0),
	  m_nBenchReceiverDraws(0),
	  m_fBenchSeconds(0.0),
	  m_fBenchFps(0.0),
	  m_fBenchCasterDraws(0.0),
	  m_fBenchReceiverDraws(0.0),
	  m_nBenchSkinned(0),
	  m_nWasCulledObjects(-1)
{
	memset(m_arrProbe, 0x00, sizeof(m_arrProbe));
	strcpy_s(m_szOutputDirectory, sizeof(m_szOutputDirectory), SHADOW_TEST_OUTPUT);
	m_szConfigPath[0] = 0;
}

CShadowTest::~CShadowTest()
{
}

float CShadowTest::GetFrameStep()
{
	return SHADOW_TEST_FRAME_STEP;
}

// Trims a line in place and answers where the value starts, or NULL if the
// line is a comment or has no "=" in it.
static char* SplitKeyValue(char* io_szLine, char** o_ppszValue)
{
	char* p = io_szLine;
	while(*p == ' ' || *p == '\t')					{ p++; }
	if(*p == 0 || *p == ';' || *p == '#')			{ return NULL; }

	char* pEquals = strchr(p, '=');
	if(NULL == pEquals)								{ return NULL; }
	*pEquals = 0;

	char* pValue = pEquals + 1;
	while(*pValue == ' ' || *pValue == '\t')		{ pValue++; }

	// Both halves lose their trailing space, and the value loses its newline.
	for(char* pEnd = pEquals - 1; pEnd >= p && (*pEnd == ' ' || *pEnd == '\t'); pEnd--)
	{
		*pEnd = 0;
	}
	for(char* pEnd = pValue + strlen(pValue) - 1;
		pEnd >= pValue && (*pEnd == '\r' || *pEnd == '\n' || *pEnd == ' ' || *pEnd == '\t');
		pEnd--)
	{
		*pEnd = 0;
	}

	*o_ppszValue = pValue;
	return p;
}

BOOL CShadowTest::LoadConfig(const char* i_szPath)
{
	strncpy_s(m_szConfigPath, sizeof(m_szConfigPath), i_szPath, _TRUNCATE);

	// Beside the client rather than in the output directory: the config has not
	// been read yet, so where the output goes is not known, and a failure to
	// read the config is exactly the failure this has to be able to report.
	ShadowMakeDirectory("old");
	strcpy_s(s_szLogPath, sizeof(s_szLogPath), SHADOW_TEST_LOG);
	DeleteFileA(s_szLogPath);
	TestSay("ShadowTest: starting, config %s\n", i_szPath);

	FILE* pf = NULL;
	if(0 != fopen_s(&pf, i_szPath, "rt") || NULL == pf)
	{
		TestSay("ShadowTest: %s would not open\n", i_szPath);
		return FALSE;
	}

	SShadowProbe* pProbe = NULL;
	char szLine[512];

	while(NULL != fgets(szLine, sizeof(szLine), pf))
	{
		// A section header starts a probe.  Everything before the first one is
		// the run's own settings.
		char* pTrim = szLine;
		while(*pTrim == ' ' || *pTrim == '\t')		{ pTrim++; }
		if(*pTrim == '[')
		{
			if(m_nProbes >= (int)(sizeof(m_arrProbe) / sizeof(m_arrProbe[0])))
			{
				TestSay("ShadowTest: more than %d probes; the rest are ignored\n", m_nProbes);
				break;
			}
			pProbe = &m_arrProbe[m_nProbes++];
			memset(pProbe, 0x00, sizeof(SShadowProbe));
			_snprintf_s(pProbe->szName, sizeof(pProbe->szName), _TRUNCATE, "probe%d", m_nProbes - 1);
			pProbe->fDistance = 250.0f;
			pProbe->fPitchDegrees = -15.0f;
			// -1 and not the memset's zero: zero is "force it off", and every
			// probe in the regression set would then be measuring a client
			// with the focus map switched off without saying so.
			pProbe->nFocus = -1;
			continue;
		}

		char* pszValue = NULL;
		char* pszKey = SplitKeyValue(szLine, &pszValue);
		if(NULL == pszKey)							{ continue; }

		if(NULL == pProbe)
		{
			if(0 == _stricmp(pszKey, "Width"))				{ m_nWidth = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "Height"))		{ m_nHeight = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "WindowMode"))	{ m_nWindowMode = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "WarmupFrames"))	{ m_nWarmupFrames = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "WarmupMaxFrames")){ m_nWarmupMaxFrames = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "SweepFrames"))	{ m_nSweepFrames = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "NearPlane"))	{ m_fNearPlane = (float)atof(pszValue); }
			else if(0 == _stricmp(pszKey, "FarPlane"))	{ m_fFarPlane = (float)atof(pszValue); }
			else if(0 == _stricmp(pszKey, "BenchmarkFrames"))
			{
				m_nBenchmarkFrames = atoi(pszValue);
			}
			else if(0 == _stricmp(pszKey, "YawStepDegrees"))
			{
				m_fYawStepDegrees = (float)atof(pszValue);
			}
			else if(0 == _stricmp(pszKey, "PitchStepDegrees"))
			{
				m_fPitchStepDegrees = (float)atof(pszValue);
			}
			else if(0 == _stricmp(pszKey, "Experiment"))	{ m_nExperiment = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "ShadowMode"))	{ m_nShadowMode = atoi(pszValue); }
			else if(0 == _stricmp(pszKey, "OutputDirectory"))
			{
				strncpy_s(m_szOutputDirectory, sizeof(m_szOutputDirectory), pszValue, _TRUNCATE);
			}
			continue;
		}

		if(0 == _stricmp(pszKey, "Name"))
		{
			strncpy_s(pProbe->szName, sizeof(pProbe->szName), pszValue, _TRUNCATE);
		}
		else if(0 == _stricmp(pszKey, "Map"))		{ pProbe->nMapIndex = atoi(pszValue); }
		else if(0 == _stricmp(pszKey, "Target"))
		{
			sscanf_s(pszValue, "%f , %f , %f",
					 &pProbe->vTarget.x, &pProbe->vTarget.y, &pProbe->vTarget.z);
		}
		else if(0 == _stricmp(pszKey, "Yaw"))		{ pProbe->fYawDegrees = (float)atof(pszValue); }
		else if(0 == _stricmp(pszKey, "Pitch"))		{ pProbe->fPitchDegrees = (float)atof(pszValue); }
		else if(0 == _stricmp(pszKey, "Distance"))	{ pProbe->fDistance = (float)atof(pszValue); }
		else if(0 == _stricmp(pszKey, "Night"))		{ pProbe->bNight = (0 != atoi(pszValue)); }
		else if(0 == _stricmp(pszKey, "Ground"))	{ pProbe->bOnGround = (0 != atoi(pszValue)); }
		else if(0 == _stricmp(pszKey, "Focus"))		{ pProbe->nFocus = atoi(pszValue); }
		else if(0 == _stricmp(pszKey, "Player"))
		{
			if(3 == sscanf_s(pszValue, "%f , %f , %f",
							 &pProbe->vPlayer.x, &pProbe->vPlayer.y, &pProbe->vPlayer.z))
			{
				pProbe->bHasPlayer = TRUE;
			}
		}
		else if(0 == _stricmp(pszKey, "Crowd"))
		{
			// Crowd = <model index>, <how many>, <spacing> [, <yaw degrees>]
			//
			// Forty characters standing in a square around the probe's target.
			int   nIndex   = 0;
			int   nCount   = 0;
			float fSpacing = 6.0f;
			float fYaw     = 0.0f;
			const int nRead = sscanf_s(pszValue, "%d , %d , %f , %f",
									   &nIndex, &nCount, &fSpacing, &fYaw);
			if(nRead >= 2)
			{
				const int nMax = (int)(sizeof(pProbe->arrUnit) / sizeof(pProbe->arrUnit[0]));

				// A square, filled row by row, centred on the target.
				int nSide = 1;
				while(nSide * nSide < nCount) { nSide++; }
				const float fHalf = 0.5f * (float)(nSide - 1) * fSpacing;

				for(int n = 0; n < nCount && pProbe->nUnits < nMax; n++)
				{
					SShadowUnit* pUnit = &pProbe->arrUnit[pProbe->nUnits++];
					pUnit->nUnitIndex  = nIndex;
					pUnit->fYawDegrees = fYaw;
					pUnit->vPos        = pProbe->vTarget;
					pUnit->vPos.x += (float)(n % nSide) * fSpacing - fHalf;
					pUnit->vPos.z += (float)(n / nSide) * fSpacing - fHalf;
				}
			}
			else
			{
				TestSay("ShadowTest: cannot read  Crowd = %s\n", pszValue);
			}
		}
		else if(0 == _stricmp(pszKey, "Unit"))
		{
			// Unit = <model index>, <x>, <y>, <z> [, <yaw degrees>]
			//
			// The index is what CUnitRender::GetUnitMesh() takes.
			if(pProbe->nUnits < (int)(sizeof(pProbe->arrUnit) / sizeof(pProbe->arrUnit[0])))
			{
				SShadowUnit* pUnit = &pProbe->arrUnit[pProbe->nUnits];
				pUnit->fYawDegrees = 0.0f;
				const int nRead = sscanf_s(pszValue, "%d , %f , %f , %f , %f",
										   &pUnit->nUnitIndex,
										   &pUnit->vPos.x, &pUnit->vPos.y, &pUnit->vPos.z,
										   &pUnit->fYawDegrees);
				if(nRead >= 4)
				{
					pProbe->nUnits++;
				}
				else
				{
					TestSay("ShadowTest: cannot read  Unit = %s\n", pszValue);
				}
			}
		}
	}

	fclose(pf);

	if(0 == m_nProbes)
	{
		TestSay("ShadowTest: %s has no [probe] section in it\n", i_szPath);
		return FALSE;
	}

	if(m_nSweepFrames < 2)
	{
		// One frame cannot disagree with anything.
		m_nSweepFrames = 2;
	}

	TestSay("ShadowTest: %s read, %d probe(s), %d sweep frames, %.4f degrees a frame, experiment %d\n",
			i_szPath, m_nProbes, m_nSweepFrames, m_fYawStepDegrees, m_nExperiment);
	return TRUE;
}
// Every scripted model dropped onto the terrain under it.
//
// Called once, after the map has finished loading and before the warm-up,
// since the height cannot be asked for until the ground exists.
void CShadowTest::PlaceUnitsOnGround()
{
	SShadowProbe& probe = m_arrProbe[m_nProbe];
	if(!probe.bOnGround || NULL == g_pGround)
	{
		return;
	}

	// The camera's own target as well, or a probe that stands its models on the
	// ground still points the camera at a height somebody guessed - which on the
	// first try was underneath it, looking up at the bottom of the map.
	{
		float       fHeight = probe.vTarget.y;
		D3DXVECTOR3 vNormal(0.0f, 1.0f, 0.0f);
		g_pGround->CheckCollMap(probe.vTarget, &fHeight, &vNormal);
		probe.vTarget.y = fHeight;
	}

	for(int n = 0; n < probe.nUnits; n++)
	{
		float       fHeight = probe.arrUnit[n].vPos.y;
		D3DXVECTOR3 vNormal(0.0f, 1.0f, 0.0f);

		g_pGround->CheckCollMap(probe.arrUnit[n].vPos, &fHeight, &vNormal);
		probe.arrUnit[n].vPos.y = fHeight;
	}

	TestSay("ShadowTest: %d model(s) dropped onto the ground\n", probe.nUnits);
}


//////////////////////////////////////////////////////////////////////
//  The camera
//////////////////////////////////////////////////////////////////////

void CShadowTest::PlaceCamera(float i_fYawDegrees, float i_fPitchDegrees)
{
	if(NULL == g_pD3dApp || NULL == g_pD3dApp->m_pCamera)
	{
		return;
	}

	const SShadowProbe& probe = m_arrProbe[m_nProbe];

	// The projection, every frame, and not once when the map loaded.
	//
	// The client sets its own near plane in CAtumApplication::SetCamPosInit(),
	// and since that became a different number for the flight camera than for the
	// on-foot one, whichever of the two ran last decided what the frame was drawn
	// with.
	const FLOAT fAspect = ((FLOAT)g_pD3dApp->GetBackBufferDesc().Width) /
						   (FLOAT)g_pD3dApp->GetBackBufferDesc().Height;
	g_pD3dApp->m_pCamera->SetProjParams(D3DX_PI / 2.5f, fAspect,
										m_fNearPlane, m_fFarPlane);

	const float fYaw   = D3DXToRadian(i_fYawDegrees);
	const float fPitch = D3DXToRadian(i_fPitchDegrees);

	// Yaw 0 looks down +z and increases towards +x, which is how the capture
	// key writes it and how a person reading the config would guess.
	D3DXVECTOR3 vDir(cosf(fPitch) * sinf(fYaw),
					 sinf(fPitch),
					 cosf(fPitch) * cosf(fYaw));
	D3DXVec3Normalize(&vDir, &vDir);

	D3DXVECTOR3 vAt  = probe.vTarget;
	D3DXVECTOR3 vEye = vAt - vDir * probe.fDistance;
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);

	CCamera* pCamera = g_pD3dApp->m_pCamera;
	pCamera->SetViewParams(vEye, vAt, vUp);

	// The renderers read these directly rather than going back to the view
	// matrix - the terrain level of detail, the object fog cull and the shadow
	// box all do - so they have to agree with what was just set.
	pCamera->m_vCamCurrentPos    = vEye;
	pCamera->m_vCamNextPos       = vEye;
	pCamera->m_vCurrentTargetPos = vAt;
	pCamera->m_vNextTargetPos    = vAt;
	pCamera->m_vObjectPos        = vAt;

	// Where the player stands. CheckObjectRenderList() builds the range list
	// round the ship, not round the camera, so putting it anywhere else quietly
	// changes which objects exist at all.
	if(NULL != g_pD3dApp->m_pShuttleChild)
	{
		g_pD3dApp->m_pShuttleChild->m_vPos = probe.bHasPlayer ? probe.vPlayer : vAt;
		g_pD3dApp->m_pShuttleChild->m_vVel = vDir;
	}
}

//////////////////////////////////////////////////////////////////////
//  One frame's worth of what FrameMove() would have done
//////////////////////////////////////////////////////////////////////

void CShadowTest::PrepareFrame(float i_fYawDegrees, float i_fPitchDegrees)
{
	PlaceCamera(i_fYawDegrees, i_fPitchDegrees);

	CCamera* pCamera = g_pD3dApp->m_pCamera;

	// The frustum is read out of the device's transforms, so they have to be on
	// the device before it is built.
	g_pD3dDev->SetTransform(D3DTS_VIEW,       &pCamera->GetViewMatrix());
	g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pCamera->GetProjMatrix());

	pCamera->SetViewPlane();
	if(NULL != g_pFrustum)
	{
		g_pFrustum->Construct(g_pD3dDev);
	}

	if(NULL != g_pScene)
	{
		g_pScene->CheckObjectRenderList();
	}

	// The ground, which does not draw itself without this: CQuadGround::Render()
	// draws from an index buffer CQuadGround::Tick() rebuilds every frame, and
	// the harness does not call CSceneData::Tick(), which is what ticks it in
	// play.
	if(NULL != g_pGround && NULL != g_pGround->m_pQuad &&
	   IsTileMapRenderEnable((USHORT)m_arrProbe[m_nProbe].nMapIndex))
	{
		g_pGround->m_pQuad->Tick();
	}

	// Before the light pass, which runs at the top of the frame.  A model that
	// is not on this list at that point casts nothing.
	QueueUnitCasters();
}

BOOL CShadowTest::IsSettled()
{
	if(NULL == g_pD3dApp || NULL == g_pScene)
	{
		return FALSE;
	}

	// Two conditions, and both are needed.
	if(!g_pD3dApp->IsEmptyLoadingGameDataList())
	{
		m_nSettledFrames = 0;
		m_nWasCulledObjects = (int)g_pScene->m_vectorCulledObjectPtrList.size();
		return FALSE;
	}

	const int nCulled = (int)g_pScene->m_vectorCulledObjectPtrList.size();
	if(nCulled != m_nWasCulledObjects)
	{
		m_nSettledFrames = 0;
		m_nWasCulledObjects = nCulled;
		return FALSE;
	}

	m_nSettledFrames++;
	return (m_nSettledFrames >= 30);
}

//////////////////////////////////////////////////////////////////////
//  Starting a probe
//////////////////////////////////////////////////////////////////////

void CShadowTest::BeginProbe()
{
	const SShadowProbe& probe = m_arrProbe[m_nProbe];

	TestSay("ShadowTest: probe %d/%d '%s' map %04d at (%.1f, %.1f, %.1f) yaw %.2f\n",
			m_nProbe + 1, m_nProbes, probe.szName, probe.nMapIndex,
			probe.vTarget.x, probe.vTarget.y, probe.vTarget.z, probe.fYawDegrees);

	// The map index is the one thing the server used to supply, and half the
	// renderer reads it straight off the ship.
	if(NULL != g_pD3dApp->m_pShuttleChild)
	{
		g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex = (USHORT)probe.nMapIndex;

		// Where the *player* is, which decides which objects exist at all - see the
		// note on vPlayer.
		g_pD3dApp->m_pShuttleChild->m_vPos = probe.bHasPlayer ? probe.vPlayer
															  : probe.vTarget;
		// Off any block boundary it might already be sitting on, so the first
		// CheckObjectRenderList() rebuilds the range list rather than deciding
		// nothing has moved.
		g_pD3dApp->m_pShuttleChild->m_ptOldPoint.x = -100000;
		g_pD3dApp->m_pShuttleChild->m_ptOldPoint.y = -100000;
	}

#ifdef _SHADOW_MAP
	// Before the map loads, not after.
	if(NULL != g_pD3dApp->m_pShadowMap)
	{
		g_pD3dApp->m_pShadowMap->SetDumpTarget(m_szOutputDirectory, probe.szName);

		// And the focus map, if this probe pins it.  Per probe, so the two
		// halves of its launch test are two probes in one config and one run.
		g_pD3dApp->m_pShadowMap->SetFocusOverride(probe.nFocus);
	}
#endif

	m_eStage      = STAGE_LOADMAP;
	m_nLoadStep   = 0;
	m_nStageFrame = 0;
	m_nSettledFrames = 0;
	m_nWasCulledObjects = -1;
}

//////////////////////////////////////////////////////////////////////
//  The run
//////////////////////////////////////////////////////////////////////

BOOL CShadowTest::Tick()
{
	if(NULL == g_pD3dApp)
	{
		return FALSE;
	}


	// The mesh loader's main-thread half runs at the top of Render(), so
	// nothing has to be done for it here.

	switch(m_eStage)
	{
	case STAGE_START:
		{
			ShadowMakeDirectory(m_szOutputDirectory);

			// _GAME, so that everything around the harness's own render behaves as it
			// does in play: the frame is cleared to the map's fog colour rather than to
			// black, the render state prologue in CAtumApplication::Render() runs, and
			// the caster pass - which tests for _GAME - is not skipped.
			g_pD3dApp->m_dwGameState = _GAME;

#ifdef _SHADOW_MAP
			g_nShadowExperiment = m_nExperiment;
			g_pD3dApp->m_nShadowMapMode = m_nShadowMode;
#endif
			BeginProbe();
		}
		break;

	case STAGE_LOADMAP:
		{
			// One of CSceneData's five steps a frame, the same order and the same
			// functions CMapLoad::TickMapLoad() drives them in.
			switch(m_nLoadStep)
			{
			case 0:	g_pScene->StepBackground_Step1();	break;
			case 1:	g_pScene->StepBackground_Step2();	break;
			case 2:	g_pScene->StepBackground_Step3();	break;
			case 3:	g_pScene->StepBackground_Step4();	break;
			case 4:	g_pScene->StepBackground_Step5();	break;
			default:
				break;
			}
			m_nLoadStep++;

			if(m_nLoadStep > 4)
			{
				const SShadowProbe& probe = m_arrProbe[m_nProbe];

				if(NULL == g_pGround)
				{
					TestSay("ShadowTest: map %04d did not load - no ground\n", probe.nMapIndex);
					WriteResult("FAIL map did not load");
					m_eStage = STAGE_DONE;
					break;
				}

				// The day/night state is frozen where the config put it rather than left
				// to CSceneData::SetDay(), which reads the wall clock, cross-fades the sun
				// over five seconds and sends a packet on the way past.
				g_pScene->m_bNight = probe.bNight;
				g_pScene->m_byWeatherType = WEATHER_DEFAULT;
				g_pScene->SetupLights();
				g_pScene->m_light0.Direction = g_pScene->SetLightDirection();
				g_pD3dDev->SetLight(0, &g_pScene->m_light0);
				::SetFogLevel((USHORT)probe.nMapIndex, !probe.bNight);

				// And push the fog at the device, which ::SetFogLevel() does not do.
				g_pScene->m_fFogStartValue = g_pScene->m_fOrgFogStartValue;
				g_pScene->m_fFogEndValue   = g_pScene->m_fOrgFogEndValue;

				g_pD3dDev->SetRenderState(D3DRS_FOGENABLE,
										  IsFogEnableMap((USHORT)probe.nMapIndex));
				g_pD3dDev->SetRenderState(D3DRS_FOGTABLEMODE,   D3DFOG_NONE);
				g_pD3dDev->SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR);
				g_pD3dDev->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
				g_pD3dDev->SetRenderState(D3DRS_FOGSTART,  FtoDW(g_pScene->m_fFogStartValue));
				g_pD3dDev->SetRenderState(D3DRS_FOGEND,    FtoDW(g_pScene->m_fFogEndValue));
				g_pD3dDev->SetRenderState(D3DRS_FOGCOLOR,  g_pScene->m_dwFogColor);

				TestSay("ShadowTest: fog %.1f .. %.1f, colour %06x, %s\n",
						g_pScene->m_fFogStartValue, g_pScene->m_fFogEndValue,
						g_pScene->m_dwFogColor & 0xFFFFFF,
						IsFogEnableMap((USHORT)probe.nMapIndex) ? "on" : "off");

				// The projection the game uses on a field map, so the shadow
				// box - which reads its half angles back out of the projection -
				// is the size it would be in play.
				const FLOAT fAspect = ((FLOAT)g_pD3dApp->GetBackBufferDesc().Width) /
									   (FLOAT)g_pD3dApp->GetBackBufferDesc().Height;
				g_pD3dApp->m_pCamera->SetProjParams(D3DX_PI / 2.5f, fAspect, m_fNearPlane, m_fFarPlane);

				// One forced rebuild of the range list: CheckObjectRenderList()
				// only rebuilds it when the ship crosses a block boundary, and
				// the ship has just been teleported rather than flown.
				PlaceCamera(probe.fYawDegrees, probe.fPitchDegrees);
				g_pD3dDev->SetTransform(D3DTS_VIEW,       &g_pD3dApp->m_pCamera->GetViewMatrix());
				g_pD3dDev->SetTransform(D3DTS_PROJECTION, &g_pD3dApp->m_pCamera->GetProjMatrix());
				g_pD3dApp->m_pCamera->SetViewPlane();
				if(NULL != g_pFrustum)
				{
					g_pFrustum->Construct(g_pD3dDev);
				}
				g_pScene->CheckObjectRenderList(FALSE);

				TestSay("ShadowTest: map %04d loaded, %d objects in range\n",
						probe.nMapIndex, (int)g_pScene->m_vectorRangeObjectPtrList.size());

				// The shadow option, which the ladder now reads, is whatever happens to be
				// saved in option.sys - and a harness measuring the shadow system with the
				// shadow option turned down measures nothing at all, silently.
				if(NULL != g_pSOption)
				{
					g_pSOption->sShadowState = SHADOW_QUALITY_MAX;
				}

				PlaceUnitsOnGround();

#ifdef _SHADOW_MAP
				// Zeroed here, so what the benchmark reports is the timed frames and
				// not the map load that preceded them.
				if(NULL != g_pD3dApp->m_pShadowMap)
				{
					g_pD3dApp->m_pShadowMap->ResetCpuTimers();
				}
#endif

				m_eStage      = STAGE_WARMUP;
				m_nStageFrame = 0;
			}
		}
		break;

	case STAGE_WARMUP:
		{
			// Rendered, but not measured.  The mesh loader is still working, and
			// an object that appears half way through a sweep would show up as a
			// difference between two frames that has nothing to do with shadows.
			PrepareFrame(m_arrProbe[m_nProbe].fYawDegrees, m_arrProbe[m_nProbe].fPitchDegrees);

			// CUnitRender::Tick() is what finishes a queued model load on this thread.
			if(NULL != g_pD3dApp->m_pUnitRender)
			{
				g_pD3dApp->m_pUnitRender->Tick(SHADOW_TEST_FRAME_STEP);
			}

			m_nStageFrame++;

			const BOOL bSettled = IsSettled() && AreUnitsLoaded();
			if((m_nStageFrame >= m_nWarmupFrames && bSettled) ||
			    m_nStageFrame >= m_nWarmupMaxFrames)
			{
				TestSay("ShadowTest: warm-up done after %d frames (%s), %d objects drawn\n",
						m_nStageFrame, bSettled ? "settled" : "gave up",
						(int)g_pScene->m_vectorCulledObjectPtrList.size());

#ifdef _SHADOW_MAP
				if(NULL != g_pD3dApp->m_pShadowMap)
				{
					char szPrefix[64];
					_snprintf_s(szPrefix, sizeof(szPrefix), _TRUNCATE, "%s", m_arrProbe[m_nProbe].szName);
					g_pD3dApp->m_pShadowMap->SetDumpTarget(m_szOutputDirectory, szPrefix);
				}
#endif
				m_eStage      = (m_nBenchmarkFrames > 0) ? STAGE_BENCH : STAGE_SWEEP;
				m_nStageFrame = 0;
			}
		}
		break;

	case STAGE_BENCH:
		{
			// Frames timed and thrown away.
			//
			// Everything else here measures whether the shading is *steady*; this
			// measures what it costs, which is the one question the harness could not
			// answer and the game could only answer with a server and a crowd of real
			// people in front of it.
			PrepareFrame(m_arrProbe[m_nProbe].fYawDegrees, m_arrProbe[m_nProbe].fPitchDegrees);

			if(0 == m_nStageFrame)
			{
				LARGE_INTEGER liNow;
				QueryPerformanceCounter(&liNow);
				m_nBenchStartTicks   = liNow.QuadPart;
				m_nBenchCasterDraws  = 0;
				m_nBenchReceiverDraws = 0;
#ifdef _SHADOW_MAP
				if(NULL != g_pD3dApp->m_pShadowMap)
				{
					m_nBenchCasterDraws   = g_pD3dApp->m_pShadowMap->GetCasterDraws();
					m_nBenchReceiverDraws = g_pD3dApp->m_pShadowMap->GetReceiverDraws();
				}
#endif
			}

			m_nStageFrame++;

			if(m_nStageFrame >= m_nBenchmarkFrames)
			{
				LARGE_INTEGER liNow, liFreq;
				QueryPerformanceCounter(&liNow);
				QueryPerformanceFrequency(&liFreq);

				m_fBenchSeconds = (double)(liNow.QuadPart - m_nBenchStartTicks) /
								  (double)liFreq.QuadPart;
				m_fBenchFps = (m_fBenchSeconds > 0.0)
							? (double)m_nStageFrame / m_fBenchSeconds : 0.0;

				m_fBenchCasterDraws   = 0.0;
				m_fBenchReceiverDraws = 0.0;
				m_nBenchSkinned       = 0;
#ifdef _SHADOW_MAP
				if(NULL != g_pD3dApp->m_pShadowMap)
				{
					m_fBenchCasterDraws =
						(double)(g_pD3dApp->m_pShadowMap->GetCasterDraws() - m_nBenchCasterDraws) /
						(double)m_nStageFrame;
					m_fBenchReceiverDraws =
						(double)(g_pD3dApp->m_pShadowMap->GetReceiverDraws() - m_nBenchReceiverDraws) /
						(double)m_nStageFrame;
					m_nBenchSkinned = g_pD3dApp->m_pShadowMap->GetSkinnedCasters();
				}
#endif
				TestSay("ShadowTest: %d frames in %.3f s = %.1f fps, %.1f caster draws, %.1f receiver draws\n",
						m_nStageFrame, m_fBenchSeconds, m_fBenchFps,
						m_fBenchCasterDraws, m_fBenchReceiverDraws);

#ifdef _SHADOW_MAP
				// Where the caster pass spent its CPU over those frames.
				//
				// The static world is the most expensive rung of the ladder, and until now
				// that was the whole of what was known about it.
				if(NULL != g_pD3dApp->m_pShadowMap)
				{
					CShadowMap*  pMap    = g_pD3dApp->m_pShadowMap;
					const double fFrames = (m_nStageFrame > 0) ? (double)m_nStageFrame : 1.0;
					TestSay("ShadowTest: caster CPU - pose %.4f ms/frame, submit %.4f ms/frame, %.1f poses/frame\n",
							pMap->GetPoseMs() / fFrames,
							pMap->GetDrawMs() / fFrames,
							(double)pMap->GetPoseCalls() / fFrames);
				}
#endif

				WriteBenchmark();

				m_eStage      = STAGE_SWEEP;
				m_nStageFrame = 0;
			}
		}
		break;

	case STAGE_SWEEP:
		{
			if(m_nStageFrame >= m_nSweepFrames)
			{
				m_eStage = STAGE_NEXT;
				break;
			}

			const float fYaw   = m_arrProbe[m_nProbe].fYawDegrees +
								 m_fYawStepDegrees * (float)m_nStageFrame;
			const float fPitch = m_arrProbe[m_nProbe].fPitchDegrees +
								 m_fPitchStepDegrees * (float)m_nStageFrame;
			PrepareFrame(fYaw, fPitch);

#ifdef _SHADOW_MAP
			if(NULL != g_pD3dApp->m_pShadowMap)
			{
				// One dump for this frame, not a burst: the sweep is the burst.
				g_pD3dApp->m_pShadowMap->RequestDumpOnce();
			}
#endif
			WriteProbeText(m_nStageFrame, fYaw, fPitch);
			m_nStageFrame++;
		}
		break;

	case STAGE_NEXT:
		{
			TestSay("ShadowTest: probe '%s' done, %d frames written\n",
					m_arrProbe[m_nProbe].szName, m_nSweepFrames);
			m_nProbe++;
			if(m_nProbe >= m_nProbes)
			{
				WriteResult("OK");
				m_eStage = STAGE_DONE;
			}
			else
			{
				BeginProbe();
			}
		}
		break;

	case STAGE_DONE:
	default:
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//  The scripted units - skinned receivers, made measurable
//////////////////////////////////////////////////////////////////////

// Asks for every model the probe wants.
BOOL CShadowTest::AreUnitsLoaded()
{
	const SShadowProbe& probe = m_arrProbe[m_nProbe];
	if(0 == probe.nUnits)
	{
		return TRUE;
	}
	if(NULL == g_pD3dApp->m_pUnitRender)
	{
		return FALSE;
	}

	BOOL bAll = TRUE;
	for(int n = 0; n < probe.nUnits; n++)
	{
		if(NULL == g_pD3dApp->m_pUnitRender->GetUnitMesh(probe.arrUnit[n].nUnitIndex))
		{
			bAll = FALSE;
		}
	}
	return bAll;
}
void CShadowTest::UnitWorldMatrix(const SShadowUnit& i_unit, D3DXMATRIX* o_pWorld) const
{
	D3DXMatrixRotationY(o_pWorld, D3DXToRadian(i_unit.fYawDegrees));
	o_pWorld->_41 = i_unit.vPos.x;
	o_pWorld->_42 = i_unit.vPos.y;
	o_pWorld->_43 = i_unit.vPos.z;
}

// The scripted models, offered to the caster pass by hand.
//
// A harness run has no network and no player, so CSceneData's unit list is
// empty and the caster walk would find nothing to cast.
void CShadowTest::QueueUnitCasters()
{
#ifdef _SHADOW_MAP
	if(NULL == g_pD3dApp->m_pShadowMap)
	{
		return;
	}

	// Cleared every frame, not once: the pass is told afresh what is standing
	// in the scene, so a probe that ends leaves nothing behind for the next.
	g_pD3dApp->m_pShadowMap->ClearExtraCasters();

	const SShadowProbe& probe = m_arrProbe[m_nProbe];
	if(0 == probe.nUnits || NULL == g_pD3dApp->m_pUnitRender)
	{
		return;
	}

	for(int n = 0; n < probe.nUnits; n++)
	{
		CSkinnedMesh* pMesh = g_pD3dApp->m_pUnitRender->GetUnitMesh(probe.arrUnit[n].nUnitIndex);
		if(NULL == pMesh)
		{
			continue;
		}

		D3DXMATRIX matWorld;
		UnitWorldMatrix(probe.arrUnit[n], &matWorld);

		g_pD3dApp->m_pShadowMap->AddExtraCaster(pMesh, matWorld, SHADOW_TEST_UNIT_POSE_TIME);
	}
#endif
}



// Draws each scripted unit the way CUnitRender::Render() draws a ship, and
// shades it immediately afterwards through the same call the game makes.
//
// Deliberately the real path and not a shortcut: the whole point of standing a
// model here is that whatever CSkinnedMesh::Render() leaves in its buffers is
// what CShadowMap::ShadeSkinnedMesh() then reads.  A hand-rolled draw would
// measure something the game never does.
BOOL CShadowTest::RenderUnits()
{
	const SShadowProbe& probe = m_arrProbe[m_nProbe];
	if(0 == probe.nUnits || NULL == g_pD3dApp->m_pUnitRender)
	{
		return TRUE;
	}

	// The same fixed clock as everything else.
	const float fPoseTime = SHADOW_TEST_UNIT_POSE_TIME;

	BOOL bAll = TRUE;

	g_pD3dDev->SetLight(0, &g_pScene->m_light0);
	g_pD3dDev->LightEnable(0, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	for(int n = 0; n < probe.nUnits; n++)
	{
		const SShadowUnit& unit = probe.arrUnit[n];

		CSkinnedMesh* pMesh = g_pD3dApp->m_pUnitRender->GetUnitMesh(unit.nUnitIndex);
		if(NULL == pMesh)
		{
			bAll = FALSE;
			continue;
		}

		D3DXMATRIX matWorld;
		UnitWorldMatrix(unit, &matWorld);

		pMesh->Tick(fPoseTime);
		pMesh->SetWorldMatrix(matWorld);
		pMesh->AnotherTexture(1);
		pMesh->Render(FALSE, _SHUTTLE);

#ifdef _SHADOW_MAP
		if(NULL != g_pD3dApp->m_pShadowMap)
		{
			g_pD3dApp->m_pShadowMap->ShadeSkinnedMesh(pMesh);
		}
#endif
	}

	return bAll;
}

void CShadowTest::Render()
{
	if(m_eStage != STAGE_WARMUP && m_eStage != STAGE_BENCH && m_eStage != STAGE_SWEEP)
	{
		return;
	}
	if(NULL == g_pD3dApp || NULL == g_pD3dApp->m_pCamera)
	{
		return;
	}

	// The scene, and nothing else.
	g_pD3dDev->SetTransform(D3DTS_VIEW,       &g_pD3dApp->m_pCamera->GetViewMatrix());
	g_pD3dDev->SetTransform(D3DTS_PROJECTION, &g_pD3dApp->m_pCamera->GetProjMatrix());

	if(NULL != g_pScene)
	{
		g_pD3dDev->SetLight(0, &g_pScene->m_light0);
	}
	g_pD3dDev->LightEnable(1, FALSE);
	g_pD3dDev->LightEnable(2, FALSE);
	g_pD3dDev->LightEnable(3, FALSE);

	if(NULL != g_pGround)
	{
		g_pGround->Render();
	}
	if(NULL != g_pScene)
	{
		g_pScene->Render();
	}

	// The scripted units last, so they draw over the scene the way a unit does
	// in the game - after the ground and the static objects, before anything
	// transparent.  Each one shades itself on the way past.
	RenderUnits();
}
// What the timed stage cost, in its own file, one line of numbers and a header
// saying what was standing in the scene when they were taken.
void CShadowTest::WriteBenchmark()
{
	const SShadowProbe& probe = m_arrProbe[m_nProbe];

	char szPath[MAX_PATH];
	_snprintf_s(szPath, sizeof(szPath), _TRUNCATE, "%s\\%s-bench.txt",
				m_szOutputDirectory, probe.szName);

	FILE* pf = NULL;
	if(0 != fopen_s(&pf, szPath, "wt") || NULL == pf)
	{
		return;
	}

	fprintf(pf, "probe            %s\n", probe.szName);
	fprintf(pf, "map              %04d\n", probe.nMapIndex);
	fprintf(pf, "shadow mode      %d\n", m_nShadowMode);
	fprintf(pf, "experiment       %d\n", m_nExperiment);
	fprintf(pf, "models standing  %d\n", probe.nUnits);
	fprintf(pf, "objects in range %d\n",
			(NULL != g_pScene) ? (int)g_pScene->m_vectorRangeObjectPtrList.size() : -1);
	fprintf(pf, "objects drawn    %d\n",
			(NULL != g_pScene) ? (int)g_pScene->m_vectorCulledObjectPtrList.size() : -1);
	fprintf(pf, "\n");
	fprintf(pf, "frames           %d\n", m_nBenchmarkFrames);
	fprintf(pf, "seconds          %.4f\n", m_fBenchSeconds);
	fprintf(pf, "fps              %.2f\n", m_fBenchFps);
	fprintf(pf, "ms per frame     %.4f\n",
			(m_fBenchFps > 0.0) ? (1000.0 / m_fBenchFps) : 0.0);
	fprintf(pf, "caster draws     %.1f per frame\n", m_fBenchCasterDraws);
	fprintf(pf, "receiver draws   %.1f per frame\n", m_fBenchReceiverDraws);
	fprintf(pf, "skinned casters  %d in the last frame\n", m_nBenchSkinned);

	fclose(pf);
}


//////////////////////////////////////////////////////////////////////
//  What a run leaves behind
//////////////////////////////////////////////////////////////////////

// One line a frame, in one file per probe, saying what the harness set.
void CShadowTest::WriteProbeText(int i_nFrame, float i_fYaw, float i_fPitch)
{
	char szPath[MAX_PATH];
	_snprintf_s(szPath, sizeof(szPath), _TRUNCATE, "%s\\%s-camera.txt",
				m_szOutputDirectory, m_arrProbe[m_nProbe].szName);

	FILE* pf = NULL;
	if(0 != fopen_s(&pf, szPath, (0 == i_nFrame) ? "wt" : "at") || NULL == pf)
	{
		return;
	}

	if(0 == i_nFrame)
	{
		const SShadowProbe& probe = m_arrProbe[m_nProbe];
		fprintf(pf, "probe            %s\n", probe.szName);
		fprintf(pf, "map              %04d\n", probe.nMapIndex);
		fprintf(pf, "target           %.3f %.3f %.3f\n",
				probe.vTarget.x, probe.vTarget.y, probe.vTarget.z);
		fprintf(pf, "pitch            %.4f\n", probe.fPitchDegrees);
		fprintf(pf, "distance         %.3f\n", probe.fDistance);
		fprintf(pf, "night            %d\n", probe.bNight ? 1 : 0);
		fprintf(pf, "yaw step         %.5f\n", m_fYawStepDegrees);
		fprintf(pf, "pitch step       %.5f\n", m_fPitchStepDegrees);
		fprintf(pf, "experiment       %d\n", m_nExperiment);
		fprintf(pf, "shadow mode      %d\n", m_nShadowMode);
		fprintf(pf, "\n");
		fprintf(pf, "frame       yaw     pitch          eye x          eye y          eye z    culled  range\n");
	}

	const D3DXVECTOR3 vEye = g_pD3dApp->m_pCamera->m_vCamCurrentPos;
	fprintf(pf, "%5d %9.4f %9.4f %14.4f %14.4f %14.4f %9d %6d\n",
			i_nFrame, i_fYaw, i_fPitch, vEye.x, vEye.y, vEye.z,
			(NULL != g_pScene) ? (int)g_pScene->m_vectorCulledObjectPtrList.size() : -1,
			(NULL != g_pScene) ? (int)g_pScene->m_vectorRangeObjectPtrList.size() : -1);

	fclose(pf);
}

// The file the measuring script looks for.
void CShadowTest::WriteResult(const char* i_szStatus)
{
	char szPath[MAX_PATH];
	_snprintf_s(szPath, sizeof(szPath), _TRUNCATE, "%s\\%s", m_szOutputDirectory, SHADOW_TEST_RESULT);

	FILE* pf = NULL;
	if(0 == fopen_s(&pf, szPath, "wt") && NULL != pf)
	{
		fprintf(pf, "status      %s\n", i_szStatus);
		fprintf(pf, "config      %s\n", m_szConfigPath);
		fprintf(pf, "probes      %d\n", m_nProbes);
		fprintf(pf, "frames      %d\n", m_nSweepFrames);
		fprintf(pf, "yawstep     %.5f\n", m_fYawStepDegrees);
		fprintf(pf, "pitchstep   %.5f\n", m_fPitchStepDegrees);
		fprintf(pf, "experiment  %d\n", m_nExperiment);
		fprintf(pf, "shadowmode  %d\n", m_nShadowMode);
		if(m_nBenchmarkFrames > 0)
		{
			fprintf(pf, "benchframes %d\n", m_nBenchmarkFrames);
			fprintf(pf, "fps         %.2f\n", m_fBenchFps);
			fprintf(pf, "msframe     %.4f\n",
					(m_fBenchFps > 0.0) ? (1000.0 / m_fBenchFps) : 0.0);
			fprintf(pf, "casterdraws %.1f\n", m_fBenchCasterDraws);
			fprintf(pf, "recvdraws   %.1f\n", m_fBenchReceiverDraws);
		}
		for(int n = 0; n < m_nProbes; n++)
		{
			fprintf(pf, "probe       %s map %04d\n", m_arrProbe[n].szName, m_arrProbe[n].nMapIndex);
		}
		fclose(pf);
	}

	TestSay("ShadowTest: run finished - %s\n", i_szStatus);
}

//////////////////////////////////////////////////////////////////////
//  Capturing a probe from a real session
//////////////////////////////////////////////////////////////////////

// The one part of the loop a script cannot do: finding the hut.
//
// A person flies to whatever is misbehaving and presses Ctrl+Shift+F8, and
// this writes the camera they are looking through as a config the harness can
// replay unattended for ever after.
const char* CShadowTest::CaptureProbeFromGame()
{
	static char s_szPath[MAX_PATH] = "Shadow-Test.ini";

	if(NULL == g_pD3dApp || NULL == g_pD3dApp->m_pCamera || NULL == g_pShuttleChild)
	{
		return NULL;
	}

	CCamera* pCamera = g_pD3dApp->m_pCamera;

	const D3DXVECTOR3 vEye = pCamera->m_vCamCurrentPos;
	const D3DXVECTOR3 vAt  = pCamera->GetLookatPt();

	D3DXVECTOR3 vDir = vAt - vEye;
	const float fDistance = D3DXVec3Length(&vDir);
	if(fDistance < 0.001f)
	{
		return NULL;
	}
	vDir /= fDistance;

	// Back out the same two angles PlaceCamera() builds the direction from, so
	// that replaying this puts the camera exactly where it is now.
	float fSin = vDir.y;
	if(fSin < -1.0f)	{ fSin = -1.0f; }
	if(fSin >  1.0f)	{ fSin =  1.0f; }

	const float fPitch = D3DXToDegree(asinf(fSin));
	const float fYaw   = D3DXToDegree(atan2f(vDir.x, vDir.z));

	FILE* pf = NULL;
	if(0 != fopen_s(&pf, s_szPath, "wt") || NULL == pf)
	{
		return NULL;
	}

	fprintf(pf, "; Written by Ctrl+Shift+F8 in game.  Replay with:\n");
	fprintf(pf, ";   Engine.atm -shadowtest %s\n", s_szPath);
	fprintf(pf, "\n");
	fprintf(pf, "Width           = 1280\n");
	fprintf(pf, "Height          = 720\n");
	fprintf(pf, "WindowMode      = %d\n", ATUM_WINDOW_MODE_WINDOWED);
	fprintf(pf, "WarmupFrames    = %d\n", SHADOW_TEST_WARMUP_FRAMES);
	fprintf(pf, "SweepFrames     = %d\n", SHADOW_TEST_SWEEP_FRAMES);
	fprintf(pf, "YawStepDegrees  = %.4f\n", SHADOW_TEST_YAW_STEP);
	fprintf(pf, "; Pitch matters as much as yaw - two corridors kept flickering on\n");
	fprintf(pf, "; pitch alone after the yaw sweep had gone quiet.\n");
	fprintf(pf, "PitchStepDegrees= 0.0000\n");
	fprintf(pf, "OutputDirectory = %s\n", SHADOW_TEST_OUTPUT);
	fprintf(pf, "; 0 none, 1 freeze the light matrix, 2 no object receiver,\n");
	fprintf(pf, "; 3 no caster cull, 4 shade with a constant\n");
	fprintf(pf, "Experiment      = 0\n");
	fprintf(pf, "; 0 turns the light pass off altogether - the control for every\n");
	fprintf(pf, "; experiment above, because it is the scene with none of this work in it.\n");
	fprintf(pf, "ShadowMode      = 1\n");
	fprintf(pf, "\n");
	fprintf(pf, "[probe]\n");
	fprintf(pf, "Name     = captured\n");
	fprintf(pf, "Map      = %d\n", (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
	fprintf(pf, "Target   = %.4f, %.4f, %.4f\n", vAt.x, vAt.y, vAt.z);
	fprintf(pf, "Yaw      = %.4f\n", fYaw);
	fprintf(pf, "Pitch    = %.4f\n", fPitch);
	fprintf(pf, "Distance = %.4f\n", fDistance);

	// And where the player was standing.
	fprintf(pf, "Player   = %.4f, %.4f, %.4f\n",
			g_pShuttleChild->m_vPos.x, g_pShuttleChild->m_vPos.y,
			g_pShuttleChild->m_vPos.z);
	fprintf(pf, "Night    = %d\n", (NULL != g_pScene && g_pScene->m_bNight) ? 1 : 0);

	fclose(pf);

	TestSay("ShadowTest: probe written to %s - map %d, target (%.1f, %.1f, %.1f), yaw %.2f, pitch %.2f, distance %.1f\n",
			s_szPath, (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,
			vAt.x, vAt.y, vAt.z, fYaw, fPitch, fDistance);

	return s_szPath;
}

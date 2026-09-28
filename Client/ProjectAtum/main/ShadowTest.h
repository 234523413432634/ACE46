///////////////////////////////////////////////////////////////////////////////
//  ShadowTest.h : the client rendering one scene to a script, with no server
//
//  The shadow work has one fault left - shading on objects flickers when the
//  camera moves - and six rounds of fixes went past it because the only
//  instrument was a person flying to a hut and describing what they saw.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_SHADOW_TEST_H_
#define _ATUM_SHADOW_TEST_H_

#include <d3dx9.h>

// One skinned model stood in the scene on purpose: a harness run has no
// player, no network and no enemy list, so otherwise no skinned receiver would
// be in the picture.
struct SShadowUnit
{
	int			nUnitIndex;			// what CUnitRender::GetUnitMesh() is asked for
	D3DXVECTOR3	vPos;
	float		fYawDegrees;
};

// One camera to put the scene in front of.
struct SShadowProbe
{
	char		szName[64];
	int			nMapIndex;
	D3DXVECTOR3	vTarget;			// what the camera looks at, in world units
	float		fYawDegrees;		// 0 looks down +z, 90 looks down +x
	float		fPitchDegrees;		// negative looks down, which is the usual case
	float		fDistance;			// how far behind the target the eye sits
	BOOL		bNight;				// which of the map's two sun directions to use

	// Put every scripted model on the ground rather than where its y says.
	//
	// Guessing a height by hand put three probes' worth of models inside the
	// shadow of something else and read as a caster that did not work.
	BOOL		bOnGround;

	// Where the player is standing, which is *not* the same as what the camera is
	// looking at and must not be inferred from it.
	D3DXVECTOR3	vPlayer;
	BOOL		bHasPlayer;

	// The focus map, forced on or off for this probe, or -1 to leave it to
	// Shadow.ini and the quality ladder.
	int			nFocus;

	// Sixty-four, because a city is a crowd and the open question
	// is what a crowd costs.  Crowd= fills these from one line.
	SShadowUnit	arrUnit[64];
	int			nUnits;
};

class CShadowTest
{
public:
	CShadowTest();
	~CShadowTest();

	// Reads a config, or answers FALSE and says why through DbgOut.  A config
	// with no probes in it is an error, not an empty run.
	BOOL	LoadConfig(const char* i_szPath);

	// What the window should be, before the device is made.  Answers the config
	// values, or the defaults if no config was read.
	int		GetWidth() const		{ return m_nWidth; }
	int		GetHeight() const		{ return m_nHeight; }
	int		GetWindowMode() const	{ return m_nWindowMode; }

	// Once a frame, in place of CAtumApplication::FrameMove().  Answers FALSE
	// when the run is over, at which point the client should close.
	BOOL	Tick();

	// What the caller should put in the application's elapsed time before calling
	// Tick(), in place of the wall clock.
	static float	GetFrameStep();

	// Once a frame, inside BeginScene, in place of the game's render.
	void	Render();

	// Writes what a person is looking at now as a probe, so that a scene found by
	// hand can be replayed by script for ever after.
	static const char*	CaptureProbeFromGame();

	// Where a run puts its dumps and its result file.
	const char*	GetOutputDirectory() const	{ return m_szOutputDirectory; }

private:
	enum EStage
	{
		STAGE_START = 0,	// nothing loaded yet
		STAGE_LOADMAP,		// stepping CSceneData's five load steps
		STAGE_WARMUP,		// rendering while the mesh loader catches up
		STAGE_BENCH,		// frames timed, nothing measured, nothing dumped
		STAGE_SWEEP,		// the frames that get measured
		STAGE_NEXT,			// this probe is done
		STAGE_DONE			// the run is over
	};

	void	BeginProbe();
	// Draws the probe's scripted units and shades each one, the way
	// CUnitRender::Render() does.
	BOOL	RenderUnits();
	// Asks for every unit the probe wants, so the loader starts on them.
	BOOL	AreUnitsLoaded();

	// Where a scripted model stands, as a world matrix.  Read by the draw and
	// by the caster, so that both put it in exactly the same place.
	void	UnitWorldMatrix(const SShadowUnit& i_unit, D3DXMATRIX* o_pWorld) const;

	// Hands the scripted models to the shadow map as casters.
	//
	// The caster pass finds the game's models through
	// CSceneData::m_vecUnitRenderList, which a harness run has nothing in - no
	// network, no player, no enemies.
	void	QueueUnitCasters();
	// Drops the probe's models onto the terrain, once the map is loaded.
	void	PlaceUnitsOnGround();

	void	PlaceCamera(float i_fYawDegrees, float i_fPitchDegrees);
	void	PrepareFrame(float i_fYawDegrees, float i_fPitchDegrees);
	BOOL	IsSettled();
	void	WriteResult(const char* i_szStatus);
	void	WriteProbeText(int i_nFrame, float i_fYaw, float i_fPitch);

	// What the timed stage found, appended to the result file.
	void	WriteBenchmark();

	SShadowProbe	m_arrProbe[16];
	int				m_nProbes;
	int				m_nProbe;

	EStage			m_eStage;
	int				m_nLoadStep;
	int				m_nStageFrame;
	int				m_nSettledFrames;

	// Config.
	int				m_nWidth;
	int				m_nHeight;
	int				m_nWindowMode;
	int				m_nWarmupFrames;
	int				m_nWarmupMaxFrames;
	int				m_nSweepFrames;
	// How many frames to time before the sweep, with the camera standing still.
	int				m_nBenchmarkFrames;

	// The camera's own near and far planes, which the game sets to 1 and 100000
	// and which decide how much depth precision there is to fight over.
	float			m_fNearPlane;
	float			m_fFarPlane;
	float			m_fYawStepDegrees;
	// Pitch matters as much as yaw and for a while this could not sweep it at
	// all, which is why two corridors kept flickering after the yaw sweep had
	// gone quiet: nothing here was moving the camera the way the report said.
	float			m_fPitchStepDegrees;
	int				m_nExperiment;
	// The control for every experiment: 0 turns the whole light pass off, so what
	// is left is the scene the client always drew.
	int				m_nShadowMode;
	char			m_szOutputDirectory[MAX_PATH];
	char			m_szConfigPath[MAX_PATH];

	// The timed stage's readings: the clock at the first timed frame, and the
	// shadow pass's cumulative counters at the same moment.
	__int64			m_nBenchStartTicks;
	unsigned __int64 m_nBenchCasterDraws;
	unsigned __int64 m_nBenchReceiverDraws;
	double			m_fBenchSeconds;
	double			m_fBenchFps;
	double			m_fBenchCasterDraws;
	double			m_fBenchReceiverDraws;
	int				m_nBenchSkinned;

	// What the last settle check saw, for IsSettled().
	int				m_nWasCulledObjects;
};

#endif // _ATUM_SHADOW_TEST_H_

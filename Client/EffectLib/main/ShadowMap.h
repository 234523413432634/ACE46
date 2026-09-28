///////////////////////////////////////////////////////////////////////////////
//  ShadowMap.h : the sun's view of the world, rendered into a depth map
//
//  This draws the terrain from the sun's angle into an off-screen target and
//  puts that target on the screen behind a debug key.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_SHADOW_MAP_H_
#define _ATUM_SHADOW_MAP_H_

#include <d3d9.h>
#include <d3dx9.h>

class CQuadTree;

///////////////////////////////////////////////////////////////////////////////
//  The quality ladder - g_pSOption->sShadowState
//
//  A saved option with a combo box in the interface that already runs 0 to 9
//  (MAX_OPTION_VALUE).
///////////////////////////////////////////////////////////////////////////////
enum
{
	SHADOW_QUALITY_NONE			= 0,	// nothing at all, blobs included
	SHADOW_QUALITY_BLOB			= 1,	// the blob shadows, as the game always did
	SHADOW_QUALITY_TERRAIN		= 2,	// the ground casts
	SHADOW_QUALITY_OBJECTS		= 3,	// and the static world
	SHADOW_QUALITY_SKINNED		= 4,	// and ships and people, one cascade
	SHADOW_QUALITY_SKINNED_2	= 5,	// two
	SHADOW_QUALITY_FILTER		= 6,	// and the wide filter
	SHADOW_QUALITY_SOFT			= 7,	// and soft edges
	SHADOW_QUALITY_SKINNED_3	= 8,	// three cascades of skinned casters
	SHADOW_QUALITY_MAX			= 9		// four
};

// How much of the old blob shadow the client should still draw, on the same
// scale as g_pSOption->sShadowState - so the existing tests read the same way.
int ShadowBlobLevel();

// Makes a directory and every one it sits inside.
void ShadowMakeDirectory(const char* i_szPath);

// The shadow map is an atlas: SHADOW_CASCADE_COUNT tiles of the tile size laid
// out two by two, with the fourth quarter unused.
#define SHADOW_MAP_SIZE_DEFAULT		2048
#define SHADOW_TILE_SIZE_DEFAULT	1024
#define SHADOW_CASCADE_COUNT		4

///////////////////////////////////////////////////////////////////////////////
//  The focus map
//
//  A fifth box, fitted to the player rather than to a slice of the view
//  frustum, and chosen by containment rather than by depth.
///////////////////////////////////////////////////////////////////////////////
#define SHADOW_FOCUS_SIZE_DEFAULT	1024

// The focus box is one entry past the last cascade, so SelectCascade(),
// IsSphereLit() and the dump all read it without knowing it is special.
#define SHADOW_FOCUS_CASCADE		SHADOW_CASCADE_COUNT

// How much wider than the model the box is.
#define SHADOW_FOCUS_MARGIN			3.0f
#define SHADOW_FOCUS_MIN_RADIUS		4.0f
#define SHADOW_FOCUS_MAX_RADIUS		80.0f

// How much of the box's edge fades back into the cascade underneath it, as a
// fraction of its half width.
#define SHADOW_FOCUS_EDGE_FADE		0.15f

// How much more bias the focus map gets than the same-sized patch of a
// cascade.
//
// One, in principle: the bias is what covers the depth a shadow map texel
// spans, so a box with a texel a thirtieth the size needs a thirtieth the
// headroom and the scaling in SetReceiverConstants() does exactly that.
#define SHADOW_FOCUS_BIAS_DEFAULT	1.0f

// Which rung of the quality ladder turns it on when Focus is left at auto.
#define SHADOW_QUALITY_FOCUS		SHADOW_QUALITY_SKINNED_2

// Where the splits fall, in view depth. Shadow.ini overrides all four.
//
// Not a textbook logarithmic split over the far plane, and not the old
// 150/600/2000 either.
#define SHADOW_SPLIT_0_DEFAULT		200.0f
#define SHADOW_SPLIT_1_DEFAULT		700.0f
#define SHADOW_SPLIT_2_DEFAULT		2000.0f

#define SHADOW_CITY_SPLIT_0_DEFAULT	130.0f
#define SHADOW_CITY_SPLIT_1_DEFAULT	400.0f
#define SHADOW_CITY_SPLIT_2_DEFAULT	1200.0f
#define SHADOW_CITY_RANGE_DEFAULT	4000.0f

// How much of each cascade fades into the next, as a fraction of where it
// ends.
//
// Nothing else removes the line at a split.
#define SHADOW_BLEND_DEFAULT		0.20f

// Which maps count as cities, by index.
//
// Not by CSceneData::m_byMapType, which sounds like the right thing and is
// not: **nothing in this client ever sets it to MAP_TYPE_CITY.** The line that
// would is commented out in SceneData.cpp, which is also why
// CCamera::SetCityCamera() and its 20-to-112 unit zoom never run.
#define SHADOW_CITY_MAP_MIN_DEFAULT	1000
#define SHADOW_CITY_MAP_MAX_DEFAULT	2999

// How much each cascade reaches back past its split, as a fraction of its own
// depth.
#define SHADOW_SPLIT_OVERLAP	0.15f

// How many cascades the skinned casters go into, starting from the nearest.
//
// Two by default, so a person casts out to the middle cascade's 600 units.
#define SHADOW_SKINNED_CASCADES_DEFAULT	2

// How many cascades the *static* world casts into, nearest first.
//
// The same dial as the one above, for the rung that actually costs something.
#define SHADOW_OBJECT_CASCADES_DEFAULT	4

// Which PCF kernel the receiver uses.  1 is four bilinear-weighted taps, 2 is
// nine.  See the Receiver techniques.
#define SHADOW_FILTER_DEFAULT	1

// How wide a penumbra gets, as a ratio: world units of blur per world unit
// between the caster and the ground it lands on.
#define SHADOW_SOFT_SIZE_DEFAULT	0.025f

// And the widest it is allowed to get, in **world units**, so a caster a long
// way up cannot ask for a kernel that costs the frame.
#define SHADOW_SOFT_MAX_DEFAULT		5.0f

// How wide the shadow edge filter is, in **world units**, in every cascade and
// in the focus map.
#define SHADOW_FILTER_WIDTH_DEFAULT	0.30f

// And the most it may become in any one cascade's texels, which is what bounds
// the cost: the kernel is twelve bilinear taps once it is wider than a texel,
// and it reaches this far past the sample.
#define SHADOW_FILTER_MAX_TEXELS	12.0f

// And the least it may be, in texels, whatever the world width works out to.
//
// FilterWidth is a world distance, and a far cascade's texel is already wider
// than any width worth setting - twelve world units at Range - so every far
// cascade sat on the old floor of one texel, which is a single bilinear tap
// and no filtering at all.
#define SHADOW_FILTER_MIN_TEXELS	2.0f

// And the focus map's own kernel width, in world units, kept apart from the
// cascades'.
#define SHADOW_FOCUS_FILTER_WIDTH_DEFAULT	0.30f

///////////////////////////////////////////////////////////////////////////////
//  The two numbers the receiver pass is depth-tested by
//
//  This pass redraws geometry the main pass already drew and depth-tests it
//  against the main pass's own output, and the two do not compute the same
//  depth - see the long note in the Receiver technique.
///////////////////////////////////////////////////////////////////////////////
#define SHADOW_RECEIVER_BIAS_DEFAULT		-0.000002f
#define SHADOW_RECEIVER_SLOPE_DEFAULT	-2.0f

// Named maps whose sun is tilted down to a given pitch, in degrees below the
// horizon.
enum { MAX_SUN_OVERRIDES = 32 };

///////////////////////////////////////////////////////////////////////////////
//  Freezing one input at a time
//
//  Object shading flickers when the camera moves and stops dead when it does
//  not, and six rounds of changing constants went past the cause.
///////////////////////////////////////////////////////////////////////////////
enum
{
	// Everything as it is.
	SHADOW_EXPERIMENT_NONE				= 0,

	// The light's box, its matrix and its extents are worked out once and then
	// held.
	SHADOW_EXPERIMENT_FREEZE_LIGHT		= 1,

	// Only the ground receives.  If the flicker goes with the objects it is in
	// the object path; if the ground keeps flickering it is in the terrain
	// path, which has no pose and no hierarchy in it at all.
	SHADOW_EXPERIMENT_NO_OBJ_RECEIVER	= 2,

	// Every object casts, with no light-box cull.  Slow, and it removes the one
	// remaining per-frame decision the caster pass makes.
	SHADOW_EXPERIMENT_NO_CASTER_CULL	= 3,

	// The shadow term is replaced by a constant, so every receiver goes uniformly
	// darker and nothing should change from frame to frame.
	SHADOW_EXPERIMENT_FLAT_SHADE		= 4,

	// The skinned receivers - units, characters, monsters - are left out, so what
	// shades is only the ground and the static objects.
	SHADOW_EXPERIMENT_NO_SKINNED_RECV	= 5,

	// Nothing skinned casts, so the only shadows in the map are the terrain's and
	// the static objects'.
	SHADOW_EXPERIMENT_NO_SKINNED_CAST	= 6
};

extern int g_nShadowExperiment;

class CShadowMap
{
public:
	CShadowMap();
	~CShadowMap();

	// The same four-part lifetime every other renderer in the client has, wired
	// into CAtumApplication's chain next to CFxSystem's.
	void	InitDeviceObjects();
	void	RestoreDeviceObjects();
	void	InvalidateDeviceObjects();
	void	DeleteDeviceObjects();

	// FALSE when the hardware or the effect would not have it, in which case
	// every other call here does nothing and the client renders as before.
	BOOL	IsAvailable() const		{ return m_bAvailable; }

	// Draws the casters into the map.
	void	RenderCasters();

	// The map itself, in a corner of the screen, when the debug view is on.
	// Call last, so nothing draws over it.
	void	RenderDebugOverlay();

	///////////////////////////////////////////////////////////////////////////
	//  Receiving
	//
	//  The scene is drawn exactly as it always was, and then the receivers are
	//  drawn a second time with a shader that outputs only the shadow term,
	//  multiplied over what is already in the frame.
	///////////////////////////////////////////////////////////////////////////
	void	QueueTerrainReceiver(LPDIRECT3DVERTEXBUFFER9 i_pVB, LPDIRECT3DINDEXBUFFER9 i_pIB,
								 int i_nVertices, int i_nTriangles);
	void	FlushTerrainReceivers();

	// The static objects, shaded in one go after CSceneData::Render() has drawn
	// them.
	void	FlushObjectReceivers();

	// One skinned model, shaded, immediately after it has drawn itself.
	//
	// Units, characters and monsters are not in CSceneData's object list and are
	// not drawn through CObjectChild, so FlushObjectReceivers() never sees them;
	// each one draws itself from its own renderer and this is called on the way
	// past.
	void	ShadeSkinnedMesh(class CSkinnedMesh* i_pMesh);

	// A skinned model that is in none of the game's lists, offered to the caster
	// pass by hand.
	void	ClearExtraCasters()			{ m_nExtraCasters = 0; }
	void	AddExtraCaster(class CSkinnedMesh* i_pMesh, const D3DXMATRIX& i_matWorld,
						   float i_fPoseTime);

	// FALSE when there is nothing to receive - no sun, the pass turned off, or
	// the hardware would not have it - so a caller can skip the work.
	BOOL	IsReceiving() const;

	// Writes the map as a .png and everything that went into it as a .txt, into
	// Debug-ShadowMap\ beside the client.
	void	RequestDump();

	// One dump of the frame about to be drawn, rather than a burst.
	void	RequestDumpOnce()			{ m_nDumpsWanted = 1; }

	// Where dumps go and what they are called.
	void	SetDumpTarget(const char* i_szDirectory, const char* i_szPrefix);

	int		GetDumpsWritten() const		{ return m_nDumpsWritten; }

	// The rung of the ladder in force: the option, or Shadow.ini's Quality
	// if one was written.
	int		GetQuality() const			{ return m_nQuality; }

	// Pins the focus map on (1) or off (0) whatever Shadow.ini and the quality
	// ladder say, or -1 to hand it back to them.
	void	SetFocusOverride(int i_nFocus)
	{
		m_nFocusSetting = (i_nFocus < -1) ? -1 : ((i_nFocus > 1) ? 1 : i_nFocus);
	}

	// TRUE when the focus map was built and drawn this frame.
	BOOL	HasFocusThisFrame() const	{ return m_bFocusThisFrame; }

	// TRUE when the map the player is on is on the excluded list, so the
	// cascade system is off and the blob shadows are back.
	BOOL	IsExcludedMap() const;

	// What the pass cost this run, cumulative since the client started.
	unsigned __int64	GetCasterDraws() const		{ return m_nCasterDraws; }

	// Where the object caster pass's CPU time went, in milliseconds, since the
	// last ResetCpuTimers().
	double				GetPoseMs() const;
	double				GetDrawMs() const;
	unsigned __int64	GetPoseCalls() const		{ return m_nPoseCalls; }
	void				ResetCpuTimers()			{ m_fPoseUs = 0.0; m_fDrawUs = 0.0; m_nPoseCalls = 0; }
	unsigned __int64	GetReceiverDraws() const	{ return m_nReceiverDraws; }
	int					GetSkinnedCasters() const	{ return m_nSkinnedCasters; }

	// What the scene passes of steps two onwards will need.  Nothing reads
	// these yet.
	LPDIRECT3DTEXTURE9	GetShadowTexture() const	{ return m_pShadowTexture; }
	const D3DXMATRIX&	GetLightViewProj() const	{ return m_matLightViewProj; }
	BOOL				HasShadowsThisFrame() const	{ return m_bLitThisFrame; }

private:

	// One cascade: the box fitted to one slice of the view frustum, and
	// everything the passes need to know about it.
	struct SCascade
	{
		D3DXMATRIX	matViewProj;
		D3DXVECTOR3	vCentre;
		D3DXVECTOR3	vEye;
		float		fRadius;		// half the box's width, in world units
		float		fNear;
		float		fFar;
		float		fSliceNear;		// the view depths this cascade covers
		float		fSliceFar;
	};
	// One past the cascades, so that the focus box is m_arrCascade[
	// SHADOW_FOCUS_CASCADE] and every pass that reads a box through
	// SelectCascade() reads it without a special case.
	SCascade				m_arrCascade[SHADOW_CASCADE_COUNT + 1];

	// Which one the caster pass is drawing into.  The receiver reads all of
	// them and picks per pixel.
	int						m_nCascade;

	// What Shadow.ini said, or the defaults.  Read once, before the target is
	// made, and clamped to what the hardware will actually give.
	int						m_nMapSize;
	int						m_nTileSize;
	int						m_nSkinnedCascades;
	int						m_nObjectCascades;
	BOOL					m_bSetObjectCascades;

	// Whether the animated part of a static object casts and receives.
	BOOL					m_bAnimatedObjects;
	int						m_nFilter;

	// The focus map.  m_nFocusSetting is Shadow.ini's Focus: -1 leaves it to the
	// quality ladder, 0 forces it off, 1 forces it on.
	int						m_nFocusSize;
	int						m_nFocusSetting;
	float					m_fFocusBias;
	// The cascades' kernel, in world units - one value for field maps and one for
	// cities, chosen per frame the way the splits are.
	float					m_fMapFilterWidth;
	float					m_fCityFilterWidth;
	float					m_fFilterWidth;

	// And the same for which kernel is used at all.
	int						m_nMapFilter;
	int						m_nCityFilter;
	float					m_fFocusFilterWidth;	// and the focus map's own
	float					m_fReceiverBias;

	// The slope bias, one for field maps and one for cities, chosen per frame
	// beside the splits and the filter width.
	float					m_fMapSlopeBias;
	float					m_fCitySlopeBias;
	float					m_fReceiverSlopeBias;
	// Maps whose sun is tilted, by index, and to what pitch in degrees.
	struct SSunOverride
	{
		int		nMapIndex;
		float	fPitchDegrees;
	};
	SSunOverride			m_arrSunOverride[MAX_SUN_OVERRIDES];
	int						m_nSunOverrides;

	// The pitch this map is pinned to, in degrees, or 0 for "leave it alone".
	float	SunPitchForThisMap() const;
	BOOL					m_bFocusWanted;		// what the ladder and the setting decided
	BOOL					m_bFocusThisFrame;	// and whether there was anyone to fit it to

	// What the option asked for this frame, and which of the settings above
	// came from Shadow.ini and so must not be overwritten by it.
	int						m_nQuality;

	// Shadow.ini's Quality, which pins the ladder regardless of the option.
	// -1 - the default - leaves the option in charge.
	int						m_nQualityOverride;
	BOOL					m_bSetSkinned;
	BOOL					m_bSetFilter;

	// Whether the static world casts.  Terrain always does at quality 2 and
	// up; this is the rung above it.
	BOOL					m_bCastObjects;
	float					m_fSoftSize;
	float					m_fSoftMax;
	// Two sets of splits and the one in force this frame.
	float					m_arrMapSplit[SHADOW_CASCADE_COUNT];
	float					m_arrCitySplit[SHADOW_CASCADE_COUNT];
	float					m_arrSplit[SHADOW_CASCADE_COUNT];	// the far end of each
	float					m_fBlend;
	int						m_nCityMapMin;
	int						m_nCityMapMax;

	// Maps that opt out of the cascade system entirely and go back to the blob
	// shadows the game shipped with.
	enum { MAX_EXCLUDED_MAPS = 64 };
	int						m_arrExcludedMap[MAX_EXCLUDED_MAPS];
	int						m_nExcludedMaps;

	// Whether entering a map writes a dump.  Off: it is a 2048-square png
	// and a text file every time anybody walks through a gate.
	BOOL					m_bDumpOnMapChange;
	// Fits every cascade's box round its slice of the view frustum.
	BOOL	BuildLightMatrices();

	// One cascade, fitted to the view depths between i_fSliceNear and
	// i_fSliceFar.
	BOOL	BuildCascade(int i_nCascade, const D3DXVECTOR3& i_vLight,
						 float i_fSliceNear, float i_fSliceFar);

	// Points m_matLightViewProj and the m_fDump* box fields at one cascade, so
	// that everything the caster pass does - the culls, the terrain, the objects,
	// the dump - goes on reading one box and needs no cascade of its own.
	void	SelectCascade(int i_nCascade);

	// Reads Shadow.ini if there is one.
	void	LoadSettings();

	// Turns g_pSOption->sShadowState into the settings above, once a frame.
	void	ApplyQuality();

	// Which set of splits a line in the ini is writing to.
	float*	Splits(BOOL i_bCity)	{ return i_bCity ? m_arrCitySplit : m_arrMapSplit; }

	// The atlas rectangle a cascade lives in, in texels.
	void	CascadeTile(int i_nCascade, int* o_pX, int* o_pY) const;

	// The full-block index buffer, built once the first block says how big a
	// block is.  Every block has the same vertex layout, so they share it.
	BOOL	EnsureBlockIndexBuffer(int i_nBlockSize);

	void	RenderTerrainCasters();
	void	GatherBlocks(CQuadTree* i_pNode);

	// The static objects, into the map.
	void	RenderObjectCasters();

	// The skinned casters - ships, characters and monsters - into the map.
	//
	// One list walk, not three: everything skinned that the scene draws is in
	// CSceneData::m_vecUnitRenderList, whatever renderer it came from.
	void	RenderSkinnedCasters();

	// Answers the mesh one unit draws itself with, and the world matrix it draws
	// with, or NULL if the model has not been loaded yet.
	class CSkinnedMesh*	FindUnitMesh(class CUnitData* i_pUnit, D3DXMATRIX* o_pWorld,
									float* o_pPoseTime);

	// Poses, skins and draws one skinned model into the map.
	void	PoseSkinnedCaster(class CSkinnedMesh* i_pMesh, const D3DXMATRIX& i_matWorld,
							  float i_fPoseTime);

	// And draws what that left behind, into whichever cascade is bound.
	void	DrawSkinnedCaster(class CSkinnedMesh* i_pMesh);

	// Sets the viewport to one cascade's tile.  FALSE if the device refuses it.
	// SHADOW_FOCUS_CASCADE is not a tile and has no viewport of its own here;
	// RenderFocusCasters() binds its whole target once instead.
	BOOL	SetCascadeViewport(int i_nCascade);

	// Where the focus box goes, how big, and who it is for.
	BOOL	FindFocusTarget(D3DXVECTOR3* o_pPos, float* o_pRadius);

	// TRUE when this entry of the skinned caster walk is the one the focus map
	// is drawing, so the cascade pass leaves it out.
	BOOL	IsFocusCaster(int i_nSlot, int i_nUnits, class CUnitData* i_pUnit) const;

	// The box itself, fitted round what FindFocusTarget() answered and snapped
	// to its own texel grid the same way BuildCascade() snaps the cascades.
	BOOL	BuildFocusCascade(const D3DXVECTOR3& i_vLight);

	// The player, and only the player, into the focus map. The receiver takes the
	// darker of the focus and cascade answers rather than replacing one with the
	// other, so nothing can go missing and the cascades hold everything except
	// this one model.
	void	RenderFocusCasters();

	// TRUE when a sphere of this radius at this point can be inside the light's
	// box.
	BOOL	IsSphereLit(const D3DXVECTOR3& i_vPos, float i_fRadius) const;

	// Poses one object's hierarchy the way CObjRender::Render() would, and
	// answers its mesh, or NULL if it has nothing to draw.
	class CSkinnedMesh*	PoseObject(class CObjectChild* i_pObject);

	// Walks a posed hierarchy setting g_matWorld per container and drawing every
	// subset.
	void	DrawPosedHierarchy(struct SFrame* i_pFrame, LPDIRECT3DTEXTURE9* io_ppBoundTexture,
							   BOOL i_bNeedTexture, unsigned __int64* io_pnDraws,
							   const D3DXMATRIX* i_pObjectWorld,
							   class CSkinnedMesh* i_pMesh,
							   BOOL i_bDrawSkinned);

	// Ships, characters and monsters: DrawPosedHierarchy() with no texture
	// wanted and no object world matrix, which is all a unit ever needed.
	void	DrawSkinnedFrames(struct SFrame* i_pFrame, class CSkinnedMesh* i_pMesh,
							  unsigned __int64* io_pnDraws);

	// TRUE when any part of the block can be inside the light's box.
	BOOL	IsBlockLit(const class CQuadGround* i_pBlock) const;

	// Where the caster pass's CPU time actually goes, split between posing the
	// hierarchies and submitting the draws.
	double				m_fPoseUs;
	double				m_fDrawUs;
	unsigned __int64	m_nPoseCalls;

	// One QueryPerformanceCounter tick, in microseconds.
	double				m_fUsPerTick;
	__int64	NowTicks() const;

	// A line a minute into the debug output, on the same footing as the one
	// CD3DFilteredDevice writes - what the light pass is costing.
	void	ReportOccasionally();

	// The two halves of a dump.
	void	WriteDump();
	// The frame and nothing else, for a dump asked for on a frame where the light
	// pass did not run at all.
	void	WriteDumpFrameOnly();
	BOOL	WriteDumpImage(const char* i_szPath);
	BOOL	WriteDumpImageOf(const char* i_szPath, LPDIRECT3DSURFACE9 i_pSurface,
							 int i_nSize);
	BOOL	WriteDumpScreen(const char* i_szPath);
	void	CapturePendingFrame();
	char					m_szPendingScreen[MAX_PATH];
	BOOL					m_bScreenPending;
	void	WriteDumpText(const char* i_szPath);

	// What the last WriteDumpImage() found in the map, for the text half.
	float					m_fDumpDepthMin;
	float					m_fDumpDepthMax;
	double					m_fDumpDepthMean;
	double					m_fDumpUndrawn;		// fraction of texels still at the clear value
	BOOL					m_bDumpStatsValid;

	// The values BuildLightMatrices() worked out, kept so that a dump can say
	// what the box was rather than working it out a second time.
	D3DXVECTOR3				m_vDumpLightDir;
	D3DXVECTOR3				m_vDumpCentre;
	D3DXVECTOR3				m_vDumpEye;
	float					m_fDumpRadius;
	float					m_fDumpNear;
	float					m_fDumpFar;

	int						m_nDumpsWanted;
	int						m_nDumpsWritten;
	int						m_nLastMapIndex;

	// Where SetDumpTarget() put them.  Defaulted, so a client that never calls
	// it writes exactly where it always did.
	char					m_szDumpDirectory[MAX_PATH];
	char					m_szDumpPrefix[64];

	// The light matrix and box SHADOW_EXPERIMENT_FREEZE_LIGHT holds still, and
	// whether one has been taken yet.
	SCascade				m_arrFrozenCascade[SHADOW_CASCADE_COUNT + 1];
	D3DXVECTOR3				m_vFrozenLightDir;
	BOOL					m_bLightFrozen;

	LPDIRECT3DTEXTURE9		m_pShadowTexture;
	LPDIRECT3DSURFACE9		m_pShadowSurface;
	LPDIRECT3DSURFACE9		m_pShadowDepth;

	// The focus map's own target.  D3DPOOL_DEFAULT like the atlas, and released
	// and rebuilt with it.
	LPDIRECT3DTEXTURE9		m_pFocusTexture;
	LPDIRECT3DSURFACE9		m_pFocusSurface;
	LPDIRECT3DSURFACE9		m_pFocusDepth;

	LPD3DXEFFECT			m_pEffect;
	D3DXHANDLE				m_hTechCaster;
	D3DXHANDLE				m_hTechDebug;
	D3DXHANDLE				m_hTechCasterAlpha;
	D3DXHANDLE				m_hTechReceiver;
	D3DXHANDLE				m_hTechReceiverAlpha;
	D3DXHANDLE				m_hTechReceiverWide;
	D3DXHANDLE				m_hTechReceiverWideAlpha;
	D3DXHANDLE				m_hTechReceiverSoft;
	D3DXHANDLE				m_hTechReceiverSoftAlpha;
	D3DXHANDLE				m_hLightViewProj;
	D3DXHANDLE				m_hShadowTexture;
	D3DXHANDLE				m_hWorld;
	D3DXHANDLE				m_hViewProj;
	D3DXHANDLE				m_hWorldViewProj;
	D3DXHANDLE				m_hLightDirection;
	D3DXHANDLE				m_hShadowColour;
	D3DXHANDLE				m_hCameraPos;
	D3DXHANDLE				m_hFog;
	D3DXHANDLE				m_hBias;
	D3DXHANDLE				m_hShadowTexel;
	D3DXHANDLE				m_hBaseTexture;
	D3DXHANDLE				m_hFlatShade;
	D3DXHANDLE				m_hCasterViewProj;
	D3DXHANDLE				m_hSplit;
	D3DXHANDLE				m_hTileU;
	D3DXHANDLE				m_hTileV;
	D3DXHANDLE				m_hTileScale;
	D3DXHANDLE				m_hNormalOffset;
	D3DXHANDLE				m_hBiasScale;
	D3DXHANDLE				m_hShadowSize;
	D3DXHANDLE				m_hWide;
	D3DXHANDLE				m_hBlend;
	D3DXHANDLE				m_hSoftScale;
	D3DXHANDLE				m_hSoftMax;
	D3DXHANDLE				m_hFocusViewProj;
	D3DXHANDLE				m_hFocusTexture;
	D3DXHANDLE				m_hFocusOn;
	D3DXHANDLE				m_hFocusTexel;
	D3DXHANDLE				m_hFocusSize;
	D3DXHANDLE				m_hFocusBias;
	D3DXHANDLE				m_hFocusOffset;
	D3DXHANDLE				m_hFocusFade;
	D3DXHANDLE				m_hFocusFilter;
	D3DXHANDLE				m_hReceiverBias;
	D3DXHANDLE				m_hReceiverSlopeBias;
	D3DXHANDLE				m_hFilterRadius;

	// Sets everything the receiver shader reads that is the same for every
	// receiver in the frame, and answers FALSE if the frame has no shadow.
	BOOL	SetReceiverConstants();

	// Which receiver technique the Filter setting asks for.  Two of each, so
	// the cut-outs get the same kernel as everything else.
	D3DXHANDLE	ReceiverTechnique(BOOL i_bAlpha) const
	{
		if(m_nFilter >= 3)
		{
			return i_bAlpha ? m_hTechReceiverSoftAlpha : m_hTechReceiverSoft;
		}
		if(m_nFilter >= 2)
		{
			return i_bAlpha ? m_hTechReceiverWideAlpha : m_hTechReceiverWide;
		}
		return i_bAlpha ? m_hTechReceiverAlpha : m_hTechReceiver;
	}

	// One block of ground waiting to be shaded.
	struct SReceiver
	{
		LPDIRECT3DVERTEXBUFFER9	pVB;
		LPDIRECT3DINDEXBUFFER9	pIB;
		int						nVertices;
		int						nTriangles;
	};
	enum { MAX_RECEIVERS = 64 };
	SReceiver				m_arrReceivers[MAX_RECEIVERS];
	int						m_nReceivers;
	unsigned __int64		m_nReceiverDraws;

	LPDIRECT3DINDEXBUFFER9	m_pBlockIB;
	int						m_nBlockSize;		// tiles down one side of a block
	int						m_nBlockVertices;
	int						m_nBlockTriangles;

	D3DXMATRIX				m_matLightViewProj;
	D3DXMATRIX				m_matCameraViewProj;
	BOOL					m_bShadingPass;
	BOOL					m_bLitThisFrame;

	BOOL					m_bAvailable;

	// TRUE when the effect compiled at vs_3_0/ps_3_0.  The filtering needs it;
	// everything else works either way.
	BOOL					m_bShaderModel3;

	// Enough room for every degree-two block of the biggest map, gathered once
	// per pass rather than allocated.
	enum { MAX_TERRAIN_BLOCKS = 64 };
	class CQuadGround*		m_arrBlocks[MAX_TERRAIN_BLOCKS];
	int						m_nBlocks;
	// What ClearExtraCasters()/AddExtraCaster() collected for this frame.  Four
	// is what a probe can script, and the harness is the only caller.
	struct SExtraCaster
	{
		class CSkinnedMesh*	pMesh;
		D3DXMATRIX			matWorld;
		float				fPoseTime;
	};
	// Enough for a crowd: the whole point of standing forty characters in one
	// scene is to find out what forty of them cost.
	enum { MAX_EXTRA_CASTERS = 64 };
	SExtraCaster			m_arrExtraCaster[MAX_EXTRA_CASTERS];
	int						m_nExtraCasters;

	// How many skinned models the caster pass posed and drew, for the report
	// line and for a dump to say whether the walk found anything at all.
	int						m_nSkinnedCasters;
	unsigned __int64		m_nSkinnedCasterDraws;
	int						m_nCasterObjects;
	int						m_nCasterPoses;
	int						m_nReceiverObjects;
	int						m_nFocusSkinned;
	// Which caster the focus box is for, in the terms the skinned walk uses:
	// the on-foot player, an entry of the unit render list, or one of the
	// harness's models.  Exactly one of these is set when m_bFocusThisFrame.
	BOOL					m_bFocusIsCharacter;
	class CUnitData*		m_pFocusUnit;
	int						m_nFocusExtra;		// index into m_arrExtraCaster, or -1

	// What FindFocusTarget() settled on, kept for the pass that draws it.
	class CSkinnedMesh*		m_pFocusMesh;
	D3DXMATRIX				m_matFocusWorld;
	float					m_fFocusPoseTime;
	unsigned int			m_nCasterPoseSum;
	unsigned int			m_nReceiverPoseSum;
	unsigned __int64		m_nCasterDrawsWas;
	unsigned __int64		m_nReceiverDrawsWas;

	// Counters for ReportOccasionally().
	unsigned __int64		m_nPasses;
	unsigned __int64		m_nCasterDraws;
	unsigned __int64		m_nCasterTriangles;
};

#endif // _ATUM_SHADOW_MAP_H_

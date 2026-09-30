///////////////////////////////////////////////////////////////////////////////
//  ShadowMap.cpp : see ShadowMap.h
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AtumApplication.h"
#include "ShadowMap.h"
#include "Camera.h"
#include "SceneData.h"
#include "Background.h"
#include "QuadGround.h"
#include "Frustum.h"
#include "ShuttleChild.h"
#include "AtumDatabase.h"
#include "SkinnedMesh.h"
#include "ObjectChild.h"
#include "UnitData.h"
#include "EnemyData.h"
#include "MonsterData.h"
#include "CharacterChild.h"
#include "UnitRender.h"
#include "CharacterRender.h"
#include "dxutil.h"
#include "D3DFilteredDevice.h"		// g_bAtumDebugInfo
#include "ResourcePack.h"
#include <vector>

// How far the one cascade reaches.
#define SHADOW_MIN_RANGE		600.0f
#define SHADOW_MAX_RANGE		4000.0f

// How far the last cascade reaches.
#define SHADOW_RANGE_DEFAULT			4000.0f

// How much room to leave above and below the ground for things that cast into
// it.
#define SHADOW_HEIGHT_MARGIN	3000.0f

// Size of the debug view down the screen, as a fraction of screen height.
#define SHADOW_DEBUG_SIZE		0.45f

// How far towards the ambient-only colour a fully shadowed pixel goes.
#define SHADOW_STRENGTH					0.70f

// Where a dump goes, and what its files are called, unless SetDumpTarget()
// says otherwise.
#define SHADOW_DUMP_DIRECTORY			"old\\Debug-ShadowMap"
#define SHADOW_DUMP_PREFIX				"shadow-map"

// Self-shadowing controls.
//
// The normal offset does the coarse work on a heightfield: pushing the sample
// point out along the surface normal moves it off the surface that cast it.
#define SHADOW_NORMAL_OFFSET_TEXELS		2.0f

// The constant part of the depth bias, in normalised light-space depth.
#define SHADOW_DEPTH_BIAS				0.00015f

// And the part that scales with how steeply the surface leans away from the
// light.
#define SHADOW_SLOPE_BIAS				0.00035f

// How much of the box's edge is faded out rather than stopped at, as a
// fraction of half its width.
#define SHADOW_EDGE_FADE				0.10f

// DBGOUT is ((void)0) in a release build, and everything worth saying here has
// to be readable from a shipped client - it is how anyone can tell whether the
// shadow map came up on a given card without a debugger.
void ShadowMakeDirectory(const char* i_szPath)
{
	char szPart[MAX_PATH];
	strncpy_s(szPart, sizeof(szPart), i_szPath, _TRUNCATE);

	for(char* psz = szPart; 0 != *psz; psz++)
	{
		if('\\' == *psz || '/' == *psz)
		{
			const char cWas = *psz;
			*psz = 0;
			CreateDirectoryA(szPart, NULL);
			*psz = cWas;
		}
	}
	CreateDirectoryA(szPart, NULL);
}

static void ShadowSay(const char* i_szFormat, ...)
{
	// Silent unless Shadow.ini's DebugInfo asks for it, so an ordinary client
	// neither talks to the debugger nor leaves a log file behind.
	if(FALSE == g_bAtumDebugInfo)
	{
		return;
	}

	char    szLine[512];
	va_list args;

	va_start(args, i_szFormat);
	_vsnprintf_s(szLine, sizeof(szLine), _TRUNCATE, i_szFormat, args);
	va_end(args);

	OutputDebugStringA(szLine);

	// And to a file beside the client.
	//
	// OutputDebugString is invisible unless somebody is running DebugView, and
	// the one thing worth saying here - why the effect would not compile - is the
	// one thing that leaves no other trace: the feature just quietly does
	// nothing.
	FILE* pfLog = NULL;
	ShadowMakeDirectory("old");
	if(0 == fopen_s(&pfLog, "old\\shadowmap.log", "at") && NULL != pfLog)
	{
		fputs(szLine, pfLog);
		fclose(pfLog);
	}
}

///////////////////////////////////////////////////////////////////////////////
//  The effect.
//
//  Kept as a string so that the client ships as one .atm with no new asset and
//  no patcher work.
///////////////////////////////////////////////////////////////////////////////
static const char g_szShadowEffect[] =
	//-------------------------------------------------------------------------
	//  Shared
	//-------------------------------------------------------------------------
	"float4x4 g_matLightViewProj[4];\n"
	// The one cascade the caster pass is drawing into.
	"float4x4 g_matCasterViewProj;\n"
	"float4x4 g_matWorld;\n"
	"float4x4 g_matViewProj;\n"
	"float4x4 g_matWorldViewProj;\n"
	"float3   g_vLightDir;\n"
	"float3   g_vShadowColour;\n"
	"float3   g_vCameraPos;\n"
	"float2   g_vFog;\n"			// x = fog end, y = 1 / (end - start)
	"float4   g_vBias;\n"			// x = normal offset, y = constant bias, z = 1/edge fade, w = slope bias
	"float    g_fShadowTexel;\n"	// 1 / the atlas size
	"float    g_fShadowSize;\n"
	"float    g_fWide;\n"
	"float    g_fBlend;\n"
	"float4   g_vSoftScale;\n"	// per cascade: normalised depth -> penumbra in texels
	"float4   g_vSoftMax;\n"		// the widest penumbra, in texels, per cascade
	"float4   g_vSplit;\n"
	"float4   g_vTileU;\n"
	"float4   g_vTileV;\n"
	"float    g_fTileScale;\n"
	"float4   g_vNormalOffset;\n"
	"float4   g_vBiasScale;\n"
	// The focus map.
	"float4x4 g_matFocusViewProj;\n"
	"float    g_fFocusOn;\n"
	"float    g_fFocusTexel;\n"		// 1 / the focus map's size
	"float    g_fFocusSize;\n"
	"float    g_fFocusBias;\n"		// the slope bias, in this box's own depth
	"float    g_fFocusOffset;\n"	// the normal offset, in world units
	"float    g_fFocusFade;\n"		// 1 / the fraction of the edge that fades
	"float    g_fFocusFilter;\n"	// and its kernel, in its own texels
	// How wide the kernel is in each cascade, in that cascade's own texels.
	//
	// Set from a width in world units rather than in texels: a texel is 0.2 world
	// units in the near cascade and 2 in the far one, so a fixed tap count blurs
	// each of them differently and the step shows.
	"float4   g_vFilterRadius;\n"
	// One at the far end of the lerp below replaces the whole shadow term with a
	// constant, so every receiver goes uniformly darker and nothing can change
	// frame to frame; anything that still moves is a difference in which pixels
	// the pass covers, not in what it computed.
	"float    g_fReceiverBias;\n"
	"float    g_fReceiverSlopeBias;\n"
	"float    g_fFlatShade;\n"
	"\n"
	"texture  g_texShadow;\n"
	"sampler  g_sampShadow = sampler_state\n"
	"{\n"
	"    Texture   = <g_texShadow>;\n"
	"    MinFilter = POINT;  MagFilter = POINT;  MipFilter = NONE;\n"
	"    AddressU  = CLAMP;  AddressV  = CLAMP;\n"
	"};\n"
	"\n"
	"texture  g_texFocus;\n"
	"sampler  g_sampFocus = sampler_state\n"
	"{\n"
	"    Texture   = <g_texFocus>;\n"
	"    MinFilter = POINT;  MagFilter = POINT;  MipFilter = NONE;\n"
	"    AddressU  = CLAMP;  AddressV  = CLAMP;\n"
	"};\n"
	"\n"
	"texture  g_texBase;\n"
	"sampler  g_sampBase = sampler_state\n"
	"{\n"
	"    Texture   = <g_texBase>;\n"
	"    MinFilter = LINEAR;  MagFilter = LINEAR;  MipFilter = LINEAR;\n"
	"};\n"
	"\n"
	//-------------------------------------------------------------------------
	//  The caster. Writes light-space depth and nothing else.
	//-------------------------------------------------------------------------
	"struct VS_CASTER_OUT { float4 vPos : POSITION; float2 vDepth : TEXCOORD0; };\n"
	"\n"
	"VS_CASTER_OUT CasterVS(float3 vPos : POSITION)\n"
	"{\n"
	"    VS_CASTER_OUT o;\n"
	"    float4 vWorld = mul(float4(vPos, 1.0f), g_matWorld);\n"
	"    o.vPos   = mul(vWorld, g_matCasterViewProj);\n"
	"    o.vDepth = o.vPos.zw;\n"
	"    return o;\n"
	"}\n"
	"\n"
	"float4 CasterPS(float2 vDepth : TEXCOORD0) : COLOR\n"
	"{\n"
	"    return vDepth.x / vDepth.y;\n"
	"}\n"
	"\n"
	"technique Caster\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL CasterVS();\n"
	"        PixelShader  = compile PS_MODEL CasterPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = TRUE;  ZFunc = LESSEQUAL;\n"
	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = FALSE;  AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"    }\n"
	"}\n"
	"\n"
	//-------------------------------------------------------------------------
	//  The same caster for the cut-out objects.
	//
	//  Fences, railings and foliage are a sheet with most of the texture alpha at
	//  zero, and the main pass draws them with an alpha test at 0x7F - see
	//  ObjRender.cpp:70.
	//-------------------------------------------------------------------------
	"struct VS_CASTERA_OUT\n"
	"{\n"
	"    float4 vPos   : POSITION;\n"
	"    float2 vDepth : TEXCOORD0;\n"
	"    float2 vUV    : TEXCOORD1;\n"
	"};\n"
	"\n"
	"VS_CASTERA_OUT CasterAlphaVS(float3 vPos : POSITION, float2 vUV : TEXCOORD0)\n"
	"{\n"
	"    VS_CASTERA_OUT o;\n"
	"    float4 vWorld = mul(float4(vPos, 1.0f), g_matWorld);\n"
	"    o.vPos   = mul(vWorld, g_matCasterViewProj);\n"
	"    o.vDepth = o.vPos.zw;\n"
	"    o.vUV    = vUV;\n"
	"    return o;\n"
	"}\n"
	"\n"
	"float4 CasterAlphaPS(float2 vDepth : TEXCOORD0, float2 vUV : TEXCOORD1) : COLOR\n"
	"{\n"
	"    clip(tex2D(g_sampBase, vUV).a - 0.5f);\n"
	"    return vDepth.x / vDepth.y;\n"
	"}\n"
	"\n"
	"technique CasterAlpha\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL CasterAlphaVS();\n"
	"        PixelShader  = compile PS_MODEL CasterAlphaPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = TRUE;  ZFunc = LESSEQUAL;\n"
	// A cut-out is a single sheet with nothing behind it, so culling would lose
	// whichever half of it faces away from the sun.
	"        CullMode = NONE;\n"
	"        AlphaBlendEnable = FALSE;  AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"    }\n"
	"}\n"
	"\n"
	//-------------------------------------------------------------------------
	//  The debug view of the map.
	//-------------------------------------------------------------------------
	"struct VS_DEBUG_OUT { float4 vPos : POSITION; float2 vUV : TEXCOORD0; };\n"
	"\n"
	"VS_DEBUG_OUT DebugVS(float3 vPos : POSITION, float2 vUV : TEXCOORD0)\n"
	"{\n"
	"    VS_DEBUG_OUT o;\n"
	"    o.vPos = float4(vPos, 1.0f);\n"
	"    o.vUV  = vUV;\n"
	"    return o;\n"
	"}\n"
	"\n"
	"float4 DebugPS(float2 vUV : TEXCOORD0) : COLOR\n"
	"{\n"
	"    float d = tex2D(g_sampShadow, vUV).r;\n"
	"    return float4(d, d, d, 1.0f);\n"
	"}\n"
	"\n"
	"technique Debug\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL DebugVS();\n"
	"        PixelShader  = compile PS_MODEL DebugPS();\n"
	"        ZEnable = FALSE;  ZWriteEnable = FALSE;\n"
	"        CullMode = NONE;\n"
	"        AlphaBlendEnable = FALSE;  AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"    }\n"
	"}\n"
	"\n"
	//-------------------------------------------------------------------------
	//  The receiver.
	//
	//  Outputs the shadow term and nothing else, blended over the frame with ZERO
	//  / SRCCOLOR - so white leaves the pixel exactly as the fixed function
	//  pipeline drew it and g_vShadowColour darkens it.
	//-------------------------------------------------------------------------
	"struct VS_RECV_OUT\n"
	"{\n"
	"    float4 vPos    : POSITION;\n"
	"    float4 vLight0 : TEXCOORD0;\n"
	"    float4 vLight1 : TEXCOORD1;\n"
	"    float4 vLight2 : TEXCOORD2;\n"
	"    float4 vLight3 : TEXCOORD3;\n"
	"    float4 vTerm   : TEXCOORD4;\n"	// x = N.L, y = fog, z = slope bias, w = view depth
	"    float2 vUV     : TEXCOORD5;\n"
	// The focus box's light-space position.
	"#ifdef SHADOW_SM3\n"
	"    float4 vFocus  : TEXCOORD6;\n"
	"#endif\n"
	"};\n"
	"\n"
	// Every cascade's light-space position, computed here and chosen in the pixel
	// shader.
	"VS_RECV_OUT ReceiverVS(float3 vPos : POSITION, float3 vNormal : NORMAL, float2 vUV : TEXCOORD0)\n"
	"{\n"
	"    VS_RECV_OUT o;\n"
	"    float4 vWorld = mul(float4(vPos, 1.0f), g_matWorld);\n"
	"    float3 vN     = normalize(mul(vNormal, (float3x3)g_matWorld));\n"
	"    o.vPos    = mul(float4(vPos, 1.0f), g_matWorldViewProj);\n"
	"    o.vLight0 = mul(float4(vWorld.xyz + vN * g_vNormalOffset.x, 1.0f), g_matLightViewProj[0]);\n"
	"    o.vLight1 = mul(float4(vWorld.xyz + vN * g_vNormalOffset.y, 1.0f), g_matLightViewProj[1]);\n"
	"    o.vLight2 = mul(float4(vWorld.xyz + vN * g_vNormalOffset.z, 1.0f), g_matLightViewProj[2]);\n"
	"    o.vLight3 = mul(float4(vWorld.xyz + vN * g_vNormalOffset.w, 1.0f), g_matLightViewProj[3]);\n"
	"#ifdef SHADOW_SM3\n"
	"    o.vFocus  = mul(float4(vWorld.xyz + vN * g_fFocusOffset, 1.0f), g_matFocusViewProj);\n"
	"#endif\n"
	"\n"
	"    float fNdotL = saturate(dot(vN, -g_vLightDir));\n"
	"    o.vTerm.x = fNdotL;\n"
	"    o.vTerm.y = saturate((g_vFog.x - distance(vWorld.xyz, g_vCameraPos)) * g_vFog.y);\n"
	// The bias a surface needs depends on how steeply it leans away from the
	// light.
	"    float fSlope = sqrt(saturate(1.0f - fNdotL * fNdotL)) / max(fNdotL, 0.1f);\n"
	"    o.vTerm.z = g_vBias.y + g_vBias.w * min(fSlope, 8.0f);\n"
	// The depth this pixel is at, which is what decides its cascade.
	"    o.vTerm.w = o.vPos.w;\n"
	"    o.vUV     = vUV;\n"
	"    return o;\n"
	"}\n"
	"\n"
	//-------------------------------------------------------------------------
	//  Percentage-closer filtering, with the bilinear weights done by hand.
	//
	//  Four taps on their own is a box filter with five possible answers, and
	//  five possible answers along an edge is a staircase - which is most of what
	//  "blocky" means here, more than the texel size is.
	//-------------------------------------------------------------------------
	"#ifdef SHADOW_SM3\n"
	"float Filter(float2 vTex, float fD)\n"
	"{\n"
	"    float2 vTexel = vTex * g_fShadowSize - 0.5f;\n"
	"    float2 vFrac  = frac(vTexel);\n"
	"    float2 vBase  = (floor(vTexel) + 0.5f) * g_fShadowTexel;\n"
	"\n"
	"    float4 vTaps;\n"
	"    vTaps.x = tex2D(g_sampShadow, vBase).r;\n"
	"    vTaps.y = tex2D(g_sampShadow, vBase + float2(g_fShadowTexel, 0.0f)).r;\n"
	"    vTaps.z = tex2D(g_sampShadow, vBase + float2(0.0f, g_fShadowTexel)).r;\n"
	"    vTaps.w = tex2D(g_sampShadow, vBase + float2(g_fShadowTexel, g_fShadowTexel)).r;\n"
	"\n"
	"    float4 vLit = step(fD.xxxx, vTaps);\n"
	"    return lerp(lerp(vLit.x, vLit.y, vFrac.x), lerp(vLit.z, vLit.w, vFrac.x), vFrac.y);\n"
	"}\n"
	"#else\n"
	"float Filter(float2 vTex, float fD)\n"
	"{\n"
	"    float4 vTaps;\n"
	"    vTaps.x = tex2D(g_sampShadow, vTex + float2(-0.5f, -0.5f) * g_fShadowTexel).r;\n"
	"    vTaps.y = tex2D(g_sampShadow, vTex + float2( 0.5f, -0.5f) * g_fShadowTexel).r;\n"
	"    vTaps.z = tex2D(g_sampShadow, vTex + float2(-0.5f,  0.5f) * g_fShadowTexel).r;\n"
	"    vTaps.w = tex2D(g_sampShadow, vTex + float2( 0.5f,  0.5f) * g_fShadowTexel).r;\n"
	"    return dot(step(fD.xxxx, vTaps), float4(0.25f, 0.25f, 0.25f, 0.25f));\n"
	"}\n"
	"#endif\n"
	"\n"
	//-------------------------------------------------------------------------
	//  The focus map, read
	//
	//  The same bilinear four taps as Filter(), against the other texture.
	//-------------------------------------------------------------------------
	"#ifdef SHADOW_SM3\n"
	//-------------------------------------------------------------------------
	//  Where the twelve taps go, and why they are not on a wheel
	//
	//  They were: six round the rim at sixty degree steps and six more at half
	//  the radius on the same six angles.
	//-------------------------------------------------------------------------
	"static const float2 g_arrDisc[12] =\n"
	"{\n"
	"    float2( 0.2041f,  0.0000f), float2(-0.2607f,  0.2388f), float2( 0.0399f, -0.4547f),\n"
	"    float2( 0.3286f,  0.4286f), float2(-0.6030f, -0.1067f), float2( 0.5712f, -0.3634f),\n"
	"    float2(-0.1911f,  0.7107f), float2(-0.3644f, -0.7016f), float2( 0.7906f,  0.2887f),\n"
	"    float2(-0.8224f,  0.3395f), float2( 0.3965f, -0.8472f), float2( 0.2930f,  0.9341f)\n"
	"};\n"
	"\n"
	"float FocusFilter(float2 vTex, float fD)\n"
	"{\n"
	"    float2 vTexel = vTex * g_fFocusSize - 0.5f;\n"
	"    float2 vFrac  = frac(vTexel);\n"
	"    float2 vBase  = (floor(vTexel) + 0.5f) * g_fFocusTexel;\n"
	"\n"
	"    float4 vTaps;\n"
	"    vTaps.x = tex2D(g_sampFocus, vBase).r;\n"
	"    vTaps.y = tex2D(g_sampFocus, vBase + float2(g_fFocusTexel, 0.0f)).r;\n"
	"    vTaps.z = tex2D(g_sampFocus, vBase + float2(0.0f, g_fFocusTexel)).r;\n"
	"    vTaps.w = tex2D(g_sampFocus, vBase + float2(g_fFocusTexel, g_fFocusTexel)).r;\n"
	"\n"
	"    float4 vLit = step(fD.xxxx, vTaps);\n"
	"    return lerp(lerp(vLit.x, vLit.y, vFrac.x), lerp(vLit.z, vLit.w, vFrac.x), vFrac.y);\n"
	"}\n"
	"\n"
	// The focus map's texel is thirty times finer than the near cascade's, so a
	// single bilinear tap there is a shadow edge a fiftieth of a world unit
	// wide - which is not sharp, it is aliased.  Same disc, its own sampler.
	"float FocusDiscFilter(float2 vTex, float fD, float fRadius)\n"
	"{\n"
	"    if(fRadius <= 1.05f)\n"
	"    {\n"
	"        return FocusFilter(vTex, fD);\n"
	"    }\n"
	"    float  fR   = fRadius * g_fFocusTexel;\n"
	"    float  fLit = 0.0f;\n"
	"    for(int n = 0; n < 12; n++)\n"
	"    {\n"
	"        fLit += FocusFilter(vTex + g_arrDisc[n] * fR, fD);\n"
	"    }\n"
	"    return fLit * (1.0f / 12.0f);\n"
	"}\n"
	"\n"
	"float FocusTerm(float4 vFocus, float fSlopeBias, float fLit)\n"
	"{\n"
	"    if(g_fFocusOn < 0.5f)\n"
	"    {\n"
	"        return fLit;\n"
	"    }\n"
	"\n"
	"    float3 vNDC  = vFocus.xyz / vFocus.w;\n"
	"    float2 vFade = saturate((1.0f - abs(vNDC.xy)) * g_fFocusFade);\n"
	"    float  fIn   = vFade.x * vFade.y * step(0.0f, vNDC.z) * step(vNDC.z, 1.0f);\n"
	"    if(fIn <= 0.0f)\n"
	"    {\n"
	"        return fLit;\n"
	"    }\n"
	"\n"
	"    float2 vTex   = float2(0.5f + 0.5f * vNDC.x, 0.5f - 0.5f * vNDC.y);\n"
	// Held far enough inside that the widest kernel still lands on the map.
	"    float2 vInset = (2.0f + g_fFocusFilter) * g_fFocusTexel;\n"
	"    vTex = clamp(vTex, vInset, 1.0f - vInset);\n"
	"    float fD = vNDC.z - fSlopeBias * g_fFocusBias;\n"
	"    float fFocusLit = FocusDiscFilter(vTex, fD, g_fFocusFilter);\n"
	// Faded to fully lit at the edge of the box, so that what min() sees out
	// there is "no opinion" rather than a hard stop.
	"    fFocusLit = lerp(1.0f, fFocusLit, fIn);\n"
	"    return min(fLit, fFocusLit);\n"
	"}\n"
	"#endif\n"
	"\n"
	//-------------------------------------------------------------------------
	//  A kernel of a chosen width
	//
	//  Twelve bilinear taps spread round a disc, which is the same sampling
	//  SoftFilter() does for its penumbra - written once here so that the wide
	//  filter, the soft filter and the focus map all widen the same way and by
	//  the same rule.
	//-------------------------------------------------------------------------
	"#ifdef SHADOW_SM3\n"
	"float DiscFilter12(float2 vTex, float fD, float fRadius)\n"
	"{\n"
	"    float  fR   = fRadius * g_fShadowTexel;\n"
	"    float  fLit = 0.0f;\n"
	"    for(int n = 0; n < 12; n++)\n"
	"    {\n"
	"        fLit += Filter(vTex + g_arrDisc[n] * fR, fD);\n"
	"    }\n"
	"    return fLit * (1.0f / 12.0f);\n"
	"}\n"
	"\n"
	// The same, dropping to one bilinear tap when the radius does not warrant
	// twelve.
	"float DiscFilter(float2 vTex, float fD, float fRadius)\n"
	"{\n"
	"    if(fRadius <= 1.05f)\n"
	"    {\n"
	"        return Filter(vTex, fD);\n"
	"    }\n"
	"    return DiscFilter12(vTex, fD, fRadius);\n"
	"}\n"
	"#endif\n"
	"\n"
	"float3 ShadowTerm(VS_RECV_OUT i)\n"
	"{\n"
	// Which cascade this pixel belongs to, as three weights of which exactly one
	// is 1.
	"    float4 vSel;\n"
	"    vSel.x = step(i.vTerm.w, g_vSplit.x);\n"
	"    vSel.y = step(g_vSplit.x, i.vTerm.w) * step(i.vTerm.w, g_vSplit.y);\n"
	"    vSel.z = step(g_vSplit.y, i.vTerm.w) * step(i.vTerm.w, g_vSplit.z);\n"
	"    vSel.w = step(g_vSplit.z, i.vTerm.w);\n"
	"\n"
	"    float4 vLight = i.vLight0 * vSel.x + i.vLight1 * vSel.y\n"
	"                  + i.vLight2 * vSel.z + i.vLight3 * vSel.w;\n"
	"\n"
	"    float2 vNDC = vLight.xy / vLight.w;\n"
	"    float  fRef = vLight.z / vLight.w;\n"
	// Into the atlas: the cascade's own 0..1 square, scaled down to its tile
	// and moved to that tile's corner.
	"    float2 vCorner = float2(dot(g_vTileU, vSel), dot(g_vTileV, vSel));\n"
	"    float2 vTex = float2(0.5f + 0.5f * vNDC.x, 0.5f - 0.5f * vNDC.y);\n"
	"    vTex = vTex * g_fTileScale + vCorner;\n"
	// Held two texels inside the tile.
	"    float2 vInset = (2.0f + 2.0f * g_fWide) * g_fShadowTexel;\n"
	"    vTex = clamp(vTex, vCorner + vInset, vCorner + g_fTileScale - vInset);\n"
	"\n"
	// The edge fade belongs to the last cascade only.
	"    float2 vFade = saturate((1.0f - abs(vNDC)) * g_vBias.z);\n"
	"    float  fEdge = lerp(1.0f, vFade.x * vFade.y, vSel.w);\n"
	"    float  fInside = fEdge * step(0.0f, fRef) * step(fRef, 1.0f);\n"
	"\n"
	// The bias is in normalised light depth, so it means a different number of
	// world units in each cascade - the near one's depth range is a fraction of
	// the far one's.
	"    float fD = fRef - i.vTerm.z * dot(g_vBiasScale, vSel);\n"
	"    float fLit = Filter(vTex, fD);\n"
	"\n"
	// And across the split, the next cascade blended in.
	//
	// Without this the change from one cascade to the next is a line on the
	// ground where the texel size trebles - which is what "the transition is
	// visible" is, and no amount of resolution or filtering touches it: both
	// sides are as sharp as they can be and they are sharp by different amounts.
	"#ifdef SHADOW_SM3\n"
	"    float fFar   = dot(g_vSplit, vSel);\n"
	"    float fBand  = fFar * g_fBlend;\n"
	"    float fMix   = (fBand > 0.0f)\n"
	"                 ? saturate((i.vTerm.w - (fFar - fBand)) / fBand) * (1.0f - vSel.w)\n"
	"                 : 0.0f;\n"
	"    if(fMix > 0.0f)\n"
	"    {\n"
	// The next cascade along, by rotating the selection.  The last one has no
	// next, which is what the (1 - vSel.w) above takes out.
	"        float4 vNext = vSel.wxyz;\n"
	"        float4 vL2   = i.vLight0 * vNext.x + i.vLight1 * vNext.y\n"
	"                     + i.vLight2 * vNext.z + i.vLight3 * vNext.w;\n"
	"        float2 vN2   = vL2.xy / vL2.w;\n"
	"        float  fRef2 = vL2.z / vL2.w;\n"
	"        float2 vC2   = float2(dot(g_vTileU, vNext), dot(g_vTileV, vNext));\n"
	"        float2 vT2   = float2(0.5f + 0.5f * vN2.x, 0.5f - 0.5f * vN2.y);\n"
	"        vT2 = vT2 * g_fTileScale + vC2;\n"
	"        vT2 = clamp(vT2, vC2 + vInset, vC2 + g_fTileScale - vInset);\n"
	"        float fD2 = fRef2 - i.vTerm.z * dot(g_vBiasScale, vNext);\n"
	"        fLit = lerp(fLit, Filter(vT2, fD2), fMix);\n"
	"    }\n"
	"    fLit = FocusTerm(i.vFocus, i.vTerm.z, fLit);\n"
	"#endif\n"
	"\n"
	"    float fShade = (1.0f - fLit) * i.vTerm.x * i.vTerm.y * fInside;\n"
	"    fShade = lerp(fShade, 1.0f, g_fFlatShade);\n"
	"    return lerp(float3(1.0f, 1.0f, 1.0f), g_vShadowColour, fShade);\n"
	"}\n"
	"\n"
	"#ifdef SHADOW_SM3\n"
	// The wide kernel: the same bilinear 2x2 taken at four places a texel apart
	// and averaged, so sixteen taps across a three-texel square.
	"#ifdef SHADOW_SM3\n"
	// The disc itself is declared above, next to DiscFilter().
	"float SoftFilter(float2 vTex, float fD, float fSoftScale, float fSoftMax, float fMin)\n"
	"{\n"
	// 1.  How far above this pixel the nearest things are.
	"    float fSearch = min(fSoftMax, 8.0f) * g_fShadowTexel;\n"
	// **Not** rotated, and it was, briefly.
	"    float fSum    = 0.0f;\n"
	"    float fCount  = 0.0f;\n"
	"    for(int n = 0; n < 6; n++)\n"
	"    {\n"
	"        float t = tex2D(g_sampShadow, vTex + g_arrDisc[n] * fSearch).r;\n"
	"        float b = step(t, fD);\n"
	"        fSum   += t * b;\n"
	"        fCount += b;\n"
	"    }\n"
	"\n"
	"    if(fCount < 0.5f)\n"
	"    {\n"
	"        return 1.0f;\n"
	"    }\n"
	"\n"
	// 2. The penumbra, in texels, from the gap between them and us.
	"    float fBlocker = fSum / fCount;\n"
	"    float fWidth   = clamp((fD - fBlocker) * fSoftScale, fMin, fSoftMax);\n"
	"    return DiscFilter12(vTex, fD, fWidth);\n"
	"}\n"
	"#endif\n"
	"\n"
	"float3 ShadowTermWide(VS_RECV_OUT i)\n"
	"{\n"
	"    float4 vSel;\n"
	"    vSel.x = step(i.vTerm.w, g_vSplit.x);\n"
	"    vSel.y = step(g_vSplit.x, i.vTerm.w) * step(i.vTerm.w, g_vSplit.y);\n"
	"    vSel.z = step(g_vSplit.y, i.vTerm.w) * step(i.vTerm.w, g_vSplit.z);\n"
	"    vSel.w = step(g_vSplit.z, i.vTerm.w);\n"
	"\n"
	"    float4 vLight = i.vLight0 * vSel.x + i.vLight1 * vSel.y\n"
	"                  + i.vLight2 * vSel.z + i.vLight3 * vSel.w;\n"
	"    float2 vNDC = vLight.xy / vLight.w;\n"
	"    float  fRef = vLight.z / vLight.w;\n"
	"\n"
	"    float2 vCorner = float2(dot(g_vTileU, vSel), dot(g_vTileV, vSel));\n"
	"    float2 vTex = float2(0.5f + 0.5f * vNDC.x, 0.5f - 0.5f * vNDC.y);\n"
	"    vTex = vTex * g_fTileScale + vCorner;\n"
	// Wide enough for the widest kernel this cascade will use, or the near
	// cascade's edge samples the middle one - a wrong-cascade fringe along a line
	// in mid air.
	"    float  fRad   = dot(g_vFilterRadius, vSel);\n"
	"    float2 vInset = (2.0f + fRad) * g_fShadowTexel;\n"
	"    vTex = clamp(vTex, vCorner + vInset, vCorner + g_fTileScale - vInset);\n"
	"\n"
	"    float2 vFade = saturate((1.0f - abs(vNDC)) * g_vBias.z);\n"
	"    float  fEdge = lerp(1.0f, vFade.x * vFade.y, vSel.w);\n"
	"    float  fInside = fEdge * step(0.0f, fRef) * step(fRef, 1.0f);\n"
	"\n"
	"    float fD = fRef - i.vTerm.z * dot(g_vBiasScale, vSel);\n"
	"    float fLit;\n"
	"    if(fRad <= 1.05f)\n"
	"    {\n"
	"        float fT = g_fShadowTexel;\n"
	"        fLit = 0.25f * (Filter(vTex + float2(-fT, -fT), fD)\n"
	"                      + Filter(vTex + float2( fT, -fT), fD)\n"
	"                      + Filter(vTex + float2(-fT,  fT), fD)\n"
	"                      + Filter(vTex + float2( fT,  fT), fD));\n"
	"    }\n"
	"    else\n"
	"    {\n"
	"        fLit = DiscFilter12(vTex, fD, fRad);\n"
	"    }\n"
	"    fLit = FocusTerm(i.vFocus, i.vTerm.z, fLit);\n"
	"\n"
	"    float fShade = (1.0f - fLit) * i.vTerm.x * i.vTerm.y * fInside;\n"
	"    fShade = lerp(fShade, 1.0f, g_fFlatShade);\n"
	"    return lerp(float3(1.0f, 1.0f, 1.0f), g_vShadowColour, fShade);\n"
	"}\n"

	"#ifdef SHADOW_SM3\n"
	"float3 ShadowTermSoft(VS_RECV_OUT i)\n"
	"{\n"
	"    float4 vSel;\n"
	"    vSel.x = step(i.vTerm.w, g_vSplit.x);\n"
	"    vSel.y = step(g_vSplit.x, i.vTerm.w) * step(i.vTerm.w, g_vSplit.y);\n"
	"    vSel.z = step(g_vSplit.y, i.vTerm.w) * step(i.vTerm.w, g_vSplit.z);\n"
	"    vSel.w = step(g_vSplit.z, i.vTerm.w);\n"
	"\n"
	"    float4 vLight = i.vLight0 * vSel.x + i.vLight1 * vSel.y\n"
	"                  + i.vLight2 * vSel.z + i.vLight3 * vSel.w;\n"
	"    float2 vNDC = vLight.xy / vLight.w;\n"
	"    float  fRef = vLight.z / vLight.w;\n"
	"\n"
	"    float2 vCorner = float2(dot(g_vTileU, vSel), dot(g_vTileV, vSel));\n"
	"    float2 vTex = float2(0.5f + 0.5f * vNDC.x, 0.5f - 0.5f * vNDC.y);\n"
	"    vTex = vTex * g_fTileScale + vCorner;\n"
	"    float2 vInset = (2.0f + max(dot(g_vFilterRadius, vSel), dot(g_vSoftMax, vSel)))\n"
	"                  * g_fShadowTexel;\n"
	"    vTex = clamp(vTex, vCorner + vInset, vCorner + g_fTileScale - vInset);\n"
	"\n"
	"    float2 vFade = saturate((1.0f - abs(vNDC)) * g_vBias.z);\n"
	"    float  fEdge = lerp(1.0f, vFade.x * vFade.y, vSel.w);\n"
	"    float  fInside = fEdge * step(0.0f, fRef) * step(fRef, 1.0f);\n"
	"\n"
	"    float fD = fRef - i.vTerm.z * dot(g_vBiasScale, vSel);\n"
	"    float fLit = SoftFilter(vTex, fD, dot(g_vSoftScale, vSel), dot(g_vSoftMax, vSel),\n"
	"                            dot(g_vFilterRadius, vSel));\n"
	"    fLit = FocusTerm(i.vFocus, i.vTerm.z, fLit);\n"
	"\n"
	"    float fShade = (1.0f - fLit) * i.vTerm.x * i.vTerm.y * fInside;\n"
	"    fShade = lerp(fShade, 1.0f, g_fFlatShade);\n"
	"    return lerp(float3(1.0f, 1.0f, 1.0f), g_vShadowColour, fShade);\n"
	"}\n"
	// Closes the "#ifdef SHADOW_SM3" round the soft kernel itself.
	"#endif\n"
	"#else\n"
	// Both of the soft kernel's halves need Shader Model 3 - the blocker search
	// is a branch and the variable-width filter is a loop - so at ps_2_0 the two
	// upper filter settings quietly become the plain four taps.
	"float3 ShadowTermWide(VS_RECV_OUT i) { return ShadowTerm(i); }\n"
	"float3 ShadowTermSoft(VS_RECV_OUT i) { return ShadowTerm(i); }\n"
	"#endif\n"
	"\n"
	"float4 ReceiverPS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    return float4(ShadowTerm(i), 1.0f);\n"
	"}\n"
	"float4 ReceiverSoftPS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    return float4(ShadowTermSoft(i), 1.0f);\n"
	"}\n"
	"\n"
	"float4 ReceiverSoftAlphaPS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    clip(tex2D(g_sampBase, i.vUV).a - 0.5f);\n"
	"    return float4(ShadowTermSoft(i), 1.0f);\n"
	"}\n"
	"\n"
	"float4 ReceiverWidePS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    return float4(ShadowTermWide(i), 1.0f);\n"
	"}\n"
	"\n"
	"\n"
	// A cut-out receives shadow only where it is actually there.
	"float4 ReceiverAlphaPS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    clip(tex2D(g_sampBase, i.vUV).a - 0.5f);\n"
	"    return float4(ShadowTerm(i), 1.0f);\n"
	"}\n"
	"\n"
	"float4 ReceiverWideAlphaPS(VS_RECV_OUT i) : COLOR\n"
	"{\n"
	"    clip(tex2D(g_sampBase, i.vUV).a - 0.5f);\n"
	"    return float4(ShadowTermWide(i), 1.0f);\n"
	"}\n"
	"\n"
	"technique Receiver\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The constant and slope depth biases the receiver pass is tested by, read
	// from Shadow.ini: it redraws geometry the main pass already drew, and the
	// two do not compute the same depth.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n"
	"\n"
	"technique ReceiverAlpha\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverAlphaPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The same values as the Receiver technique above, for the same reasons and
	// off the same measurement.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n"
	"\n"
	"technique ReceiverWide\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverWidePS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The constant and slope depth biases the receiver pass is tested by, read
	// from Shadow.ini: it redraws geometry the main pass already drew, and the
	// two do not compute the same depth.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n"
	"\n"
	"technique ReceiverWideAlpha\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverWideAlphaPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The same values as the Receiver technique above, for the same reasons and
	// off the same measurement.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n"
	"\n"
	"technique ReceiverSoft\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverSoftPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The constant and slope depth biases the receiver pass is tested by, read
	// from Shadow.ini: it redraws geometry the main pass already drew, and the
	// two do not compute the same depth.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n"
	"\n"
	"technique ReceiverSoftAlpha\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        VertexShader = compile VS_MODEL ReceiverVS();\n"
	"        PixelShader  = compile PS_MODEL ReceiverSoftAlphaPS();\n"
	"        ZEnable = TRUE;  ZWriteEnable = FALSE;  ZFunc = LESSEQUAL;\n"
	// The same values as the Receiver technique above, for the same reasons and
	// off the same measurement.
	"        DepthBias = <g_fReceiverBias>;\n"
	"        SlopeScaleDepthBias = <g_fReceiverSlopeBias>;\n"

	"        CullMode = CCW;\n"
	"        AlphaBlendEnable = TRUE;  SrcBlend = ZERO;  DestBlend = SRCCOLOR;\n"
	"        AlphaTestEnable = FALSE;\n"
	"        Lighting = FALSE;  FogEnable = FALSE;  StencilEnable = FALSE;\n"
	"        ColorWriteEnable = RED | GREEN | BLUE;\n"
	"    }\n"
	"}\n";

//////////////////////////////////////////////////////////////////////

CShadowMap::CShadowMap()
	: m_pShadowTexture(NULL),
	  m_pShadowSurface(NULL),
	  m_pShadowDepth(NULL),
	  m_pFocusTexture(NULL),
	  m_pFocusSurface(NULL),
	  m_pFocusDepth(NULL),
	  m_nFocusSize(SHADOW_FOCUS_SIZE_DEFAULT),
	  m_nFocusSetting(-1),
	  m_fFocusBias(SHADOW_FOCUS_BIAS_DEFAULT),
	  m_fMapFilterWidth(SHADOW_FILTER_WIDTH_DEFAULT),
	  m_fCityFilterWidth(SHADOW_FILTER_WIDTH_DEFAULT),
	  m_fFilterWidth(SHADOW_FILTER_WIDTH_DEFAULT),
	  m_nMapFilter(-1),
	  m_nCityFilter(-1),
	  m_fFocusFilterWidth(SHADOW_FOCUS_FILTER_WIDTH_DEFAULT),
	  m_fReceiverBias(SHADOW_RECEIVER_BIAS_DEFAULT),
	  m_fMapSlopeBias(SHADOW_RECEIVER_SLOPE_DEFAULT),
	  m_fCitySlopeBias(SHADOW_RECEIVER_SLOPE_DEFAULT),
	  m_fReceiverSlopeBias(SHADOW_RECEIVER_SLOPE_DEFAULT),
	  m_nSunOverrides(0),
	  m_bFocusWanted(FALSE),
	  m_bFocusThisFrame(FALSE),
	  m_nFocusSkinned(0),
	  m_bFocusIsCharacter(FALSE),
	  m_pFocusUnit(NULL),
	  m_nFocusExtra(-1),
	  m_pFocusMesh(NULL),
	  m_fFocusPoseTime(0.0f),
	  m_hFocusViewProj(NULL),
	  m_hFocusTexture(NULL),
	  m_hFocusOn(NULL),
	  m_hFocusTexel(NULL),
	  m_hFocusSize(NULL),
	  m_hFocusBias(NULL),
	  m_hFocusOffset(NULL),
	  m_hFocusFade(NULL),
	  m_hFocusFilter(NULL),
	  m_hReceiverBias(NULL),
	  m_hReceiverSlopeBias(NULL),
	  m_hFilterRadius(NULL),
	  m_pEffect(NULL),
	  m_hTechCaster(NULL),
	  m_hTechDebug(NULL),
	  m_hLightViewProj(NULL),
	  m_hCasterViewProj(NULL),
	  m_hSplit(NULL),
	  m_hTileU(NULL),
	  m_hTileV(NULL),
	  m_hTileScale(NULL),
	  m_hNormalOffset(NULL),
	  m_hBiasScale(NULL),
	  m_nCascade(0),
	  m_nMapSize(SHADOW_MAP_SIZE_DEFAULT),
	  m_nTileSize(SHADOW_TILE_SIZE_DEFAULT),
	  m_nSkinnedCascades(SHADOW_SKINNED_CASCADES_DEFAULT),
	  m_nObjectCascades(SHADOW_OBJECT_CASCADES_DEFAULT),
	  m_bSetObjectCascades(FALSE),
	  m_bAnimatedObjects(TRUE),
	  m_nFilter(SHADOW_FILTER_DEFAULT),
	  m_fBlend(SHADOW_BLEND_DEFAULT),
	  m_nCityMapMin(SHADOW_CITY_MAP_MIN_DEFAULT),
	  m_nCityMapMax(SHADOW_CITY_MAP_MAX_DEFAULT),
	  m_nQuality(SHADOW_QUALITY_MAX),
	  m_nQualityOverride(-1),
	  m_nExcludedMaps(0),
	  m_bDumpOnMapChange(FALSE),
	  m_bSetSkinned(FALSE),
	  m_bSetFilter(FALSE),
	  m_bCastObjects(TRUE),
	  m_fSoftSize(SHADOW_SOFT_SIZE_DEFAULT),
	  m_fSoftMax(SHADOW_SOFT_MAX_DEFAULT),
	  m_hShadowTexture(NULL),
	  m_pBlockIB(NULL),
	  m_nBlockSize(0),
	  m_nBlockVertices(0),
	  m_nBlockTriangles(0),
	  m_bLitThisFrame(FALSE),
	  m_bAvailable(FALSE),
	  m_bShaderModel3(FALSE),
	  m_nBlocks(0),
	  m_nCasterObjects(0),
	  m_nCasterPoses(0),
	  m_nReceiverObjects(0),
	  m_nCasterPoseSum(0),
	  m_nReceiverPoseSum(0),
	  m_nCasterDrawsWas(0),
	  m_nReceiverDrawsWas(0),
	  m_fPoseUs(0.0),
	  m_fDrawUs(0.0),
	  m_nPoseCalls(0),
	  m_fUsPerTick(0.0),
	  m_nPasses(0),
	  m_nCasterDraws(0),
	  m_nCasterTriangles(0),
	  m_vDumpLightDir(0.0f, 0.0f, 0.0f),
	  m_vDumpCentre(0.0f, 0.0f, 0.0f),
	  m_vDumpEye(0.0f, 0.0f, 0.0f),
	  m_fDumpRadius(0.0f),
	  m_fDumpNear(0.0f),
	  m_fDumpFar(0.0f),
	  m_nDumpsWanted(0),
	  m_nDumpsWritten(0),
	  m_nLastMapIndex(-1),
	  m_fDumpDepthMin(0.0f),
	  m_fDumpDepthMax(0.0f),
	  m_fDumpDepthMean(0.0),
	  m_fDumpUndrawn(0.0),
	  m_bDumpStatsValid(FALSE),
	  m_hTechReceiver(NULL),
	  m_hWorld(NULL),
	  m_hViewProj(NULL),
	  m_hLightDirection(NULL),
	  m_hShadowColour(NULL),
	  m_hCameraPos(NULL),
	  m_hFog(NULL),
	  m_hBias(NULL),
	  m_hShadowTexel(NULL),
	  m_nReceivers(0),
	  m_nReceiverDraws(0),
	  m_hTechCasterAlpha(NULL),
	  m_hBaseTexture(NULL),
	  m_hTechReceiverAlpha(NULL),
	  m_hTechReceiverWide(NULL),
	  m_hTechReceiverWideAlpha(NULL),
	  m_hShadowSize(NULL),
	  m_hWide(NULL),
	  m_hBlend(NULL),
	  m_hSoftScale(NULL),
	  m_hSoftMax(NULL),
	  m_hTechReceiverSoft(NULL),
	  m_hTechReceiverSoftAlpha(NULL),
	  m_hWorldViewProj(NULL),
	  m_hFlatShade(NULL),
	  m_bShadingPass(FALSE),
	  m_bScreenPending(FALSE),
	  m_nExtraCasters(0),
	  m_nSkinnedCasters(0),
	  m_nSkinnedCasterDraws(0),
	  m_bLightFrozen(FALSE)
{
	D3DXMatrixIdentity(&m_matLightViewProj);
	m_arrMapSplit[0] = SHADOW_SPLIT_0_DEFAULT;
	m_arrMapSplit[1] = SHADOW_SPLIT_1_DEFAULT;
	m_arrMapSplit[2] = SHADOW_SPLIT_2_DEFAULT;
	m_arrMapSplit[3] = SHADOW_RANGE_DEFAULT;

	m_arrCitySplit[0] = SHADOW_CITY_SPLIT_0_DEFAULT;
	m_arrCitySplit[1] = SHADOW_CITY_SPLIT_1_DEFAULT;
	m_arrCitySplit[2] = SHADOW_CITY_SPLIT_2_DEFAULT;
	m_arrCitySplit[3] = SHADOW_CITY_RANGE_DEFAULT;

	memcpy(m_arrSplit, m_arrMapSplit, sizeof(m_arrSplit));

	memset(m_arrCascade, 0x00, sizeof(m_arrCascade));
	// One past the cascades: the focus box lives in the last entry and starts
	// out as valid an identity as the rest of them.
	for(int n = 0; n <= SHADOW_FOCUS_CASCADE; n++)
	{
		D3DXMatrixIdentity(&m_arrCascade[n].matViewProj);
	}
	D3DXMatrixIdentity(&m_matCameraViewProj);
	memset(m_arrFrozenCascade, 0x00, sizeof(m_arrFrozenCascade));
	memset(m_arrBlocks, 0x00, sizeof(m_arrBlocks));
	memset(m_arrReceivers, 0x00, sizeof(m_arrReceivers));
	memset(m_arrExtraCaster, 0x00, sizeof(m_arrExtraCaster));
	memset(m_arrExcludedMap, 0x00, sizeof(m_arrExcludedMap));
	memset(m_arrSunOverride, 0x00, sizeof(m_arrSunOverride));
	memset(m_szPendingScreen, 0x00, sizeof(m_szPendingScreen));

	strcpy_s(m_szDumpDirectory, sizeof(m_szDumpDirectory), SHADOW_DUMP_DIRECTORY);
	strcpy_s(m_szDumpPrefix, sizeof(m_szDumpPrefix), SHADOW_DUMP_PREFIX);
}

// Which of the SHADOW_EXPERIMENT_* probes is running.
int g_nShadowExperiment = SHADOW_EXPERIMENT_NONE;

// QueryPerformanceCounter, and its period worked out once.
__int64 CShadowMap::NowTicks() const
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (__int64)li.QuadPart;
}

static double ShadowUsPerTick()
{
	static double s_fUs = 0.0;
	if(s_fUs <= 0.0)
	{
		LARGE_INTEGER liFreq;
		QueryPerformanceFrequency(&liFreq);
		s_fUs = (liFreq.QuadPart > 0) ? (1000000.0 / (double)liFreq.QuadPart) : 0.0;
	}
	return s_fUs;
}

double CShadowMap::GetPoseMs() const	{ return m_fPoseUs * ShadowUsPerTick() / 1000.0; }
double CShadowMap::GetDrawMs() const	{ return m_fDrawUs * ShadowUsPerTick() / 1000.0; }

// A run of the harness writes its frames into its own folder, numbered from
// zero and named after its probe, so that a scripted run does not mix with the
// dumps a person took by hand and can be deleted whole.
void CShadowMap::SetDumpTarget(const char* i_szDirectory, const char* i_szPrefix)
{
	if(NULL != i_szDirectory && 0 != i_szDirectory[0])
	{
		strncpy_s(m_szDumpDirectory, sizeof(m_szDumpDirectory), i_szDirectory, _TRUNCATE);
	}
	if(NULL != i_szPrefix && 0 != i_szPrefix[0])
	{
		strncpy_s(m_szDumpPrefix, sizeof(m_szDumpPrefix), i_szPrefix, _TRUNCATE);
	}

	m_nDumpsWritten = 0;
	ShadowMakeDirectory(m_szDumpDirectory);
}

CShadowMap::~CShadowMap()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

//////////////////////////////////////////////////////////////////////
//  Lifetime
//////////////////////////////////////////////////////////////////////

// What Shadow.ini says, if anybody wrote one.
//
// Nothing in it is required and nothing in it is fatal.
void CShadowMap::LoadSettings()
{
	// Read if there is one, clamped and reported either way: a log that only
	// says what the settings are when somebody wrote a file is no use for
	// working out what a client is actually doing.
	FILE* pf = NULL;
	char  szLine[256];
	BOOL  bCity = FALSE;

	if(0 == fopen_s(&pf, "Shadow.ini", "rt") && NULL != pf)
	{
		while(NULL != fgets(szLine, sizeof(szLine), pf))
		{
			// [city] switches the splits being written to the city set.
			if('[' == szLine[0])
			{
				bCity = (NULL != strstr(szLine, "city") || NULL != strstr(szLine, "City"));
				continue;
			}

			char* pszEq = strchr(szLine, '=');
			if(NULL == pszEq || ';' == szLine[0])
			{
				continue;
			}
			*pszEq = 0;

			// Trim the key.
			char* pszKey = szLine;
			while(' ' == *pszKey || '\t' == *pszKey)	{ pszKey++; }
			char* pszEnd = pszKey + strlen(pszKey);
			while(pszEnd > pszKey && (' ' == pszEnd[-1] || '\t' == pszEnd[-1]))	{ *--pszEnd = 0; }

			const int nValue = atoi(pszEq + 1);

			if(0 == _stricmp(pszKey, "MapSize"))				{ m_nMapSize = nValue; }
			else if(0 == _stricmp(pszKey, "TileSize"))			{ m_nTileSize = nValue; }
			else if(0 == _stricmp(pszKey, "SkinnedCascades"))	{ m_nSkinnedCascades = nValue; m_bSetSkinned = TRUE; }
			else if(0 == _stricmp(pszKey, "ObjectCascades"))		{ m_nObjectCascades = nValue; m_bSetObjectCascades = TRUE; }
			else if(0 == _stricmp(pszKey, "AnimatedObjects"))	{ m_bAnimatedObjects = (0 != nValue); }
			else if(0 == _stricmp(pszKey, "Filter"))
			{
				if(bCity)	{ m_nCityFilter = nValue; }
				else		{ m_nFilter = nValue; m_bSetFilter = TRUE; }
			}
			else if(0 == _stricmp(pszKey, "Blend"))				{ m_fBlend = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "CityMapMin"))			{ m_nCityMapMin = nValue; }
			else if(0 == _stricmp(pszKey, "CityMapMax"))			{ m_nCityMapMax = nValue; }
			else if(0 == _stricmp(pszKey, "SoftSize"))			{ m_fSoftSize = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "SoftMax"))			{ m_fSoftMax = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "Quality"))			{ m_nQualityOverride = nValue; }
			else if(0 == _stricmp(pszKey, "Focus"))				{ m_nFocusSetting = nValue; }
			else if(0 == _stricmp(pszKey, "FocusSize"))			{ m_nFocusSize = nValue; }
			else if(0 == _stricmp(pszKey, "FocusBias"))			{ m_fFocusBias = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "FilterWidth"))
			{
				// Under [city] it is the city's; anywhere else it is the field
				// maps', exactly like Split0 and Range above.
				if(bCity)	{ m_fCityFilterWidth = (float)atof(pszEq + 1); }
				else		{ m_fMapFilterWidth  = (float)atof(pszEq + 1); }
			}
			else if(0 == _stricmp(pszKey, "FocusFilterWidth"))	{ m_fFocusFilterWidth = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "ReceiverBias"))		{ m_fReceiverBias = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "ReceiverSlopeBias"))
			{
				// Under [city] it is the cities', anywhere else the field maps'.
				if(bCity)	{ m_fCitySlopeBias = (float)atof(pszEq + 1); }
				else		{ m_fMapSlopeBias  = (float)atof(pszEq + 1); }
			}
			else if(0 == _stricmp(pszKey, "SunPitchOverride"))
			{
				// <map index>, <degrees below the horizon>.  Cumulative, so the
				// key may appear once per map, the way ExcludeMaps does.
				int   nMap    = 0;
				float fPitch  = 0.0f;
				if(2 == sscanf_s(pszEq + 1, "%d , %f", &nMap, &fPitch) &&
				   m_nSunOverrides < MAX_SUN_OVERRIDES)
				{
					if(fPitch < 0.0f)	{ fPitch = 0.0f; }
					if(fPitch > 89.0f)	{ fPitch = 89.0f; }
					m_arrSunOverride[m_nSunOverrides].nMapIndex     = nMap;
					m_arrSunOverride[m_nSunOverrides].fPitchDegrees = fPitch;
					m_nSunOverrides++;
				}
			}
			else if(0 == _stricmp(pszKey, "DumpOnMapChange"))	{ m_bDumpOnMapChange = (0 != nValue); }
			// The measurement reporting for the whole client, not just the
			// shadows: this is the only file any of it reads.
			else if(0 == _stricmp(pszKey, "DebugInfo"))			{ g_bAtumDebugInfo = (0 != nValue); }
			else if(0 == _stricmp(pszKey, "ExcludeMaps"))
			{
				// A list, comma or space separated.
				const char* psz = pszEq + 1;
				while(*psz != 0 && m_nExcludedMaps < MAX_EXCLUDED_MAPS)
				{
					while(*psz != 0 && (*psz < '0' || *psz > '9'))	{ psz++; }
					if(0 == *psz)	{ break; }
					m_arrExcludedMap[m_nExcludedMaps++] = atoi(psz);
					while(*psz >= '0' && *psz <= '9')				{ psz++; }
				}
			}
			else if(0 == _stricmp(pszKey, "Split0"))			{ Splits(bCity)[0] = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "Split1"))			{ Splits(bCity)[1] = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "Split2"))			{ Splits(bCity)[2] = (float)atof(pszEq + 1); }
			else if(0 == _stricmp(pszKey, "Range"))				{ Splits(bCity)[3] = (float)atof(pszEq + 1); }
		}
		fclose(pf);
	}

	// Powers of two, and no bigger than the card will make.
	D3DCAPS9 caps;
	memset(&caps, 0x00, sizeof(caps));
	int nMax = 4096;
	if(NULL != g_pD3dDev && SUCCEEDED(g_pD3dDev->GetDeviceCaps(&caps)))
	{
		nMax = (int)((caps.MaxTextureWidth < caps.MaxTextureHeight)
				   ? caps.MaxTextureWidth : caps.MaxTextureHeight);
	}

	if(m_nMapSize < 512)	{ m_nMapSize = 512; }
	if(m_nMapSize > nMax)	{ m_nMapSize = nMax; }

	int nPow = 512;
	while(nPow * 2 <= m_nMapSize)	{ nPow *= 2; }
	m_nMapSize = nPow;

	// Two by two, so a tile is at most half the atlas.
	if(m_nTileSize < 256)					{ m_nTileSize = 256; }
	if(m_nTileSize > m_nMapSize / 2)		{ m_nTileSize = m_nMapSize / 2; }
	nPow = 256;
	while(nPow * 2 <= m_nTileSize)	{ nPow *= 2; }
	m_nTileSize = nPow;

	if(m_nSkinnedCascades < 0)						{ m_nSkinnedCascades = 0; }
	if(m_nSkinnedCascades > SHADOW_CASCADE_COUNT)	{ m_nSkinnedCascades = SHADOW_CASCADE_COUNT; }

	if(m_nObjectCascades < 1)						{ m_nObjectCascades = 1; }
	if(m_nObjectCascades > SHADOW_CASCADE_COUNT)	{ m_nObjectCascades = SHADOW_CASCADE_COUNT; }

	if(m_nFilter < 1)	{ m_nFilter = 1; }
	if(m_nFilter > 3)	{ m_nFilter = 3; }

	// The focus map is its own texture and not a tile, so it is not bounded by
	// the atlas - only by the card and by what is worth spending on a fifteen
	// unit box.
	if(m_nFocusSize < 256)		{ m_nFocusSize = 256; }
	if(m_nFocusSize > nMax)		{ m_nFocusSize = nMax; }
	if(m_nFocusSize > 2048)		{ m_nFocusSize = 2048; }
	nPow = 256;
	while(nPow * 2 <= m_nFocusSize)	{ nPow *= 2; }
	m_nFocusSize = nPow;

	if(m_nFocusSetting > 1)		{ m_nFocusSetting = 1; }
	if(m_nFocusSetting < -1)	{ m_nFocusSetting = -1; }

	if(m_fFocusBias < 0.1f)		{ m_fFocusBias = 0.1f; }
	if(m_fFocusBias > 64.0f)	{ m_fFocusBias = 64.0f; }

	// Zero is allowed and means "one texel", which is what it was before this
	// setting existed.
	if(m_fMapFilterWidth < 0.0f)		{ m_fMapFilterWidth = 0.0f; }
	if(m_fMapFilterWidth > 20.0f)		{ m_fMapFilterWidth = 20.0f; }
	if(m_fCityFilterWidth < 0.0f)		{ m_fCityFilterWidth = 0.0f; }
	if(m_fCityFilterWidth > 20.0f)		{ m_fCityFilterWidth = 20.0f; }
	m_fFilterWidth = m_fMapFilterWidth;

	if(m_nCityFilter > 3)	{ m_nCityFilter = 3; }
	if(m_nCityFilter < 1 && m_nCityFilter != -1)	{ m_nCityFilter = 1; }

	if(m_fFocusFilterWidth < 0.0f)	{ m_fFocusFilterWidth = 0.0f; }
	if(m_fFocusFilterWidth > 20.0f)	{ m_fFocusFilterWidth = 20.0f; }

	if(m_fMapSlopeBias > 0.0f)		{ m_fMapSlopeBias = -m_fMapSlopeBias; }
	if(m_fCitySlopeBias > 0.0f)		{ m_fCitySlopeBias = -m_fCitySlopeBias; }
	if(m_fMapSlopeBias < -64.0f)	{ m_fMapSlopeBias = -64.0f; }
	if(m_fCitySlopeBias < -64.0f)	{ m_fCitySlopeBias = -64.0f; }
	m_fReceiverSlopeBias = m_fMapSlopeBias;


	if(m_fSoftSize < 0.0f)		{ m_fSoftSize = 0.0f; }
	if(m_fSoftSize > 0.5f)		{ m_fSoftSize = 0.5f; }
	if(m_fSoftMax < 0.5f)		{ m_fSoftMax = 0.5f; }
	if(m_fSoftMax > 40.0f)		{ m_fSoftMax = 40.0f; }

	if(m_fBlend < 0.0f)		{ m_fBlend = 0.0f; }
	if(m_fBlend > 0.5f)		{ m_fBlend = 0.5f; }

	// The splits have to climb, and the first one has to be in front of the near
	// plane.
	for(int nSet = 0; nSet < 2; nSet++)
	{
		float* pSplit = Splits(0 != nSet);
		if(pSplit[0] < 5.0f)	{ pSplit[0] = 5.0f; }
		for(int nS = 1; nS < SHADOW_CASCADE_COUNT; nS++)
		{
			if(pSplit[nS] < pSplit[nS - 1] * 1.05f)
			{
				pSplit[nS] = pSplit[nS - 1] * 1.05f;
			}
		}
	}

	ShadowSay("ShadowMap: settings - atlas %d, tile %d, skinned cascades %d, "
			  "object cascades %d, filter %d\n",
			  m_nMapSize, m_nTileSize, m_nSkinnedCascades, m_nObjectCascades, m_nFilter);
	ShadowSay("ShadowMap: focus map %d square, Focus = %d (-1 follows the quality ladder), bias x%.2f\n",
			  m_nFocusSize, m_nFocusSetting, m_fFocusBias);
	ShadowSay("ShadowMap: filter width - map %.2f, city %.2f, focus map %.2f\n",
			  m_fMapFilterWidth, m_fCityFilterWidth, m_fFocusFilterWidth);
	if(m_nCityFilter > 0)
	{
		ShadowSay("ShadowMap: cities use Filter = %d\n", m_nCityFilter);
	}
	ShadowSay("ShadowMap: receiver depth bias %.8f, slope bias - map %.2f, city %.2f\n",
			  m_fReceiverBias, m_fMapSlopeBias, m_fCitySlopeBias);
	for(int nS = 0; nS < m_nSunOverrides; nS++)
	{
		ShadowSay("ShadowMap: map %d has its sun tilted to %.1f degrees below the horizon\n",
				  m_arrSunOverride[nS].nMapIndex, m_arrSunOverride[nS].fPitchDegrees);
	}
	ShadowSay("ShadowMap: splits - map %.0f/%.0f/%.0f/%.0f, city %.0f/%.0f/%.0f/%.0f, blend %.2f\n",
			  m_arrMapSplit[0], m_arrMapSplit[1], m_arrMapSplit[2], m_arrMapSplit[3],
			  m_arrCitySplit[0], m_arrCitySplit[1], m_arrCitySplit[2], m_arrCitySplit[3],
			  m_fBlend);
	ShadowSay("ShadowMap: maps %d to %d use the city splits\n",
			  m_nCityMapMin, m_nCityMapMax);
	for(int nE = 0; nE < m_nExcludedMaps; nE++)
	{
		ShadowSay("ShadowMap: map %d excluded - blob shadows only\n", m_arrExcludedMap[nE]);
	}
}

void CShadowMap::InitDeviceObjects()
{
	LoadSettings();

	m_bAvailable = FALSE;

	if(NULL == g_pD3dDev)
	{
		return;
	}

	D3DCAPS9 caps;
	if(FAILED(g_pD3dDev->GetDeviceCaps(&caps)))
	{
		return;
	}
	if(caps.VertexShaderVersion < D3DVS_VERSION(2, 0) ||
	   caps.PixelShaderVersion  < D3DPS_VERSION(2, 0))
	{
		ShadowSay("ShadowMap: shader model 2.0 wanted, card has vs %d.%d ps %d.%d\n",
			   D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion),
			   D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion),
			   D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion),
			   D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion));
		return;
	}

	// A loose Res-Eff\shadow.fx wins when there is one, so that the shader can be
	// worked on without rebuilding.
	struct SProfile
	{
		const char*	szVS;
		const char*	szPS;
		BOOL		bSM3;
	};
	static const SProfile arrProfile[2] =
	{
		{ "vs_3_0", "ps_3_0", TRUE  },
		{ "vs_2_0", "ps_2_0", FALSE },
	};

	LPD3DXBUFFER      pErrors = NULL;
	std::vector<BYTE> vectSource;
	HRESULT           hr      = E_FAIL;

	for(int nProfile = 0; nProfile < 2 && FAILED(hr); nProfile++)
	{
		D3DXMACRO arrMacro[4];
		int       nMacro = 0;
		arrMacro[nMacro].Name = "VS_MODEL";
		arrMacro[nMacro].Definition = arrProfile[nProfile].szVS;
		nMacro++;
		arrMacro[nMacro].Name = "PS_MODEL";
		arrMacro[nMacro].Definition = arrProfile[nProfile].szPS;
		nMacro++;
		if(arrProfile[nProfile].bSM3)
		{
			arrMacro[nMacro].Name = "SHADOW_SM3";
			arrMacro[nMacro].Definition = "1";
			nMacro++;
		}
		arrMacro[nMacro].Name = NULL;
		arrMacro[nMacro].Definition = NULL;

		// A loose Res-Eff\shadow.fx wins over the built-in one, which is what
		// makes iterating on the shader a file save rather than a rebuild.
		if(0 == nProfile && vectSource.empty())
		{
			CResourcePack::Instance().Read("Res-Eff/shadow.fx", vectSource);
		}

		if(!vectSource.empty())
		{
			hr = D3DXCreateEffect(g_pD3dDev, &vectSource[0], (UINT)vectSource.size(),
								  arrMacro, NULL, 0, NULL, &m_pEffect, &pErrors);
			if(FAILED(hr))
			{
				ShadowSay("ShadowMap: Res-Eff/shadow.fx would not compile at %s (%s)\n",
						  arrProfile[nProfile].szPS,
						  pErrors ? (const char*)pErrors->GetBufferPointer() : "no message");
				SAFE_RELEASE(pErrors);
			}
		}

		if(FAILED(hr))
		{
			hr = D3DXCreateEffect(g_pD3dDev, g_szShadowEffect, (UINT)strlen(g_szShadowEffect),
								  arrMacro, NULL, 0, NULL, &m_pEffect, &pErrors);
			if(FAILED(hr))
			{
				ShadowSay("ShadowMap: the built-in effect would not compile at %s (%s)\n",
						  arrProfile[nProfile].szPS,
						  pErrors ? (const char*)pErrors->GetBufferPointer() : "no message");
				SAFE_RELEASE(pErrors);
				SAFE_RELEASE(m_pEffect);
				continue;
			}
		}

		SAFE_RELEASE(pErrors);
		m_bShaderModel3 = arrProfile[nProfile].bSM3;
		ShadowSay("ShadowMap: effect compiled at %s / %s\n",
				  arrProfile[nProfile].szVS, arrProfile[nProfile].szPS);
	}

	if(FAILED(hr))
	{
		return;
	}

	// The wide kernel only exists at Shader Model 3.  Saying so once here beats
	// a setting that silently means nothing.
	if(!m_bShaderModel3 && m_nFilter > 1)
	{
		ShadowSay("ShadowMap: Filter=%d needs Shader Model 3; using the four-tap kernel\n",
				  m_nFilter);
		m_nFilter = 1;
	}

	// Every technique, and whether the device will actually run it.
	//
	// A technique that compiles and then fails ValidateTechnique() binds silently
	// and draws nothing, which looks exactly like the shader being wrong - and
	// the shader is the last place anyone looks when the code compiled.
	static const char* arrTech[] =
	{
		"Caster", "CasterAlpha", "Debug",
		"Receiver", "ReceiverAlpha",
		"ReceiverWide", "ReceiverWideAlpha",
		"ReceiverSoft", "ReceiverSoftAlpha",
	};
	for(int nT = 0; nT < (int)(sizeof(arrTech) / sizeof(arrTech[0])); nT++)
	{
		D3DXHANDLE hTech = m_pEffect->GetTechniqueByName(arrTech[nT]);
		const HRESULT hrValid = (NULL != hTech) ? m_pEffect->ValidateTechnique(hTech) : E_FAIL;
		if(FAILED(hrValid))
		{
			ShadowSay("ShadowMap: technique %s will not run on this device (%08x)\n",
					  arrTech[nT], hrValid);
		}
	}

	m_hTechCaster     = m_pEffect->GetTechniqueByName("Caster");
	m_hTechDebug      = m_pEffect->GetTechniqueByName("Debug");
	m_hTechCasterAlpha = m_pEffect->GetTechniqueByName("CasterAlpha");
	m_hTechReceiver   = m_pEffect->GetTechniqueByName("Receiver");
	m_hTechReceiverAlpha = m_pEffect->GetTechniqueByName("ReceiverAlpha");
	m_hLightViewProj  = m_pEffect->GetParameterByName(NULL, "g_matLightViewProj");
	m_hCasterViewProj = m_pEffect->GetParameterByName(NULL, "g_matCasterViewProj");
	m_hSplit          = m_pEffect->GetParameterByName(NULL, "g_vSplit");
	m_hTileU          = m_pEffect->GetParameterByName(NULL, "g_vTileU");
	m_hTileV          = m_pEffect->GetParameterByName(NULL, "g_vTileV");
	m_hTileScale      = m_pEffect->GetParameterByName(NULL, "g_fTileScale");
	m_hNormalOffset   = m_pEffect->GetParameterByName(NULL, "g_vNormalOffset");
	m_hBiasScale      = m_pEffect->GetParameterByName(NULL, "g_vBiasScale");
	m_hShadowSize     = m_pEffect->GetParameterByName(NULL, "g_fShadowSize");
	m_hWide           = m_pEffect->GetParameterByName(NULL, "g_fWide");
	m_hBlend          = m_pEffect->GetParameterByName(NULL, "g_fBlend");
	m_hSoftScale      = m_pEffect->GetParameterByName(NULL, "g_vSoftScale");
	m_hSoftMax        = m_pEffect->GetParameterByName(NULL, "g_vSoftMax");
	m_hFocusViewProj  = m_pEffect->GetParameterByName(NULL, "g_matFocusViewProj");
	m_hFocusTexture   = m_pEffect->GetParameterByName(NULL, "g_texFocus");
	m_hFocusOn        = m_pEffect->GetParameterByName(NULL, "g_fFocusOn");
	m_hFocusTexel     = m_pEffect->GetParameterByName(NULL, "g_fFocusTexel");
	m_hFocusSize      = m_pEffect->GetParameterByName(NULL, "g_fFocusSize");
	m_hFocusBias      = m_pEffect->GetParameterByName(NULL, "g_fFocusBias");
	m_hFocusOffset    = m_pEffect->GetParameterByName(NULL, "g_fFocusOffset");
	m_hFocusFade      = m_pEffect->GetParameterByName(NULL, "g_fFocusFade");
	m_hFocusFilter    = m_pEffect->GetParameterByName(NULL, "g_fFocusFilter");
	m_hFilterRadius   = m_pEffect->GetParameterByName(NULL, "g_vFilterRadius");
	m_hTechReceiverSoft      = m_pEffect->GetTechniqueByName("ReceiverSoft");
	m_hTechReceiverSoftAlpha = m_pEffect->GetTechniqueByName("ReceiverSoftAlpha");
	m_hTechReceiverWide      = m_pEffect->GetTechniqueByName("ReceiverWide");
	m_hTechReceiverWideAlpha = m_pEffect->GetTechniqueByName("ReceiverWideAlpha");
	m_hShadowTexture  = m_pEffect->GetParameterByName(NULL, "g_texShadow");
	m_hWorld          = m_pEffect->GetParameterByName(NULL, "g_matWorld");
	m_hViewProj       = m_pEffect->GetParameterByName(NULL, "g_matViewProj");
	m_hWorldViewProj  = m_pEffect->GetParameterByName(NULL, "g_matWorldViewProj");
	m_hLightDirection = m_pEffect->GetParameterByName(NULL, "g_vLightDir");
	m_hShadowColour   = m_pEffect->GetParameterByName(NULL, "g_vShadowColour");
	m_hCameraPos      = m_pEffect->GetParameterByName(NULL, "g_vCameraPos");
	m_hFog            = m_pEffect->GetParameterByName(NULL, "g_vFog");
	m_hBias           = m_pEffect->GetParameterByName(NULL, "g_vBias");
	m_hShadowTexel    = m_pEffect->GetParameterByName(NULL, "g_fShadowTexel");
	m_hBaseTexture    = m_pEffect->GetParameterByName(NULL, "g_texBase");
	m_hFlatShade      = m_pEffect->GetParameterByName(NULL, "g_fFlatShade");
	m_hReceiverBias      = m_pEffect->GetParameterByName(NULL, "g_fReceiverBias");
	m_hReceiverSlopeBias = m_pEffect->GetParameterByName(NULL, "g_fReceiverSlopeBias");

	if(NULL == m_hTechCaster || NULL == m_hTechDebug || NULL == m_hTechReceiver ||
	   NULL == m_hLightViewProj || NULL == m_hShadowTexture ||
	   NULL == m_hCasterViewProj || NULL == m_hSplit ||
	   NULL == m_hTileU || NULL == m_hTileV || NULL == m_hTileScale ||
	   NULL == m_hNormalOffset || NULL == m_hBiasScale ||
	   NULL == m_hShadowSize || NULL == m_hWide || NULL == m_hBlend ||
	   NULL == m_hSoftScale || NULL == m_hSoftMax ||
	   NULL == m_hFocusViewProj || NULL == m_hFocusTexture || NULL == m_hFocusOn ||
	   NULL == m_hFocusTexel || NULL == m_hFocusSize || NULL == m_hFocusBias ||
	   NULL == m_hFocusOffset || NULL == m_hFocusFade ||
	   NULL == m_hFocusFilter || NULL == m_hFilterRadius ||
	   NULL == m_hTechReceiverSoft || NULL == m_hTechReceiverSoftAlpha ||
	   NULL == m_hTechReceiverWide || NULL == m_hTechReceiverWideAlpha ||
	   NULL == m_hWorld || NULL == m_hViewProj || NULL == m_hLightDirection ||
	   NULL == m_hShadowColour || NULL == m_hCameraPos || NULL == m_hFog ||
	   NULL == m_hBias || NULL == m_hShadowTexel ||
	   NULL == m_hTechCasterAlpha || NULL == m_hBaseTexture ||
	   NULL == m_hTechReceiverAlpha || NULL == m_hWorldViewProj ||
	   NULL == m_hFlatShade ||
	   NULL == m_hReceiverBias || NULL == m_hReceiverSlopeBias)
	{
		ShadowSay("ShadowMap: the effect is missing a technique or a parameter\n");
		SAFE_RELEASE(m_pEffect);
		return;
	}

	m_bAvailable = TRUE;
}

void CShadowMap::RestoreDeviceObjects()
{
	if(!m_bAvailable || NULL == g_pD3dDev)
	{
		return;
	}

	// R32F rather than a hardware depth texture: which spelling of that a card
	// takes depends on the vendor, and this one is taken by all of them.
	if(FAILED(g_pD3dDev->CreateTexture(m_nMapSize, m_nMapSize, 1,
									   D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
									   D3DPOOL_DEFAULT, &m_pShadowTexture, NULL)))
	{
		ShadowSay("ShadowMap: no %dx%d R32F render target, shadows off\n",
			   m_nMapSize, m_nMapSize);
		m_bAvailable = FALSE;
		return;
	}

	if(FAILED(m_pShadowTexture->GetSurfaceLevel(0, &m_pShadowSurface)))
	{
		SAFE_RELEASE(m_pShadowTexture);
		m_bAvailable = FALSE;
		return;
	}

	// Its own depth surface, and no multisampling on it.
	if(FAILED(g_pD3dDev->CreateDepthStencilSurface(m_nMapSize, m_nMapSize,
												   D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0,
												   TRUE, &m_pShadowDepth, NULL)))
	{
		if(FAILED(g_pD3dDev->CreateDepthStencilSurface(m_nMapSize, m_nMapSize,
													   D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
													   TRUE, &m_pShadowDepth, NULL)))
		{
			ShadowSay("ShadowMap: no depth surface for the map, shadows off\n");
			SAFE_RELEASE(m_pShadowSurface);
			SAFE_RELEASE(m_pShadowTexture);
			m_bAvailable = FALSE;
			return;
		}
	}

	// The focus map.
	//
	// Its own target rather than a fifth tile of the atlas, which is two by two
	// and full.
	if(m_bShaderModel3)
	{
		if(SUCCEEDED(g_pD3dDev->CreateTexture(m_nFocusSize, m_nFocusSize, 1,
											  D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
											  D3DPOOL_DEFAULT, &m_pFocusTexture, NULL)) &&
		   SUCCEEDED(m_pFocusTexture->GetSurfaceLevel(0, &m_pFocusSurface)) &&
		   (SUCCEEDED(g_pD3dDev->CreateDepthStencilSurface(m_nFocusSize, m_nFocusSize,
														   D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0,
														   TRUE, &m_pFocusDepth, NULL)) ||
			SUCCEEDED(g_pD3dDev->CreateDepthStencilSurface(m_nFocusSize, m_nFocusSize,
														   D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
														   TRUE, &m_pFocusDepth, NULL))))
		{
			ShadowSay("ShadowMap: focus map ready, %dx%d R32F\n", m_nFocusSize, m_nFocusSize);
		}
		else
		{
			ShadowSay("ShadowMap: no %dx%d focus target - the cascades carry on without it\n",
					  m_nFocusSize, m_nFocusSize);
			SAFE_RELEASE(m_pFocusDepth);
			SAFE_RELEASE(m_pFocusSurface);
			SAFE_RELEASE(m_pFocusTexture);
		}
	}

	if(m_pEffect)
	{
		m_pEffect->OnResetDevice();
	}

	// The one line that says the feature came up, and on what.
	ShadowSay("ShadowMap: ready, %dx%d R32F\n", m_nMapSize, m_nMapSize);
}

void CShadowMap::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pFocusDepth);
	SAFE_RELEASE(m_pFocusSurface);
	SAFE_RELEASE(m_pFocusTexture);

	SAFE_RELEASE(m_pShadowDepth);
	SAFE_RELEASE(m_pShadowSurface);
	SAFE_RELEASE(m_pShadowTexture);

	if(m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}
}

void CShadowMap::DeleteDeviceObjects()
{
	// D3DPOOL_MANAGED, so it lives across a device reset and only goes here.
	SAFE_RELEASE(m_pBlockIB);
	m_nBlockSize      = 0;
	m_nBlockVertices  = 0;
	m_nBlockTriangles = 0;

	SAFE_RELEASE(m_pEffect);
	m_hTechCaster    = NULL;
	m_hTechDebug     = NULL;
	m_hLightViewProj = NULL;
	m_hCasterViewProj = NULL;
	m_hSplit          = NULL;
	m_hTileU          = NULL;
	m_hTileV          = NULL;
	m_hTileScale      = NULL;
	m_hNormalOffset   = NULL;
	m_hBiasScale      = NULL;
	m_hShadowSize     = NULL;
	m_hWide           = NULL;
	m_hBlend          = NULL;
	m_hSoftScale      = NULL;
	m_hSoftMax        = NULL;
	m_hFocusViewProj  = NULL;
	m_hFocusTexture   = NULL;
	m_hFocusOn        = NULL;
	m_hFocusTexel     = NULL;
	m_hFocusSize      = NULL;
	m_hFocusBias      = NULL;
	m_hFocusOffset    = NULL;
	m_hFocusFade      = NULL;
	m_hFocusFilter    = NULL;
	m_hFilterRadius   = NULL;
	m_hTechReceiverSoft      = NULL;
	m_hTechReceiverSoftAlpha = NULL;
	m_hTechReceiverWide      = NULL;
	m_hTechReceiverWideAlpha = NULL;
	m_hShadowTexture = NULL;
	m_bAvailable     = FALSE;
}

//////////////////////////////////////////////////////////////////////
//  Where the light is looking
//////////////////////////////////////////////////////////////////////

// Which quarter of the atlas a cascade lives in.
void CShadowMap::CascadeTile(int i_nCascade, int* o_pX, int* o_pY) const
{
	// Two by two, all four used.
	*o_pX = (0 != (i_nCascade & 1)) ? m_nTileSize : 0;
	*o_pY = (0 != (i_nCascade & 2)) ? m_nTileSize : 0;
}

// Points the box that every pass reads at one cascade.
//
// The caster pass, the two culls and the dump all work from m_matLightViewProj
// and the m_fDump* fields, and none of them needs to know that there is more
// than one box - they are run once per cascade with this called in between.
void CShadowMap::SelectCascade(int i_nCascade)
{
	const SCascade& c = m_arrCascade[i_nCascade];

	m_nCascade         = i_nCascade;
	m_matLightViewProj = c.matViewProj;
	m_vDumpCentre      = c.vCentre;
	m_vDumpEye         = c.vEye;
	m_fDumpRadius      = c.fRadius;
	m_fDumpNear        = c.fNear;
	m_fDumpFar         = c.fFar;
}

// The option, turned into settings.
//
// Called at the top of the caster pass, because the option can be changed from
// the interface while the client is running and a shadow setting that needs a
// restart is a shadow setting nobody will ever try.
float CShadowMap::SunPitchForThisMap() const
{
	if(0 == m_nSunOverrides || NULL == g_pShuttleChild)
	{
		return 0.0f;
	}

	const int nMapIndex = (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex;
	for(int n = 0; n < m_nSunOverrides; n++)
	{
		if(m_arrSunOverride[n].nMapIndex == nMapIndex)
		{
			return m_arrSunOverride[n].fPitchDegrees;
		}
	}
	return 0.0f;
}

// Whether this map has opted out.
BOOL CShadowMap::IsExcludedMap() const
{
	if(0 == m_nExcludedMaps || NULL == g_pShuttleChild)
	{
		return FALSE;
	}

	const int nMapIndex = (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex;
	for(int n = 0; n < m_nExcludedMaps; n++)
	{
		if(m_arrExcludedMap[n] == nMapIndex)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CShadowMap::ApplyQuality()
{
	m_nQuality = (m_nQualityOverride >= 0)
			   ? m_nQualityOverride
			   : ((NULL != g_pSOption) ? g_pSOption->sShadowState : SHADOW_QUALITY_MAX);
	if(m_nQuality < 0)					{ m_nQuality = 0; }
	if(m_nQuality > SHADOW_QUALITY_MAX)	{ m_nQuality = SHADOW_QUALITY_MAX; }

	// A map on the excluded list gets the game's own shadows and nothing else,
	// whatever the option says.
	if(IsExcludedMap())
	{
		m_nQuality = SHADOW_QUALITY_BLOB;
	}

	m_bCastObjects = (m_nQuality >= SHADOW_QUALITY_OBJECTS);

	if(!m_bSetSkinned)
	{
		// Nought at the bottom, then one cascade a rung.
		if(m_nQuality >= SHADOW_QUALITY_MAX)			{ m_nSkinnedCascades = 4; }
		else if(m_nQuality >= SHADOW_QUALITY_SKINNED_3)	{ m_nSkinnedCascades = 3; }
		else if(m_nQuality >= SHADOW_QUALITY_SKINNED_2)	{ m_nSkinnedCascades = 2; }
		else if(m_nQuality >= SHADOW_QUALITY_SKINNED)	{ m_nSkinnedCascades = 1; }
		else											{ m_nSkinnedCascades = 0; }

		if(m_nSkinnedCascades > SHADOW_CASCADE_COUNT)
		{
			m_nSkinnedCascades = SHADOW_CASCADE_COUNT;
		}
	}

	// The focus map.  Shadow.ini's Focus pins it either way; -1 puts it on the
	// ladder, and it needs Shader Model 3 because the containment test in the
	// receiver has to be a branch and not a lerp of both answers.
	m_bFocusWanted = m_bShaderModel3 &&
					 ((m_nFocusSetting > 0) ||
					  (m_nFocusSetting < 0 && m_nQuality >= SHADOW_QUALITY_FOCUS));

	if(!m_bSetFilter)
	{
		if(m_nQuality >= SHADOW_QUALITY_SOFT)			{ m_nFilter = 3; }
		else if(m_nQuality >= SHADOW_QUALITY_FILTER)		{ m_nFilter = 2; }
		else											{ m_nFilter = 1; }
		if(!m_bShaderModel3)	{ m_nFilter = 1; }
	}

	// One line the first time the ladder settles on something, and again
	// whenever it changes - so the log says what the option is actually doing
	// rather than what it was asked for.
	static int s_nWas = -1;
	if(s_nWas != m_nQuality)
	{
		s_nWas = m_nQuality;

		// FilterWidth needs a kernel to widen, and Filter 1 is a single bilinear tap
		// on purpose - it is the cheap setting.
		if(m_fFilterWidth > 0.0f && m_nFilter < 2)
		{
			ShadowSay("ShadowMap: FilterWidth = %.2f does nothing at Filter = 1 "
					  "(one tap); use Filter = 2 or 3\n", m_fFilterWidth);
		}
		ShadowSay("ShadowMap: quality %d - objects %d, skinned cascades %d, filter %d, focus %d\n",
				  m_nQuality, m_bCastObjects ? 1 : 0, m_nSkinnedCascades, m_nFilter,
				  m_bFocusWanted ? 1 : 0);
	}
}

// Whether the client should still be drawing its own blob shadows.
//
// The blob and a real shadow together is a soft round smudge underneath a
// sharp shaped one, which looks worse than either by itself - so the blobs
// stop as soon as the map takes over at quality 2.
int ShadowBlobLevel()
{
	if(NULL == g_pSOption)
	{
		return 0;
	}

	// The option's own value is what the blobs have always scaled their draw
	// distance by, so that is what comes back whenever they are still wanted.
	const int nOption = g_pSOption->sShadowState;

	CShadowMap* pMap = (NULL != g_pD3dApp) ? g_pD3dApp->m_pShadowMap : NULL;
	if(NULL == pMap || !pMap->IsAvailable() || g_pD3dApp->m_nShadowMapMode <= 0)
	{
		// No cascade map on this machine, or it is switched off.  The blobs are
		// all there is and the client behaves exactly as it always did.
		return nOption;
	}

	if(pMap->IsExcludedMap())
	{
		// This map opted out.  Same again.
		return nOption;
	}

	// Otherwise the blobs stop as soon as the map takes over, because the two
	// together is a soft round smudge underneath a sharp shaped shadow.
	return (pMap->GetQuality() >= SHADOW_QUALITY_TERRAIN) ? 0 : nOption;
}

BOOL CShadowMap::BuildLightMatrices()
{
	if(NULL == g_pScene || NULL == g_pCamera || NULL == g_pGround)
	{
		static BOOL s_bSaidNotReady = FALSE;
		if(!s_bSaidNotReady)
		{
			s_bSaidNotReady = TRUE;
			ShadowSay("ShadowMap: no shadows yet - scene %p, camera %p, ground %p\n",
					  (void*)g_pScene, (void*)g_pCamera, (void*)g_pGround);
		}
		return FALSE;
	}

	D3DXVECTOR3 vLight(g_pScene->m_light0.Direction.x,
					   g_pScene->m_light0.Direction.y,
					   g_pScene->m_light0.Direction.z);
	if(D3DXVec3Length(&vLight) < 0.001f)
	{
		// The map's sun direction is zero, so there is no direction to cast along.
		static int s_nSaidZero = -2;
		const int nMapZ = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;
		if(s_nSaidZero != nMapZ)
		{
			s_nSaidZero = nMapZ;
			ShadowSay("ShadowMap: map %d has no shadows - its sun direction is "
					  "zero in the map file, so there is nothing to cast along\n", nMapZ);
		}
		return FALSE;
	}
	D3DXVec3Normalize(&vLight, &vLight);

	// Tilt this map's sun down, if this map is one of the named ones.
	//
	// Only ever downwards and only when the map's own sun is shallower than what
	// was asked for, so naming a map whose sun is already lower changes nothing.
	const float fPitch = SunPitchForThisMap();
	if(fPitch > 0.0f)
	{
		const float fWantY = -sinf(fPitch * (3.14159265f / 180.0f));
		if(vLight.y > fWantY)
		{
			D3DXVECTOR3 vFlat(vLight.x, 0.0f, vLight.z);
			const float fLen = D3DXVec3Length(&vFlat);
			if(fLen > 0.0001f)
			{
				const float fWantFlat = sqrtf(1.0f - fWantY * fWantY);
				vFlat *= (fWantFlat / fLen);
				vLight = D3DXVECTOR3(vFlat.x, fWantY, vFlat.z);
			}
			else
			{
				vLight = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			}
		}
	}

	// D3DLIGHT9::Direction is the way the light travels, so a sun up in the sky
	// points down and y is negative.
	if(vLight.y > -0.05f)
	{
		// The map's own sun is at or above the horizon, so there is nothing to cast
		// and the whole pass is skipped.
		static int s_nSaidFor = -2;
		const int nMap = (NULL != g_pShuttleChild)
					   ? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;
		if(s_nSaidFor != nMap)
		{
			s_nSaidFor = nMap;
			ShadowSay("ShadowMap: map %d has no shadows - its sun points along or "
					  "above the horizon (%.3f %.3f %.3f), so nothing can cast\n",
					  nMap, vLight.x, vLight.y, vLight.z);
		}
		return FALSE;
	}

	// Which set of splits this map wants: a city is played with the camera a few
	// dozen units from the player and a field map with it much further out, and
	// one set cannot be right for both.
	const int nMapIndex = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;
	const BOOL bCity = (nMapIndex >= m_nCityMapMin && nMapIndex <= m_nCityMapMax);

	memcpy(m_arrSplit, bCity ? m_arrCitySplit : m_arrMapSplit, sizeof(m_arrSplit));

	// The kernel goes with them.
	m_fFilterWidth       = bCity ? m_fCityFilterWidth : m_fMapFilterWidth;

	// The slope bias is not about the map, though.
	const float fNearPlane = (NULL != g_pCamera) ? g_pCamera->GetNearPlane() : 1.0f;
	m_fReceiverSlopeBias = (bCity || fNearPlane < 3.0f) ? m_fCitySlopeBias : m_fMapSlopeBias;
	if(bCity && m_nCityFilter > 0)
	{
		m_nFilter = m_nCityFilter;
	}

	// Four slices of the view frustum, each reaching back a little past the
	// one in front so that a receiver near a boundary is inside both boxes and
	// the change is a change of sharpness rather than a step.
	float arrSplit[SHADOW_CASCADE_COUNT + 1];
	arrSplit[0] = 1.0f;
	for(int nS = 0; nS < SHADOW_CASCADE_COUNT; nS++)
	{
		arrSplit[nS + 1] = m_arrSplit[nS];
	}

	for(int n = 0; n < SHADOW_CASCADE_COUNT; n++)
	{
		float fNearZ = arrSplit[n];
		const float fFarZ = arrSplit[n + 1];
		if(n > 0)
		{
			fNearZ -= (fFarZ - fNearZ) * SHADOW_SPLIT_OVERLAP;
			if(fNearZ < 1.0f)	{ fNearZ = 1.0f; }
		}

		if(!BuildCascade(n, vLight, fNearZ, fFarZ))
		{
			return FALSE;
		}
	}

	m_vDumpLightDir = vLight;

	// Experiment 1: hold the boxes where they first landed.
	//
	// Everything a box decides - which texel a receiver samples, whether it is
	// inside at all, which objects cast - moves when it moves, and the dumps say
	// it moves whenever the camera turns, which is exactly when the flicker
	// appeared.
	if(SHADOW_EXPERIMENT_FREEZE_LIGHT == g_nShadowExperiment)
	{
		if(!m_bLightFrozen)
		{
			memcpy(m_arrFrozenCascade, m_arrCascade, sizeof(m_arrCascade));
			m_vFrozenLightDir = m_vDumpLightDir;
			m_bLightFrozen    = TRUE;
			ShadowSay("ShadowMap: experiment 1 - light matrices frozen\n");
		}

		memcpy(m_arrCascade, m_arrFrozenCascade, sizeof(m_arrCascade));
		m_vDumpLightDir = m_vFrozenLightDir;
	}
	else
	{
		m_bLightFrozen = FALSE;
	}

	// And the focus box, fitted to the player rather than to a slice.
	m_bFocusThisFrame = FALSE;
	if(m_bFocusWanted && NULL != m_pFocusSurface)
	{
		m_bFocusThisFrame = BuildFocusCascade(m_vDumpLightDir);
	}

	SelectCascade(0);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//  The focus box
//////////////////////////////////////////////////////////////////////

// Who the focus map is for, and how much room round them it needs.
//
// Three cases and they are in order of how much they matter.
BOOL CShadowMap::FindFocusTarget(D3DXVECTOR3* o_pPos, float* o_pRadius)
{
	if(NULL == g_pD3dApp)
	{
		return FALSE;
	}

	CSkinnedMesh* pMesh = NULL;
	D3DXVECTOR3   vPos(0.0f, 0.0f, 0.0f);

	m_bFocusIsCharacter = FALSE;
	m_pFocusUnit        = NULL;
	m_nFocusExtra       = -1;
	m_pFocusMesh        = NULL;
	m_fFocusPoseTime    = 0.0f;
	D3DXMatrixIdentity(&m_matFocusWorld);

	if(g_pD3dApp->m_bCharacter && NULL != g_pCharacterChild &&
	   NULL != g_pD3dApp->m_pCharacterRender)
	{
		map<int, CSkinnedMesh*>::iterator it =
			g_pD3dApp->m_pCharacterRender->m_mapSkinnedMesh.find(g_pCharacterChild->m_nUnitNum);
		if(it != g_pD3dApp->m_pCharacterRender->m_mapSkinnedMesh.end() &&
		   NULL != it->second && NULL != it->second->m_pdeHead &&
		   SOFTWARE == it->second->GetMethod())
		{
			pMesh               = it->second;
			vPos                = g_pCharacterChild->m_vPos;
			m_bFocusIsCharacter = TRUE;
			m_matFocusWorld     = g_pCharacterChild->m_mMatrix;
			m_fFocusPoseTime    = g_pCharacterChild->m_fCurrentTime;
		}
	}
	else if(NULL != g_pShuttleChild)
	{
		D3DXMATRIX matWorld;
		float      fPoseTime = 0.0f;
		pMesh = FindUnitMesh(g_pShuttleChild, &matWorld, &fPoseTime);
		if(NULL != pMesh)
		{
			vPos             = g_pShuttleChild->m_vPos;
			m_pFocusUnit     = (CUnitData*)g_pShuttleChild;
			m_matFocusWorld  = matWorld;
			m_fFocusPoseTime = fPoseTime;
		}
	}

	// The harness stands its models by hand and is in none of the game's lists.
	if(NULL == pMesh && m_nExtraCasters > 0 &&
	   NULL != m_arrExtraCaster[0].pMesh && NULL != m_arrExtraCaster[0].pMesh->m_pdeHead &&
	   SOFTWARE == m_arrExtraCaster[0].pMesh->GetMethod())
	{
		pMesh            = m_arrExtraCaster[0].pMesh;
		vPos             = D3DXVECTOR3(m_arrExtraCaster[0].matWorld._41,
									   m_arrExtraCaster[0].matWorld._42,
									   m_arrExtraCaster[0].matWorld._43);
		m_nFocusExtra    = 0;
		m_matFocusWorld  = m_arrExtraCaster[0].matWorld;
		m_fFocusPoseTime = m_arrExtraCaster[0].fPoseTime;
	}

	if(NULL == pMesh || pMesh->m_fRadius <= 0.0f)
	{
		return FALSE;
	}

	float fRadius = pMesh->m_fRadius * SHADOW_FOCUS_MARGIN;

	// In the air, the box goes where the shadow lands and not where the ship is.
	//
	// A box fitted round the ship itself at five hundred units up contains the
	// ship and nothing else: the ground it shades is five hundred units below and
	// nowhere near inside, so the receiver never selects the focus map and the
	// whole thing is a render target drawn for nobody.
	{
		const float fDown = -m_vDumpLightDir.y;

		if(NULL != g_pGround && NULL != g_pShuttleChild &&
		   IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
		{
			// And it goes where the shadow lands rather than where the caster is.
			const float fGround = g_pGround->CheckHeightMap(vPos);
			const float fDrop   = vPos.y - fGround;
			if(fDrop > 1.0f && fDown > 0.05f)
			{
				vPos += m_vDumpLightDir * (fDrop / fDown);
			}
		}

		if(fDown > 0.05f)
		{
			fRadius /= fDown;
		}
	}

	if(fRadius < SHADOW_FOCUS_MIN_RADIUS)	{ fRadius = SHADOW_FOCUS_MIN_RADIUS; }
	if(fRadius > SHADOW_FOCUS_MAX_RADIUS)	{ fRadius = SHADOW_FOCUS_MAX_RADIUS; }

	m_pFocusMesh = pMesh;
	*o_pPos      = vPos;
	*o_pRadius   = fRadius;
	return TRUE;
}

// Whether this entry of the skinned caster walk is the one the focus map
// holds.
BOOL CShadowMap::IsFocusCaster(int i_nSlot, int i_nUnits, CUnitData* i_pUnit) const
{
	if(!m_bFocusThisFrame)
	{
		return FALSE;
	}

	if(i_nSlot < i_nUnits)
	{
		return (NULL != m_pFocusUnit && m_pFocusUnit == i_pUnit);
	}
	if(i_nSlot == i_nUnits)
	{
		return m_bFocusIsCharacter;
	}
	return (m_nFocusExtra == (i_nSlot - i_nUnits - 1));
}

// The focus box itself. Everything BuildCascade() does applies here, the
// world-anchored snap grid included; what differs is where the centre comes
// from and how far back the near plane reaches.
BOOL CShadowMap::BuildFocusCascade(const D3DXVECTOR3& i_vLight)
{
	D3DXVECTOR3 vCentre(0.0f, 0.0f, 0.0f);
	float       fRadius = 0.0f;
	if(!FindFocusTarget(&vCentre, &fRadius))
	{
		return FALSE;
	}

	// Where the caster came from, before the centre is snapped - the near plane
	// has to reach back past it, and for a ship at altitude that is much
	// further than a cascade's own margin.
	const D3DXVECTOR3 vTarget = vCentre;

	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	if(fabsf(i_vLight.y) > 0.99f)
	{
		vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	// The texel grid, anchored on the world origin.
	{
		D3DXMATRIX  matSnap;
		D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
		D3DXMatrixLookAtLH(&matSnap, &vOrigin, &i_vLight, &vUp);

		D3DXVECTOR3 vLightSpace;
		D3DXVec3TransformCoord(&vLightSpace, &vCentre, &matSnap);

		const float fTexel = (fRadius * 2.0f) / (float)m_nFocusSize;
		if(fTexel > 0.000001f)
		{
			vLightSpace.x = floorf(vLightSpace.x / fTexel) * fTexel;
			vLightSpace.y = floorf(vLightSpace.y / fTexel) * fTexel;
			vLightSpace.z = floorf(vLightSpace.z / fTexel) * fTexel;

			D3DXMATRIX matSnapInv;
			D3DXMatrixInverse(&matSnapInv, NULL, &matSnap);
			D3DXVec3TransformCoord(&vCentre, &vLightSpace, &matSnapInv);
		}
	}

	float fHeight = (NULL != g_pGround)
				  ? (g_pGround->m_projectInfo.fHeightMax - g_pGround->m_projectInfo.fHeightMin)
				  : SHADOW_HEIGHT_MARGIN;
	if(!(fHeight > 0.0f) || fHeight > SHADOW_HEIGHT_MARGIN)
	{
		fHeight = SHADOW_HEIGHT_MARGIN;
	}

	// Far enough back to hold whatever this box is for.
	D3DXVECTOR3 vToTarget = vTarget - vCentre;
	float       fBack     = fRadius * 2.0f + fHeight;
	const float fReach    = D3DXVec3Length(&vToTarget) + fRadius * 2.0f;
	if(fReach > fBack)	{ fBack = fReach; }

	const D3DXVECTOR3 vEye = vCentre - i_vLight * fBack;
	const float fNear = 1.0f;
	const float fFar  = fBack + fRadius * 2.0f + fHeight;

	SCascade& out = m_arrCascade[SHADOW_FOCUS_CASCADE];
	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH(&matView, &vEye, &vCentre, &vUp);
	D3DXMatrixOrthoLH(&matProj, fRadius * 2.0f, fRadius * 2.0f, fNear, fFar);
	D3DXMatrixMultiply(&out.matViewProj, &matView, &matProj);

	out.vCentre    = vCentre;
	out.vEye       = vEye;
	out.fRadius    = fRadius;
	out.fNear      = fNear;
	out.fFar       = fFar;
	// It is not a slice of the view frustum and nothing may read these as one:
	// the receiver picks the focus map by containment, never by depth.
	out.fSliceNear = 0.0f;
	out.fSliceFar  = 0.0f;
	return TRUE;
}

// One cascade, fitted to the view depths between i_fSliceNear and i_fSliceFar.
BOOL CShadowMap::BuildCascade(int i_nCascade, const D3DXVECTOR3& i_vLight,
							  float i_fSliceNear, float i_fSliceFar)
{
	SCascade& out = m_arrCascade[i_nCascade];
	out.fSliceNear = i_fSliceNear;
	out.fSliceFar  = i_fSliceFar;

	// Fit the box to what the camera can see across this slice: the eight corners
	// of it, bounded by a sphere.
	const D3DXMATRIX matCamView = g_pCamera->GetViewMatrix();
	const D3DXMATRIX matCamProj = g_pCamera->GetProjMatrix();

	// The field of view comes back out of the projection: at a view depth of z
	// the frustum is z/_22 high and z/_11 wide.
	const float fInvH = (fabsf(matCamProj._22) > 0.0001f) ? (1.0f / matCamProj._22) : 1.0f;
	const float fInvW = (fabsf(matCamProj._11) > 0.0001f) ? (1.0f / matCamProj._11) : 1.0f;

	D3DXMATRIX matInvView;
	D3DXMatrixInverse(&matInvView, NULL, &matCamView);

	D3DXVECTOR3 arrCorner[8];
	int         nCorner = 0;
	for(int nEnd = 0; nEnd < 2; nEnd++)
	{
		const float z = (0 == nEnd) ? i_fSliceNear : i_fSliceFar;
		for(int nY = -1; nY <= 1; nY += 2)
		{
			for(int nX = -1; nX <= 1; nX += 2)
			{
				D3DXVECTOR3 vView((float)nX * z * fInvW, (float)nY * z * fInvH, z);
				D3DXVec3TransformCoord(&arrCorner[nCorner++], &vView, &matInvView);
			}
		}
	}

	D3DXVECTOR3 vCentre(0.0f, 0.0f, 0.0f);
	for(int n = 0; n < 8; n++)
	{
		vCentre += arrCorner[n];
	}
	vCentre /= 8.0f;

	float fRadius = 0.0f;
	for(int n = 0; n < 8; n++)
	{
		D3DXVECTOR3 vOff = arrCorner[n] - vCentre;
		const float fLen = D3DXVec3Length(&vOff);
		if(fLen > fRadius)	{ fRadius = fLen; }
	}
	if(fRadius < 1.0f)	{ fRadius = 1.0f; }

	float fHeight = g_pGround->m_projectInfo.fHeightMax - g_pGround->m_projectInfo.fHeightMin;
	if(!(fHeight > 0.0f) || fHeight > SHADOW_HEIGHT_MARGIN)
	{
		fHeight = SHADOW_HEIGHT_MARGIN;
	}

	// Keep the box on the ground.
	//
	// Fitting the frustum honestly puts the centre wherever the camera is
	// pointed, and this is a flight game: look at the horizon from altitude and
	// the centre goes with it, so the box is mostly sky.
	if(g_pGround->m_projectInfo.fHeightMax > g_pGround->m_projectInfo.fHeightMin)
	{
		if(vCentre.y < g_pGround->m_projectInfo.fHeightMin)
		{
			vCentre.y = g_pGround->m_projectInfo.fHeightMin;
		}
		else if(vCentre.y > g_pGround->m_projectInfo.fHeightMax)
		{
			vCentre.y = g_pGround->m_projectInfo.fHeightMax;
		}
	}

	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	if(fabsf(i_vLight.y) > 0.99f)
	{
		vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	// Snap the centre to this cascade's own texel grid.
	//
	// The box moves with the camera, and a shift of less than one texel still
	// moves which texel every shadow edge lands in - so the edges crawl and
	// sparkle as the ship flies, on a map that is otherwise perfectly stable.
	{
		D3DXMATRIX  matSnap;
		D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
		D3DXMatrixLookAtLH(&matSnap, &vOrigin, &i_vLight, &vUp);

		D3DXVECTOR3 vLightSpace;
		D3DXVec3TransformCoord(&vLightSpace, &vCentre, &matSnap);

		const float fTexel = (fRadius * 2.0f) / (float)m_nTileSize;
		if(fTexel > 0.0001f)
		{
			vLightSpace.x = floorf(vLightSpace.x / fTexel) * fTexel;
			vLightSpace.y = floorf(vLightSpace.y / fTexel) * fTexel;

			// And along the light as well.
			//
			// Only x and y decide which texel a thing lands in, so leaving the third
			// free looked harmless: it shifts every depth in the map and every depth
			// the receiver computes by the same amount, and the comparison between them
			// is untouched.
			vLightSpace.z = floorf(vLightSpace.z / fTexel) * fTexel;

			D3DXMATRIX matSnapInv;
			D3DXMatrixInverse(&matSnapInv, NULL, &matSnap);
			D3DXVec3TransformCoord(&vCentre, &vLightSpace, &matSnapInv);
		}
	}

	// Far enough back that everything above the centre is in front of the near
	// plane, and deep enough that everything below it is in front of the far one.
	const float fBack = fRadius * 2.0f + fHeight;

	const D3DXVECTOR3 vEye = vCentre - i_vLight * fBack;

	const float fNear = 1.0f;
	const float fFar  = fBack + fRadius * 2.0f + fHeight;

	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH(&matView, &vEye, &vCentre, &vUp);
	D3DXMatrixOrthoLH(&matProj, fRadius * 2.0f, fRadius * 2.0f, fNear, fFar);
	D3DXMatrixMultiply(&out.matViewProj, &matView, &matProj);

	out.vCentre = vCentre;
	out.vEye    = vEye;
	out.fRadius = fRadius;
	out.fNear   = fNear;
	out.fFar    = fFar;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//  The terrain, from the light
//////////////////////////////////////////////////////////////////////

// Every degree-two block has the same vertex layout - (size+1) squared
// vertices indexed as x * (size+1) + y, laid down by
// CQuadGround::RestoreDeviceObjects().
BOOL CShadowMap::EnsureBlockIndexBuffer(int i_nBlockSize)
{
	if(i_nBlockSize <= 0 || i_nBlockSize > 255)
	{
		return FALSE;
	}
	if(NULL != m_pBlockIB && m_nBlockSize == i_nBlockSize)
	{
		return TRUE;
	}

	SAFE_RELEASE(m_pBlockIB);
	m_nBlockSize      = 0;
	m_nBlockVertices  = 0;
	m_nBlockTriangles = 0;

	const int nStride    = i_nBlockSize + 1;
	const int nVertices  = nStride * nStride;
	const int nTriangles = i_nBlockSize * i_nBlockSize * 2;

	if(nVertices > 65535)
	{
		ShadowSay("ShadowMap: a %d block needs 32-bit indices, terrain will not cast\n", i_nBlockSize);
		return FALSE;
	}

	if(FAILED(g_pD3dDev->CreateIndexBuffer(nTriangles * 3 * sizeof(WORD),
										   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										   D3DPOOL_MANAGED, &m_pBlockIB, NULL)))
	{
		ShadowSay("ShadowMap: no index buffer for the terrain caster pass\n");
		return FALSE;
	}

	WORD* pIndex = NULL;
	if(FAILED(m_pBlockIB->Lock(0, 0, (void**)&pIndex, 0)))
	{
		SAFE_RELEASE(m_pBlockIB);
		return FALSE;
	}

	for(int nX = 0; nX < i_nBlockSize; nX++)
	{
		for(int nY = 0; nY < i_nBlockSize; nY++)
		{
			*pIndex++ = (WORD)( nX      * nStride + nY);
			*pIndex++ = (WORD)( nX      * nStride + nY + 1);
			*pIndex++ = (WORD)((nX + 1) * nStride + nY + 1);

			*pIndex++ = (WORD)( nX      * nStride + nY);
			*pIndex++ = (WORD)((nX + 1) * nStride + nY + 1);
			*pIndex++ = (WORD)((nX + 1) * nStride + nY);
		}
	}
	m_pBlockIB->Unlock();

	m_nBlockSize      = i_nBlockSize;
	m_nBlockVertices  = nVertices;
	m_nBlockTriangles = nTriangles;
	return TRUE;
}

// The block's world box against the light's clip box.
BOOL CShadowMap::IsBlockLit(const CQuadGround* i_pBlock) const
{
	float fMinX = i_pBlock->m_vPos[0].x, fMaxX = fMinX;
	float fMinZ = i_pBlock->m_vPos[0].z, fMaxZ = fMinZ;

	for(int n = 1; n < 4; n++)
	{
		if(i_pBlock->m_vPos[n].x < fMinX)	{ fMinX = i_pBlock->m_vPos[n].x; }
		if(i_pBlock->m_vPos[n].x > fMaxX)	{ fMaxX = i_pBlock->m_vPos[n].x; }
		if(i_pBlock->m_vPos[n].z < fMinZ)	{ fMinZ = i_pBlock->m_vPos[n].z; }
		if(i_pBlock->m_vPos[n].z > fMaxZ)	{ fMaxZ = i_pBlock->m_vPos[n].z; }
	}

	// The corners carry their own heights but the ground between them does not
	// stay inside those, so the box is opened out to the map's own range.
	float fMinY = g_pGround->m_projectInfo.fHeightMin;
	float fMaxY = g_pGround->m_projectInfo.fHeightMax;
	if(!(fMaxY > fMinY))
	{
		fMinY = -SHADOW_HEIGHT_MARGIN;
		fMaxY =  SHADOW_HEIGHT_MARGIN;
	}

	// Outside a clip plane only counts when every corner is outside the same
	// one; a box straddling two planes is still inside the box.
	int nOutLeft = 0, nOutRight = 0, nOutBottom = 0, nOutTop = 0, nOutNear = 0, nOutFar = 0;

	for(int n = 0; n < 8; n++)
	{
		D3DXVECTOR3 vCorner((n & 1) ? fMaxX : fMinX,
							(n & 2) ? fMaxY : fMinY,
							(n & 4) ? fMaxZ : fMinZ);
		D3DXVECTOR4 vClip;
		D3DXVec3Transform(&vClip, &vCorner, &m_matLightViewProj);

		if(vClip.x < -vClip.w)	{ nOutLeft++; }
		if(vClip.x >  vClip.w)	{ nOutRight++; }
		if(vClip.y < -vClip.w)	{ nOutBottom++; }
		if(vClip.y >  vClip.w)	{ nOutTop++; }
		if(vClip.z <  0.0f)		{ nOutNear++; }
		if(vClip.z >  vClip.w)	{ nOutFar++; }
	}

	return !(8 == nOutLeft || 8 == nOutRight || 8 == nOutBottom ||
			 8 == nOutTop  || 8 == nOutNear  || 8 == nOutFar);
}

void CShadowMap::GatherBlocks(CQuadTree* i_pNode)
{
	if(NULL == i_pNode || m_nBlocks >= MAX_TERRAIN_BLOCKS)
	{
		return;
	}

	// Only the degree-two nodes own a vertex buffer, which is what makes one a
	// block; everything above them is structure and everything below shares it.
	CQuadGround* pGround = (CQuadGround*)i_pNode;
	if(NULL != pGround->m_pVBTest)
	{
		if(IsBlockLit(pGround))
		{
			m_arrBlocks[m_nBlocks++] = pGround;
		}
		return;
	}

	for(int n = 0; n < QUAD_CHILD_NUM; n++)
	{
		GatherBlocks(i_pNode->m_pChild[n]);
	}
}

void CShadowMap::RenderTerrainCasters()
{
	// Emptied before anything can return, not after.
	//
	// m_arrBlocks holds raw CQuadGround pointers and the dump walks it, so a
	// return that leaves the count behind leaves the dump reading the *previous
	// map's* blocks - which have been deleted.
	m_nBlocks = 0;

	if(NULL == g_pGround || NULL == g_pGround->m_pQuad)
	{
		return;
	}
	GatherBlocks(g_pGround->m_pQuad);
	if(0 == m_nBlocks)
	{
		return;
	}

	if(!EnsureBlockIndexBuffer(m_arrBlocks[0]->m_nSize))
	{
		return;
	}

	m_pEffect->SetTechnique(m_hTechCaster);
	m_pEffect->SetMatrix(m_hCasterViewProj, &m_matLightViewProj);

	// The ground is drawn with an identity world matrix, and this has to say so.
	D3DXMATRIX matGroundWorld;
	D3DXMatrixIdentity(&matGroundWorld);
	m_pEffect->SetMatrix(m_hWorld, &matGroundWorld);

	UINT nPasses = 0;
	if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
	{
		return;
	}

	if(SUCCEEDED(m_pEffect->BeginPass(0)))
	{
		// Nothing changes between these draws but the stream, which is what
		// keeps them at the cheap end of what a draw costs on this client.
		g_pD3dDev->SetFVF(D3DFVF_GROUNDVERTEX);

		g_pD3dDev->SetIndices(m_pBlockIB);

		for(int n = 0; n < m_nBlocks; n++)
		{
			if(FAILED(g_pD3dDev->SetStreamSource(0, m_arrBlocks[n]->m_pVBTest, 0,
												 sizeof(GROUNDVERTEX))))
			{
				continue;
			}
			g_pD3dDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
											m_nBlockVertices, 0, m_nBlockTriangles);
			m_nCasterDraws     += 1;
			m_nCasterTriangles += m_nBlockTriangles;
		}
		m_pEffect->EndPass();
	}
	m_pEffect->End();
	m_nPasses++;
}

//////////////////////////////////////////////////////////////////////
//  The pass
//////////////////////////////////////////////////////////////////////

void CShadowMap::RenderCasters()
{
	m_bLitThisFrame = FALSE;

	ApplyQuality();
	if(m_nQuality < SHADOW_QUALITY_TERRAIN)
	{
		// The blobs, or nothing at all.  Either way this pass has no business
		// running, and IsReceiving() answers FALSE off m_bLitThisFrame.
		return;
	}

	if(!m_bAvailable || NULL == m_pShadowSurface || NULL == m_pShadowDepth || NULL == m_pEffect)
	{
		return;
	}
	if(!BuildLightMatrices())
	{
		return;
	}

	// The debug overlay leaves the map bound to a sampler, and binding a texture
	// as a render target while it is still readable is not allowed.
	for(DWORD nStage = 0; nStage < 4; nStage++)
	{
		g_pD3dDev->SetTexture(nStage, NULL);
	}

	LPDIRECT3DSURFACE9 pWasTarget = NULL;
	LPDIRECT3DSURFACE9 pWasDepth  = NULL;
	D3DVIEWPORT9       wasViewport;

	if(FAILED(g_pD3dDev->GetRenderTarget(0, &pWasTarget)))
	{
		return;
	}
	// D3DERR_NOTFOUND when there is none, which is not an error here.
	if(FAILED(g_pD3dDev->GetDepthStencilSurface(&pWasDepth)))
	{
		pWasDepth = NULL;
	}
	g_pD3dDev->GetViewport(&wasViewport);

	// The depth surface comes off first: the back buffer's is 4x multisampled
	// and the map's target is not, and a mismatched pair is refused.
	g_pD3dDev->SetDepthStencilSurface(NULL);

	if(SUCCEEDED(g_pD3dDev->SetRenderTarget(0, m_pShadowSurface)) &&
	   SUCCEEDED(g_pD3dDev->SetDepthStencilSurface(m_pShadowDepth)))
	{
		// The whole atlas to white first.
		g_pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

		// Then one cascade at a time into its own quarter.
		for(int nCascade = 0; nCascade < SHADOW_CASCADE_COUNT; nCascade++)
		{
			int nTileX = 0, nTileY = 0;
			CascadeTile(nCascade, &nTileX, &nTileY);

			D3DVIEWPORT9 tile;
			tile.X      = (DWORD)nTileX;
			tile.Y      = (DWORD)nTileY;
			tile.Width  = (DWORD)m_nTileSize;
			tile.Height = (DWORD)m_nTileSize;
			tile.MinZ   = 0.0f;
			tile.MaxZ   = 1.0f;
			if(FAILED(g_pD3dDev->SetViewport(&tile)))
			{
				continue;
			}

			SelectCascade(nCascade);

			RenderTerrainCasters();
		}

		// The static objects, once rather than once per cascade, for the same
		// reason as the skinned ones below: posing an object walks its whole
		// frame hierarchy, and the pose does not depend on which cascade it is
		// about to be drawn into.
		RenderObjectCasters();

		// The skinned casters last, and once rather than once per cascade.
		//
		// They are the one caster whose *preparation* is expensive - a pose and a
		// CPU skin, 82% of what one costs by 4.4's measurement - and none of that
		// depends on which cascade it is going into.
		RenderSkinnedCasters();

		// And the focus map, last, because it is the one pass that changes the
		// render target.
		if(m_bFocusThisFrame)
		{
			RenderFocusCasters();
		}

		SelectCascade(0);
		m_bLitThisFrame = TRUE;
	}

	g_pD3dDev->SetDepthStencilSurface(NULL);
	g_pD3dDev->SetRenderTarget(0, pWasTarget);
	g_pD3dDev->SetDepthStencilSurface(pWasDepth);
	g_pD3dDev->SetViewport(&wasViewport);

	SAFE_RELEASE(pWasTarget);
	SAFE_RELEASE(pWasDepth);

	// A dump on entering a map, but only if somebody asked for one.
	//
	// This used to fire on every map change, which was right while the pass was
	// being built and is litter in a shipped client: a 2048-square png and a text
	// file into Debug-ShadowMap every time anybody walks through a gate.
	const int nMapIndex = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;
	if(m_bLitThisFrame && nMapIndex != m_nLastMapIndex)
	{
		m_nLastMapIndex = nMapIndex;
		if(m_bDumpOnMapChange)
		{
			m_nDumpsWanted = 1;
		}
	}

	if(m_nDumpsWanted > 0 && m_bLitThisFrame)
	{
		m_nDumpsWanted--;
		WriteDump();
	}

	ReportOccasionally();
}

//////////////////////////////////////////////////////////////////////
//  The debug view
//////////////////////////////////////////////////////////////////////

void CShadowMap::RenderDebugOverlay()
{
	// Before the mode gate and before the corner quad is drawn: the frame is
	// finished at this point and the overlay is not in it yet.
	if(m_nDumpsWanted > 0 && !m_bScreenPending)
	{
		m_nDumpsWanted--;
		WriteDumpFrameOnly();
	}

	CapturePendingFrame();

	if(!m_bAvailable || NULL == m_pShadowTexture || NULL == m_pEffect)
	{
		return;
	}
	if(NULL == g_pD3dApp || g_pD3dApp->m_nShadowMapMode < 2)
	{
		return;
	}

	struct DEBUGVERTEX
	{
		float x, y, z;
		float tu, tv;
	};

	// Clip space, so the quad needs no transform at all - but clip space is
	// square and the screen is not, so the width is scaled to keep the map
	// looking like the square it is.
	const D3DSURFACE_DESC desc  = g_pD3dApp->GetBackBufferDesc();
	const float           fAsp  = (desc.Width > 0) ? (float)desc.Height / (float)desc.Width : 1.0f;
	const float           fSizeY = SHADOW_DEBUG_SIZE;
	const float           fSizeX = SHADOW_DEBUG_SIZE * fAsp;
	const float           fRight = 0.98f;
	const float           fTop   = 0.98f;

	DEBUGVERTEX v[4] =
	{
		{ fRight - fSizeX, fTop - fSizeY, 0.0f, 0.0f, 1.0f },
		{ fRight - fSizeX, fTop,          0.0f, 0.0f, 0.0f },
		{ fRight,          fTop - fSizeY, 0.0f, 1.0f, 1.0f },
		{ fRight,          fTop,          0.0f, 1.0f, 0.0f },
	};

	m_pEffect->SetTechnique(m_hTechDebug);
	m_pEffect->SetTexture(m_hShadowTexture, m_pShadowTexture);

	UINT nPasses = 0;
	if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
	{
		return;
	}
	if(SUCCEEDED(m_pEffect->BeginPass(0)))
	{
		g_pD3dDev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		g_pD3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(DEBUGVERTEX));
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}

//////////////////////////////////////////////////////////////////////
//  Receiving
//////////////////////////////////////////////////////////////////////

BOOL CShadowMap::IsReceiving() const
{
	// m_bLitThisFrame covers the sun being below the horizon and the map not
	// being ready; the mode covers the pass being switched off.
	return (m_bAvailable && m_bLitThisFrame && NULL != m_pShadowTexture &&
			NULL != g_pD3dApp && g_pD3dApp->m_nShadowMapMode > 0);
}

// Everything the receiver shader reads that does not change between receivers.
BOOL CShadowMap::SetReceiverConstants()
{
	if(NULL == g_pCamera || NULL == g_pScene)
	{
		return FALSE;
	}

	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &g_pCamera->GetViewMatrix(), &g_pCamera->GetProjMatrix());

	// The three matrices, packed out of the cascades: SetMatrixArray wants them
	// contiguous and SCascade carries its box alongside each one.
	D3DXMATRIX arrLight[SHADOW_CASCADE_COUNT];
	float      arrSplit[SHADOW_CASCADE_COUNT];
	float      arrTileU[SHADOW_CASCADE_COUNT];
	float      arrTileV[SHADOW_CASCADE_COUNT];
	float      arrOffset[SHADOW_CASCADE_COUNT];
	float      arrBiasScale[SHADOW_CASCADE_COUNT];
	float      arrSoft[SHADOW_CASCADE_COUNT];
	float      arrSoftMax[SHADOW_CASCADE_COUNT];
	float      arrFilter[SHADOW_CASCADE_COUNT];

	// The bias, per cascade, and the rule is not the obvious one.
	//
	// It is written in normalised light depth, so carrying the same number across
	// would mean a different distance on the ground in each cascade - the near
	// one's depth range is a fraction of the far one's.
	const SCascade& ref = m_arrCascade[SHADOW_CASCADE_COUNT - 1];
	const float fRefDepth  = ref.fFar - ref.fNear;
	const float fRefRadius = (ref.fRadius > 0.001f) ? ref.fRadius : 1.0f;

	for(int nC = 0; nC < SHADOW_CASCADE_COUNT; nC++)
	{
		const SCascade& c = m_arrCascade[nC];
		arrLight[nC] = c.matViewProj;

		int nTileX = 0, nTileY = 0;
		CascadeTile(nC, &nTileX, &nTileY);
		arrTileU[nC] = (float)nTileX / (float)m_nMapSize;
		arrTileV[nC] = (float)nTileY / (float)m_nMapSize;

		// The normal offset is in world units and comes off the size of a texel,
		// so it is per cascade too - two texels of the near cascade is a few
		// centimetres and two of the far one is four metres.
		arrOffset[nC] = ((c.fRadius * 2.0f) / (float)m_nTileSize) *
						SHADOW_NORMAL_OFFSET_TEXELS;

		const float fDepth = c.fFar - c.fNear;
		arrBiasScale[nC] = (fDepth > 0.001f)
						 ? ((c.fRadius / fRefRadius) * (fRefDepth / fDepth)) : 1.0f;

		// Normalised light depth, into a penumbra in texels.
		const float fTexelWorld = (c.fRadius * 2.0f) / (float)m_nTileSize;
		arrSoft[nC] = (fTexelWorld > 0.0001f)
					? (fDepth * m_fSoftSize / fTexelWorld) : 0.0f;

		// The widest blur, in texels of *this* cascade, so that it is the same
		// distance on the ground in all of them.
		arrSoftMax[nC] = (fTexelWorld > 0.0001f) ? (m_fSoftMax / fTexelWorld) : 1.0f;
		if(arrSoftMax[nC] < 1.0f)	{ arrSoftMax[nC] = 1.0f; }
		if(arrSoftMax[nC] > 16.0f)	{ arrSoftMax[nC] = 16.0f; }

		// The kernel, in this cascade's own texels, from one width in world units.
		arrFilter[nC] = (fTexelWorld > 0.0001f) ? (m_fFilterWidth / fTexelWorld) : 1.0f;
		if(arrFilter[nC] < SHADOW_FILTER_MIN_TEXELS)	{ arrFilter[nC] = SHADOW_FILTER_MIN_TEXELS; }
		if(arrFilter[nC] > SHADOW_FILTER_MAX_TEXELS)	{ arrFilter[nC] = SHADOW_FILTER_MAX_TEXELS; }
	}

	// Where each cascade stops, in view depth.  The shader compares the pixel's
	// own depth against these to decide which one it is in.
	for(int nS = 0; nS < SHADOW_CASCADE_COUNT; nS++)
	{
		arrSplit[nS] = m_arrCascade[nS].fSliceFar;
	}

	m_pEffect->SetMatrixArray(m_hLightViewProj, arrLight, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hSplit, arrSplit, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hTileU, arrTileU, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hTileV, arrTileV, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hNormalOffset, arrOffset, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hBiasScale, arrBiasScale, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hSoftScale, arrSoft, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hSoftMax, arrSoftMax, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloatArray(m_hFilterRadius, arrFilter, SHADOW_CASCADE_COUNT);
	m_pEffect->SetFloat(m_hTileScale, (float)m_nTileSize / (float)m_nMapSize);
	m_pEffect->SetMatrix(m_hViewProj, &matViewProj);
	m_matCameraViewProj = matViewProj;
	m_pEffect->SetTexture(m_hShadowTexture, m_pShadowTexture);

	const D3DXVECTOR3 vEye = g_pCamera->GetEyePt();
	m_pEffect->SetFloatArray(m_hCameraPos, (const float*)&vEye, 3);
	m_pEffect->SetFloatArray(m_hLightDirection, (const float*)&m_vDumpLightDir, 3);

	// What a fully shadowed pixel keeps.
	const D3DCOLORVALUE& amb = g_pScene->m_light0.Ambient;
	const D3DCOLORVALUE& dif = g_pScene->m_light0.Diffuse;

	float arrShadow[3];
	arrShadow[0] = amb.r / ((amb.r + dif.r > 0.001f) ? (amb.r + dif.r) : 1.0f);
	arrShadow[1] = amb.g / ((amb.g + dif.g > 0.001f) ? (amb.g + dif.g) : 1.0f);
	arrShadow[2] = amb.b / ((amb.b + dif.b > 0.001f) ? (amb.b + dif.b) : 1.0f);
	for(int n = 0; n < 3; n++)
	{
		arrShadow[n] = 1.0f + (arrShadow[n] - 1.0f) * SHADOW_STRENGTH;
	}
	m_pEffect->SetFloatArray(m_hShadowColour, arrShadow, 3);

	m_pEffect->SetFloat(m_hFlatShade,
						(SHADOW_EXPERIMENT_FLAT_SHADE == g_nShadowExperiment) ? 1.0f : 0.0f);

	// The shadow fades out over the same distance the fog does, or it would be
	// a dark wash on ground that is otherwise lost in haze.
	float fFogEnd  = g_pScene->m_fFogEndValue;
	float fFogSpan = g_pScene->m_fFogEndValue - g_pScene->m_fFogStartValue;
	if(!(fFogEnd > 0.0f))		{ fFogEnd = SHADOW_MAX_RANGE; }
	if(!(fFogSpan > 1.0f))		{ fFogSpan = 1.0f; }

	float arrFog[2] = { fFogEnd, 1.0f / fFogSpan };
	m_pEffect->SetFloatArray(m_hFog, arrFog, 2);

	// The constant and slope biases are shared; the normal offset above is not,
	// because it is a world distance and a texel is worth a different number of
	// them in each cascade.
	float arrBias[4];
	arrBias[0] = 0.0f;					// unused - see g_vNormalOffset
	arrBias[1] = SHADOW_DEPTH_BIAS;
	arrBias[2] = 1.0f / SHADOW_EDGE_FADE;
	arrBias[3] = SHADOW_SLOPE_BIAS;
	m_pEffect->SetFloatArray(m_hBias, arrBias, 4);

	// The two numbers the depth test is decided by, into the effect, which
	// applies them at BeginPass because the passes name them.
	m_pEffect->SetFloat(m_hReceiverBias, m_fReceiverBias);
	m_pEffect->SetFloat(m_hReceiverSlopeBias, m_fReceiverSlopeBias);

	m_pEffect->SetFloat(m_hShadowTexel, 1.0f / (float)m_nMapSize);
	m_pEffect->SetFloat(m_hShadowSize, (float)m_nMapSize);
	m_pEffect->SetFloat(m_hWide, (m_nFilter >= 2) ? 1.0f : 0.0f);
	m_pEffect->SetFloat(m_hBlend, m_fBlend);

	// The focus map.  Off is one float, and the receiver's branch then costs a
	// comparison and nothing else - which is the whole reason this needs Shader
	// Model 3.
	m_pEffect->SetFloat(m_hFocusOn, (m_bFocusThisFrame && NULL != m_pFocusTexture) ? 1.0f : 0.0f);
	m_pEffect->SetTexture(m_hFocusTexture, m_pFocusTexture);
	if(m_bFocusThisFrame && NULL != m_pFocusTexture)
	{
		const SCascade& f = m_arrCascade[SHADOW_FOCUS_CASCADE];
		const float fTexelWorld = (f.fRadius * 2.0f) / (float)m_nFocusSize;
		const float fDepth      = f.fFar - f.fNear;

		m_pEffect->SetMatrix(m_hFocusViewProj, &f.matViewProj);
		m_pEffect->SetFloat(m_hFocusTexel, 1.0f / (float)m_nFocusSize);
		m_pEffect->SetFloat(m_hFocusSize, (float)m_nFocusSize);
		m_pEffect->SetFloat(m_hFocusOffset, fTexelWorld * SHADOW_NORMAL_OFFSET_TEXELS);
		m_pEffect->SetFloat(m_hFocusFade, 1.0f / SHADOW_FOCUS_EDGE_FADE);

		// The same world width in the focus map's own texels.
		float fFocusFilter = (fTexelWorld > 0.0001f) ? (m_fFocusFilterWidth / fTexelWorld) : 1.0f;
		if(fFocusFilter < 1.0f)							{ fFocusFilter = 1.0f; }
		if(fFocusFilter > SHADOW_FILTER_MAX_TEXELS)		{ fFocusFilter = SHADOW_FILTER_MAX_TEXELS; }
		m_pEffect->SetFloat(m_hFocusFilter, fFocusFilter);

		// The same rule as the cascades', by the **texel** and not by the radius.
		const float fRefTexel = (ref.fRadius * 2.0f) / (float)m_nTileSize;
		m_pEffect->SetFloat(m_hFocusBias,
							(fDepth > 0.001f && fRefTexel > 0.000001f)
						  ? (m_fFocusBias * (fTexelWorld / fRefTexel) * (fRefDepth / fDepth)) : 1.0f);
	}

	return TRUE;
}

void CShadowMap::QueueTerrainReceiver(LPDIRECT3DVERTEXBUFFER9 i_pVB, LPDIRECT3DINDEXBUFFER9 i_pIB,
									  int i_nVertices, int i_nTriangles)
{
	if(!IsReceiving() || NULL == i_pVB || NULL == i_pIB || i_nTriangles <= 0)
	{
		return;
	}
	if(m_nReceivers >= MAX_RECEIVERS)
	{
		return;
	}

	// Nothing is addrefed: these live in CQuadGround for the life of the map,
	// and the queue is emptied inside the same frame it is filled.
	m_arrReceivers[m_nReceivers].pVB        = i_pVB;
	m_arrReceivers[m_nReceivers].pIB        = i_pIB;
	m_arrReceivers[m_nReceivers].nVertices  = i_nVertices;
	m_arrReceivers[m_nReceivers].nTriangles = i_nTriangles;
	m_nReceivers++;
}

void CShadowMap::FlushTerrainReceivers()
{
	const int nQueued = m_nReceivers;
	m_nReceivers = 0;

	if(nQueued <= 0 || !IsReceiving() || NULL == m_pEffect)
	{
		return;
	}
	if(!SetReceiverConstants())
	{
		return;
	}

	// The ground is drawn with an identity world matrix - see
	// CBackground::Render() - so the shader gets the same one.
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pEffect->SetMatrix(m_hWorld, &matWorld);
	m_pEffect->SetMatrix(m_hWorldViewProj, &m_matCameraViewProj);
	m_pEffect->SetTechnique(ReceiverTechnique(FALSE));

	UINT nPasses = 0;
	if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
	{
		return;
	}
	if(SUCCEEDED(m_pEffect->BeginPass(0)))
	{
		g_pD3dDev->SetFVF(D3DFVF_GROUNDVERTEX);

		// Mode three drops the depth test.
		if(NULL != g_pD3dApp && 3 == g_pD3dApp->m_nShadowMapMode)
		{
			g_pD3dDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		}

		for(int n = 0; n < nQueued; n++)
		{
			const SReceiver& r = m_arrReceivers[n];
			if(FAILED(g_pD3dDev->SetStreamSource(0, r.pVB, 0, sizeof(GROUNDVERTEX))))
			{
				continue;
			}
			g_pD3dDev->SetIndices(r.pIB);
			g_pD3dDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
											r.nVertices, 0, r.nTriangles);
			m_nReceiverDraws++;
		}
		m_pEffect->EndPass();
	}
	m_pEffect->End();

	// The map must not still be on a sampler when the next frame binds it as a
	// render target.
	g_pD3dDev->SetTexture(0, NULL);
}

//////////////////////////////////////////////////////////////////////
//  The static objects.
//////////////////////////////////////////////////////////////////////

// Poses one object the way CObjRender::Render() does, and answers its mesh.
// The pose has to be rebuilt rather than reused, because one CSkinnedMesh is
// shared by every copy of a model and matCombined holds whichever instance
// touched it last.
static void AccumulateMatrix(unsigned int* io_pnSum, const D3DXMATRIX& i_mat)
{
	const unsigned int* pBits = (const unsigned int*)&i_mat;
	for(int n = 0; n < 16; n++)
	{
		*io_pnSum = (*io_pnSum * 16777619u) ^ pBits[n];
	}
}

CSkinnedMesh* CShadowMap::PoseObject(CObjectChild* i_pObject)
{
	if(NULL == i_pObject || NULL == i_pObject->m_pObjMesh || NULL == i_pObject->m_pObjectInfo)
	{
		return NULL;
	}
	if(!i_pObject->IsShowNode())
	{
		return NULL;
	}
	// The big effect objects are not geometry the main pass draws either.
	if(OBJECT_BIG_EFFECT == i_pObject->m_pObjectInfo->ObjectRenderType)
	{
		return NULL;
	}

	m_nCasterPoses++;

	CSkinnedMesh* pMesh = i_pObject->m_pObjMesh;
	if(NULL == pMesh->m_pdeHead)
	{
		return NULL;
	}

	pMesh->Tick(i_pObject->m_fCurrentTime);
	pMesh->SetWorldMatrix(i_pObject->m_mMatrix);

	// The same texture set CObjRender::Render() picks, so GetFallbackTexture()
	// answers what the frame would have drawn with.
	pMesh->AnotherTexture(1);

	// And this copy's own buffer for its animated parts, so the blend below
	// writes somewhere the frame and the receiver can read it again rather than
	// into the buffer every copy of this model shares.
	pMesh->SetSkinInstance(i_pObject->SkinInstance());

	// Pose *every* node, not just the ones the camera can see.
	//
	// CSkinnedMesh::UpdateFrames() does not recurse into a child that fails the
	// camera frustum test or the fog distance test (SkinnedMesh.cpp:1910), so
	// that child's matCombined is left holding whatever was last written there.
	const BOOL bWasDetailDrawFrame = g_bDetailDrawFrame;
	g_bDetailDrawFrame = FALSE;

	for(SDrawElement* pde = pMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
	{
		pMesh->UpdateFrames(pde->pframeRoot, i_pObject->m_mMatrix);
	}

	// Some of the static world is animated - the plants on Shadow-Test-plants are
	// a rigid stem with a skinned crown on top - and a skinned container's
	// vertices are blended on the CPU into pMesh.
	if(m_bAnimatedObjects)
	{
		pMesh->UpdateSkinnedVertices();
	}

	g_bDetailDrawFrame = bWasDetailDrawFrame;
	return pMesh;
}

// Walks a posed hierarchy, setting the world matrix per container and drawing
// every subset.
void CShadowMap::DrawPosedHierarchy(SFrame* i_pFrame, LPDIRECT3DTEXTURE9* io_ppBoundTexture,
									BOOL i_bNeedTexture, unsigned __int64* io_pnDraws,
									const D3DXMATRIX* i_pObjectWorld,
									CSkinnedMesh* i_pMesh,
									BOOL i_bDrawSkinned)
{
	if(NULL == i_pFrame)
	{
		return;
	}

	// What a subset with no texture of its own is drawn with.  A property of
	// the mesh, so it is the same answer for every node below here.
	const LPDIRECT3DTEXTURE9 pFallbackTexture =
		(NULL != i_pMesh) ? i_pMesh->GetFallbackTexture() : NULL;

	if(NULL != i_pFrame->pmcMesh)
	{
		AccumulateMatrix(m_bShadingPass ? &m_nReceiverPoseSum : &m_nCasterPoseSum, i_pFrame->matCombined);

		BOOL bCommitted = FALSE;
		int  nWorldSet  = -1;		// which matrix is in the effect: 0 the
									// frame's, 1 identity, -1 neither yet

		for(SMeshContainer* pmc = i_pFrame->pmcMesh; NULL != pmc; pmc = pmc->pmcNext)
		{
			// Where this container's geometry is.
			ID3DXMesh* pDraw = (NULL != i_pMesh) ? i_pMesh->SkinTarget(pmc) : pmc->pMesh;

			const BOOL bSkinned = (NULL != pmc->m_pSkinMeshInfo);
			if(bSkinned && !i_bDrawSkinned)
			{
				// Not blended this frame, so pMesh holds whichever copy of this model was
				// drawn last, in world space.
				continue;
			}
			if(NULL == pDraw)
			{
				continue;
			}

			const int nWant = bSkinned ? 1 : 0;

			if(nWant != nWorldSet)
			{
				D3DXMATRIX matWorld;
				if(bSkinned)	{ D3DXMatrixIdentity(&matWorld); }
				else			{ matWorld = i_pFrame->matCombined; }

				m_pEffect->SetMatrix(m_hWorld, &matWorld);

				// The shading pass has to land on the depth the fixed function pipeline
				// already wrote, so its position goes through one concatenated matrix
				// rather than through a world and then a view-projection.
				if(m_bShadingPass)
				{
					D3DXMATRIX matWVP;
					D3DXMatrixMultiply(&matWVP, &matWorld, &m_matCameraViewProj);
					m_pEffect->SetMatrix(m_hWorldViewProj, &matWVP);
				}

				nWorldSet  = nWant;
				bCommitted = FALSE;
			}

			for(DWORD nAttr = 0; nAttr < pmc->cpattr; nAttr++)
			{
				if(i_bNeedTexture)
				{
					// A skinned container's subsets are indirected through the attribute
					// table and a rigid one's are not, which is how DrawMeshContainer() reads
					// them - so a cut-out crown would otherwise clip against the wrong sheet.
					DWORD nTex = nAttr;
					if(bSkinned && NULL != pmc->m_pAttrTable)
					{
						nTex = pmc->m_pAttrTable[nAttr].AttribId;
					}

					LPDIRECT3DTEXTURE9 pTexture = (NULL != pmc->pTextures) ? pmc->pTextures[nTex] : NULL;
					if(NULL == pTexture)
					{
						// The subset carries no texture of its own, and the frame does not skip
						// it for that: DrawMeshContainer() falls back to the mesh's
						// m_pTexture[m_bTextureNum - 1] and draws it anyway.
						pTexture = pFallbackTexture;
					}
					if(NULL == pTexture)
					{
						// Nothing to clip against anywhere.
						continue;
					}
					if(pTexture != *io_ppBoundTexture)
					{
						m_pEffect->SetTexture(m_hBaseTexture, pTexture);
						*io_ppBoundTexture = pTexture;
						bCommitted = FALSE;
					}
				}

				if(!bCommitted)
				{
					m_pEffect->CommitChanges();
					bCommitted = TRUE;
				}

				pDraw->DrawSubset(nAttr);
				(*io_pnDraws)++;
			}
		}
	}

	for(SFrame* pChild = i_pFrame->pframeFirstChild; NULL != pChild; pChild = pChild->pframeSibling)
	{
		// Draw only the children whose pose was just refreshed.
		//
		// This is the whole bug, and it is worth being precise about.
		if(NULL != i_pObjectWorld && g_bDetailDrawFrame && NULL != pChild->pmcMesh)
		{
			D3DXMATRIX matCentre;
			D3DXMatrixIdentity(&matCentre);
			matCentre._41 = pChild->pmcMesh->vCenter.x;
			matCentre._42 = pChild->pmcMesh->vCenter.y;
			matCentre._43 = pChild->pmcMesh->vCenter.z;
			D3DXMatrixMultiply(&matCentre, &matCentre, i_pObjectWorld);

			const D3DXVECTOR3 vMeshPos(matCentre._41, matCentre._42, matCentre._43);

			D3DXVECTOR3 vOff = vMeshPos - g_pCamera->m_vCamCurrentPos;
			if(D3DXVec3Length(&vOff) - pChild->pmcMesh->fRadius > g_pScene->m_fFogEndValue)
			{
				continue;
			}
		}

		DrawPosedHierarchy(pChild, io_ppBoundTexture, i_bNeedTexture, io_pnDraws, i_pObjectWorld,
						   i_pMesh, i_bDrawSkinned);
	}
}

// The objects, into the map.
void CShadowMap::RenderObjectCasters()
{
	if(NULL == g_pScene || NULL == m_pEffect)
	{
		return;
	}

	// Quality 2 is the ground and nothing else.
	if(!m_bCastObjects)
	{
		m_nCasterObjects = 0;
		return;
	}

	// Radius of an object in the box's own units, so the test is the same
	// cheap one for every object.
	m_nCasterObjects = 0;
	m_nCasterPoses   = 0;
	m_nCasterPoseSum = 2166136261u;
	m_nCasterDrawsWas = m_nCasterDraws;

	// Two groups, because the cut-outs need a different pixel shader and a
	// texture, and switching technique per object would mean an effect Begin
	// and End per object.
	for(int nPass = 0; nPass < 2; nPass++)
	{
		const BOOL bAlpha = (1 == nPass);

		// Gather first, so an empty group costs no Begin at all.
		BOOL bBegun = FALSE;
		vectorCObjectChildPtr::iterator it = g_pScene->m_vectorRangeObjectPtrList.begin();
		for(; it != g_pScene->m_vectorRangeObjectPtrList.end(); ++it)
		{
			CObjectChild* pObject = *it;
			if(NULL == pObject || NULL == pObject->m_pObjectInfo || NULL == pObject->m_pObjMesh)
			{
				continue;
			}
			if((0 != pObject->m_pObjectInfo->Alpha) != (0 != bAlpha))
			{
				continue;
			}

			// Which cascades want it, as a bit each.
			//
			// An object near the camera is inside more than one box and has to be drawn
			// into each - but it only has to be *posed* once, and posing walks its
			// whole frame hierarchy with the camera cull turned off.
			int nMask = 0;
			for(int nC = 0; nC < m_nObjectCascades; nC++)
			{
				SelectCascade(nC);
				if(IsSphereLit(pObject->m_vOriPos, pObject->m_pObjMesh->m_fRadius))
				{
					nMask |= (1 << nC);
				}
			}
			if(0 == nMask)
			{
				continue;
			}

			if(!bBegun)
			{
				m_pEffect->SetTechnique(bAlpha ? m_hTechCasterAlpha : m_hTechCaster);

				UINT nPasses = 0;
				if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
				{
					return;
				}
				if(FAILED(m_pEffect->BeginPass(0)))
				{
					m_pEffect->End();
					return;
				}
				bBegun = TRUE;
			}
			m_nCasterObjects++;

			// Posing and drawing timed apart.
			const __int64 nT0 = NowTicks();
			CSkinnedMesh* pPosed = PoseObject(pObject);
			const __int64 nT1 = NowTicks();
			m_fPoseUs += (double)(nT1 - nT0);
			m_nPoseCalls++;

			if(NULL == pPosed)
			{
				continue;
			}

			for(int nC = 0; nC < m_nObjectCascades; nC++)
			{
				if(0 == (nMask & (1 << nC)))
				{
					continue;
				}
				if(!SetCascadeViewport(nC))
				{
					continue;
				}
				SelectCascade(nC);
				m_pEffect->SetMatrix(m_hCasterViewProj, &m_matLightViewProj);

				LPDIRECT3DTEXTURE9 pBound = NULL;
				for(SDrawElement* pde = pObject->m_pObjMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
				{
					DrawPosedHierarchy(pde->pframeRoot, &pBound, bAlpha, &m_nCasterDraws, NULL,
									   pPosed, m_bAnimatedObjects);
				}
			}
			pObject->m_pObjMesh->SetSkinInstance(NULL);
			m_fDrawUs += (double)(NowTicks() - nT1);
		}

		if(bBegun)
		{
			m_pEffect->EndPass();
			m_pEffect->End();
		}
	}
}

//////////////////////////////////////////////////////////////////////
//  Skinned receivers
//////////////////////////////////////////////////////////////////////

// Ships, characters and monsters, whose meshes are skinned all the way down.
//
// This was a second walker, written when DrawPosedHierarchy() could only draw
// rigid containers.
void CShadowMap::DrawSkinnedFrames(SFrame* i_pFrame, CSkinnedMesh* i_pMesh,
								   unsigned __int64* io_pnDraws)
{
	LPDIRECT3DTEXTURE9 pBound = NULL;
	DrawPosedHierarchy(i_pFrame, &pBound, FALSE, io_pnDraws, NULL, i_pMesh, TRUE);
}

void CShadowMap::ShadeSkinnedMesh(CSkinnedMesh* i_pMesh)
{
	// Experiment 5: the skinned receivers stay out of the frame, so what
	// shades is only the ground and the static objects.
	if(SHADOW_EXPERIMENT_NO_SKINNED_RECV == g_nShadowExperiment)
	{
		return;
	}

	if(!IsReceiving() || NULL == m_pEffect || NULL == i_pMesh)
	{
		return;
	}
	if(NULL == i_pMesh->m_pdeHead)
	{
		return;
	}

	// Only the software path.
	//
	// The two blend paths put the bones in D3DTS_WORLDMATRIX(i) and let fixed
	// function combine them, which nothing here can reproduce - drawing their
	// meshes as if they were posed would put shading where the model is not.
	if(SOFTWARE != i_pMesh->GetMethod())
	{
		return;
	}

	if(!SetReceiverConstants())
	{
		return;
	}

	m_bShadingPass = TRUE;
	m_pEffect->SetTechnique(ReceiverTechnique(FALSE));

	UINT nPasses = 0;
	if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
	{
		m_bShadingPass = FALSE;
		return;
	}
	if(FAILED(m_pEffect->BeginPass(0)))
	{
		m_pEffect->End();
		m_bShadingPass = FALSE;
		return;
	}

	for(SDrawElement* pde = i_pMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
	{
		DrawSkinnedFrames(pde->pframeRoot, i_pMesh, &m_nReceiverDraws);
	}

	m_pEffect->EndPass();
	m_pEffect->End();
	m_bShadingPass = FALSE;

	m_nReceiverObjects++;
	g_pD3dDev->SetTexture(0, NULL);
}


//////////////////////////////////////////////////////////////////////
//  Skinned casters
//////////////////////////////////////////////////////////////////////

// TRUE when a sphere of this radius at this point can be inside the light's
// box.
BOOL CShadowMap::IsSphereLit(const D3DXVECTOR3& i_vPos, float i_fRadius) const
{
	// Experiment 3 takes the cull out, so the pass makes no per-frame decision.
	if(SHADOW_EXPERIMENT_NO_CASTER_CULL == g_nShadowExperiment)
	{
		return TRUE;
	}

	const float fInvRadius = (m_fDumpRadius > 0.001f) ? (1.0f / m_fDumpRadius) : 1.0f;
	const float fInvDepth  = ((m_fDumpFar - m_fDumpNear) > 0.001f)
						   ? (1.0f / (m_fDumpFar - m_fDumpNear)) : 1.0f;

	D3DXVECTOR4 vClip;
	D3DXVec3Transform(&vClip, &i_vPos, &m_matLightViewProj);

	const float fR = i_fRadius * fInvRadius;
	const float fZ = i_fRadius * fInvDepth;

	return !(vClip.x < -1.0f - fR || vClip.x > 1.0f + fR ||
			 vClip.y < -1.0f - fR || vClip.y > 1.0f + fR ||
			 vClip.z < -fZ        || vClip.z > 1.0f + fZ);
}

// Answers the mesh a unit draws itself with, and the world matrix it draws
// with, or NULL if there is nothing to cast from yet.
CSkinnedMesh* CShadowMap::FindUnitMesh(CUnitData* i_pUnit, D3DXMATRIX* o_pWorld,
									   float* o_pPoseTime)
{
	if(NULL == i_pUnit || NULL == g_pD3dApp)
	{
		return NULL;
	}

	// Something the main pass is fading out - a cloak, a skill - goes in the
	// scene's transparent group, and a solid shadow under a half-invisible ship
	// is worse than no shadow at all.
	if(i_pUnit->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE)
	{
		return NULL;
	}

	CSkinnedMesh* pMesh = NULL;

	switch(i_pUnit->m_dwPartType)
	{
	case _SHUTTLE:
	case _ENEMY:
	case _ADMIN:
		{
			// On foot or in a ship: two renderers, two numbers, and getting it the
			// wrong way round casts the shadow of a model that is not there.
			const BOOL bOnFoot = (_SHUTTLE != i_pUnit->m_dwPartType) &&
								 ((CEnemyData*)i_pUnit)->m_bEnemyCharacter;

			CMeshRender* pRender = bOnFoot
								 ? (CMeshRender*)g_pD3dApp->m_pCharacterRender
								 : (CMeshRender*)g_pD3dApp->m_pUnitRender;
			if(NULL == pRender)
			{
				return NULL;
			}

			const int nIndex = bOnFoot ? i_pUnit->GetPilotNum() : i_pUnit->GetUnitNum();

			// The map directly, and not GetUnitMesh(): that queues a load for anything
			// missing, and the shadow pass has no business adding to the loader's
			// queue.
			map<int, CSkinnedMesh*>::iterator it = pRender->m_mapSkinnedMesh.find(nIndex);
			if(it == pRender->m_mapSkinnedMesh.end())
			{
				return NULL;
			}
			pMesh = it->second;
		}
		break;

	case _MONSTER:
		// CMonRender draws straight from here - MonRender.cpp:98 - so there is
		// no map to look in.
		pMesh = ((CMonsterData*)i_pUnit)->m_pMonMesh;
		break;

	default:
		return NULL;
	}

	if(NULL == pMesh || NULL == pMesh->m_pdeHead)
	{
		return NULL;
	}
	// Only the software path, for the reason ShadeSkinnedMesh() gives.
	if(SOFTWARE != pMesh->GetMethod())
	{
		return NULL;
	}

	// The transformer card scales the model it turns you into, and the main pass
	// folds that into the world matrix rather than into the mesh -
	// UnitRender.cpp:190.
	if(0 != i_pUnit->GetMonsterTransformer() && i_pUnit->GetMonsterTransScale() > 0.0f)
	{
		const float fScale = i_pUnit->GetMonsterTransScale();
		D3DXMatrixScaling(o_pWorld, fScale, fScale, fScale);
		D3DXMatrixMultiply(o_pWorld, o_pWorld, &i_pUnit->m_mMatrix);
	}
	else
	{
		*o_pWorld = i_pUnit->m_mMatrix;
	}

	*o_pPoseTime = i_pUnit->m_fCurrentTime;
	return pMesh;
}

// One skinned model into the map.
void CShadowMap::PoseSkinnedCaster(CSkinnedMesh* i_pMesh, const D3DXMATRIX& i_matWorld,
								   float i_fPoseTime)
{
	if(NULL == i_pMesh)
	{
		return;
	}

	// Pose every node, not just the ones the camera can see.
	const BOOL bWasDetailDrawFrame = g_bDetailDrawFrame;
	g_bDetailDrawFrame = FALSE;

	i_pMesh->Tick(i_fPoseTime);
	i_pMesh->SetWorldMatrix(i_matWorld);

	for(SDrawElement* pde = i_pMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
	{
		i_pMesh->UpdateFrames(pde->pframeRoot, i_matWorld);
	}

	// And blend the bones into the vertex buffer, which drawing would normally
	// have done on the way past.
	i_pMesh->UpdateSkinnedVertices();

	g_bDetailDrawFrame = bWasDetailDrawFrame;
}

// The draw, on its own, so that a model going into two cascades is posed and
// skinned once and drawn twice.
void CShadowMap::DrawSkinnedCaster(CSkinnedMesh* i_pMesh)
{
	if(NULL == i_pMesh)
	{
		return;
	}

	for(SDrawElement* pde = i_pMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
	{
		DrawSkinnedFrames(pde->pframeRoot, i_pMesh, &m_nSkinnedCasterDraws);
	}

	m_nSkinnedCasters++;
}

// A model the harness stood in the scene, offered by hand because it is in none
// of the game's lists.
void CShadowMap::AddExtraCaster(CSkinnedMesh* i_pMesh, const D3DXMATRIX& i_matWorld,
								float i_fPoseTime)
{
	if(NULL == i_pMesh || m_nExtraCasters >= MAX_EXTRA_CASTERS)
	{
		return;
	}

	m_arrExtraCaster[m_nExtraCasters].pMesh     = i_pMesh;
	m_arrExtraCaster[m_nExtraCasters].matWorld  = i_matWorld;
	m_arrExtraCaster[m_nExtraCasters].fPoseTime = i_fPoseTime;
	m_nExtraCasters++;
}
// One cascade's quarter of the atlas, as a viewport.
BOOL CShadowMap::SetCascadeViewport(int i_nCascade)
{
	int nTileX = 0, nTileY = 0;
	CascadeTile(i_nCascade, &nTileX, &nTileY);

	D3DVIEWPORT9 tile;
	tile.X      = (DWORD)nTileX;
	tile.Y      = (DWORD)nTileY;
	tile.Width  = (DWORD)m_nTileSize;
	tile.Height = (DWORD)m_nTileSize;
	tile.MinZ   = 0.0f;
	tile.MaxZ   = 1.0f;
	return SUCCEEDED(g_pD3dDev->SetViewport(&tile));
}


// Ships, characters and monsters, into the map.
//
// One list walk and not three.
void CShadowMap::RenderSkinnedCasters()
{
	m_nSkinnedCasters = 0;
	m_nSkinnedCasterDraws = 0;

	// Experiment 6: nothing skinned casts, so any shadow left in the map is the
	// terrain's or a static object's.  The mirror of experiment 5.
	if(SHADOW_EXPERIMENT_NO_SKINNED_CAST == g_nShadowExperiment)
	{
		return;
	}
	if(NULL == m_pEffect || NULL == g_pD3dApp)
	{
		return;
	}

	const int nUnits = (NULL != g_pScene) ? (int)g_pScene->m_vecUnitRenderList.size() : 0;
	const int nTotal = nUnits + 1 + m_nExtraCasters;

	// Begun on the first caster that survives the cull, so a scene with nobody in
	// it costs no effect Begin at all - the same shape as RenderObjectCasters().
	BOOL bBegun = FALSE;

	for(int n = 0; n < nTotal; n++)
	{
		CSkinnedMesh* pMesh     = NULL;
		float         fPoseTime = 0.0f;
		D3DXMATRIX    matWorld;
		D3DXMatrixIdentity(&matWorld);

		// The one model the focus map is drawing is left out of the cascades.
		//
		// It has to be left out, not merely duplicated: the receiver takes the
		// darker of the cascade and the focus answers, so a blurry copy of the same
		// shadow in the cascades would show through as a halo round the sharp one
		// wherever it reached further.
		if(IsFocusCaster(n, nUnits, (n < nUnits) ? g_pScene->m_vecUnitRenderList[n] : NULL))
		{
			continue;
		}

		if(n < nUnits)
		{
			CUnitData* pUnit = g_pScene->m_vecUnitRenderList[n];
			pMesh = FindUnitMesh(pUnit, &matWorld, &fPoseTime);
			if(NULL == pMesh)
			{
				continue;
			}
			if(!IsSphereLit(pUnit->m_vPos, pMesh->m_fRadius))
			{
				continue;
			}
		}
		else if(n == nUnits)
		{
			// The player, on foot.  Their ship is in the list above; while they
			// are walking round a city it is not drawn at all and this is.
			if(!g_pD3dApp->m_bCharacter || NULL == g_pCharacterChild ||
			   NULL == g_pD3dApp->m_pCharacterRender)
			{
				continue;
			}

			map<int, CSkinnedMesh*>::iterator it =
				g_pD3dApp->m_pCharacterRender->m_mapSkinnedMesh.find(g_pCharacterChild->m_nUnitNum);
			if(it == g_pD3dApp->m_pCharacterRender->m_mapSkinnedMesh.end())
			{
				continue;
			}

			pMesh = it->second;
			if(NULL == pMesh || NULL == pMesh->m_pdeHead || SOFTWARE != pMesh->GetMethod())
			{
				continue;
			}

			matWorld  = g_pCharacterChild->m_mMatrix;
			fPoseTime = g_pCharacterChild->m_fCurrentTime;

			if(!IsSphereLit(g_pCharacterChild->m_vPos, pMesh->m_fRadius))
			{
				continue;
			}
		}
		else
		{
			const SExtraCaster& extra = m_arrExtraCaster[n - nUnits - 1];
			if(NULL == extra.pMesh || NULL == extra.pMesh->m_pdeHead ||
			   SOFTWARE != extra.pMesh->GetMethod())
			{
				continue;
			}

			pMesh     = extra.pMesh;
			matWorld  = extra.matWorld;
			fPoseTime = extra.fPoseTime;

			// Not culled.  A probe stands its models on purpose, and a harness
			// run that silently dropped one would measure the wrong thing.
		}

		if(!bBegun)
		{
			m_pEffect->SetTechnique(m_hTechCaster);
			m_pEffect->SetMatrix(m_hCasterViewProj, &m_matLightViewProj);

			UINT nPasses = 0;
			if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
			{
				return;
			}
			if(FAILED(m_pEffect->BeginPass(0)))
			{
				m_pEffect->End();
				return;
			}
			bBegun = TRUE;
		}

		// Posed and skinned once, then drawn into each cascade it belongs in.
		PoseSkinnedCaster(pMesh, matWorld, fPoseTime);

		for(int nC = 0; nC < m_nSkinnedCascades; nC++)
		{
			if(!SetCascadeViewport(nC))
			{
				continue;
			}
			SelectCascade(nC);
			m_pEffect->SetMatrix(m_hCasterViewProj, &m_matLightViewProj);
			m_pEffect->CommitChanges();
			DrawSkinnedCaster(pMesh);
		}
	}
	// Counted apart so a dump can say whether the walk drew anything, then
	// folded in so the report line still says what the whole pass cost.
	m_nCasterDraws += m_nSkinnedCasterDraws;


	if(bBegun)
	{
		m_pEffect->EndPass();
		m_pEffect->End();
	}
}

// Everything that overlaps the focus box, into the focus map.
//
// A second walk of the same two lists, with the focus box selected.
void CShadowMap::RenderFocusCasters()
{
	m_nFocusSkinned = 0;

	if(NULL == m_pEffect || NULL == m_pFocusSurface || NULL == m_pFocusDepth ||
	   NULL == g_pD3dDev || NULL == m_pFocusMesh)
	{
		return;
	}

	// The depth surface comes off first - the sizes differ from the atlas's and a
	// mismatched pair is refused.
	g_pD3dDev->SetDepthStencilSurface(NULL);
	if(FAILED(g_pD3dDev->SetRenderTarget(0, m_pFocusSurface)) ||
	   FAILED(g_pD3dDev->SetDepthStencilSurface(m_pFocusDepth)))
	{
		return;
	}

	D3DVIEWPORT9 view;
	view.X      = 0;
	view.Y      = 0;
	view.Width  = (DWORD)m_nFocusSize;
	view.Height = (DWORD)m_nFocusSize;
	view.MinZ   = 0.0f;
	view.MaxZ   = 1.0f;
	if(FAILED(g_pD3dDev->SetViewport(&view)))
	{
		return;
	}

	// White is the far plane, so everything this pass does not draw on reads
	// back as nothing in the way - which for this map is almost all of it, and
	// is exactly right: the receiver takes the darker of this and the cascade,
	// so "nothing here" means "whatever the cascades said".
	g_pD3dDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					 D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

	SelectCascade(SHADOW_FOCUS_CASCADE);

	m_pEffect->SetTechnique(m_hTechCaster);
	m_pEffect->SetMatrix(m_hCasterViewProj, &m_matLightViewProj);

	UINT nPasses = 0;
	if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
	{
		return;
	}
	if(FAILED(m_pEffect->BeginPass(0)))
	{
		m_pEffect->End();
		return;
	}

	// Posed here and not reused from the cascade pass, because this model is
	// deliberately not in the cascade pass at all - see IsFocusCaster().
	PoseSkinnedCaster(m_pFocusMesh, m_matFocusWorld, m_fFocusPoseTime);
	m_pEffect->CommitChanges();
	DrawSkinnedCaster(m_pFocusMesh);
	m_nFocusSkinned = 1;

	m_pEffect->EndPass();
	m_pEffect->End();
}

// The objects, shaded. Called once CSceneData::Render() has drawn them, and it
// walks the list it drew from rather than anything queued on the way past.
void CShadowMap::FlushObjectReceivers()
{
	// Experiment 2: only the ground receives, so the object path is out of the
	// picture entirely.
	if(SHADOW_EXPERIMENT_NO_OBJ_RECEIVER == g_nShadowExperiment)
	{
		return;
	}

	if(!IsReceiving() || NULL == m_pEffect || NULL == g_pScene)
	{
		return;
	}
	if(g_pScene->m_vectorCulledObjectPtrList.empty())
	{
		return;
	}
	if(!SetReceiverConstants())
	{
		return;
	}

	m_bShadingPass = TRUE;
	m_nReceiverObjects = 0;
	m_nReceiverPoseSum = 2166136261u;
	m_nReceiverDrawsWas = m_nReceiverDraws;

	for(int nGroup = 0; nGroup < 2; nGroup++)
	{
		const BOOL bAlpha = (1 == nGroup);

		int nDrawn = 0;
		vectorCObjectChildPtr::iterator it = g_pScene->m_vectorCulledObjectPtrList.begin();
		for(; it != g_pScene->m_vectorCulledObjectPtrList.end(); ++it)
		{
			CObjectChild* pObject = *it;
			if(NULL == pObject || NULL == pObject->m_pObjectInfo)
			{
				continue;
			}
			if((0 != pObject->m_pObjectInfo->Alpha) != (0 != bAlpha))
			{
				continue;
			}

			if(0 == nDrawn)
			{
				m_pEffect->SetTechnique(ReceiverTechnique(bAlpha));

				UINT nPasses = 0;
				if(FAILED(m_pEffect->Begin(&nPasses, 0)) || 0 == nPasses)
				{
					m_bShadingPass = FALSE;
					return;
				}
				if(FAILED(m_pEffect->BeginPass(0)))
				{
					m_pEffect->End();
					m_bShadingPass = FALSE;
					return;
				}
				// Mode three drops the depth test on the shading pass.
				if(NULL != g_pD3dApp && 3 == g_pD3dApp->m_nShadowMapMode)
				{
					g_pD3dDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
				}
			}
			nDrawn++;
			m_nReceiverObjects++;

			if(NULL == PoseObject(pObject))
			{
				continue;
			}

			LPDIRECT3DTEXTURE9 pBound = NULL;
			for(SDrawElement* pde = pObject->m_pObjMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
			{
				DrawPosedHierarchy(pde->pframeRoot, &pBound, bAlpha, &m_nReceiverDraws,
								   &pObject->m_mMatrix, pObject->m_pObjMesh, m_bAnimatedObjects);
			}
			pObject->m_pObjMesh->SetSkinInstance(NULL);
		}

		if(nDrawn > 0)
		{
			m_pEffect->EndPass();
			m_pEffect->End();
		}
	}

	m_bShadingPass = FALSE;

	g_pD3dDev->SetTexture(0, NULL);
}

//////////////////////////////////////////////////////////////////////
//  The dump
//////////////////////////////////////////////////////////////////////

// How many frames a dump request writes.  A shadow that flickers is two
// frames disagreeing, and one dump cannot show a disagreement.
#define SHADOW_DUMP_BURST		4

void CShadowMap::RequestDump()
{
	// A burst rather than a single frame.
	//
	// A shadow that flickers is two frames disagreeing, and one dump cannot show
	// a disagreement.
	m_nDumpsWanted = SHADOW_DUMP_BURST;
}

// The float data down into system memory, and the picture built out of it here
// rather than on the card.
void CShadowMap::CapturePendingFrame()
{
	if(!m_bScreenPending)
	{
		return;
	}
	m_bScreenPending = FALSE;

	if(WriteDumpScreen(m_szPendingScreen))
	{
		ShadowSay("ShadowMap: frame %s saved\n", m_szPendingScreen);
	}
	else
	{
		ShadowSay("ShadowMap: the frame would not save to %s\n", m_szPendingScreen);
	}
}

BOOL CShadowMap::WriteDumpScreen(const char* i_szPath)
{
	if(NULL == g_pD3dApp)
	{
		return FALSE;
	}

	const D3DSURFACE_DESC desc = g_pD3dApp->GetBackBufferDesc();
	if(desc.Width < 1 || desc.Height < 1)
	{
		return FALSE;
	}

	LPDIRECT3DSURFACE9 pBack     = NULL;
	LPDIRECT3DSURFACE9 pResolved = NULL;
	LPDIRECT3DSURFACE9 pSysMem   = NULL;
	BOOL               bOk       = FALSE;

	if(SUCCEEDED(g_pD3dDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBack)) &&
	   SUCCEEDED(g_pD3dDev->CreateRenderTarget(desc.Width, desc.Height, desc.Format,
											   D3DMULTISAMPLE_NONE, 0, FALSE,
											   &pResolved, NULL)) &&
	   SUCCEEDED(g_pD3dDev->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format,
														D3DPOOL_SYSTEMMEM, &pSysMem, NULL)))
	{
		// StretchRect from a multisampled surface to a plain one is the resolve.
		if(SUCCEEDED(g_pD3dDev->StretchRect(pBack, NULL, pResolved, NULL, D3DTEXF_NONE)) &&
		   SUCCEEDED(g_pD3dDev->GetRenderTargetData(pResolved, pSysMem)))
		{
			bOk = SUCCEEDED(D3DXSaveSurfaceToFileA(i_szPath, D3DXIFF_PNG, pSysMem, NULL, NULL));
		}
	}

	SAFE_RELEASE(pSysMem);
	SAFE_RELEASE(pResolved);
	SAFE_RELEASE(pBack);
	return bOk;
}

BOOL CShadowMap::WriteDumpImage(const char* i_szPath)
{
	return WriteDumpImageOf(i_szPath, m_pShadowSurface, m_nMapSize);
}

// The picture of one map, whichever it is.
BOOL CShadowMap::WriteDumpImageOf(const char* i_szPath, LPDIRECT3DSURFACE9 i_pSurface,
								  int i_nSize)
{
	m_bDumpStatsValid = FALSE;

	if(NULL == i_pSurface || i_nSize <= 0)
	{
		return FALSE;
	}

	LPDIRECT3DSURFACE9 pDepth = NULL;	// R32F, system memory
	LPDIRECT3DSURFACE9 pImage = NULL;	// A8R8G8B8, what gets saved

	if(FAILED(g_pD3dDev->CreateOffscreenPlainSurface(i_nSize, i_nSize,
													 D3DFMT_R32F, D3DPOOL_SYSTEMMEM,
													 &pDepth, NULL)))
	{
		ShadowSay("ShadowMap: no system memory R32F surface, cannot dump the image\n");
		return FALSE;
	}
	if(FAILED(g_pD3dDev->GetRenderTargetData(i_pSurface, pDepth)))
	{
		ShadowSay("ShadowMap: the map would not copy down to system memory\n");
		SAFE_RELEASE(pDepth);
		return FALSE;
	}
	if(FAILED(g_pD3dDev->CreateOffscreenPlainSurface(i_nSize, i_nSize,
													 D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH,
													 &pImage, NULL)))
	{
		SAFE_RELEASE(pDepth);
		return FALSE;
	}

	D3DLOCKED_RECT lockedDepth;
	D3DLOCKED_RECT lockedImage;
	if(FAILED(pDepth->LockRect(&lockedDepth, NULL, D3DLOCK_READONLY)) )
	{
		SAFE_RELEASE(pImage);
		SAFE_RELEASE(pDepth);
		return FALSE;
	}

	// Pass one: what is in there.  Anything at or above the clear value was
	// never drawn on.
	const float FAR_ENOUGH = 0.9999f;

	float    fMin   = 1.0f;
	float    fMax   = 0.0f;
	double   fSum   = 0.0;
	__int64  nDrawn = 0;

	for(int nY = 0; nY < i_nSize; nY++)
	{
		const float* pRow = (const float*)((const BYTE*)lockedDepth.pBits + nY * lockedDepth.Pitch);
		for(int nX = 0; nX < i_nSize; nX++)
		{
			const float f = pRow[nX];
			if(f >= FAR_ENOUGH)	{ continue; }

			if(f < fMin)	{ fMin = f; }
			if(f > fMax)	{ fMax = f; }
			fSum += (double)f;
			nDrawn++;
		}
	}

	const __int64 nTexels = (__int64)i_nSize * (__int64)i_nSize;
	m_fDumpDepthMin   = (nDrawn > 0) ? fMin : 0.0f;
	m_fDumpDepthMax   = (nDrawn > 0) ? fMax : 0.0f;
	m_fDumpDepthMean  = (nDrawn > 0) ? fSum / (double)nDrawn : 0.0;
	m_fDumpUndrawn    = 1.0 - ((double)nDrawn / (double)nTexels);
	m_bDumpStatsValid = TRUE;

	// Pass two: the picture, stretched across whatever pass one found.
	BOOL bOk = FALSE;
	if(SUCCEEDED(pImage->LockRect(&lockedImage, NULL, 0)))
	{
		const float fSpan  = (fMax > fMin) ? (fMax - fMin) : 1.0f;
		const float fScale = 255.0f / fSpan;

		for(int nY = 0; nY < i_nSize; nY++)
		{
			const float* pRow = (const float*)((const BYTE*)lockedDepth.pBits + nY * lockedDepth.Pitch);
			DWORD*       pOut = (DWORD*)((BYTE*)lockedImage.pBits + nY * lockedImage.Pitch);

			for(int nX = 0; nX < i_nSize; nX++)
			{
				const float f = pRow[nX];
				DWORD       n = 255;

				if(f < FAR_ENOUGH)
				{
					float fLevel = (f - fMin) * fScale;
					if(fLevel < 0.0f)		{ fLevel = 0.0f; }
					if(fLevel > 255.0f)		{ fLevel = 255.0f; }
					n = (DWORD)fLevel;
				}
				pOut[nX] = 0xFF000000 | (n << 16) | (n << 8) | n;
			}
		}
		pImage->UnlockRect();
		bOk = SUCCEEDED(D3DXSaveSurfaceToFileA(i_szPath, D3DXIFF_PNG, pImage, NULL, NULL));
	}

	pDepth->UnlockRect();
	SAFE_RELEASE(pImage);
	SAFE_RELEASE(pDepth);
	return bOk;
}

// What the caster pass would actually submit for one object, counted the way
// DrawPosedHierarchy() decides it.
struct SCasterCensus
{
	int nSkinned;		// containers the caster skips for holding skin info
	int nSubsets;		// attribute groups in the rest
	int nNoTexture;		// of those, the ones with no texture of their own
};

static void CountCasterSubsets(SFrame* i_pFrame, SCasterCensus* io_pCensus)
{
	if(NULL == i_pFrame)
	{
		return;
	}

	if(NULL != i_pFrame->pmcMesh)
	{
		for(SMeshContainer* pmc = i_pFrame->pmcMesh; NULL != pmc; pmc = pmc->pmcNext)
		{
			if(NULL == pmc->pMesh || NULL != pmc->m_pSkinMeshInfo)
			{
				io_pCensus->nSkinned++;
				continue;
			}
			for(DWORD nAttr = 0; nAttr < pmc->cpattr; nAttr++)
			{
				io_pCensus->nSubsets++;
				if(NULL == pmc->pTextures || NULL == pmc->pTextures[nAttr])
				{
					io_pCensus->nNoTexture++;
				}
			}
		}
	}

	for(SFrame* pChild = i_pFrame->pframeFirstChild; NULL != pChild; pChild = pChild->pframeSibling)
	{
		CountCasterSubsets(pChild, io_pCensus);
	}
}

void CShadowMap::WriteDumpText(const char* i_szPath)
{
	FILE* pf = NULL;
	if(0 != fopen_s(&pf, i_szPath, "w") || NULL == pf)
	{
		return;
	}

	const int nMapIndex = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;

	fprintf(pf, "shadow map dump %d\n", m_nDumpsWritten);
	// Which SHADOW_EXPERIMENT_* probe produced this, so that a folder of
	// dumps cannot be read as if it came from an ordinary frame.
	fprintf(pf, "experiment               %d\n", g_nShadowExperiment);
	fprintf(pf, "map index                %d\n", nMapIndex);
	// The terrain's own texture state, which is nothing to do with shadows and is
	// here because this is the one per-probe report the harness already writes.
	{
		DWORD dwOp = 0, dwArg1 = 0, dwArg2 = 0;
		LPDIRECT3DBASETEXTURE9 pStage1 = NULL;
		if(NULL != g_pD3dDev)
		{
			g_pD3dDev->GetTextureStageState(1, D3DTSS_COLOROP,   &dwOp);
			g_pD3dDev->GetTextureStageState(1, D3DTSS_COLORARG1, &dwArg1);
			g_pD3dDev->GetTextureStageState(1, D3DTSS_COLORARG2, &dwArg2);
			g_pD3dDev->GetTexture(1, &pStage1);
		}
		fprintf(pf, "terrain stage 1          texture %p, colorop %lu, arg1 %lu, arg2 %lu\n",
				(void*)pStage1, (unsigned long)dwOp,
				(unsigned long)dwArg1, (unsigned long)dwArg2);
		fprintf(pf, "terrain detail pointer   %p\n",
				(NULL != g_pGround) ? (void*)g_pGround->m_pDetailMap : NULL);
		SAFE_RELEASE(pStage1);
	}
	fprintf(pf, "terrain detail map       %s, block by block %d, low quality %d\n",
			(NULL != g_pGround && NULL != g_pGround->m_pDetailMap) ? "loaded" : "MISSING",
			(NULL != g_pGround) ? (g_pGround->m_bDetailMapBlockByBlock ? 1 : 0) : -1,
			(NULL != g_pSOption) ? (int)g_pSOption->sLowQuality : -1);
	fprintf(pf, "terrain light            ambient %.3f %.3f %.3f, diffuse %.3f %.3f %.3f\n",
			(NULL != g_pScene) ? g_pScene->m_light0.Ambient.r : -1.0f,
			(NULL != g_pScene) ? g_pScene->m_light0.Ambient.g : -1.0f,
			(NULL != g_pScene) ? g_pScene->m_light0.Ambient.b : -1.0f,
			(NULL != g_pScene) ? g_pScene->m_light0.Diffuse.r : -1.0f,
			(NULL != g_pScene) ? g_pScene->m_light0.Diffuse.g : -1.0f,
			(NULL != g_pScene) ? g_pScene->m_light0.Diffuse.b : -1.0f);
	fprintf(pf, "map size                 %d x %d tiles of %.1f\n",
			(NULL != g_pGround) ? g_pGround->m_projectInfo.sXSize : 0,
			(NULL != g_pGround) ? g_pGround->m_projectInfo.sYSize : 0,
			TILE_SIZE);
	fprintf(pf, "\n");

	// The sun.  Both of the map's own directions, so that the one that is not
	// in use can be checked without waiting for the clock to come round.
	MAP_INFO* pMapInfo = (NULL != g_pDatabase && nMapIndex >= 0)
					   ? g_pDatabase->GetMapInfo((MapIndex_t)nMapIndex) : NULL;
	// AVECTOR3 is three CoordUnit_t, which is a SHORT - so a map's sun direction
	// is three whole numbers, and the raw values are worth seeing next to what
	// GetMapDirection() makes of them.
	if(NULL != pMapInfo)
	{
		fprintf(pf, "MAP_INFO at              %p, sizeof %d, DaySunDirection at +%d\n",
				(void*)pMapInfo, (int)sizeof(MAP_INFO), (int)offsetof(MAP_INFO, DaySunDirection));
		fprintf(pf, "MAP_INFO DaySunDirection    %6d %6d %6d  (raw, whole numbers)\n",
				(int)pMapInfo->DaySunDirection.x, (int)pMapInfo->DaySunDirection.y,
				(int)pMapInfo->DaySunDirection.z);
		fprintf(pf, "MAP_INFO NightSunDirection  %6d %6d %6d\n",
				(int)pMapInfo->NightSunDirection.x, (int)pMapInfo->NightSunDirection.y,
				(int)pMapInfo->NightSunDirection.z);
	}
	else
	{
		fprintf(pf, "MAP_INFO                 not available\n");
	}

	// What the client's own two calls answer right now.
	if(nMapIndex >= 0)
	{
		const D3DXVECTOR3 vDay   = GetMapDirection((USHORT)nMapIndex, TRUE);
		const D3DXVECTOR3 vNight = GetMapDirection((USHORT)nMapIndex, FALSE);
		fprintf(pf, "GetMapDirection(day)        %8.4f %8.4f %8.4f\n", vDay.x, vDay.y, vDay.z);
		fprintf(pf, "GetMapDirection(night)      %8.4f %8.4f %8.4f\n", vNight.x, vNight.y, vNight.z);
	}
	if(NULL != g_pScene)
	{
		const D3DXVECTOR3 vNow = g_pScene->SetLightDirection();
		fprintf(pf, "SetLightDirection() now     %8.4f %8.4f %8.4f\n", vNow.x, vNow.y, vNow.z);
	}

	if(NULL != g_pScene)
	{
		fprintf(pf, "scene says it is         %s\n", g_pScene->m_bNight ? "night" : "day");
		fprintf(pf, "m_light0.Direction       %8.4f %8.4f %8.4f   (the way the light travels)\n",
				g_pScene->m_light0.Direction.x, g_pScene->m_light0.Direction.y,
				g_pScene->m_light0.Direction.z);
		fprintf(pf, "m_light0.Diffuse         %6.3f %6.3f %6.3f\n",
				g_pScene->m_light0.Diffuse.r, g_pScene->m_light0.Diffuse.g,
				g_pScene->m_light0.Diffuse.b);
		fprintf(pf, "m_light0.Ambient         %6.3f %6.3f %6.3f\n",
				g_pScene->m_light0.Ambient.r, g_pScene->m_light0.Ambient.g,
				g_pScene->m_light0.Ambient.b);
		fprintf(pf, "fog start / end          %.1f / %.1f\n",
				g_pScene->m_fFogStartValue, g_pScene->m_fFogEndValue);
	}
	fprintf(pf, "\n");

	if(NULL != g_pCamera)
	{
		const D3DXVECTOR3 vEye  = g_pCamera->GetEyePt();
		const D3DXVECTOR3 vLook = g_pCamera->GetLookatPt();
		fprintf(pf, "camera eye               %10.1f %10.1f %10.1f\n", vEye.x, vEye.y, vEye.z);
		fprintf(pf, "camera lookat            %10.1f %10.1f %10.1f\n", vLook.x, vLook.y, vLook.z);
	}
	fprintf(pf, "\n");

	fprintf(pf, "light direction used     %8.4f %8.4f %8.4f\n",
			m_vDumpLightDir.x, m_vDumpLightDir.y, m_vDumpLightDir.z);
	fprintf(pf, "box centre               %10.1f %10.1f %10.1f\n",
			m_vDumpCentre.x, m_vDumpCentre.y, m_vDumpCentre.z);
	fprintf(pf, "light eye                %10.1f %10.1f %10.1f\n",
			m_vDumpEye.x, m_vDumpEye.y, m_vDumpEye.z);
	fprintf(pf, "box half width           %.1f  (so %.1f across)\n",
			m_fDumpRadius, m_fDumpRadius * 2.0f);
	fprintf(pf, "near / far               %.1f / %.1f\n", m_fDumpNear, m_fDumpFar);
	fprintf(pf, "cascade                  %d of %d, covering %.0f to %.0f\n",
			m_nCascade, SHADOW_CASCADE_COUNT,
			m_arrCascade[m_nCascade].fSliceNear, m_arrCascade[m_nCascade].fSliceFar);
	// What a texel is worth in each cascade, and how many of them the filter
	// spans there.
	for(int nT = 0; nT <= SHADOW_FOCUS_CASCADE; nT++)
	{
		const SCascade& t = m_arrCascade[nT];
		const int   nSide  = (SHADOW_FOCUS_CASCADE == nT) ? m_nFocusSize : m_nTileSize;
		const float fTexel = (t.fRadius * 2.0f) / (float)nSide;
		if(!(fTexel > 0.0f))	{ continue; }

		// The focus map has its own width, so the report has to use it too -
		// otherwise this line says the cascades' number for a row that does
		// not use it, which is worse than not printing it at all.
		const float fWidth = (SHADOW_FOCUS_CASCADE == nT)
						   ? m_fFocusFilterWidth : m_fFilterWidth;
		float fKernel = fWidth / fTexel;
		if(fKernel < SHADOW_FILTER_MIN_TEXELS)	{ fKernel = SHADOW_FILTER_MIN_TEXELS; }
		if(fKernel > SHADOW_FILTER_MAX_TEXELS)	{ fKernel = SHADOW_FILTER_MAX_TEXELS; }

		fprintf(pf, "%-8s %d kernel        texel %.4f units, %.2f texels = %.3f units\n",
				(SHADOW_FOCUS_CASCADE == nT) ? "focus" : "cascade", nT,
				fTexel, fKernel, fKernel * fTexel);
	}
	fprintf(pf, "splits in force          %.0f / %.0f / %.0f / %.0f\n",
			m_arrSplit[0], m_arrSplit[1], m_arrSplit[2], m_arrSplit[3]);
	fprintf(pf, "world units per texel    %.3f   at %d square\n",
			(m_fDumpRadius * 2.0f) / (float)m_nTileSize, m_nTileSize);
	fprintf(pf, "ground height min / max  %.1f / %.1f\n",
			(NULL != g_pGround) ? g_pGround->m_projectInfo.fHeightMin : 0.0f,
			(NULL != g_pGround) ? g_pGround->m_projectInfo.fHeightMax : 0.0f);
	fprintf(pf, "\n");

	fprintf(pf, "light view-projection (row major, as D3D holds it)\n");
	for(int nRow = 0; nRow < 4; nRow++)
	{
		fprintf(pf, "  %12.6f %12.6f %12.6f %12.6f\n",
				m_matLightViewProj.m[nRow][0], m_matLightViewProj.m[nRow][1],
				m_matLightViewProj.m[nRow][2], m_matLightViewProj.m[nRow][3]);
	}
	fprintf(pf, "\n");

	// White in the image is the far plane, which is either ground a long way
	// below the sun or nothing drawn at all - so what got drawn matters.
	if(NULL != g_pScene && NULL != g_pCamera)
	{
		const D3DXVECTOR3 vEye = g_pCamera->GetEyePt();

		fprintf(pf, "\n");
		fprintf(pf, "nearest casters in the range list\n");
		fprintf(pf, "  %6s %8s %6s %5s %5s %5s %5s  %-12s %s\n",
				"code", "dist", "radius", "alpha", "sets", "notex", "skin",
				"cascades", "world position, then m_vOriPos");

		// A selection sort over the list, twelve times.
		float fLast = -1.0f;
		for(int nShown = 0; nShown < 12; nShown++)
		{
			CObjectChild* pBest = NULL;
			float         fBest = 1e30f;

			vectorCObjectChildPtr::iterator it = g_pScene->m_vectorRangeObjectPtrList.begin();
			for(; it != g_pScene->m_vectorRangeObjectPtrList.end(); ++it)
			{
				CObjectChild* pObj = *it;
				if(NULL == pObj || NULL == pObj->m_pObjectInfo || NULL == pObj->m_pObjMesh)
				{
					continue;
				}
				// The object's own world matrix.
				D3DXVECTOR3 vOff(pObj->m_mMatrix._41 - vEye.x,
								 pObj->m_mMatrix._42 - vEye.y,
								 pObj->m_mMatrix._43 - vEye.z);
				const float fD = D3DXVec3Length(&vOff);
				if(fD > fLast && fD < fBest)
				{
					fBest = fD;
					pBest = pObj;
				}
			}
			if(NULL == pBest)	{ break; }
			fLast = fBest;

			SCasterCensus census;
			memset(&census, 0, sizeof(census));
			for(SDrawElement* pde = pBest->m_pObjMesh->m_pdeHead; NULL != pde; pde = pde->pdeNext)
			{
				CountCasterSubsets(pde->pframeRoot, &census);
			}

			char szMask[8] = "....";
			for(int nC = 0; nC < SHADOW_CASCADE_COUNT; nC++)
			{
				SelectCascade(nC);
				if(IsSphereLit(pBest->m_vOriPos, pBest->m_pObjMesh->m_fRadius))
				{
					szMask[nC] = (char)('0' + nC);
				}
			}
			SelectCascade(0);

			fprintf(pf, "  %6d %8.1f %6.1f %5d %5d %5d %5d  %-12s "
						"%.0f %.0f %.0f   %.0f %.0f %.0f\n",
					pBest->m_nCode, fBest, pBest->m_pObjMesh->m_fRadius,
					(int)pBest->m_pObjectInfo->Alpha,
					census.nSubsets, census.nNoTexture, census.nSkinned, szMask,
					pBest->m_mMatrix._41, pBest->m_mMatrix._42, pBest->m_mMatrix._43,
					pBest->m_vOriPos.x, pBest->m_vOriPos.y, pBest->m_vOriPos.z);
		}
		fprintf(pf, "\n");
	}

	fprintf(pf, "object casters drawn     %d objects, %d poses, %I64u subsets, pose %08x\n",
			m_nCasterObjects, m_nCasterPoses,
			m_nCasterDraws - m_nCasterDrawsWas, m_nCasterPoseSum);
	fprintf(pf, "skinned casters drawn    %d models, %I64u subsets\n",
			m_nSkinnedCasters, m_nSkinnedCasterDraws);
	fprintf(pf, "animated object parts    %s\n",
			m_bAnimatedObjects ? "cast and receive" : "off - AnimatedObjects = 0");

	// What the per-instance blend buffers are costing in memory, which is the
	// price of blending each object once a frame instead of once a pass.
	if(NULL != g_pScene)
	{
		int   nWith  = 0;
		DWORD dwBytes = 0;
		vectorCObjectChildPtr::iterator it = g_pScene->m_vectorRangeObjectPtrList.begin();
		for(; it != g_pScene->m_vectorRangeObjectPtrList.end(); ++it)
		{
			CObjectChild* pObj = *it;
			if(NULL == pObj || NULL == pObj->m_pSkinInstance)
			{
				continue;
			}
			nWith++;
			dwBytes += pObj->m_pSkinInstance->Bytes();
		}
		fprintf(pf, "  own blend buffers      %d objects in range hold %.1f KB\n",
				nWith, (double)dwBytes / 1024.0);
	}
	if(m_bFocusThisFrame)
	{
		const SCascade& f = m_arrCascade[SHADOW_FOCUS_CASCADE];
		fprintf(pf, "focus map                %d square, half width %.2f, texel %.4f world units\n",
				m_nFocusSize, f.fRadius, (f.fRadius * 2.0f) / (float)m_nFocusSize);
		fprintf(pf, "  centre                 %.1f %.1f %.1f, depth %.1f\n",
				f.vCentre.x, f.vCentre.y, f.vCentre.z, f.fFar - f.fNear);
		fprintf(pf, "  holds                  %d model (the player), and nothing else\n",
				m_nFocusSkinned);
		fprintf(pf, "  combined with           the cascades by min(), so everything\n");
		fprintf(pf, "                          round it keeps its cascade shadow\n");
	}
	else
	{
		fprintf(pf, "focus map                not drawn this frame (wanted %d)\n",
				m_bFocusWanted ? 1 : 0);
	}
	fprintf(pf, "object receivers shaded  %d objects, %I64u subsets, pose %08x\n",
			m_nReceiverObjects, m_nReceiverDraws - m_nReceiverDrawsWas, m_nReceiverPoseSum);
	fprintf(pf, "\n");
	fprintf(pf, "terrain blocks drawn     %d of %d that exist, %d triangles each\n",
			m_nBlocks, 16, m_nBlockTriangles);
	for(int n = 0; n < m_nBlocks; n++)
	{
		const CQuadGround* pBlock = m_arrBlocks[n];
		if(NULL == pBlock)
		{
			continue;
		}
		float fMinX = pBlock->m_vPos[0].x, fMaxX = fMinX;
		float fMinZ = pBlock->m_vPos[0].z, fMaxZ = fMinZ;
		for(int c = 1; c < 4; c++)
		{
			if(pBlock->m_vPos[c].x < fMinX)	{ fMinX = pBlock->m_vPos[c].x; }
			if(pBlock->m_vPos[c].x > fMaxX)	{ fMaxX = pBlock->m_vPos[c].x; }
			if(pBlock->m_vPos[c].z < fMinZ)	{ fMinZ = pBlock->m_vPos[c].z; }
			if(pBlock->m_vPos[c].z > fMaxZ)	{ fMaxZ = pBlock->m_vPos[c].z; }
		}
		fprintf(pf, "  block %2d  x %9.1f .. %9.1f   z %9.1f .. %9.1f\n",
				n, fMinX, fMaxX, fMinZ, fMaxZ);
	}
	fprintf(pf, "\n");

	// What the pass actually wrote.
	if(m_bDumpStatsValid)
	{
		fprintf(pf, "depth written            %.6f .. %.6f, mean %.6f\n",
				m_fDumpDepthMin, m_fDumpDepthMax, m_fDumpDepthMean);
		fprintf(pf, "  as a share of near..far  %.1f%% of the range is used\n",
				100.0 * (double)(m_fDumpDepthMax - m_fDumpDepthMin));
		fprintf(pf, "  in world units           %.1f of %.1f\n",
				(double)(m_fDumpDepthMax - m_fDumpDepthMin) * (m_fDumpFar - m_fDumpNear),
				m_fDumpFar - m_fDumpNear);
		fprintf(pf, "texels never drawn on    %.1f%%\n", 100.0 * m_fDumpUndrawn);
		fprintf(pf, "\n");
	}

	fprintf(pf, "reading the .png\n");
	fprintf(pf, "  the picture is stretched across the depth range above, or the\n");
	fprintf(pf, "  terrain would be lost in a couple of percent of a flat grey\n");
	fprintf(pf, "  field.  Texels never drawn on are left pure white, outside the\n");
	fprintf(pf, "  stretch, so empty is not mistaken for far.\n");
	fprintf(pf, "  black is close to the sun, white is far from it or was never\n");
	fprintf(pf, "  drawn at all - the target is cleared to the far plane.  A dip\n");
	fprintf(pf, "  in the ground is further from the sun than the ground round it\n");
	fprintf(pf, "  and so is lighter, which is right and not a sign flip.  A large\n");
	fprintf(pf, "  flat white area with a straight edge is the box reaching past\n");
	fprintf(pf, "  the terrain, or a block that should have been drawn and was not.\n");

	fclose(pf);
}

void CShadowMap::WriteDump()
{
	if(NULL == m_pShadowTexture || NULL == m_pEffect)
	{
		return;
	}

	ShadowMakeDirectory(m_szDumpDirectory);

	const int nMapIndex = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;

	char szPng[MAX_PATH];
	char szTxt[MAX_PATH];
	_snprintf_s(szPng, sizeof(szPng), _TRUNCATE, "%s\\%s%04d-%03d.png",
				m_szDumpDirectory, m_szDumpPrefix, nMapIndex, m_nDumpsWritten);
	_snprintf_s(szTxt, sizeof(szTxt), _TRUNCATE, "%s\\%s%04d-%03d.txt",
				m_szDumpDirectory, m_szDumpPrefix, nMapIndex, m_nDumpsWritten);

	char szScreen[MAX_PATH];
	_snprintf_s(szScreen, sizeof(szScreen), _TRUNCATE, "%s\\%s%04d-%03d-screen.png",
				m_szDumpDirectory, m_szDumpPrefix, nMapIndex, m_nDumpsWritten);
	strncpy_s(m_szPendingScreen, sizeof(m_szPendingScreen), szScreen, _TRUNCATE);
	m_bScreenPending = TRUE;
	const BOOL bImage = WriteDumpImage(szPng);

	// And the focus map beside it when there is one.
	if(m_bFocusThisFrame && NULL != m_pFocusSurface)
	{
		char szFocus[MAX_PATH];
		_snprintf_s(szFocus, sizeof(szFocus), _TRUNCATE, "%s\\%s%04d-%03d-focus.png",
					m_szDumpDirectory, m_szDumpPrefix, nMapIndex, m_nDumpsWritten);
		WriteDumpImageOf(szFocus, m_pFocusSurface, m_nFocusSize);
	}

	WriteDumpText(szTxt);

	ShadowSay("ShadowMap: dump %d written to %s (%s)\n",
			  m_nDumpsWritten, m_szDumpDirectory,
			  bImage ? "map and text, frame to follow at end of frame" : "text only");

	m_nDumpsWritten++;
}

void CShadowMap::WriteDumpFrameOnly()
{
	ShadowMakeDirectory(m_szDumpDirectory);

	const int nMapIndex = (NULL != g_pShuttleChild)
						? (int)g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex : -1;

	char szScreen[MAX_PATH];
	_snprintf_s(szScreen, sizeof(szScreen), _TRUNCATE, "%s\\%s%04d-%03d-screen.png",
				m_szDumpDirectory, m_szDumpPrefix, nMapIndex, m_nDumpsWritten);

	// Straight away rather than through m_bScreenPending: this is already being
	// called from the far end of the frame, which is the only reason the
	// pending flag exists.
	if(WriteDumpScreen(szScreen))
	{
		ShadowSay("ShadowMap: frame %s saved (no light pass this frame)\n", szScreen);
	}

	m_nDumpsWritten++;
}

//////////////////////////////////////////////////////////////////////
//  What it costs
//////////////////////////////////////////////////////////////////////

// A line a minute next to the one CD3DFilteredDevice::ReportOccasionally()
// writes, so that the light pass can be read off without having to run the
// client twice with shadows on and off.
void CShadowMap::ReportOccasionally()
{
	if(FALSE == g_bAtumDebugInfo)
	{
		return;
	}

	const DWORD SIZE_REPORT_INTERVAL_MS = 60 * 1000;

	static DWORD			s_dwNextReport = 0;
	static unsigned __int64	s_nWasPasses    = 0;
	static unsigned __int64	s_nWasDraws     = 0;
	static unsigned __int64	s_nWasTriangles = 0;
	static unsigned __int64	s_nWasReceivers = 0;

	const DWORD dwNow = GetTickCount();
	if(0 == s_dwNextReport)
	{
		s_dwNextReport = dwNow + SIZE_REPORT_INTERVAL_MS;
		return;
	}
	if((int)(dwNow - s_dwNextReport) < 0)
	{
		return;
	}

	const unsigned __int64 nPasses    = m_nPasses          - s_nWasPasses;
	const unsigned __int64 nDraws     = m_nCasterDraws     - s_nWasDraws;
	const unsigned __int64 nTriangles = m_nCasterTriangles - s_nWasTriangles;
	const unsigned __int64 nReceivers = m_nReceiverDraws   - s_nWasReceivers;

	char szLine[256];
	_snprintf_s(szLine, sizeof(szLine), _TRUNCATE,
				"Shadow: %d square, %.1f caster draws/frame, %.0f triangles/frame, "
				"%.1f receiver draws/frame, over %I64u frames\n",
				m_nMapSize,
				(nPasses > 0) ? (double)nDraws / (double)nPasses : 0.0,
				(nPasses > 0) ? (double)nTriangles / (double)nPasses : 0.0,
				(nPasses > 0) ? (double)nReceivers / (double)nPasses : 0.0,
				nPasses);
	OutputDebugStringA(szLine);

	// Where the object caster pass's CPU time goes.  Cumulative, divided by the
	// frames it ran over, in milliseconds.
	if(m_fUsPerTick <= 0.0)
	{
		m_fUsPerTick = ShadowUsPerTick();
	}
	_snprintf_s(szLine, sizeof(szLine), _TRUNCATE,
				"Shadow CPU: object casters pose %.3f ms/frame, submit %.3f ms/frame, "
				"%.1f poses/frame\n",
				(m_nPasses > 0) ? (m_fPoseUs * m_fUsPerTick / 1000.0 / (double)m_nPasses) : 0.0,
				(m_nPasses > 0) ? (m_fDrawUs * m_fUsPerTick / 1000.0 / (double)m_nPasses) : 0.0,
				(m_nPasses > 0) ? ((double)m_nPoseCalls / (double)m_nPasses) : 0.0);
	OutputDebugStringA(szLine);

	s_dwNextReport  = dwNow + SIZE_REPORT_INTERVAL_MS;
	s_nWasPasses    = m_nPasses;
	s_nWasDraws     = m_nCasterDraws;
	s_nWasTriangles = m_nCasterTriangles;
	s_nWasReceivers = m_nReceiverDraws;
}

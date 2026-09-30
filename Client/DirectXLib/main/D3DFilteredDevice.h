///////////////////////////////////////////////////////////////////////////////
//  D3DFilteredDevice.h : drop the D3D9 calls that would not change anything
//
//  A profile of the client in the field puts two thirds of its CPU inside
//  D3D9.dll and the display driver, against a tenth in the client's own code.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_D3D_FILTERED_DEVICE_H_
#define _ATUM_D3D_FILTERED_DEVICE_H_

#include <windows.h>
#include <d3d9.h>

// Shadow.ini's DebugInfo, read by CShadowMap::LoadSettings().  With it off, and
// off is the default, none of the periodic measurement reporting reaches the
// debugger and no log file is written.
extern BOOL g_bAtumDebugInfo;

// The id Wrap() looks for to tell a wrapped device from a bare one.  It is
// ours, not a D3D interface, and nothing outside this file should ask for it.
class __declspec(uuid("83ABEDB4-8D02-4405-AA42-D2E98F8171E8")) CD3DFilteredDevice : public IDirect3DDevice9
{
public:
	// Takes over the caller's reference to i_pDevice and answers the device to
	// use in its place.
	static IDirect3DDevice9 *Wrap(IDirect3DDevice9 *i_pDevice);

	// What has gone through since the process started: state calls made, state
	// calls that reached the device, draw calls, and frames.
	static void GetCounters(unsigned __int64 *o_pnSeen, unsigned __int64 *o_pnPassed,
							unsigned __int64 *o_pnDraws, unsigned __int64 *o_pnFrames);

	///////////////////////////////////////////////////////////////////////////
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	///////////////////////////////////////////////////////////////////////////
	// the calls this exists for
	STDMETHOD(SetRenderState)(D3DRENDERSTATETYPE State, DWORD Value);
	STDMETHOD(GetRenderState)(D3DRENDERSTATETYPE State, DWORD *pValue);
	STDMETHOD(SetTextureStageState)(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	STDMETHOD(GetTextureStageState)(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue);
	STDMETHOD(SetSamplerState)(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	STDMETHOD(SetTexture)(DWORD Stage, IDirect3DBaseTexture9 *pTexture);
	STDMETHOD(GetTexture)(DWORD Stage, IDirect3DBaseTexture9 **ppTexture);
	STDMETHOD(SetMaterial)(CONST D3DMATERIAL9 *pMaterial);
	STDMETHOD(GetMaterial)(D3DMATERIAL9 *pMaterial);
	STDMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
	STDMETHOD(GetTransform)(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix);
	STDMETHOD(MultiplyTransform)(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);

	///////////////////////////////////////////////////////////////////////////
	// counted, so that a frame can be described rather than guessed at
	STDMETHOD(Present)(CONST RECT *pSourceRect, CONST RECT *pDestRect,
					   HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion);
	STDMETHOD(DrawPrimitive)(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
							 UINT PrimitiveCount);
	STDMETHOD(DrawIndexedPrimitive)(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex,
									UINT MinVertexIndex, UINT NumVertices,
									UINT startIndex, UINT primCount);
	STDMETHOD(DrawPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
							   CONST void *pVertexStreamZeroData,
							   UINT VertexStreamZeroStride);
	STDMETHOD(DrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex,
									  UINT NumVertices, UINT PrimitiveCount,
									  CONST void *pIndexData, D3DFORMAT IndexDataFormat,
									  CONST void *pVertexStreamZeroData,
									  UINT VertexStreamZeroStride);

	///////////////////////////////////////////////////////////////////////////
	// the ones that move the state out from under the shadow
	STDMETHOD(Reset)(D3DPRESENT_PARAMETERS *pPresentationParameters);
	STDMETHOD(CreateStateBlock)(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB);
	STDMETHOD(BeginStateBlock)();
	STDMETHOD(EndStateBlock)(IDirect3DStateBlock9 **ppSB);

	///////////////////////////////////////////////////////////////////////////
	// everything else, passed straight through.
	///////////////////////////////////////////////////////////////////////////
#include "D3DFilteredDevice.inl"

	// Throws the shadow away.  Called from the wrapped state blocks.
	void Invalidate();

private:
	explicit CD3DFilteredDevice(IDirect3DDevice9 *i_pDevice);
	~CD3DFilteredDevice();
	CD3DFilteredDevice(const CD3DFilteredDevice &);
	CD3DFilteredDevice &operator=(const CD3DFilteredDevice &);

	enum
	{
		COUNT_RENDER_STATE		= 256,		// D3DRS_BLENDOPALPHA is 209
		COUNT_STAGE				= 8,		// the fixed function stages
		COUNT_STAGE_STATE		= 33,		// D3DTSS_CONSTANT is 32
		COUNT_SAMPLER			= 16,
		COUNT_SAMPLER_STATE		= 14,		// D3DSAMP_DMAPOFFSET is 13
		COUNT_TEXTURE			= 16,
		COUNT_WORLD_TRANSFORM	= 8,
		COUNT_TRANSFORM			= 10 + COUNT_WORLD_TRANSFORM
	};

	// TRUE when this call may be answered from the shadow at all: the owning
	// thread, and no state block being recorded.
	BOOL CanFilter();

	// D3DTS_VIEW, D3DTS_PROJECTION, D3DTS_TEXTUREn and D3DTS_WORLDMATRIX(n)
	// packed into one array, or -1 for a transform that is not shadowed.
	static int TransformSlot(D3DTRANSFORMSTATETYPE i_state);

	IDirect3DDevice9	*m_pDevice;
	LONG				m_nReferences;
	DWORD				m_dwOwnerThread;
	volatile LONG		m_nForeignWrite;	// another thread set state
	int					m_nRecording;		// state block being recorded

	DWORD	m_arrRenderState[COUNT_RENDER_STATE];
	BOOL	m_arrRenderStateKnown[COUNT_RENDER_STATE];

	DWORD	m_arrStageState[COUNT_STAGE][COUNT_STAGE_STATE];
	BOOL	m_arrStageStateKnown[COUNT_STAGE][COUNT_STAGE_STATE];

	DWORD	m_arrSamplerState[COUNT_SAMPLER][COUNT_SAMPLER_STATE];
	BOOL	m_arrSamplerStateKnown[COUNT_SAMPLER][COUNT_SAMPLER_STATE];

	// The device holds a reference to whatever is bound, so a bound texture
	// cannot be freed and something else take its address; comparing the
	// pointers is enough.
	IDirect3DBaseTexture9	*m_arrTexture[COUNT_TEXTURE];
	BOOL					m_arrTextureKnown[COUNT_TEXTURE];

	D3DMATERIAL9	m_material;
	BOOL			m_bMaterialKnown;

	D3DMATRIX	m_arrTransform[COUNT_TRANSFORM];
	BOOL		m_arrTransformKnown[COUNT_TRANSFORM];
};

#endif	// _ATUM_D3D_FILTERED_DEVICE_H_

///////////////////////////////////////////////////////////////////////////////
//  D3DFilteredDevice.cpp : see D3DFilteredDevice.h
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <d3d9.h>
#include <stdio.h>
#include <intrin.h>
#include "D3DFilteredDevice.h"

#pragma intrinsic(_ReturnAddress)

namespace
{
	unsigned __int64 g_nCallsSeen	= 0;
	unsigned __int64 g_nCallsPassed	= 0;
	unsigned __int64 g_nDraws		= 0;
	unsigned __int64 g_nFrames		= 0;

	///////////////////////////////////////////////////////////////////////////
	// Where the calls come from.
	//
	// Sixteen thousand state calls a frame is a number without an address on it,
	// and guessing which loop they belong to has been wrong twice.
	///////////////////////////////////////////////////////////////////////////

	const unsigned int	COUNT_SITE			= 256;		// power of two
	const unsigned int	SIZE_SAMPLE_EVERY	= 64;
	const int			COUNT_SITE_PROBE	= 8;

	struct SCallSite
	{
		unsigned __int64	nOffset;		// from the start of the module
		unsigned __int64	nCount;
	};

	SCallSite			g_arrStateSite[COUNT_SITE];
	SCallSite			g_arrDrawSite[COUNT_SITE];
	unsigned __int64	g_nModuleBase	= 0;
	unsigned __int64	g_nModuleEnd	= 0;

	// The client's own address range, read out of its PE header - no dependency
	// on psapi for one number.
	void FindModuleRange()
	{
		const HMODULE hModule = GetModuleHandle(NULL);
		if (NULL == hModule)
		{
			return;
		}
		const IMAGE_DOS_HEADER *pDos = (const IMAGE_DOS_HEADER*)hModule;
		if (IMAGE_DOS_SIGNATURE != pDos->e_magic)
		{
			return;
		}
		const IMAGE_NT_HEADERS *pNt =
			(const IMAGE_NT_HEADERS*)((const BYTE*)hModule + pDos->e_lfanew);
		if (IMAGE_NT_SIGNATURE != pNt->Signature)
		{
			return;
		}
		g_nModuleBase	= (unsigned __int64)hModule;
		g_nModuleEnd	= g_nModuleBase + pNt->OptionalHeader.SizeOfImage;
	}

	inline BOOL IsInClient(unsigned __int64 i_nAddress)
	{
		return (i_nAddress >= g_nModuleBase && i_nAddress < g_nModuleEnd) ? TRUE : FALSE;
	}

	void Tally(SCallSite *io_arrSite, unsigned __int64 i_nOffset)
	{
		unsigned int nSlot = (unsigned int)((i_nOffset * 2654435761u) % COUNT_SITE);
		for (int n = 0; n < COUNT_SITE_PROBE; n++)
		{
			SCallSite &site = io_arrSite[(nSlot + n) % COUNT_SITE];
			if (0 == site.nCount)
			{
				site.nOffset	= i_nOffset;
				site.nCount		= 1;
				return;
			}
			if (site.nOffset == i_nOffset)
			{
				site.nCount++;
				return;
			}
		}
		// This bucket is full of other sites; drop the sample rather than
		// evict one, since the ranking is what matters and it is only a sample.
	}

	// The client frame behind a call that came through D3DX: step past this
	// wrapper's own frames, past whatever D3DX did, and take the next one that
	// belongs to the client.
	unsigned __int64 ClientFrameBehindD3DX()
	{
		void *arrFrame[16];
		const USHORT nGot = CaptureStackBackTrace(1, 16, arrFrame, NULL);

		int n = 0;
		while (n < nGot && IsInClient((unsigned __int64)arrFrame[n]))	{ n++; }
		while (n < nGot && !IsInClient((unsigned __int64)arrFrame[n]))	{ n++; }
		return (n < nGot) ? (unsigned __int64)arrFrame[n] : 0;
	}

	void NoteStateSite(void *i_pReturnAddress)
	{
		static unsigned __int64 s_nTick = 0;
		if (0 != (++s_nTick % SIZE_SAMPLE_EVERY) || 0 == g_nModuleEnd)
		{
			return;
		}
		const unsigned __int64 nAt = (unsigned __int64)i_pReturnAddress;
		if (IsInClient(nAt))
		{
			Tally(g_arrStateSite, nAt - g_nModuleBase);
		}
	}

	void NoteDrawSite(void *i_pReturnAddress)
	{
		static unsigned __int64 s_nTick = 0;
		if (0 != (++s_nTick % SIZE_SAMPLE_EVERY) || 0 == g_nModuleEnd)
		{
			return;
		}
		unsigned __int64 nAt = (unsigned __int64)i_pReturnAddress;
		if (!IsInClient(nAt))
		{
			nAt = ClientFrameBehindD3DX();		// through DrawSubset, most likely
		}
		if (IsInClient(nAt))
		{
			Tally(g_arrDrawSite, nAt - g_nModuleBase);
		}
	}

	// The busiest sites, most first, written as offsets into the client -
	void AppendTopSites(char *o_szBuffer, int i_nSize, const SCallSite *i_arrSite, int i_nWanted)
	{
		SCallSite arrTop[8];
		memset(arrTop, 0x00, sizeof(arrTop));
		if (i_nWanted > 8)		{ i_nWanted = 8; }

		for (unsigned int n = 0; n < COUNT_SITE; n++)
		{
			if (0 == i_arrSite[n].nCount)	{ continue; }
			for (int k = 0; k < i_nWanted; k++)
			{
				if (i_arrSite[n].nCount > arrTop[k].nCount)
				{
					for (int j = i_nWanted - 1; j > k; j--)		{ arrTop[j] = arrTop[j - 1]; }
					arrTop[k] = i_arrSite[n];
					break;
				}
			}
		}

		int nAt = (int)strlen(o_szBuffer);
		for (int k = 0; k < i_nWanted && arrTop[k].nCount > 0 && nAt < i_nSize - 1; k++)
		{
			nAt += _snprintf_s(o_szBuffer + nAt, i_nSize - nAt, _TRUNCATE,
							   " +0x%08I64x x%I64u", arrTop[k].nOffset, arrTop[k].nCount);
		}
	}

	// A line a minute into the debug output describing the frames since the last
	// one - what the client is asking of D3D right there and then, rather than an
	// average over everything since it started, which stops moving.
	const DWORD SIZE_REPORT_INTERVAL_MS = 60 * 1000;

	void ReportOccasionally()
	{
		static DWORD			s_dwNextReport	= 0;
		static unsigned __int64	s_nWasSeen		= 0;
		static unsigned __int64	s_nWasPassed	= 0;
		static unsigned __int64	s_nWasDraws		= 0;
		static unsigned __int64	s_nWasFrames	= 0;
		static DWORD			s_dwWasAt		= 0;

		const DWORD dwNow = GetTickCount();
		if (0 == s_dwNextReport)
		{
			s_dwNextReport = dwNow + SIZE_REPORT_INTERVAL_MS;
			s_dwWasAt = dwNow;
			return;
		}
		if ((int)(dwNow - s_dwNextReport) < 0)
		{
			return;
		}

		const unsigned __int64 nSeen	= g_nCallsSeen	- s_nWasSeen;
		const unsigned __int64 nPassed	= g_nCallsPassed - s_nWasPassed;
		const unsigned __int64 nDraws	= g_nDraws		- s_nWasDraws;
		const unsigned __int64 nFrames	= g_nFrames		- s_nWasFrames;
		const DWORD dwElapsed			= dwNow - s_dwWasAt;

		char szLine[320];
		_snprintf_s(szLine, sizeof(szLine), _TRUNCATE,
					"D3D: %.0f fps, %.0f draws/frame, %.0f state calls/frame "
					"(%.1f%% dropped, %.0f still reaching the device)\n",
					(dwElapsed > 0) ? 1000.0 * (double)nFrames / (double)dwElapsed : 0.0,
					(nFrames > 0) ? (double)nDraws / (double)nFrames : 0.0,
					(nFrames > 0) ? (double)nSeen / (double)nFrames : 0.0,
					(nSeen > 0) ? 100.0 * (double)(nSeen - nPassed) / (double)nSeen : 0.0,
					(nFrames > 0) ? (double)nPassed / (double)nFrames : 0.0);
		OutputDebugStringA(szLine);

		char szSites[512];
		_snprintf_s(szSites, sizeof(szSites), _TRUNCATE, "D3D state call sites:");
		AppendTopSites(szSites, sizeof(szSites), g_arrStateSite, 8);
		strncat_s(szSites, sizeof(szSites), "\n", _TRUNCATE);
		OutputDebugStringA(szSites);

		_snprintf_s(szSites, sizeof(szSites), _TRUNCATE, "D3D draw call sites: ");
		AppendTopSites(szSites, sizeof(szSites), g_arrDrawSite, 6);
		strncat_s(szSites, sizeof(szSites), "\n", _TRUNCATE);
		OutputDebugStringA(szSites);

		memset(g_arrStateSite, 0x00, sizeof(g_arrStateSite));
		memset(g_arrDrawSite, 0x00, sizeof(g_arrDrawSite));

		s_dwNextReport	= dwNow + SIZE_REPORT_INTERVAL_MS;
		s_dwWasAt		= dwNow;
		s_nWasSeen		= g_nCallsSeen;
		s_nWasPassed	= g_nCallsPassed;
		s_nWasDraws		= g_nDraws;
		s_nWasFrames	= g_nFrames;
	}

	// One wrapped state block. It exists only so that Apply() can be seen: it
	// puts back a whole set of state without a single Set call coming through the
	// device, which would leave the shadow describing something that is no longer
	// there.
	class CD3DFilteredStateBlock : public IDirect3DStateBlock9
	{
	public:
		CD3DFilteredStateBlock(IDirect3DStateBlock9 *i_pBlock, CD3DFilteredDevice *i_pDevice)
			: m_pBlock(i_pBlock), m_pDevice(i_pDevice), m_nReferences(1)
		{
			m_pDevice->AddRef();
		}

		STDMETHOD(QueryInterface)(REFIID riid, void **ppvObj)
		{
			if (NULL == ppvObj)
			{
				return E_POINTER;
			}
			if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DStateBlock9))
			{
				AddRef();
				*ppvObj = static_cast<IDirect3DStateBlock9*>(this);
				return S_OK;
			}
			return m_pBlock->QueryInterface(riid, ppvObj);
		}

		STDMETHOD_(ULONG, AddRef)()
		{
			return (ULONG)InterlockedIncrement(&m_nReferences);
		}

		STDMETHOD_(ULONG, Release)()
		{
			const LONG nLeft = InterlockedDecrement(&m_nReferences);
			if (0 == nLeft)
			{
				m_pBlock->Release();
				m_pDevice->Release();
				delete this;
			}
			return (ULONG)nLeft;
		}

		STDMETHOD(GetDevice)(IDirect3DDevice9 **ppDevice)
		{
			if (NULL == ppDevice)
			{
				return E_POINTER;
			}
			m_pDevice->AddRef();
			*ppDevice = m_pDevice;			// the wrapper, not what is behind it
			return S_OK;
		}

		// Reads the device's state into the block; the device is unchanged.
		STDMETHOD(Capture)()
		{
			return m_pBlock->Capture();
		}

		// Writes the block over the device's state, all of it, invisibly.
		STDMETHOD(Apply)()
		{
			m_pDevice->Invalidate();
			return m_pBlock->Apply();
		}

	private:
		~CD3DFilteredStateBlock() {}

		IDirect3DStateBlock9	*m_pBlock;
		CD3DFilteredDevice		*m_pDevice;
		LONG					m_nReferences;
	};
}

///////////////////////////////////////////////////////////////////////////////

CD3DFilteredDevice::CD3DFilteredDevice(IDirect3DDevice9 *i_pDevice)
	: m_pDevice(i_pDevice)
	, m_nReferences(1)
	, m_dwOwnerThread(GetCurrentThreadId())
	, m_nForeignWrite(0)
	, m_nRecording(0)
{
	Invalidate();
}

CD3DFilteredDevice::~CD3DFilteredDevice()
{
	if (NULL != m_pDevice)
	{
		m_pDevice->Release();
	}
}

IDirect3DDevice9 *CD3DFilteredDevice::Wrap(IDirect3DDevice9 *i_pDevice)
{
	if (NULL == i_pDevice)
	{
		return NULL;
	}

	// Wrapping a wrapper would work but would cost a second indirection for
	// nothing, and it is a sign the caller has lost track of which is which.
	void *pAlready = NULL;
	if (SUCCEEDED(i_pDevice->QueryInterface(__uuidof(CD3DFilteredDevice), &pAlready))
		&& NULL != pAlready)
	{
		static_cast<IUnknown*>(pAlready)->Release();
		return i_pDevice;
	}

	FindModuleRange();
	return new CD3DFilteredDevice(i_pDevice);
}

void CD3DFilteredDevice::GetCounters(unsigned __int64 *o_pnSeen, unsigned __int64 *o_pnPassed,
									 unsigned __int64 *o_pnDraws, unsigned __int64 *o_pnFrames)
{
	if (NULL != o_pnSeen)	{ *o_pnSeen		= g_nCallsSeen; }
	if (NULL != o_pnPassed)	{ *o_pnPassed	= g_nCallsPassed; }
	if (NULL != o_pnDraws)	{ *o_pnDraws	= g_nDraws; }
	if (NULL != o_pnFrames)	{ *o_pnFrames	= g_nFrames; }
}

void CD3DFilteredDevice::Invalidate()
{
	memset(m_arrRenderStateKnown,	0x00, sizeof(m_arrRenderStateKnown));
	memset(m_arrStageStateKnown,	0x00, sizeof(m_arrStageStateKnown));
	memset(m_arrSamplerStateKnown,	0x00, sizeof(m_arrSamplerStateKnown));
	memset(m_arrTextureKnown,		0x00, sizeof(m_arrTextureKnown));
	memset(m_arrTransformKnown,		0x00, sizeof(m_arrTransformKnown));
	m_bMaterialKnown = FALSE;
}

BOOL CD3DFilteredDevice::CanFilter()
{
	if (GetCurrentThreadId() != m_dwOwnerThread)
	{	// Not ours to shadow.  Say so, and make the owner throw its shadow away
		// the next time it looks, since this call is about to change the state
		// behind its back.
		InterlockedExchange(&m_nForeignWrite, 1);
		return FALSE;
	}

	if (0 != InterlockedExchange(&m_nForeignWrite, 0))
	{
		Invalidate();
	}

	g_nCallsSeen++;
	ReportOccasionally();
	return (0 == m_nRecording) ? TRUE : FALSE;
}

int CD3DFilteredDevice::TransformSlot(D3DTRANSFORMSTATETYPE i_state)
{
	switch (i_state)
	{
	case D3DTS_VIEW:		return 0;
	case D3DTS_PROJECTION:	return 1;
	default:				break;
	}
	if (i_state >= D3DTS_TEXTURE0 && i_state <= D3DTS_TEXTURE7)
	{
		return 2 + (int)(i_state - D3DTS_TEXTURE0);
	}
	// D3DTS_WORLD is D3DTS_WORLDMATRIX(0), which is 256.
	if (i_state >= D3DTS_WORLD
		&& (int)(i_state - D3DTS_WORLD) < COUNT_WORLD_TRANSFORM)
	{
		return 10 + (int)(i_state - D3DTS_WORLD);
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// IUnknown

STDMETHODIMP CD3DFilteredDevice::QueryInterface(REFIID riid, void **ppvObj)
{
	if (NULL == ppvObj)
	{
		return E_POINTER;
	}
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DDevice9))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DDevice9*>(this);
		return S_OK;
	}
	// Our own id, so that Wrap() can tell a wrapped device from a bare one.
	if (IsEqualGUID(riid, __uuidof(CD3DFilteredDevice)))
	{
		AddRef();
		*ppvObj = this;
		return S_OK;
	}
	return m_pDevice->QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) CD3DFilteredDevice::AddRef()
{
	return (ULONG)InterlockedIncrement(&m_nReferences);
}

STDMETHODIMP_(ULONG) CD3DFilteredDevice::Release()
{
	const LONG nLeft = InterlockedDecrement(&m_nReferences);
	if (0 == nLeft)
	{
		delete this;
	}
	return (ULONG)nLeft;
}

///////////////////////////////////////////////////////////////////////////////
// the calls this exists for

STDMETHODIMP CD3DFilteredDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	if (bFilter && State < COUNT_RENDER_STATE)
	{
		if (m_arrRenderStateKnown[State] && m_arrRenderState[State] == Value)
		{
			return D3D_OK;
		}
		m_arrRenderState[State]			= Value;
		m_arrRenderStateKnown[State]	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetRenderState(State, Value);
}

STDMETHODIMP CD3DFilteredDevice::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue)
{
	NoteStateSite(_ReturnAddress());
	if (CanFilter() && State < COUNT_RENDER_STATE && NULL != pValue)
	{
		if (m_arrRenderStateKnown[State])
		{
			*pValue = m_arrRenderState[State];
			return D3D_OK;
		}
		const HRESULT hr = m_pDevice->GetRenderState(State, pValue);
		if (SUCCEEDED(hr))
		{
			m_arrRenderState[State]			= *pValue;
			m_arrRenderStateKnown[State]	= TRUE;
		}
		g_nCallsPassed++;
		return hr;
	}
	g_nCallsPassed++;
	return m_pDevice->GetRenderState(State, pValue);
}

STDMETHODIMP CD3DFilteredDevice::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type,
													  DWORD Value)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	if (bFilter && Stage < COUNT_STAGE && Type < COUNT_STAGE_STATE)
	{
		if (m_arrStageStateKnown[Stage][Type] && m_arrStageState[Stage][Type] == Value)
		{
			return D3D_OK;
		}
		m_arrStageState[Stage][Type]		= Value;
		m_arrStageStateKnown[Stage][Type]	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetTextureStageState(Stage, Type, Value);
}

STDMETHODIMP CD3DFilteredDevice::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type,
													  DWORD *pValue)
{
	NoteStateSite(_ReturnAddress());
	if (CanFilter() && Stage < COUNT_STAGE && Type < COUNT_STAGE_STATE && NULL != pValue)
	{
		if (m_arrStageStateKnown[Stage][Type])
		{
			*pValue = m_arrStageState[Stage][Type];
			return D3D_OK;
		}
		const HRESULT hr = m_pDevice->GetTextureStageState(Stage, Type, pValue);
		if (SUCCEEDED(hr))
		{
			m_arrStageState[Stage][Type]		= *pValue;
			m_arrStageStateKnown[Stage][Type]	= TRUE;
		}
		g_nCallsPassed++;
		return hr;
	}
	g_nCallsPassed++;
	return m_pDevice->GetTextureStageState(Stage, Type, pValue);
}

STDMETHODIMP CD3DFilteredDevice::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	if (bFilter && Sampler < COUNT_SAMPLER && Type < COUNT_SAMPLER_STATE)
	{
		if (m_arrSamplerStateKnown[Sampler][Type] && m_arrSamplerState[Sampler][Type] == Value)
		{
			return D3D_OK;
		}
		m_arrSamplerState[Sampler][Type]		= Value;
		m_arrSamplerStateKnown[Sampler][Type]	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetSamplerState(Sampler, Type, Value);
}

STDMETHODIMP CD3DFilteredDevice::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	if (bFilter && Stage < COUNT_TEXTURE)
	{
		if (m_arrTextureKnown[Stage] && m_arrTexture[Stage] == pTexture)
		{
			return D3D_OK;
		}
		m_arrTexture[Stage]			= pTexture;
		m_arrTextureKnown[Stage]	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetTexture(Stage, pTexture);
}

STDMETHODIMP CD3DFilteredDevice::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture)
{
	NoteStateSite(_ReturnAddress());
	// Answered by the device: it hands back a reference, and the shadow holds
	// no reference of its own to hand back in its place.
	CanFilter();
	g_nCallsPassed++;
	return m_pDevice->GetTexture(Stage, ppTexture);
}

STDMETHODIMP CD3DFilteredDevice::SetMaterial(CONST D3DMATERIAL9 *pMaterial)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	if (bFilter && NULL != pMaterial)
	{
		if (m_bMaterialKnown && 0 == memcmp(&m_material, pMaterial, sizeof(m_material)))
		{
			return D3D_OK;
		}
		m_material			= *pMaterial;
		m_bMaterialKnown	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetMaterial(pMaterial);
}

STDMETHODIMP CD3DFilteredDevice::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	NoteStateSite(_ReturnAddress());
	if (CanFilter() && NULL != pMaterial && m_bMaterialKnown)
	{
		*pMaterial = m_material;
		return D3D_OK;
	}
	g_nCallsPassed++;
	return m_pDevice->GetMaterial(pMaterial);
}

STDMETHODIMP CD3DFilteredDevice::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	NoteStateSite(_ReturnAddress());
	const BOOL bFilter = CanFilter();
	const int nSlot = TransformSlot(State);
	if (bFilter && nSlot >= 0 && NULL != pMatrix)
	{
		if (m_arrTransformKnown[nSlot]
			&& 0 == memcmp(&m_arrTransform[nSlot], pMatrix, sizeof(D3DMATRIX)))
		{
			return D3D_OK;
		}
		m_arrTransform[nSlot]		= *pMatrix;
		m_arrTransformKnown[nSlot]	= TRUE;
	}
	g_nCallsPassed++;
	return m_pDevice->SetTransform(State, pMatrix);
}

STDMETHODIMP CD3DFilteredDevice::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix)
{
	NoteStateSite(_ReturnAddress());
	const int nSlot = TransformSlot(State);
	if (CanFilter() && nSlot >= 0 && NULL != pMatrix)
	{
		if (m_arrTransformKnown[nSlot])
		{
			*pMatrix = m_arrTransform[nSlot];
			return D3D_OK;
		}
		const HRESULT hr = m_pDevice->GetTransform(State, pMatrix);
		if (SUCCEEDED(hr))
		{
			m_arrTransform[nSlot]		= *pMatrix;
			m_arrTransformKnown[nSlot]	= TRUE;
		}
		g_nCallsPassed++;
		return hr;
	}
	g_nCallsPassed++;
	return m_pDevice->GetTransform(State, pMatrix);
}

STDMETHODIMP CD3DFilteredDevice::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
{
	NoteStateSite(_ReturnAddress());
	// The device works out the product; the shadow no longer knows what is
	// there and is not going to guess.
	const BOOL bFilter = CanFilter();
	const int nSlot = TransformSlot(State);
	if (bFilter && nSlot >= 0)
	{
		m_arrTransformKnown[nSlot] = FALSE;
	}
	g_nCallsPassed++;
	return m_pDevice->MultiplyTransform(State, pMatrix);
}

///////////////////////////////////////////////////////////////////////////////
// counted, so that a frame can be described rather than guessed at.
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CD3DFilteredDevice::Present(CONST RECT *pSourceRect, CONST RECT *pDestRect,
										 HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	g_nFrames++;
	return m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

STDMETHODIMP CD3DFilteredDevice::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
											   UINT PrimitiveCount)
{
	NoteDrawSite(_ReturnAddress());
	g_nDraws++;
	return m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

STDMETHODIMP CD3DFilteredDevice::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,
													  INT BaseVertexIndex, UINT MinVertexIndex,
													  UINT NumVertices, UINT startIndex,
													  UINT primCount)
{
	NoteDrawSite(_ReturnAddress());
	g_nDraws++;
	return m_pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex,
										   NumVertices, startIndex, primCount);
}

STDMETHODIMP CD3DFilteredDevice::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,
												 UINT PrimitiveCount,
												 CONST void *pVertexStreamZeroData,
												 UINT VertexStreamZeroStride)
{
	NoteDrawSite(_ReturnAddress());
	g_nDraws++;
	return m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData,
									  VertexStreamZeroStride);
}

STDMETHODIMP CD3DFilteredDevice::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,
														UINT MinVertexIndex, UINT NumVertices,
														UINT PrimitiveCount,
														CONST void *pIndexData,
														D3DFORMAT IndexDataFormat,
														CONST void *pVertexStreamZeroData,
														UINT VertexStreamZeroStride)
{
	NoteDrawSite(_ReturnAddress());
	g_nDraws++;
	return m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices,
											 PrimitiveCount, pIndexData, IndexDataFormat,
											 pVertexStreamZeroData, VertexStreamZeroStride);
}

///////////////////////////////////////////////////////////////////////////////
// the ones that move the state out from under the shadow

STDMETHODIMP CD3DFilteredDevice::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	const HRESULT hr = m_pDevice->Reset(pPresentationParameters);
	Invalidate();						// every state is back at its default
	return hr;
}

STDMETHODIMP CD3DFilteredDevice::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	IDirect3DStateBlock9 *pBlock = NULL;
	const HRESULT hr = m_pDevice->CreateStateBlock(Type, &pBlock);
	if (FAILED(hr) || NULL == ppSB)
	{
		if (NULL != pBlock)		{ pBlock->Release(); }
		return FAILED(hr) ? hr : E_POINTER;
	}
	*ppSB = new CD3DFilteredStateBlock(pBlock, this);
	return hr;
}

STDMETHODIMP CD3DFilteredDevice::BeginStateBlock()
{
	const HRESULT hr = m_pDevice->BeginStateBlock();
	if (SUCCEEDED(hr) && GetCurrentThreadId() == m_dwOwnerThread)
	{	// While a block is being recorded, a call is written into the block
		// instead of being applied.  Dropping one because the device is
		// already in that state would leave it out of the block, and the
		// device's state is not moving, so the shadow must not move either.
		m_nRecording++;
	}
	return hr;
}

STDMETHODIMP CD3DFilteredDevice::EndStateBlock(IDirect3DStateBlock9 **ppSB)
{
	IDirect3DStateBlock9 *pBlock = NULL;
	const HRESULT hr = m_pDevice->EndStateBlock(&pBlock);
	if (GetCurrentThreadId() == m_dwOwnerThread && m_nRecording > 0)
	{
		m_nRecording--;
	}
	if (FAILED(hr) || NULL == ppSB)
	{
		if (NULL != pBlock)		{ pBlock->Release(); }
		return FAILED(hr) ? hr : E_POINTER;
	}
	*ppSB = new CD3DFilteredStateBlock(pBlock, this);
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// everything else
#include "D3DFilteredDevice_Forward.inl"

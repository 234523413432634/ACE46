///////////////////////////////////////////////////////////////////////////////
//  D3DFilteredDevice_Forward.inl : the bodies of the IDirect3DDevice9 methods
//  CD3DFilteredDevice passes straight through.
///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::TestCooperativeLevel()
{
	return m_pDevice->TestCooperativeLevel();
}

UINT STDMETHODCALLTYPE CD3DFilteredDevice::GetAvailableTextureMem()
{
	return m_pDevice->GetAvailableTextureMem();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::EvictManagedResources()
{
	return m_pDevice->EvictManagedResources();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetDirect3D(IDirect3D9** ppD3D9)
{
	return m_pDevice->GetDirect3D(ppD3D9);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return m_pDevice->GetDeviceCaps(pCaps);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	return m_pDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return m_pDevice->GetCreationParameters(pParameters);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void STDMETHODCALLTYPE CD3DFilteredDevice::SetCursorPosition(int X, int Y, DWORD Flags)
{
	m_pDevice->SetCursorPosition(X, Y, Flags);
}

BOOL STDMETHODCALLTYPE CD3DFilteredDevice::ShowCursor(BOOL bShow)
{
	return m_pDevice->ShowCursor(bShow);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	return m_pDevice->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	return m_pDevice->GetSwapChain(iSwapChain, pSwapChain);
}

UINT STDMETHODCALLTYPE CD3DFilteredDevice::GetNumberOfSwapChains()
{
	return m_pDevice->GetNumberOfSwapChains();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	return m_pDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	return m_pDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetDialogBoxMode(BOOL bEnableDialogs)
{
	return m_pDevice->SetDialogBoxMode(bEnableDialogs);
}

void STDMETHODCALLTYPE CD3DFilteredDevice::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void STDMETHODCALLTYPE CD3DFilteredDevice::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
	m_pDevice->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	return m_pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	return m_pDevice->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	return m_pDevice->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	return m_pDevice->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	return m_pDevice->ColorFill(pSurface, pRect, color);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return m_pDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return m_pDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	return m_pDevice->SetDepthStencilSurface(pNewZStencil);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return m_pDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::BeginScene()
{
	return m_pDevice->BeginScene();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::EndScene()
{
	return m_pDevice->EndScene();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return m_pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	return m_pDevice->SetViewport(pViewport);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetViewport(D3DVIEWPORT9* pViewport)
{
	return m_pDevice->GetViewport(pViewport);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetLight(DWORD Index, CONST D3DLIGHT9* arg1)
{
	return m_pDevice->SetLight(Index, arg1);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetLight(DWORD Index, D3DLIGHT9* arg1)
{
	return m_pDevice->GetLight(Index, arg1);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::LightEnable(DWORD Index, BOOL Enable)
{
	return m_pDevice->LightEnable(Index, Enable);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	return m_pDevice->GetLightEnable(Index, pEnable);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	return m_pDevice->SetClipPlane(Index, pPlane);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetClipPlane(DWORD Index, float* pPlane)
{
	return m_pDevice->GetClipPlane(Index, pPlane);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
	return m_pDevice->SetClipStatus(pClipStatus);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	return m_pDevice->GetClipStatus(pClipStatus);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	return m_pDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::ValidateDevice(DWORD* pNumPasses)
{
	return m_pDevice->ValidateDevice(pNumPasses);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetCurrentTexturePalette(UINT PaletteNumber)
{
	return m_pDevice->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	return m_pDevice->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetScissorRect(CONST RECT* pRect)
{
	return m_pDevice->SetScissorRect(pRect);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetScissorRect(RECT* pRect)
{
	return m_pDevice->GetScissorRect(pRect);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	return m_pDevice->SetSoftwareVertexProcessing(bSoftware);
}

BOOL STDMETHODCALLTYPE CD3DFilteredDevice::GetSoftwareVertexProcessing()
{
	return m_pDevice->GetSoftwareVertexProcessing();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetNPatchMode(float nSegments)
{
	return m_pDevice->SetNPatchMode(nSegments);
}

float STDMETHODCALLTYPE CD3DFilteredDevice::GetNPatchMode()
{
	return m_pDevice->GetNPatchMode();
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	return m_pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	return m_pDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	return m_pDevice->SetVertexDeclaration(pDecl);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	return m_pDevice->GetVertexDeclaration(ppDecl);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetFVF(DWORD FVF)
{
	return m_pDevice->SetFVF(FVF);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetFVF(DWORD* pFVF)
{
	return m_pDevice->GetFVF(pFVF);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	return m_pDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	return m_pDevice->SetVertexShader(pShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	return m_pDevice->GetVertexShader(ppShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return m_pDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_pDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return m_pDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_pDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	return m_pDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_pDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	return m_pDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
	return m_pDevice->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
	return m_pDevice->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	return m_pDevice->SetIndices(pIndexData);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	return m_pDevice->GetIndices(ppIndexData);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	return m_pDevice->CreatePixelShader(pFunction, ppShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	return m_pDevice->SetPixelShader(pShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	return m_pDevice->GetPixelShader(ppShader);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	return m_pDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	return m_pDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
{
	return m_pDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	return m_pDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return m_pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return m_pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::DeletePatch(UINT Handle)
{
	return m_pDevice->DeletePatch(Handle);
}

HRESULT STDMETHODCALLTYPE CD3DFilteredDevice::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return m_pDevice->CreateQuery(Type, ppQuery);
}

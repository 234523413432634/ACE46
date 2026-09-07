// D3DHanFont.cpp: implementation of the CD3DHanFont class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DHanFont.h"
#include "D3DApp.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "d3dfont.h"

extern CD3DApplication* g_pApp;

#ifdef LANGUAGE_VIETNAM
#define LANGUAGE_CHARSET VIETNAMESE_CHARSET
#else
#define LANGUAGE_CHARSET DEFAULT_CHARSET
#endif

// Atlas Config
#define ATLAS_WIDTH  256
#define ATLAS_HEIGHT 256
#define MAX_BATCH_CHARS 768


// Helper to swap Red and Blue channels (ABGR -> ARGB) for DirectX9
inline DWORD SwapRB(DWORD color)
{
    return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
}

// Safe string copy helper
inline void SafeStrCpy(char* dest, const char* src, size_t destSize)
{
    if (!dest || destSize == 0) return;
    if (!src) { dest[0] = '\0'; return; }
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

CD3DHanFont::CD3DHanFont(TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags,
    BOOL outline, DWORD dwMaxWidth, DWORD dwMaxHeight,
    BOOL bCullText, BOOL bCullUV)
    : m_pd3dDevice(NULL),
    m_pTexture(NULL),
    m_pVB(NULL),
    m_hFontGD(NULL),
    m_hDC(NULL),
    m_hBitmap(NULL),
    m_pBitmapBits(NULL),
    m_dwTexWidth(ATLAS_WIDTH),
    m_dwTexHeight(ATLAS_HEIGHT),
    m_dwCurrentX(0),
    m_dwCurrentY(0),
    m_dwRowHeight(0),
    m_dwFontHeight(dwHeight),
    m_dwFontFlags(dwFlags),
    m_bOutLine(outline),
    m_bCullText(bCullText),
    m_bCullUV(bCullUV),
    m_fPosX(0), m_fPosY(0),
    m_dwColor(0),
    m_fTx1(0), m_fTy1(0), m_fTx2(0), m_fTy2(0),
    m_fWidth(0),
    m_fTextScale(1.0f),
    m_bReset(FALSE),
    m_bReLoadString(FALSE),
    m_bTextColor(TRUE)
{
    SafeStrCpy(m_strFontName, strFontName, sizeof(m_strFontName));
    memset(m_strText, 0, sizeof(m_strText));
    memset(m_fTexCoords, 0, sizeof(m_fTexCoords));

    memset(m_strSizeCheckText, 0, sizeof(m_strSizeCheckText));
    m_szCheckSize.cx = m_szCheckSize.cy = 0;
}

CD3DHanFont::~CD3DHanFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}

void CD3DHanFont::SetUV(float tx1, float ty1, float tx2, float ty2)
{
    m_fTx1 = tx1; m_fTy1 = ty1; m_fTx2 = tx2; m_fTy2 = ty2;
}

//-----------------------------------------------------------------------------
// GDI Resource Management
//-----------------------------------------------------------------------------
void CD3DHanFont::PrepareGDIResources()
{
    if (m_hDC) return;

    HDC hDC = GetDC(NULL);
    m_hDC = CreateCompatibleDC(hDC);
    ReleaseDC(NULL, hDC);

    if (!m_hDC) return;

    INT nHeight = -MulDiv(m_dwFontHeight, GetDeviceCaps(m_hDC, LOGPIXELSY), 72);
    DWORD dwBold = (m_dwFontFlags & D3DFONT_BOLD) ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags & D3DFONT_ITALIC) ? TRUE : FALSE;

    m_hFontGD = CreateFont(nHeight, 0, 0, 0, dwBold, dwItalic,
        FALSE, FALSE, LANGUAGE_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        FIXED_PITCH, m_strFontName);

    BITMAPINFO bmi;
    ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_dwFontHeight * 2;
    bmi.bmiHeader.biHeight = -(int)(m_dwFontHeight * 2 + 4);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;

    m_hBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (VOID**)&m_pBitmapBits, NULL, 0);

    if (m_hBitmap) SelectObject(m_hDC, m_hBitmap);
    if (m_hFontGD) SelectObject(m_hDC, m_hFontGD);

    SetMapMode(m_hDC, MM_TEXT);
    SetBkMode(m_hDC, TRANSPARENT);
    SetTextAlign(m_hDC, TA_TOP | TA_LEFT);
}

void CD3DHanFont::CleanupGDIResources()
{
    if (m_hFontGD) { DeleteObject(m_hFontGD); m_hFontGD = NULL; }
    if (m_hBitmap) { DeleteObject(m_hBitmap); m_hBitmap = NULL; }
    if (m_hDC) { DeleteDC(m_hDC); m_hDC = NULL; }
    m_pBitmapBits = NULL;
}

HRESULT CD3DHanFont::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
    m_pd3dDevice = pd3dDevice;
    return RestoreDeviceObjects();
}

HRESULT CD3DHanFont::RestoreDeviceObjects()
{
    HRESULT hr;

    if (!m_pTexture) {
        hr = CreateAtlasTexture();
        if (FAILED(hr)) return hr;
    }

    if (!m_pVB) {
        if (FAILED(hr = m_pd3dDevice->CreateVertexBuffer(MAX_BATCH_CHARS * 6 * sizeof(FONT2DVERTEX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_FONT2DVERTEX,
            D3DPOOL_DEFAULT, &m_pVB, NULL)))
        {
            return hr;
        }
    }

    PrepareGDIResources();
    return S_OK;
}

HRESULT CD3DHanFont::CreateAtlasTexture()
{
    SAFE_RELEASE(m_pTexture);
    return m_pd3dDevice->CreateTexture(m_dwTexWidth, m_dwTexHeight, 1, 0,
        D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pTexture, NULL);
}

HRESULT CD3DHanFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pTexture);
    CleanupGDIResources();
    m_GlyphCache.clear();
    return S_OK;
}

HRESULT CD3DHanFont::DeleteDeviceObjects()
{
    m_pd3dDevice = NULL;
    return S_OK;
}

void CD3DHanFont::FlushGlyphCache()
{
    m_GlyphCache.clear();
    m_dwCurrentX = 0;
    m_dwCurrentY = 0;
    m_dwRowHeight = 0;
}

const GlyphInfo* CD3DHanFont::GetGlyph(unsigned int charCode)
{
    if (!m_hDC || !m_hBitmap) return NULL;

    std::map<unsigned int, GlyphInfo>::iterator it = m_GlyphCache.find(charCode);
    if (it != m_GlyphCache.end()) return &it->second;

    char str[3] = { 0,0,0 };
    if (charCode > 0xFF) {
        str[0] = (charCode >> 8) & 0xFF;
        str[1] = charCode & 0xFF;
    }
    else {
        str[0] = (char)charCode;
    }

    SIZE size;
    GetTextExtentPoint32(m_hDC, str, strlen(str), &size);

    int outlineThickness = 1;
    if (m_dwFontHeight > 10) {
        outlineThickness = 2;
    }

    int padding = m_bOutLine ? (outlineThickness * 2) : 1;
    int width = size.cx + padding * 2;
    int height = size.cy + padding * 2 + 4;

    if (m_dwCurrentX + width >= m_dwTexWidth) {
        m_dwCurrentX = 0;
        m_dwCurrentY += m_dwRowHeight;
        m_dwRowHeight = 0;
    }

    if (m_dwCurrentY + height >= m_dwTexHeight) {
        FlushGlyphCache();
    }

    RECT r = { 0, 0, width, height };
    FillRect(m_hDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

    int drawX = padding;
    int drawY = padding;

    if (m_bOutLine) {
        SetTextColor(m_hDC, RGB(1, 1, 1));
        for (int i = -outlineThickness; i <= outlineThickness; i++) {
            for (int j = -outlineThickness; j <= outlineThickness; j++) {
                if (i == 0 && j == 0) continue;
                TextOut(m_hDC, drawX + i, drawY + j, str, strlen(str));
            }
        }
    }

    // Draw main face
    SetTextColor(m_hDC, RGB(255, 255, 255));
    TextOut(m_hDC, drawX, drawY, str, strlen(str));

    D3DLOCKED_RECT lr;
    if (!m_pTexture || FAILED(m_pTexture->LockRect(0, &lr, NULL, 0)))
        return NULL;

    BYTE* pDstBase = (BYTE*)lr.pBits + (m_dwCurrentY * lr.Pitch) + (m_dwCurrentX * 2);

    for (int y = 0; y < height; ++y) {
        WORD* pDst = (WORD*)(pDstBase + y * lr.Pitch);
        DWORD* pSrc = m_pBitmapBits + (y * m_dwFontHeight * 2);

        for (int x = 0; x < width; ++x) {
            DWORD color = pSrc[x] & 0x00FFFFFF;
            WORD outColor = 0x0000;
            if (color != 0) {
                outColor = 0xF000 |
                    ((color & 0x00F00000) >> 12) |
                    ((color & 0x0000F000) >> 8) |
                    ((color & 0x000000F0) >> 4);
            }
            *pDst++ = outColor;
        }
    }
    m_pTexture->UnlockRect(0);

    GlyphInfo info;
    info.tu1 = (float)m_dwCurrentX / m_dwTexWidth;
    info.tv1 = (float)m_dwCurrentY / m_dwTexHeight;
    info.tu2 = (float)(m_dwCurrentX + width) / m_dwTexWidth;
    info.tv2 = (float)(m_dwCurrentY + height) / m_dwTexHeight;
    info.nWidth = width;
    info.nHeight = height;
    info.nAdvanceX = size.cx;

    m_GlyphCache[charCode] = info;

    m_dwCurrentX += width + 1;
    if (height > (int)m_dwRowHeight) m_dwRowHeight = height;

    return &m_GlyphCache[charCode];
}

//-----------------------------------------------------------------------------
// DrawText
//-----------------------------------------------------------------------------
HRESULT CD3DHanFont::DrawText(FLOAT sx, FLOAT sy, DWORD dwColor,
    TCHAR* strText, DWORD dwFlags, RECT* i_pFillRect, BOOL bColorState)
{
    if (!m_pd3dDevice || !m_pVB || !m_pTexture || !strText || strText[0] == 0) return S_OK;

    if (strcmp(m_strText, strText) != 0) {
        SafeStrCpy(m_strText, strText, sizeof(m_strText));
        m_dwColor = dwColor;
    }
    m_fPosX = sx; m_fPosY = sy;

    // Draw Cursor/FillRect (Legacy GDI call)
    DrawFillRect(i_pFillRect);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

    m_pd3dDevice->SetTexture(0, m_pTexture);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    if (dwFlags & D3DFONT_NOTFILTERED) {
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    }
    else {
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    }

    FONT2DVERTEX* pVerts = NULL;
    if (FAILED(m_pVB->Lock(0, 0, (void**)&pVerts, D3DLOCK_DISCARD)))
        return S_OK;

    int i = 0;
    float currX = sx;
    float currY = sy;

    // Color Setup: Swap R/B for DirectX
    DWORD baseColor = SwapRB(dwColor);
    DWORD currColor = baseColor;

    int vCount = 0;
    int len = strlen(strText);

    while (i < len)
    {
        if (bColorState && m_bTextColor)
        {
            if (strText[i] == '\\' && i + 1 < len) {
                DWORD targetColorRaw = GetFontColor(strText[i + 1]);
                if (targetColorRaw != 0) {
                    DWORD targetColor = SwapRB(targetColorRaw);
                    if (currColor == targetColor) {
                        currColor = baseColor;
                    }
                    else {
                        currColor = targetColor;
                    }
                    i += 2;
                    continue;
                }
            }
        }

        unsigned int charCode = (unsigned char)strText[i];
        if (IsDBCSLeadByte((BYTE)strText[i]) && i + 1 < len) {
            charCode = (charCode << 8) | (unsigned char)strText[i + 1];
            i += 2;
        }
        else {
            i++;
        }

        if (i >= 256) break;

        const GlyphInfo* pGlyph = GetGlyph(charCode);
        if (!pGlyph) continue;

        if (vCount >= MAX_BATCH_CHARS * 6) break;

        if (m_bCullText) {
            float fGap = 0;
            if ((currX - sx) + pGlyph->nAdvanceX > m_fWidth) {
                break;
            }
        }

        float w = (float)pGlyph->nWidth;
        float h = (float)pGlyph->nHeight;
        float drawX = currX;
        float drawY = currY;

        pVerts[vCount].p = D3DXVECTOR4(drawX - 0.5f, drawY + h - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu1; pVerts[vCount].tv = pGlyph->tv2;
        vCount++;

        pVerts[vCount].p = D3DXVECTOR4(drawX - 0.5f, drawY - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu1; pVerts[vCount].tv = pGlyph->tv1;
        vCount++;

        pVerts[vCount].p = D3DXVECTOR4(drawX + w - 0.5f, drawY - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu2; pVerts[vCount].tv = pGlyph->tv1;
        vCount++;

        pVerts[vCount].p = D3DXVECTOR4(drawX + w - 0.5f, drawY - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu2; pVerts[vCount].tv = pGlyph->tv1;
        vCount++;

        pVerts[vCount].p = D3DXVECTOR4(drawX + w - 0.5f, drawY + h - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu2; pVerts[vCount].tv = pGlyph->tv2;
        vCount++;

        pVerts[vCount].p = D3DXVECTOR4(drawX - 0.5f, drawY + h - 0.5f, 0.5f, 1.0f);
        pVerts[vCount].color = currColor;
        pVerts[vCount].tu = pGlyph->tu1; pVerts[vCount].tv = pGlyph->tv2;
        vCount++;

        currX += pGlyph->nAdvanceX;
    }

    m_pVB->Unlock();

    if (vCount > 0) {
        m_pd3dDevice->SetFVF(D3DFVF_FONT2DVERTEX);
        m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(FONT2DVERTEX));
        m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vCount / 3);
    }

    return S_OK;
}

HRESULT CD3DHanFont::SetText(FLOAT sx, FLOAT sy, TCHAR* texts, DWORD color, RECT* i_pFillRect, BOOL bColorState)
{
    if (texts) SafeStrCpy(m_strText, texts, sizeof(m_strText));
    m_fPosX = sx;
    m_fPosY = sy;
    m_dwColor = color;
    return S_OK;
}

SIZE CD3DHanFont::GetStringSize(TCHAR* strText)
{
    if (!strcmp(m_strSizeCheckText, strText))
        return m_szCheckSize;

    if (!m_hDC) PrepareGDIResources();
    if (!m_hDC) {
        SIZE err = { 0, 0 };
        return err;
    }

    // Strip colors logic (CheckFontColor)
    char strTemp[2048] = { 0, };
    int nCount = 0;
    int len = strlen(strText);

    for (int i = 0; i < len; i++)
    {
        if (strText[i] == '\\' && i + 1 < len && CheckFontColor(strText[i + 1])) {
            if (!m_bTextColor) {
                strTemp[nCount++] = strText[i];
            }
            else {
                i++;
            }
        }
        else {
            strTemp[nCount++] = strText[i];
        }
    }

    SIZE size;
    GetTextExtentPoint32(m_hDC, strTemp, nCount, &size);

    SafeStrCpy(m_strSizeCheckText, strText, sizeof(m_strSizeCheckText));
    m_szCheckSize = size;
    return size;
}

void CD3DHanFont::SetReLoadString(BOOL bReset)
{
    m_bReLoadString = bReset;
}

extern int GetStringBuffPos(char* str, int pos);

void CD3DHanFont::DrawFillRect(RECT* i_pFillRect)
{
    if (NULL == i_pFillRect || !m_hDC) return;

    HDC hDC = g_pApp->GetHDC();

    int nStartPos = GetStringBuffPos(m_strText, i_pFillRect->left);
    int nEndPos = GetStringBuffPos(m_strText, i_pFillRect->right);

    if ((-1 == nStartPos) || (-1 == nEndPos)) return;

    int nCopyLen = nEndPos - nStartPos;
    if (nCopyLen <= 0) return;

    RECT rcRender;
    char chPreString[256] = { 0 };
    char chRectString[256] = { 0 };

    SIZE sizePre, sizeRect;

    if (nStartPos < 256) SafeStrCpy(chPreString, m_strText, nStartPos + 1);
    if (nCopyLen < 256)  SafeStrCpy(chRectString, &m_strText[nStartPos], nCopyLen + 1);

    GetTextExtentPoint32(hDC, chPreString, strlen(chPreString), &sizePre);
    GetTextExtentPoint32(hDC, chRectString, strlen(chRectString), &sizeRect);

    rcRender.left = sizePre.cx;
    rcRender.top = 0;
    rcRender.right = rcRender.left + sizeRect.cx;
    rcRender.bottom = rcRender.top + sizeRect.cy;

    FillRect(hDC, &rcRender, (HBRUSH)(COLOR_GRAYTEXT));
}

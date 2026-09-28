// D3DHanFont.cpp: implementation of the CD3DHanFont class.
//
// See D3DHanFont.h for what the atlas is and why it is shared.

#include "stdafx.h"
#include "D3DHanFont.h"
#include "D3DApp.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "d3dfont.h"
#include "FontGlyphRaster.h"

extern CD3DApplication* g_pApp;

///////////////////////////////////////////////////////////////////////////////
//  What encoding is the text in?
//
//  Nothing in the protocol says.
///////////////////////////////////////////////////////////////////////////////
namespace
{
    UINT g_uLegacyCodePage = 0;		// 0 until asked for, then GetACP()

    inline UINT LegacyCodePage()
    {
        if (0 == g_uLegacyCodePage) g_uLegacyCodePage = GetACP();
        return g_uLegacyCodePage;
    }
}

void CD3DHanFont::SetLegacyCodePage(UINT i_uCodePage)
{
    g_uLegacyCodePage = i_uCodePage ? i_uCodePage : GetACP();
}

UINT CD3DHanFont::GetLegacyCodePage()
{
    return LegacyCodePage();
}

// Point sizes are resolved against this and never against the monitor, so the
// Windows display scaling setting cannot change how large the text comes out.
#define ATUM_FONT_REFERENCE_DPI		96

// Atlas pages. A8L8, so a page is 2 MB; Latin text needs one, and the CJK
// builds fill a few as glyphs are met.
#define ATLAS_PAGE_SIZE			1024
#define ATLAS_MAX_PAGES			16

// Room for one frame's worth of text in the shared vertex buffer.
#define MAX_BATCH_CHARS			4096

// Swap red and blue: the callers build colours with HFONT_ARGB, which puts blue
// where D3DCOLOR wants red.
inline DWORD SwapRB(DWORD color)
{
    return (color & 0xFF00FF00) | ((color & 0x00FF0000) >> 16) | ((color & 0x000000FF) << 16);
}

inline void SafeStrCpy(char* dest, const char* src, size_t destSize)
{
    if (!dest || destSize == 0) return;
    if (!src) { dest[0] = '\0'; return; }
    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
//  CD3DHanFontAtlas - the GDI font, the glyph cache and the atlas pages for one
//  (face, size, style, outline) combination, shared by every CD3DHanFont that
//  asked for it.
///////////////////////////////////////////////////////////////////////////////
class CD3DHanFontAtlas
{
public:
    struct Key
    {
        std::string strFace;
        int         nPointHeight;
        DWORD       dwFlags;
        BOOL        bOutline;

        bool operator<(const Key& o) const
        {
            if (strFace != o.strFace)			return strFace < o.strFace;
            if (nPointHeight != o.nPointHeight)	return nPointHeight < o.nPointHeight;
            if (dwFlags != o.dwFlags)			return dwFlags < o.dwFlags;
            return bOutline < o.bOutline;
        }
    };

    CD3DHanFontAtlas(const Key& key);
    ~CD3DHanFontAtlas();

    const GlyphInfo*    GetGlyph(LPDIRECT3DDEVICE9 pDevice, wchar_t ch);
    LPDIRECT3DTEXTURE9  GetPage(int nPage) const
    {
        return (nPage >= 0 && nPage < (int)m_Pages.size()) ? m_Pages[nPage] : NULL;
    }

    HDC     GetFontDC()         { EnsureGDI(); return m_hDC; }
    int     GetLineHeight()     { EnsureGDI(); return m_nLineHeight; }

private:
    void    EnsureGDI();
    BOOL    RasteriseGlyph(wchar_t ch, GlyphInfo& out, LPDIRECT3DDEVICE9 pDevice);
    int     AllocateRect(int w, int h, int& outX, int& outY, LPDIRECT3DDEVICE9 pDevice);

    Key                                 m_Key;
    int                                 m_nPixelHeight;
    int                                 m_nOutlineRadius;
    int                                 m_nLineHeight;

    HDC                                 m_hDC;
    HFONT                               m_hFont;
    HBITMAP                             m_hBitmap;
    DWORD*                              m_pBits;
    int                                 m_nBmpW, m_nBmpH;

    std::map<wchar_t, GlyphInfo>        m_Glyphs;
    std::vector<LPDIRECT3DTEXTURE9>     m_Pages;

    // Shelf packing across the current page.
    int                                 m_nCurX, m_nCurY, m_nShelfH;

    // Scratch buffers, kept so a glyph does not allocate.
    std::vector<BYTE>                   m_Face;
    std::vector<BYTE>                   m_Union;
};

///////////////////////////////////////////////////////////////////////////////
//  Shared state: the atlas registry and the one vertex buffer everything draws
//  through.
///////////////////////////////////////////////////////////////////////////////
namespace
{
    std::map<CD3DHanFontAtlas::Key, CD3DHanFontAtlas*>	g_Atlases;
    LPDIRECT3DVERTEXBUFFER9								g_pSharedVB = NULL;
    int													g_nSharedVBChars = 0;

    CD3DHanFontAtlas* AcquireAtlas(const char* szFace, int nPointHeight,
                                   DWORD dwFlags, BOOL bOutline)
    {
        CD3DHanFontAtlas::Key key;
        key.strFace = szFace ? szFace : "";
        key.nPointHeight = nPointHeight;
        // Only what changes the raster matters for sharing; D3DFONT_ZENABLE and
        // D3DFONT_NOTFILTERED are draw time state.
        key.dwFlags = dwFlags & (D3DFONT_BOLD | D3DFONT_ITALIC);
        key.bOutline = bOutline ? TRUE : FALSE;

        std::map<CD3DHanFontAtlas::Key, CD3DHanFontAtlas*>::iterator it = g_Atlases.find(key);
        if (it != g_Atlases.end())
            return it->second;

        CD3DHanFontAtlas* pAtlas = new CD3DHanFontAtlas(key);
        g_Atlases[key] = pAtlas;
        return pAtlas;
    }

    BOOL EnsureSharedVB(LPDIRECT3DDEVICE9 pDevice)
    {
        if (g_pSharedVB) return TRUE;
        if (!pDevice) return FALSE;

        if (FAILED(pDevice->CreateVertexBuffer(MAX_BATCH_CHARS * 6 * sizeof(FONT2DVERTEX),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_FONT2DVERTEX,
            D3DPOOL_DEFAULT, &g_pSharedVB, NULL)))
        {
            g_pSharedVB = NULL;
            return FALSE;
        }
        g_nSharedVBChars = 0;
        return TRUE;
    }
}

void CD3DHanFont::ReleaseSharedDeviceObjects()
{
    SAFE_RELEASE(g_pSharedVB);
    g_nSharedVBChars = 0;
}

void CD3DHanFont::ReleaseAllAtlases()
{
    std::map<CD3DHanFontAtlas::Key, CD3DHanFontAtlas*>::iterator it = g_Atlases.begin();
    for (; it != g_Atlases.end(); ++it)
        delete it->second;
    g_Atlases.clear();
    ReleaseSharedDeviceObjects();
}

///////////////////////////////////////////////////////////////////////////////
//  CD3DHanFontAtlas
///////////////////////////////////////////////////////////////////////////////
CD3DHanFontAtlas::CD3DHanFontAtlas(const Key& key)
    : m_Key(key),
      m_nPixelHeight(0),
      m_nOutlineRadius(0),
      m_nLineHeight(0),
      m_hDC(NULL),
      m_hFont(NULL),
      m_hBitmap(NULL),
      m_pBits(NULL),
      m_nBmpW(0), m_nBmpH(0),
      m_nCurX(0), m_nCurY(0), m_nShelfH(0)
{
}

CD3DHanFontAtlas::~CD3DHanFontAtlas()
{
    for (size_t i = 0; i < m_Pages.size(); ++i)
        SAFE_RELEASE(m_Pages[i]);
    m_Pages.clear();
    m_Glyphs.clear();

    if (m_hDC)     SelectObject(m_hDC, GetStockObject(SYSTEM_FONT));
    if (m_hFont)   { DeleteObject(m_hFont);   m_hFont = NULL; }
    if (m_hBitmap) { DeleteObject(m_hBitmap); m_hBitmap = NULL; }
    if (m_hDC)     { DeleteDC(m_hDC);         m_hDC = NULL; }
    m_pBits = NULL;
}

void CD3DHanFontAtlas::EnsureGDI()
{
    if (m_hDC) return;

    HDC hScreen = ::GetDC(NULL);
    m_hDC = CreateCompatibleDC(hScreen);
    ::ReleaseDC(NULL, hScreen);
    if (!m_hDC) return;

    // Fixed reference dpi: the monitor's own setting must not reach this.
    m_nPixelHeight = MulDiv(m_Key.nPointHeight, ATUM_FONT_REFERENCE_DPI, 72);
    if (m_nPixelHeight < 1) m_nPixelHeight = 1;

    // One pixel is right for UI text; only genuinely large text carries two
    // without the outline turning into a slab.
    m_nOutlineRadius = m_Key.bOutline ? ((m_nPixelHeight >= 26) ? 2 : 1) : 0;

    m_hFont = CreateFontA(-m_nPixelHeight, 0, 0, 0,
        (m_Key.dwFlags & D3DFONT_BOLD) ? FW_BOLD : FW_NORMAL,
        (m_Key.dwFlags & D3DFONT_ITALIC) ? TRUE : FALSE,
        FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, m_Key.strFace.c_str());

    if (m_hFont) SelectObject(m_hDC, m_hFont);

    TEXTMETRICA tm;
    ZeroMemory(&tm, sizeof(tm));
    if (GetTextMetricsA(m_hDC, &tm))
        m_nLineHeight = tm.tmHeight;
    else
        m_nLineHeight = m_nPixelHeight;

    // Big enough for the widest glyph the face can produce plus the outline on
    // both sides; tmMaxCharWidth covers the wide CJK forms.
    int nMaxW = (int)tm.tmMaxCharWidth;
    if (nMaxW < m_nPixelHeight * 2) nMaxW = m_nPixelHeight * 2;
    m_nBmpW = nMaxW + m_nOutlineRadius * 2 + 8;
    m_nBmpH = (int)tm.tmHeight + (int)tm.tmExternalLeading + m_nOutlineRadius * 2 + 8;
    if (m_nBmpH < m_nPixelHeight * 2) m_nBmpH = m_nPixelHeight * 2;

    BITMAPINFO bmi;
    ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_nBmpW;
    bmi.bmiHeader.biHeight = -m_nBmpH;			// top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;

    m_hBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, (VOID**)&m_pBits, NULL, 0);
    if (m_hBitmap) SelectObject(m_hDC, m_hBitmap);

    SetMapMode(m_hDC, MM_TEXT);
    SetBkMode(m_hDC, TRANSPARENT);
    SetTextAlign(m_hDC, TA_TOP | TA_LEFT);
    SetTextColor(m_hDC, RGB(255, 255, 255));

    m_Face.resize((size_t)m_nBmpW * m_nBmpH);
    m_Union.resize((size_t)m_nBmpW * m_nBmpH);
}

// Finds room for a w*h rectangle, adding a page when the current one is full.
// Returns the page index, or -1.
int CD3DHanFontAtlas::AllocateRect(int w, int h, int& outX, int& outY, LPDIRECT3DDEVICE9 pDevice)
{
    if (w > ATLAS_PAGE_SIZE || h > ATLAS_PAGE_SIZE) return -1;

    if (m_Pages.empty())
    {
        LPDIRECT3DTEXTURE9 pTex = NULL;
        if (!pDevice || FAILED(pDevice->CreateTexture(ATLAS_PAGE_SIZE, ATLAS_PAGE_SIZE, 1, 0,
            D3DFMT_A8L8, D3DPOOL_MANAGED, &pTex, NULL)))
            return -1;
        m_Pages.push_back(pTex);
        m_nCurX = m_nCurY = m_nShelfH = 0;
    }

    if (m_nCurX + w > ATLAS_PAGE_SIZE)		// next shelf
    {
        m_nCurX = 0;
        m_nCurY += m_nShelfH;
        m_nShelfH = 0;
    }

    if (m_nCurY + h > ATLAS_PAGE_SIZE)		// next page
    {
        if ((int)m_Pages.size() >= ATLAS_MAX_PAGES) return -1;

        LPDIRECT3DTEXTURE9 pTex = NULL;
        if (!pDevice || FAILED(pDevice->CreateTexture(ATLAS_PAGE_SIZE, ATLAS_PAGE_SIZE, 1, 0,
            D3DFMT_A8L8, D3DPOOL_MANAGED, &pTex, NULL)))
            return -1;
        m_Pages.push_back(pTex);
        m_nCurX = m_nCurY = m_nShelfH = 0;
    }

    outX = m_nCurX;
    outY = m_nCurY;
    m_nCurX += w + 1;
    if (h > m_nShelfH) m_nShelfH = h;
    return (int)m_Pages.size() - 1;
}

BOOL CD3DHanFontAtlas::RasteriseGlyph(wchar_t ch, GlyphInfo& out, LPDIRECT3DDEVICE9 pDevice)
{
    if (!m_hDC || !m_pBits) return FALSE;

    const int r = m_nOutlineRadius;
    int bw = 0, bh = 0, advance = 0;
    if (!AtumRasteriseGlyph(m_hDC, m_pBits, m_nBmpW, m_nBmpH, ch, r, m_nLineHeight,
                            &m_Face[0], &m_Union[0], bw, bh, advance))
        return FALSE;

    const BYTE* pFace = &m_Face[0];
    const BYTE* pUnion = &m_Union[0];

    int px = 0, py = 0;
    const int nPage = AllocateRect(bw, bh, px, py, pDevice);
    if (nPage < 0) return FALSE;

    D3DLOCKED_RECT lr;
    RECT rcLock = { px, py, px + bw, py + bh };
    if (FAILED(m_Pages[nPage]->LockRect(0, &lr, &rcLock, 0)))
        return FALSE;

    for (int y = 0; y < bh; ++y)
    {
        WORD* pDst = (WORD*)((BYTE*)lr.pBits + (size_t)y * lr.Pitch);
        const BYTE* pF = pFace + (size_t)y * m_nBmpW;
        const BYTE* pU = pUnion + (size_t)y * m_nBmpW;
        for (int x = 0; x < bw; ++x)
        {
            // A8L8: L is the face, A the face grown by the outline.
            *pDst++ = (WORD)(((WORD)pU[x] << 8) | pF[x]);
        }
    }
    m_Pages[nPage]->UnlockRect(0);

    out.nPage = nPage;
    out.nWidth = bw;
    out.nHeight = bh;
    out.nBearingX = -r;
    out.nBearingY = -r;
    out.nAdvanceX = advance;
    out.tu1 = (float)px / ATLAS_PAGE_SIZE;
    out.tv1 = (float)py / ATLAS_PAGE_SIZE;
    out.tu2 = (float)(px + bw) / ATLAS_PAGE_SIZE;
    out.tv2 = (float)(py + bh) / ATLAS_PAGE_SIZE;
    return TRUE;
}

const GlyphInfo* CD3DHanFontAtlas::GetGlyph(LPDIRECT3DDEVICE9 pDevice, wchar_t ch)
{
    std::map<wchar_t, GlyphInfo>::iterator it = m_Glyphs.find(ch);
    if (it != m_Glyphs.end())
        return &it->second;

    EnsureGDI();
    if (!m_hDC) return NULL;

    GlyphInfo info;
    if (!RasteriseGlyph(ch, info, pDevice))
        return NULL;

    m_Glyphs[ch] = info;
    return &m_Glyphs[ch];
}

///////////////////////////////////////////////////////////////////////////////
//  Colour tags
//
//  "\r red text \r" - the tag both opens and closes, and the string tables
//  pair them up (STRMSG_C_ITEM_0040 is "%s\\g[+%.1f]\\g").
///////////////////////////////////////////////////////////////////////////////
namespace
{
    struct ColorState
    {
        DWORD   dwBase;
        DWORD   dwCurrent;
        BOOL    bOpen;

        void Reset(DWORD dwBaseColor)
        {
            dwBase = dwCurrent = dwBaseColor;
            bOpen = FALSE;
        }

        void Apply(DWORD dwTagColor)
        {
            if (bOpen && dwCurrent == dwTagColor)
            {
                dwCurrent = dwBase;			// the closing half of a pair
                bOpen = FALSE;
            }
            else
            {
                dwCurrent = dwTagColor;
                bOpen = TRUE;
            }
        }
    };

    // A tag is a backslash followed by a colour letter.
    inline BOOL IsColorTag(const char* p, int i, int len, DWORD& dwOut)
    {
        if (p[i] != '\\' || i + 1 >= len) return FALSE;
        const DWORD c = GetFontColor(p[i + 1]);
        if (0 == c) return FALSE;
        dwOut = c;
        return TRUE;
    }
}

///////////////////////////////////////////////////////////////////////////////
//  CD3DHanFont
///////////////////////////////////////////////////////////////////////////////
CD3DHanFont::CD3DHanFont(TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags,
    BOOL outline, DWORD dwMaxWidth, DWORD dwMaxHeight,
    BOOL bCullText, BOOL bCullUV)
    : m_pTexture(NULL),
    m_pAtlas(NULL),
    m_pd3dDevice(NULL),
    m_dwTexWidth(dwMaxWidth),
    m_dwTexHeight(dwMaxHeight),
    m_dwFontHeight(dwHeight),
    m_dwFontFlags(dwFlags),
    m_bOutLine(outline),
    m_bCullText(bCullText),
    m_bCullUV(bCullUV),
    m_fPosX(0), m_fPosY(0),
    m_dwColor(0),
    m_fTx1(0), m_fTy1(0), m_fTx2(0), m_fTy2(0),
    m_fWidth(0),
    m_bReset(FALSE),
    m_bReLoadString(FALSE),
    m_bTextColor(TRUE),
    m_bSizeCheckValid(FALSE)
{
    SafeStrCpy(m_strFontName, strFontName, sizeof(m_strFontName));
    m_szCheckSize.cx = m_szCheckSize.cy = 0;

    if (m_dwFontHeight < 1) m_dwFontHeight = 1;
    m_pAtlas = AcquireAtlas(m_strFontName, (int)m_dwFontHeight, m_dwFontFlags, m_bOutLine);
}

CD3DHanFont::~CD3DHanFont()
{
    // The atlas outlives the instance; only the private string texture is ours.
    SAFE_RELEASE(m_pTexture);
}

void CD3DHanFont::SetUV(float tx1, float ty1, float tx2, float ty2)
{
    m_fTx1 = tx1; m_fTy1 = ty1; m_fTx2 = tx2; m_fTy2 = ty2;
}

HRESULT CD3DHanFont::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
    m_pd3dDevice = pd3dDevice;
    return S_OK;
}

HRESULT CD3DHanFont::RestoreDeviceObjects()
{
    return S_OK;
}

HRESULT CD3DHanFont::InvalidateDeviceObjects()
{
    // Atlas pages are D3DPOOL_MANAGED and survive a reset on their own.
    SAFE_RELEASE(m_pTexture);
    return S_OK;
}

HRESULT CD3DHanFont::DeleteDeviceObjects()
{
    SAFE_RELEASE(m_pTexture);
    m_pd3dDevice = NULL;
    return S_OK;
}

void CD3DHanFont::SetReLoadString(BOOL bReset)
{
    m_bReLoadString = bReset;
    m_bSizeCheckValid = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//  Measuring
///////////////////////////////////////////////////////////////////////////////
SIZE CD3DHanFont::GetStringSize(TCHAR* strText)
{
    SIZE size = { 0, 0 };
    if (!strText) return size;

    if (m_bSizeCheckValid && 0 == strcmp(m_strSizeCheckText.c_str(), strText))
        return m_szCheckSize;

    if (!m_pAtlas) return size;
    HDC hDC = m_pAtlas->GetFontDC();
    if (!hDC) return size;

    // Drop the colour tags, exactly as DrawText() skips them, so what is
    // measured is what gets drawn.
    const int len = (int)strlen(strText);
    std::string strPlain;
    strPlain.reserve(len);
    for (int i = 0; i < len; )
    {
        DWORD dwTag;
        if (m_bTextColor && IsColorTag(strText, i, len, dwTag)) { i += 2; continue; }
        strPlain += strText[i++];
    }

    if (!strPlain.empty())
    {
        std::wstring wide;
        const UINT uCP = AtumDetectCodePage(strPlain.c_str(), (int)strPlain.size(),
                                            LegacyCodePage());
        AtumDecodeText(uCP, strPlain.c_str(), (int)strPlain.size(), wide);
        if (!wide.empty())
            GetTextExtentPoint32W(hDC, wide.c_str(), (int)wide.size(), &size);
    }

    if (0 == size.cy)
        size.cy = m_pAtlas->GetLineHeight();

    m_strSizeCheckText = strText;
    m_szCheckSize = size;
    m_bSizeCheckValid = TRUE;
    return size;
}

///////////////////////////////////////////////////////////////////////////////
//  Drawing
///////////////////////////////////////////////////////////////////////////////
HRESULT CD3DHanFont::DrawText(FLOAT sx, FLOAT sy, DWORD dwColor,
    TCHAR* strText, DWORD dwFlags, RECT* i_pFillRect, BOOL bColorState)
{
    if (!m_pd3dDevice || !m_pAtlas || !strText || 0 == strText[0]) return S_OK;
    if (!EnsureSharedVB(m_pd3dDevice)) return S_OK;

    m_fPosX = sx; m_fPosY = sy;

    // Only the chat edit box needs the drawn string kept, and it is the only
    // caller that passes a fill rect.
    if (i_pFillRect)
    {
        m_strText = strText;
        DrawFillRect(i_pFillRect);
    }

    const int len = (int)strlen(strText);
    const UINT uCP = AtumDetectCodePage(strText, len, LegacyCodePage());

    // Decode once, remembering which colour each character ends up in.
    static std::wstring wide;
    static std::vector<DWORD> colors;
    static std::wstring runWide;
    static std::vector<const GlyphInfo*> glyphs;
    wide.clear();
    colors.clear();

    ColorState color;
    color.Reset(SwapRB(dwColor));

    const BOOL bTags = (bColorState && m_bTextColor);

    // Decoded a run at a time rather than a character at a time: a Vietnamese
    // vowel is a letter plus a combining mark, and composing the two needs
    // both in hand.
    int nRunStart = 0;
    for (int i = 0; i <= len; )
    {
        DWORD dwTag = 0;
        const BOOL bAtTag = (i < len) && bTags && IsColorTag(strText, i, len, dwTag);

        if (bAtTag || i == len)
        {
            if (i > nRunStart)
            {
                AtumDecodeText(uCP, strText + nRunStart, i - nRunStart, runWide);
                wide += runWide;
                colors.resize(wide.size(), color.dwCurrent);
            }
            if (i == len) break;

            color.Apply(SwapRB(dwTag));
            i += 2;
            nRunStart = i;
            continue;
        }

        // One source character is one or two bytes in the build's code page.
        i += (IsDBCSLeadByteEx(uCP, (BYTE)strText[i]) && i + 1 < len) ? 2 : 1;
    }

    if (wide.empty()) return S_OK;

    // Gather the glyphs first: rasterising one can add an atlas page, and the
    // vertex buffer must not be locked while that happens.
    const size_t nChars = wide.size();
    glyphs.clear();
    glyphs.resize(nChars, (const GlyphInfo*)NULL);
    for (size_t c = 0; c < nChars; ++c)
        glyphs[c] = m_pAtlas->GetGlyph(m_pd3dDevice, wide[c]);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    // Blending carries the antialiasing now, so the alpha test is only here to
    // skip the fully transparent border around each glyph.
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x01);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    // Alpha comes from the glyph alone.
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    if (dwFlags & D3DFONT_NOTFILTERED)
    {
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    }
    else
    {
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    }
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    m_pd3dDevice->SetFVF(D3DFVF_FONT2DVERTEX);
    m_pd3dDevice->SetStreamSource(0, g_pSharedVB, 0, sizeof(FONT2DVERTEX));

    // Text is nearly always all on one page, so a run per page keeps a string
    // to a single draw call.
    float currX = (float)floor(sx + 0.5f);
    const float penY = (float)floor(sy + 0.5f);
    size_t c = 0;
    BOOL bStop = FALSE;

    while (c < nChars && !bStop)
    {
        while (c < nChars && !glyphs[c]) ++c;
        if (c >= nChars) break;

        const int nPage = glyphs[c]->nPage;
        LPDIRECT3DTEXTURE9 pTex = m_pAtlas->GetPage(nPage);
        if (!pTex) { ++c; continue; }

        // Wrap rather than run off the end.
        if (g_nSharedVBChars + 128 > MAX_BATCH_CHARS) g_nSharedVBChars = 0;

        const int nBase = g_nSharedVBChars * 6;
        const UINT nOffset = nBase * sizeof(FONT2DVERTEX);
        // A size of 0 only means "the whole buffer" together with an offset of
        // 0; from part way in it has to be spelt out.
        const UINT nSize = (MAX_BATCH_CHARS - g_nSharedVBChars) * 6 * sizeof(FONT2DVERTEX);

        FONT2DVERTEX* pVerts = NULL;
        if (FAILED(g_pSharedVB->Lock(nOffset, nSize, (void**)&pVerts,
                                     (0 == g_nSharedVBChars) ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE)))
            break;

        int vCount = 0;
        while (c < nChars && glyphs[c] && glyphs[c]->nPage == nPage)
        {
            if (g_nSharedVBChars + vCount / 6 >= MAX_BATCH_CHARS) break;

            const GlyphInfo* g = glyphs[c];

            if (m_bCullText && m_fWidth > 0 &&
                (currX - (float)floor(sx + 0.5f)) + g->nAdvanceX > m_fWidth)
            {
                bStop = TRUE;
                break;
            }

            const float x0 = currX + g->nBearingX - 0.5f;
            const float y0 = penY + g->nBearingY - 0.5f;
            const float x1 = x0 + g->nWidth;
            const float y1 = y0 + g->nHeight;
            const DWORD col = colors[c];

            pVerts[vCount].p = D3DXVECTOR4(x0, y1, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu1; pVerts[vCount].tv = g->tv2; ++vCount;

            pVerts[vCount].p = D3DXVECTOR4(x0, y0, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu1; pVerts[vCount].tv = g->tv1; ++vCount;

            pVerts[vCount].p = D3DXVECTOR4(x1, y0, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu2; pVerts[vCount].tv = g->tv1; ++vCount;

            pVerts[vCount].p = D3DXVECTOR4(x1, y0, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu2; pVerts[vCount].tv = g->tv1; ++vCount;

            pVerts[vCount].p = D3DXVECTOR4(x1, y1, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu2; pVerts[vCount].tv = g->tv2; ++vCount;

            pVerts[vCount].p = D3DXVECTOR4(x0, y1, 0.5f, 1.0f);
            pVerts[vCount].color = col;
            pVerts[vCount].tu = g->tu1; pVerts[vCount].tv = g->tv2; ++vCount;

            currX += g->nAdvanceX;
            ++c;
        }

        g_pSharedVB->Unlock();

        if (vCount > 0)
        {
            m_pd3dDevice->SetTexture(0, pTex);
            m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, nBase, vCount / 3);
            g_nSharedVBChars += vCount / 6;
        }
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//  SetText - the electric boards call this and then map m_pTexture onto their
//  quad, so the whole string is rendered into a texture of its own.
///////////////////////////////////////////////////////////////////////////////
HRESULT CD3DHanFont::SetText(FLOAT sx, FLOAT sy, TCHAR* texts, DWORD color, RECT* i_pFillRect, BOOL bColorState)
{
    m_fPosX = sx;
    m_fPosY = sy;

    const std::string strNew = texts ? texts : "";
    if (m_pTexture && strNew == m_strText && color == m_dwColor)
        return S_OK;

    m_dwColor = color;
    m_strText = strNew;
    return RenderStringTexture();
}

HRESULT CD3DHanFont::RenderStringTexture()
{
    SAFE_RELEASE(m_pTexture);

    if (!m_pd3dDevice || !m_pAtlas || m_strText.empty())
        return S_OK;

    HDC hFontDC = m_pAtlas->GetFontDC();
    if (!hFontDC) return S_OK;

    std::wstring wide;
    AtumDecodeText(AtumDetectCodePage(m_strText.c_str(), (int)m_strText.size(),
                                      LegacyCodePage()),
                   m_strText.c_str(), (int)m_strText.size(), wide);
    if (wide.empty()) return S_OK;

    const int texW = (int)m_dwTexWidth;
    const int texH = (int)m_dwTexHeight;
    if (texW <= 0 || texH <= 0) return S_OK;

    if (FAILED(m_pd3dDevice->CreateTexture(texW, texH, 1, 0, D3DFMT_A8R8G8B8,
                                           D3DPOOL_MANAGED, &m_pTexture, NULL)))
    {
        m_pTexture = NULL;
        return E_FAIL;
    }

    BITMAPINFO bmi;
    ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = texW;
    bmi.bmiHeader.biHeight = -texH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;

    DWORD* pBits = NULL;
    HDC hMem = CreateCompatibleDC(hFontDC);
    if (!hMem) { SAFE_RELEASE(m_pTexture); return E_FAIL; }

    HBITMAP hBmp = CreateDIBSection(hMem, &bmi, DIB_RGB_COLORS, (VOID**)&pBits, NULL, 0);
    if (!hBmp) { DeleteDC(hMem); SAFE_RELEASE(m_pTexture); return E_FAIL; }

    HGDIOBJ hOldBmp = SelectObject(hMem, hBmp);
    HGDIOBJ hOldFont = SelectObject(hMem, GetCurrentObject(hFontDC, OBJ_FONT));
    SetMapMode(hMem, MM_TEXT);
    SetBkMode(hMem, TRANSPARENT);
    SetTextAlign(hMem, TA_TOP | TA_LEFT);

    RECT rcAll = { 0, 0, texW, texH };
    FillRect(hMem, &rcAll, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetTextColor(hMem, RGB(255, 255, 255));
    ExtTextOutW(hMem, 0, 0, 0, NULL, wide.c_str(), (UINT)wide.size(), NULL);
    GdiFlush();

    const DWORD dwRGB = m_dwColor & 0x00FFFFFF;
    D3DLOCKED_RECT lr;
    if (SUCCEEDED(m_pTexture->LockRect(0, &lr, NULL, 0)))
    {
        for (int y = 0; y < texH; ++y)
        {
            DWORD* pDst = (DWORD*)((BYTE*)lr.pBits + (size_t)y * lr.Pitch);
            const DWORD* pSrc = pBits + (size_t)y * texW;
            for (int x = 0; x < texW; ++x)
            {
                const BYTE cov = (BYTE)(pSrc[x] & 0xFF);
                *pDst++ = ((DWORD)cov << 24) | dwRGB;
            }
        }
        m_pTexture->UnlockRect(0);
    }

    SelectObject(hMem, hOldFont);
    SelectObject(hMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMem);

    m_bReset = TRUE;
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//  The chat edit box asks for the selected run to be highlighted.
///////////////////////////////////////////////////////////////////////////////
extern int GetStringBuffPos(char* str, int pos);

void CD3DHanFont::DrawFillRect(RECT* i_pFillRect)
{
    if (NULL == i_pFillRect || !m_pAtlas) return;

    HDC hDC = g_pApp->GetHDC();
    if (!hDC) return;

    char szText[2048];
    SafeStrCpy(szText, m_strText.c_str(), sizeof(szText));

    const int nStartPos = GetStringBuffPos(szText, i_pFillRect->left);
    const int nEndPos = GetStringBuffPos(szText, i_pFillRect->right);
    if ((-1 == nStartPos) || (-1 == nEndPos)) return;

    const int nCopyLen = nEndPos - nStartPos;
    if (nCopyLen <= 0) return;

    char chPreString[256] = { 0 };
    char chRectString[256] = { 0 };
    if (nStartPos < 256) SafeStrCpy(chPreString, szText, nStartPos + 1);
    if (nCopyLen < 256)  SafeStrCpy(chRectString, &szText[nStartPos], nCopyLen + 1);

    SIZE sizePre, sizeRect;
    GetTextExtentPoint32(hDC, chPreString, (int)strlen(chPreString), &sizePre);
    GetTextExtentPoint32(hDC, chRectString, (int)strlen(chRectString), &sizeRect);

    RECT rcRender;
    rcRender.left = sizePre.cx;
    rcRender.top = 0;
    rcRender.right = rcRender.left + sizeRect.cx;
    rcRender.bottom = rcRender.top + sizeRect.cy;

    FillRect(hDC, &rcRender, (HBRUSH)(COLOR_GRAYTEXT));
}

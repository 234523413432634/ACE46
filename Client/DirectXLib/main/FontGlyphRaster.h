// FontGlyphRaster.h: turning a GDI glyph into coverage, and growing it.
//
// Two 8 bit coverage maps come out of it.

#ifndef _ATUM_FONT_GLYPH_RASTER_H_
#define _ATUM_FONT_GLYPH_RASTER_H_

#include <windows.h>
#include <string.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Is this text UTF-8?
//
// Nothing in the protocol says what encoding a string is in - it is a bag of
// bytes, and the same bytes are Cyrillic in one code page and Latin in
// another, which is the whole reason text from the wrong language mojibakes.
///////////////////////////////////////////////////////////////////////////////
inline BOOL AtumLooksLikeUtf8(const char* i_pBytes, int i_nBytes)
{
    if (!i_pBytes || i_nBytes <= 0) return FALSE;

    BOOL bAnyMultiByte = FALSE;
    int i = 0;
    while (i < i_nBytes)
    {
        const BYTE b = (BYTE)i_pBytes[i];
        int nTrail;

        if (b < 0x80)                      { ++i; continue; }
        else if (b >= 0xC2 && b <= 0xDF)   nTrail = 1;
        else if (b >= 0xE0 && b <= 0xEF)   nTrail = 2;
        else if (b >= 0xF0 && b <= 0xF4)   nTrail = 3;
        else                               return FALSE;	// C0, C1 and F5+ never start one

        if (i + nTrail >= i_nBytes) return FALSE;
        for (int k = 1; k <= nTrail; ++k)
        {
            const BYTE t = (BYTE)i_pBytes[i + k];
            if (t < 0x80 || t > 0xBF) return FALSE;
        }

        bAnyMultiByte = TRUE;
        i += nTrail + 1;
    }
    return bAnyMultiByte;
}

///////////////////////////////////////////////////////////////////////////////
// The code page a string should be read as: UTF-8 when it is one, otherwise the
// legacy code page the caller nominates.
///////////////////////////////////////////////////////////////////////////////
inline UINT AtumDetectCodePage(const char* i_pBytes, int i_nBytes, UINT i_uLegacy)
{
    return AtumLooksLikeUtf8(i_pBytes, i_nBytes) ? CP_UTF8 : i_uLegacy;
}

///////////////////////////////////////////////////////////////////////////////
// Decodes text from a code page into UTF-16, composed.
//
// The composing matters for Vietnamese.
///////////////////////////////////////////////////////////////////////////////
inline void AtumDecodeText(UINT i_uCodePage, const char* i_pBytes, int i_nBytes,
                           std::wstring& o_strOut)
{
    o_strOut.clear();
    if (!i_pBytes || i_nBytes <= 0) return;

    const int n = MultiByteToWideChar(i_uCodePage, 0, i_pBytes, i_nBytes, NULL, 0);
    if (n <= 0) return;

    o_strOut.resize(n);
    MultiByteToWideChar(i_uCodePage, 0, i_pBytes, i_nBytes, &o_strOut[0], n);

    // NormalizeString is Vista and later, and lives in Normaliz.dll; resolving
    // it by hand keeps the link free of it and simply skips composing if it is
    // ever missing.
    typedef int (WINAPI *PFN_NormalizeString)(int, LPCWSTR, int, LPWSTR, int);
    static PFN_NormalizeString s_pfn = NULL;
    static BOOL s_bResolved = FALSE;
    if (!s_bResolved)
    {
        s_bResolved = TRUE;
        HMODULE hMod = GetModuleHandleA("Normaliz.dll");
        if (!hMod) hMod = LoadLibraryA("Normaliz.dll");
        if (hMod) s_pfn = (PFN_NormalizeString)GetProcAddress(hMod, "NormalizeString");
    }
    if (!s_pfn) return;

    const int NormalizationC = 1;
    std::wstring composed;
    composed.resize(n);
    int r = s_pfn(NormalizationC, o_strOut.c_str(), n, &composed[0], n);
    if (r <= 0)
    {
        // Only grows for text that needs it; ERROR_INSUFFICIENT_BUFFER returns
        // the required length negated.
        if (r < 0)
        {
            composed.resize(-r);
            r = s_pfn(NormalizationC, o_strOut.c_str(), n, &composed[0], -r);
        }
        if (r <= 0) return;
    }
    composed.resize(r);
    o_strOut.swap(composed);
}

///////////////////////////////////////////////////////////////////////////////
// The blend CD3DHanFont::DrawText sets the fixed function pipeline up to do.
///////////////////////////////////////////////////////////////////////////////
inline DWORD AtumCompositeGlyphPixel(DWORD i_dwTextColour, BYTE i_L, BYTE i_A, DWORD i_dwDest)
{
    if (0 == i_A) return i_dwDest;

    const int sr = ((i_dwTextColour >> 16) & 0xFF) * i_L / 255;
    const int sg = ((i_dwTextColour >> 8) & 0xFF) * i_L / 255;
    const int sb = ((i_dwTextColour) & 0xFF) * i_L / 255;

    const int dr = (i_dwDest >> 16) & 0xFF;
    const int dg = (i_dwDest >> 8) & 0xFF;
    const int db = (i_dwDest) & 0xFF;

    const int a = i_A;
    const int r = (sr * a + dr * (255 - a)) / 255;
    const int g = (sg * a + dg * (255 - a)) / 255;
    const int b = (sb * a + db * (255 - a)) / 255;
    return ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
}

///////////////////////////////////////////////////////////////////////////////
// Grows i_pSrc by i_nRadius using a round kernel, into o_pDst.
///////////////////////////////////////////////////////////////////////////////
inline void AtumDilateCoverage(const BYTE* i_pSrc, BYTE* o_pDst,
                               int i_nStride, int i_nW, int i_nH, int i_nRadius)
{
    if (i_nRadius <= 0)
    {
        for (int y = 0; y < i_nH; ++y)
            memcpy(o_pDst + (size_t)y * i_nStride, i_pSrc + (size_t)y * i_nStride, i_nW);
        return;
    }

    const int r = i_nRadius;
    const int r2 = r * r;

    for (int y = 0; y < i_nH; ++y)
    {
        BYTE* pDst = o_pDst + (size_t)y * i_nStride;
        for (int x = 0; x < i_nW; ++x)
        {
            BYTE best = 0;
            for (int dy = -r; dy <= r; ++dy)
            {
                const int sy = y + dy;
                if (sy < 0 || sy >= i_nH) continue;

                const BYTE* pSrc = i_pSrc + (size_t)sy * i_nStride;
                for (int dx = -r; dx <= r; ++dx)
                {
                    if (dx * dx + dy * dy > r2) continue;
                    const int sx = x + dx;
                    if (sx < 0 || sx >= i_nW) continue;
                    if (pSrc[sx] > best) best = pSrc[sx];
                }
                if (255 == best) break;
            }
            pDst[x] = best;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draws one character into the DIB currently selected into i_hDC and reads it
// back as coverage.
///////////////////////////////////////////////////////////////////////////////
inline BOOL AtumRasteriseGlyph(HDC i_hDC, const DWORD* i_pDIBBits,
                               int i_nBmpW, int i_nBmpH,
                               wchar_t i_ch, int i_nRadius, int i_nFallbackHeight,
                               BYTE* o_pFace, BYTE* o_pUnion,
                               int& o_nW, int& o_nH, int& o_nAdvance)
{
    if (!i_hDC || !i_pDIBBits || !o_pFace || !o_pUnion) return FALSE;

    SIZE size;
    if (!GetTextExtentPoint32W(i_hDC, &i_ch, 1, &size)) return FALSE;

    o_nAdvance = size.cx;

    int gw = (size.cx > 0) ? size.cx : 1;
    int gh = (size.cy > 0) ? size.cy : i_nFallbackHeight;

    // Never read outside the scratch bitmap, whatever the face reports.
    if (gw + 2 * i_nRadius > i_nBmpW) gw = i_nBmpW - 2 * i_nRadius;
    if (gh + 2 * i_nRadius > i_nBmpH) gh = i_nBmpH - 2 * i_nRadius;
    if (gw <= 0 || gh <= 0) return FALSE;

    o_nW = gw + 2 * i_nRadius;
    o_nH = gh + 2 * i_nRadius;

    RECT rc = { 0, 0, i_nBmpW, i_nBmpH };
    FillRect(i_hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    ExtTextOutW(i_hDC, i_nRadius, i_nRadius, 0, NULL, &i_ch, 1, NULL);
    GdiFlush();

    memset(o_pFace, 0, (size_t)i_nBmpW * i_nBmpH);
    for (int y = 0; y < o_nH; ++y)
    {
        const DWORD* pSrc = i_pDIBBits + (size_t)y * i_nBmpW;
        BYTE* pDst = o_pFace + (size_t)y * i_nBmpW;
        for (int x = 0; x < o_nW; ++x)
        {
            // ANTIALIASED_QUALITY writes white on black through a grey ramp,
            // so any one channel is the coverage.
            pDst[x] = (BYTE)(pSrc[x] & 0xFF);
        }
    }

    memset(o_pUnion, 0, (size_t)i_nBmpW * i_nBmpH);
    AtumDilateCoverage(o_pFace, o_pUnion, i_nBmpW, o_nW, o_nH, i_nRadius);
    return TRUE;
}

#endif	// _ATUM_FONT_GLYPH_RASTER_H_

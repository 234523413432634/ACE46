// D3DHanFont.h: Optimized implementation of the CD3DHanFont class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_D3DHANFONT_H__A9C46610_34BE_44B5_9EEA_A6D9D2DC313E__INCLUDED_)
#define AFX_D3DHANFONT_H__A9C46610_34BE_44B5_9EEA_A6D9D2DC313E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <vector>
#include <string>

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004
#define D3DFONT_NOTFILTERED 0x0008

// Vertex structure
struct FONT2DVERTEX {
    D3DXVECTOR4 p;
    DWORD color;
    FLOAT tu, tv;
    FONT2DVERTEX() : p(0, 0, 0, 0), color(0), tu(0), tv(0) {}
};

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct GlyphInfo
{
    float tu1, tv1, tu2, tv2;
    int   nWidth;
    int   nHeight;
    int   nAdvanceX;
};

class CD3DHanFont
{
public:
    CD3DHanFont(TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L,
        BOOL outline = FALSE, DWORD dwMaxWidth = 256L, DWORD dwMaxHeight = 32,
        BOOL bCullText = FALSE, BOOL bCullUV = FALSE);
    ~CD3DHanFont();

    HRESULT SetText(FLOAT sx, FLOAT sy, TCHAR* texts, DWORD color, RECT* i_pFillRect = NULL, BOOL bColorState = TRUE);
    HRESULT DrawText(FLOAT x, FLOAT y, DWORD dwColor, TCHAR* strText, DWORD dwFlags = 0L, RECT* i_pFillRect = NULL, BOOL bColorState = TRUE);

    HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    void SetUV(float tx1, float ty1, float tx2, float ty2);
    void SetTextureWidth(float fWidth) { m_fWidth = fWidth; }
    SIZE GetStringSize(TCHAR* strText);

    DWORD GetTexWidth() { return m_dwTexWidth; }
    DWORD GetTexHeight() { return m_dwTexHeight; }
    BOOL GetReset() { return m_bReset; }

    void SetTexWidth(DWORD dwWidth) { m_dwTexWidth = dwWidth; }
    void SetTexHeight(DWORD dwHeight) { m_dwTexHeight = dwHeight; }

    void SetReLoadString(BOOL bReset);
    void SetTextColorMode(BOOL bMode) { m_bTextColor = bMode; }

    LPDIRECT3DTEXTURE9      m_pTexture;

private:
    void    DrawFillRect(RECT* i_pFillRect);
    void    PrepareGDIResources();
    void    CleanupGDIResources();
    const GlyphInfo* GetGlyph(unsigned int charCode);
    HRESULT CreateAtlasTexture();
    void    FlushGlyphCache();

    // D3D Objects
    LPDIRECT3DDEVICE9       m_pd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;

    // Atlas State
    DWORD                   m_dwTexWidth;
    DWORD                   m_dwTexHeight;
    DWORD                   m_dwCurrentX;
    DWORD                   m_dwCurrentY;
    DWORD                   m_dwRowHeight;

    // Cache Lookup
    std::map<unsigned int, GlyphInfo> m_GlyphCache;

    // GDI Persistent Objects
    HFONT                   m_hFontGD;
    HDC                     m_hDC;
    HBITMAP                 m_hBitmap;
    DWORD* m_pBitmapBits;

    // Font Properties
    TCHAR                   m_strFontName[80];
    DWORD                   m_dwFontHeight;
    DWORD                   m_dwFontFlags;
    BOOL                    m_bOutLine;

    // State members
    BOOL                    m_bCullText;
    BOOL                    m_bCullUV;
    TCHAR                   m_strText[2048];
    FLOAT                   m_fPosX, m_fPosY;
    DWORD                   m_dwColor;

    FLOAT                   m_fTx1, m_fTy1, m_fTx2, m_fTy2;
    FLOAT                   m_fWidth;
    FLOAT                   m_fTextScale;
    FLOAT                   m_fTexCoords[1][4];

    BOOL                    m_bReset;
    BOOL                    m_bReLoadString;
    BOOL                    m_bTextColor;

    // Caching for string size
    TCHAR                   m_strSizeCheckText[256];
    SIZE                    m_szCheckSize;
};

#endif // !defined(AFX_D3DHANFONT_H__A9C46610_34BE_44B5_9EEA_A6D9D2DC313E__INCLUDED_)
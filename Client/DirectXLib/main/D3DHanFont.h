// D3DHanFont.h: interface for the CD3DHanFont class.
//
// Text is drawn from a glyph atlas that is *shared* between every CD3DHanFont
// asking for the same face, size and style.

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
#include "UIScale.h"

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
    int   nPage;			// which atlas page holds it
    int   nWidth;			// size of the cached bitmap, outline included
    int   nHeight;
    int   nBearingX;		// where that bitmap sits relative to the pen
    int   nBearingY;
    int   nAdvanceX;		// what the pen moves by, outline excluded
};

class CD3DHanFontAtlas;

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
    // The width text is culled at, given in layout pixels like a position.
    void SetTextureWidth(float fWidth) { m_fWidth = fWidth * UIScale(); }
    SIZE GetStringSize(TCHAR* strText);

    DWORD GetTexWidth() { return m_dwTexWidth; }
    DWORD GetTexHeight() { return m_dwTexHeight; }
    BOOL GetReset() { return m_bReset; }

    void SetTexWidth(DWORD dwWidth) { m_dwTexWidth = dwWidth; }
    void SetTexHeight(DWORD dwHeight) { m_dwTexHeight = dwHeight; }

    void SetReLoadString(BOOL bReset);
    void SetTextColorMode(BOOL bMode) { m_bTextColor = bMode; }

    // Only the electric boards use this.
    LPDIRECT3DTEXTURE9      m_pTexture;

    // The shared atlases and the shared vertex buffer belong to no single
    // instance, so the device reset path has to reach them once, not once per
    // CD3DHanFont.
    static void ReleaseSharedDeviceObjects();
    static void ReleaseAllAtlases();

    // Text that is valid UTF-8 is read as UTF-8 whatever this says, so a data
    // file or a chat line can carry any language - or several - on its own
    // terms.
    static void SetLegacyCodePage(UINT i_uCodePage);
    static UINT GetLegacyCodePage();

private:
    void    DrawFillRect(RECT* i_pFillRect);
    HRESULT RenderStringTexture();

    CD3DHanFontAtlas*       m_pAtlas;		// shared; owned by the atlas registry

    LPDIRECT3DDEVICE9       m_pd3dDevice;

    // Size of the private string texture (electric boards only).
    DWORD                   m_dwTexWidth;
    DWORD                   m_dwTexHeight;

    // Font properties
    TCHAR                   m_strFontName[80];
    DWORD                   m_dwFontHeight;
    DWORD                   m_dwFontFlags;
    BOOL                    m_bOutLine;

    // State
    BOOL                    m_bCullText;
    BOOL                    m_bCullUV;
    std::string             m_strText;
    FLOAT                   m_fPosX, m_fPosY;
    DWORD                   m_dwColor;

    FLOAT                   m_fTx1, m_fTy1, m_fTx2, m_fTy2;
    FLOAT                   m_fWidth;

    BOOL                    m_bReset;
    BOOL                    m_bReLoadString;
    BOOL                    m_bTextColor;

    // GetStringSize is called about as often as DrawText, and almost always
    // with the string it was last called with.
    std::string             m_strSizeCheckText;
    SIZE                    m_szCheckSize;
    BOOL                    m_bSizeCheckValid;
};

#endif // !defined(AFX_D3DHANFONT_H__A9C46610_34BE_44B5_9EEA_A6D9D2DC313E__INCLUDED_)

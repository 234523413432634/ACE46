#include "SpriteDrawFix.h"
#include "UIScale.h"
#include <math.h>

// How wide and tall the piece of texture being drawn is, which is what the
// scaling factor multiplies.
static bool SourceExtent(LPDIRECT3DTEXTURE9 pSrcTexture, CONST RECT* pSrcRect,
						 float& o_fWidth, float& o_fHeight)
{
	if(pSrcRect)
	{
		o_fWidth  = (float)(pSrcRect->right - pSrcRect->left);
		o_fHeight = (float)(pSrcRect->bottom - pSrcRect->top);
		return o_fWidth > 0.0f && o_fHeight > 0.0f;
	}

	D3DSURFACE_DESC desc;
	if(pSrcTexture && SUCCEEDED(pSrcTexture->GetLevelDesc(0, &desc)))
	{
		o_fWidth  = (float)desc.Width;
		o_fHeight = (float)desc.Height;
		return o_fWidth > 0.0f && o_fHeight > 0.0f;
	}
	return false;
}

static float Round(float f)
{
	return (float)floor(f + 0.5f);
}

// Up to the next whole pixel, with enough slack that a size which is already
// whole is left alone.
static float CeilExtent(float f)
{
	return (float)ceil(f - 0.01f);
}

HRESULT SpriteDrawFix(
	LPD3DXSPRITE pSprite,				// The sprite
	LPDIRECT3DTEXTURE9 pSrcTexture,		// The texture to draw.
	CONST RECT* pSrcRect,				// The src rect
	CONST D3DXVECTOR2* pScaling,		// Scaling
	CONST D3DXVECTOR2* pRotationCenter,	// The rotation center if rotation is used.
	FLOAT Rotation,						// Rotation in radians.
	CONST D3DXVECTOR2* pTranslation,	// Translation, i.e. moving the object to this place.
	D3DCOLOR Color)						// Color to module the image pixels.
{
	D3DXMATRIX m;

	const float fUI = UIScale();

	// A caller works in layout pixels throughout; this is where both the size
	// and the position become back buffer pixels.
	D3DXVECTOR2 v2Scaling = pScaling
		? D3DXVECTOR2(pScaling->x * fUI, pScaling->y * fUI)
		: D3DXVECTOR2(fUI, fUI);
	const D3DXVECTOR2 v2Center = pRotationCenter
		? D3DXVECTOR2(pRotationCenter->x * fUI, pRotationCenter->y * fUI)
		: D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR2 v2Translation = pTranslation
		? D3DXVECTOR2(pTranslation->x * fUI, pTranslation->y * fUI)
		: D3DXVECTOR2(0.0f, 0.0f);

	// Land on whole pixels. Position rounds and size rounds up, each on its own:
	// deriving the size from two rounded edges instead makes it breathe by a
	// pixel from frame to frame as the thing moves, which is very visible on a
	// health bar following a monster.
	float fSrcW, fSrcH;
	if(0.0f == Rotation && SourceExtent(pSrcTexture, pSrcRect, fSrcW, fSrcH))
	{
		const float fWidth  = CeilExtent(fSrcW * v2Scaling.x);
		const float fHeight = CeilExtent(fSrcH * v2Scaling.y);

		v2Translation.x = Round(v2Translation.x);
		v2Translation.y = Round(v2Translation.y);
		v2Scaling.x = fWidth / fSrcW;
		v2Scaling.y = fHeight / fSrcH;
	}

	D3DXMatrixTransformation2D(&m, nullptr, 0, &v2Scaling, &v2Center, -Rotation, &v2Translation);

	pSprite->SetTransform(&m);

	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	auto hr = pSprite->Draw(pSrcTexture, pSrcRect, nullptr, nullptr, Color); // 2016-01-08 exception at this point, GetIBaseTexture (7)
	pSprite->End();

	return hr;
}

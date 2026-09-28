// UIScale.h: how big the interface is drawn.
//
// The interface artwork and every hard-coded offset in the INF* files were
// authored against a 1920x1080 back buffer.

#if !defined(UISCALE_H__0F5B1E42_1C7A_4B54_9E0D_B6A3E4C5D210__INCLUDED_)
#define UISCALE_H__0F5B1E42_1C7A_4B54_9E0D_B6A3E4C5D210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The buffer the interface was drawn for.
#define UI_BASE_WIDTH					1920.0f
#define UI_BASE_HEIGHT					1080.0f

// UI_SCALE, the user's multiplier on top of the resolution scale.
#define UI_SCALE_DEFAULT				1.00f
#define UI_SCALE_MIN					0.75f
#define UI_SCALE_MAX					3.00f
#define UI_SCALE_STEP					0.05f

// Back buffer height over UI_BASE_HEIGHT, clamped so it never goes below 1.
float GetUIResolutionScale();

// UI_SCALE, from the uiscale entry in setupinfo.ver - see CInterface::LoadUIScale.
// Fonts take the scale when they are built, so it is set once at start-up.
float GetUIScale();
void  SetUIScale(float fScale);

// What turns a layout pixel into a back buffer pixel.  Panels do not call this;
// SpriteDrawFix(), CD3DHanFont and CheckMouseReverse() do it for them.  It is
// here for the few places that have to size something by hand.
inline float UIScale()
{
	return GetUIResolutionScale() * GetUIScale();
}

// The screen, in the layout pixels the interface positions things in.
int UIScreenW();
int UIScreenH();

// A point that came out of a world-to-screen projection is in back buffer
// pixels, because it was projected through the real viewport.
inline int UIFromPixels(int nPixels)
{
	return (int)(nPixels / UIScale());
}

// Scaling a length that is about to become a pixel position.
inline int UIScaleI(float fValue)
{
	const float fScaled = fValue * UIScale();
	return (int)(fScaled < 0.0f ? fScaled - 0.5f : fScaled + 0.5f);
}

// The other way, for a layout position that is about to be measured against
// where the scene projected something, which is in back buffer pixels.
inline int UIToPixels(int nLayout)
{
	return UIScaleI((float)nLayout);
}

#endif // !defined(UISCALE_H__0F5B1E42_1C7A_4B54_9E0D_B6A3E4C5D210__INCLUDED_)

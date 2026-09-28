// UIScale.cpp: implementation of the interface scale.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIScale.h"
#include "AtumApplication.h"

extern CAtumApplication * g_pD3dApp;

static float s_fUIScale = UI_SCALE_DEFAULT;

float GetUIResolutionScale()
{
	if(NULL == g_pD3dApp)
	{
		return 1.0f;
	}

	const float fScale = (float)g_pD3dApp->GetBackBufferDesc().Height / UI_BASE_HEIGHT;
	return (fScale < 1.0f) ? 1.0f : fScale;
}

int UIScreenW()
{
	if(NULL == g_pD3dApp)
	{
		return (int)UI_BASE_WIDTH;
	}
	return (int)(g_pD3dApp->GetBackBufferDesc().Width / UIScale());
}

int UIScreenH()
{
	if(NULL == g_pD3dApp)
	{
		return (int)UI_BASE_HEIGHT;
	}
	return (int)(g_pD3dApp->GetBackBufferDesc().Height / UIScale());
}

float GetUIScale()
{
	return s_fUIScale;
}

void SetUIScale(float fScale)
{
	if(fScale < UI_SCALE_MIN)		fScale = UI_SCALE_MIN;
	else if(fScale > UI_SCALE_MAX)	fScale = UI_SCALE_MAX;

	s_fUIScale = fScale;
}

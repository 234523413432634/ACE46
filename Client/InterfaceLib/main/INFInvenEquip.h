// INFInvenEquip.h: interface for the CINFInvenEquip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFINVENEQUIP_H__EE8904EF_85C9_4027_AEA3_FD245E12082A__INCLUDED_)
#define AFX_INFINVENEQUIP_H__EE8904EF_85C9_4027_AEA3_FD245E12082A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"
#include "INFImageBtn.h"
#include "INFImageBtnBoth.h"
#include "INFImportantInfo.h"
#include "INFInvenExtend.h"

#define		MAX_WEIGHT		11
#define     UNIT_SCALE		0.011f

class CINFInvenEquip  : public CINFBase  
{
public:
	CINFInvenEquip(CAtumNode* pParent);
	virtual ~CINFInvenEquip();

	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	
	void Render();
	void Tick();

	int WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bShowInven);

	int OnMouseWhell(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	
	BOOL IsShowWnd();
	void ShowWnd(BOOL bShow, POINT* i_ptPos= NULL);

	void RenderInvenBack(POINT ptPos, SIZE szSize);
	void RenderMirror(POINT *pMirrorPos=NULL);

	D3DXMATRIX	GetInvenMatInven();
	void		SetWearPosition(int iWearPosition);
	POINT	GetBkPos();

	void InitEquipPosInfo ( void );
	CPosData *GetEquipPosInfo ( int iKind );
	void SetBkPos(POINT ptBkPos);
	enum SPIWPTOOLTIPSTATE {SPIWP_NONE = 0, SPI_OK, WP_OK};
	void SetTradeItemCenterState(BOOL bState){m_bTradeItemCenterState = bState;}

private:
	BOOL IsMouseCaps(POINT ptPos);
	BOOL IsWndRect(POINT ptPos);
	void UpdateBtnPos();
	BOOL GetResolutionPos(POINT ptPixel, float &fPosX,float &fPosY,float &fPosZ,float &fScaling);

	void RenderWearPosition( int iWearPosition );
	void RenderAttachItem(int nPosX, int nPosY);	

	int		ProcessRotationUnitWnd(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	int		ProcessRotationUnit(POINT pt, UINT uMsg);
	void	SetByPushButton(BOOL bFlag);

	void RenderSPIWPTooltip();
	
private:
	BOOL m_bShowWnd;

	POINT	m_ptBkPos;	
	BOOL						m_bMove;
	POINT						m_ptCommOpMouse;

	LPDIRECT3DVERTEXBUFFER9 m_pMirrorVB;
	LPDIRECT3DTEXTURE9	m_pMirrorTexture;
	D3DXMATRIX			m_pMatInvenBackProj;
	
	D3DXMATRIX			m_pMatInven;
	D3DXMATRIX			m_pMatInvenWeaponSetPosition[4];
	D3DXMATRIX			m_pMatInvenWeaponOrgPosition[4];

	CINFImageEx*		m_pWearPositionBlink;
	int					m_iWearPosition;
	CRangeTime			m_tBlinkTime;

	// Rotation
	int					m_nRotationState;
	int					m_nButtonState;
	
	BOOL				m_bTurnLeft;
	BOOL				m_bTurnRight;
	BOOL				m_bTurnUp;
	BOOL				m_bTurnDown;
	BOOL				m_bByPush[4];

	CINFImageEx*		m_pInvenRotationBase;
	CINFImageEx*		m_pInvenDirection[INVEN_ROTATION_STATE][INVEN_ROTATION_NUM];

	CD3DHanFont*		m_pFontItemNum;

	CPosData			m_cEquipPosInfo[MAX_EQUIP_POS];

	CD3DHanFont*		m_pFontSPIWPToolTip;
	POINT				m_SPIWPToolTipPos;
	SPIWPTOOLTIPSTATE	m_nSPIWPToolTipState;

	BOOL				m_bTradeItemCenterState;
	

};

#endif // !defined(AFX_INFINVENEQUIP_H__EE8904EF_85C9_4027_AEA3_FD245E12082A__INCLUDED_)

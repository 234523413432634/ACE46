#include "stdafx.h"
#include "RangeTime.h"
#include "INFInven.h"
#include "INFGameMain.h"
#include "AtumApplication.h"
#include "INFCityBase.h"
#include "GameDataLast.h"
#include "INFImage.h"
#include "D3DHanFont.h"
#include "StoreData.h"
#include "INFTrade.h"
#include "Chat.h"
#include "INFIcon.h"
#include "AtumDatabase.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				 
#include "FieldWinSocket.h"
#include "INFWindow.h"
#include "ItemInfo.h"
#include "AtumSound.h"
#include "Skill.h"
#include "Interface.h"
#include "INFCharacterInfoExtend.h"
#include "INFCommunity.h"
#include "INFGameMainQSlot.h"
#include "dxutil.h"
#include "TutorialSystem.h"
#include "INFChangeCharactor.h"
#include "ClientParty.h"
#include "INFImageEx.h"

#define INVEN_SLOT_SIZE			30
#define INVEN_SLOT_INTERVAL		32
#define INVEN_ITEM_SLOT_START_X 309
#define INVEN_ITEM_SLOT_START_Y 30
#define INVEN_SCROLL_LINE_START_X		410
#define INVEN_SCROLL_LINE_START_Y		30
#define INVEN_SCROLL_BUTTON_START_X		406
#define INVEN_SCROLL_BUTTON_START_Y		30
#define INVEN_SCROLL_BUTTON_WIDTH		11
#define INVEN_SCROLL_BUTTON_HEIGHT		38
#define INVEN_SCROLL_LINE_LENGTH		190 
#define INVEN_SCROLL_LINE_MOVE_LENGTH	(INVEN_SCROLL_LINE_LENGTH - INVEN_SCROLL_BUTTON_HEIGHT)
#define INVEN_SCROLL_NUMBER				((((float)(g_pStoreData->m_mapItemUniqueNumber.size()/ INVEN_X_NUMBER)-2 )<0)?0:(g_pStoreData->m_mapItemUniqueNumber.size() / INVEN_X_NUMBER-2)) 
#define INVEN_SCROLL_INTERVAL			(INVEN_SCROLL_LINE_LENGTH / SetScrollLine())
#define INVEN_SPI_START_X				414 
#define INVEN_GARBAGE_START_X			271
#define INVEN_GARBAGE_START_Y			221
#define INVEN_GARBAGE_SIZE				24
#define INVEN_NOT_USE_SLOT_ICON			"07900270"
#define POS_PROW_X			16		 
#define POS_PROW_Y			102
#define	POS_PROWIN_X		265		 
#define	POS_PROWIN_Y		102	
#define	POS_PROWOUT_X		16		 
#define	POS_PROWOUT_Y		67	
#define	POS_WINGIN_X		141		 
#define	POS_WINGIN_Y		172 
#define	POS_WINGOUT_X		265		 
#define	POS_WINGOUT_Y		67	
#define	POS_CENTER_X		16		 
#define	POS_CENTER_Y		172	
#define	POS_REAR_X			265		 
#define	POS_REAR_Y			137	
#define	POS_ATTACHMENT_X	16		 
#define	POS_ATTACHMENT_Y	137	
#define POS_PET_X			265		 
#define POS_PET_Y			172	
#define CITY_SHOP_INVEN_START_Y		(CITY_BASE_NPC_BOX_START_Y - SIZE_NORMAL_WINDOW_Y)
#define ROTATION_NONE			-1
#define ROTATION_LEFT			0
#define ROTATION_RIGHT			1
#define ROTATION_UP				2
#define ROTATION_DOWN			3
#define ROTATION_ORIGINAL		4
#define ROTATION_STATE_N		-1		 
#define ROTATION_STATE_O		0		 
#define ROTATION_STATE_P		1		 
#define ROTATION_BASE_GAB_W
#define ROTATION_BASE_GAB_H
#define INVEN_BACK_W			388
#define INVEN_BACK_H			237
#define INVEN_BACK_POS_X		19
#define INVEN_BACK_POS_Y		19
#define INVEN_FULL_BACK_W		426
#define INVEN_FULL_BACK_H		275
#define INVEN_WARPOINT_Y		242
#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)|| defined(LANGUAGE_THAI) 
#define INVEN_SPI_START_Y		222 
#define SPI_START_X				70 
#define SPI_START_Y				3 
#define FONTWEITGHT_X1			98
#define FONTWEITGHT_X2			65
#else
#define INVEN_SPI_START_Y		222 
#define SPI_START_X				68 
#define SPI_START_Y				3 
#define FONTWEITGHT_X1			87
#define FONTWEITGHT_X2			50
#endif

struct MIRRORVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR2 uv;
	static const DWORD FVF;
};
const DWORD MIRRORVERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1;
CINFInven::CINFInven(CAtumNode* pParent)
{
	FLOG("CINFInven(CAtumNode* pParent)");
	m_pEqip = NULL;
	m_pInven = NULL;
	m_pItemSl = NULL;
	m_pSpi = NULL;
	m_pScrollLine = NULL;
	m_pScrollButton = NULL;
	m_pInvenBase = NULL;
	m_pMultiItemSelImage = NULL;
	m_pParent = pParent;
	int i; for (i = 0; i < INVEN_NUMBER; i++)
	{
		m_pInvenDisplayInfo[i] = NULL;
	}
	for (i = 0; i < MAX_EQUIP_POS; i++)
	{
		m_pWearDisplayInfo[i] = NULL;
	}
	m_nInvenCurrentScroll = 0;
	m_bScrollLock = FALSE;
	m_ptMouse.x = 0;
	m_ptMouse.y = 0;
	m_pSelectIcon = NULL;
	m_pSelectItem = NULL;
	m_pFontSpi = NULL;
	m_pFontWeight[0] = NULL;
	m_pFontWeight[1] = NULL;
	m_pFontWeight[2] = NULL;
	for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		m_pFontItemNum[i] = NULL;
	}
	m_nItemSpi = 0;
	m_hyItemSpiUniqueNumber = 0;
	InitDeleteItemInfo();
	m_bEnchantState = FALSE;
	m_pEnchantItem = NULL;
	m_bRestored = FALSE;
	m_bInvalidated = FALSE;
	m_pCurrentItemInfo = NULL;
	m_iWearPosition = POS_INVALID_POSITION;
	m_nRenderMoveIconIntervalHeight = 0;
	m_nRenderMoveIconIntervalWidth = 0;
	m_tBlinkTime.Set(0.0f, 1.5f, 0.5f);
	m_bSelectItem = FALSE;
	m_bSelectWearItem = FALSE;
	m_bMouseDownState = FALSE;
	m_pInvenRotationBase = NULL;
	for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			m_pInvenDirection[i][j] = NULL;
		}
	}
	m_nRotationState = ROTATION_NONE;
	m_nButtonState = ROTATION_NONE;
	for (i = 0; i < 4; i++)
	{
		m_bByPush[i] = FALSE;
	}
	m_nInvenCurrentScrollPosition = 0;
	m_pMirrorVB = NULL;
	m_pMirrorTexture = NULL;
	memset(&m_struDeleteItem, 0x00, sizeof(structDelItemInfo));
}
CINFInven::~CINFInven()
{
	FLOG("~CINFInven()");
	SAFE_DELETE(m_pEqip);
	SAFE_DELETE(m_pInven);
	SAFE_DELETE(m_pItemSl);
	SAFE_DELETE(m_pSpi);
	SAFE_DELETE(m_pScrollLine);
	SAFE_DELETE(m_pScrollButton);
	SAFE_DELETE(m_pFontSpi);
	SAFE_DELETE(m_pFontWeight[0]);
	SAFE_DELETE(m_pFontWeight[1]);
	SAFE_DELETE(m_pFontWeight[2]);
	SAFE_DELETE(m_pInvenBase);
	SAFE_DELETE(m_pMultiItemSelImage);
	SAFE_DELETE(m_pInvenRotationBase);
	int i; for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			SAFE_DELETE(m_pInvenDirection[i][j]);
		}
	}
	for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		SAFE_DELETE(m_pFontItemNum[i]);
	}
	for (i = 0; i < INVEN_NUMBER; i++)
	{
		SAFE_DELETE(m_pInvenDisplayInfo[i]);
	}
	for (i = 0; i < MAX_EQUIP_POS; i++)
	{
		SAFE_DELETE(m_pWearDisplayInfo[i]);
	}
	SAFE_RELEASE(m_pMirrorVB);
	SAFE_RELEASE(m_pMirrorTexture);
}
HRESULT CINFInven::InitDeviceObjects()
{
	FLOG("CINFInven::InitDeviceObjects()");
	m_pFontSpi = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 256, 32);
	m_pFontSpi->InitDeviceObjects(g_pD3dDev);
	m_pFontWeight[0] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 256, 32);
	m_pFontWeight[0]->InitDeviceObjects(g_pD3dDev);
	m_pFontWeight[1] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 256, 32);
	m_pFontWeight[1]->InitDeviceObjects(g_pD3dDev);
	m_pFontWeight[2] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 9, D3DFONT_ZENABLE, FALSE, 256, 32);
	m_pFontWeight[2]->InitDeviceObjects(g_pD3dDev);
	int i; for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		m_pFontItemNum[i] = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 8, D3DFONT_ZENABLE, TRUE, 256, 32);
		m_pFontItemNum[i]->InitDeviceObjects(g_pD3dDev);
	}
	DataHeader* pDataHeader;
	m_pEqip = new CINFImageEx;
	pDataHeader = FindResource("equip");
	m_pEqip->InitDeviceObjects(pDataHeader);
	m_pInven = new CINFImageEx;
	pDataHeader = FindResource("inven");
	m_pInven->InitDeviceObjects(pDataHeader);
	m_pItemSl = new CINFImageEx;
	pDataHeader = FindResource("itemsl");
	m_pItemSl->InitDeviceObjects(pDataHeader);
	m_pSpi = new CINFImageEx;
	pDataHeader = FindResource("spi");
	m_pSpi->InitDeviceObjects(pDataHeader);
	m_pScrollLine = new CINFImageEx;
	pDataHeader = FindResource("scrline");
	m_pScrollLine->InitDeviceObjects(pDataHeader);
	m_pScrollButton = new CINFImageEx;
	pDataHeader = FindResource("c_scrlb");
	m_pScrollButton->InitDeviceObjects(pDataHeader);
	m_pWearPositionBlink = new CINFImageEx;
	pDataHeader = FindResource("iFocus");
	m_pWearPositionBlink->InitDeviceObjects(pDataHeader);
	m_pInvenBase = new CINFImageEx;
	pDataHeader = FindResource("cinven");
	m_pInvenBase->InitDeviceObjects(pDataHeader);
	m_pInvenRotationBase = new CINFImageEx;
	pDataHeader = FindResource("innom");
	m_pInvenRotationBase->InitDeviceObjects(pDataHeader);
	for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			char buf[64];
			ZERO_MEMORY(buf);
			wsprintf(buf, "inven%d%d", i, j);
			m_pInvenDirection[i][j] = new CINFImageEx;
			pDataHeader = FindResource(buf);
			m_pInvenDirection[i][j]->InitDeviceObjects(pDataHeader);
		}
	}
	m_pMultiItemSelImage = new CINFImageEx;
	pDataHeader = FindResource("selicon");
	m_pMultiItemSelImage->InitDeviceObjects(pDataHeader);
	if (FAILED(g_pD3dDev->CreateVertexBuffer(4 * sizeof(MIRRORVERTEX),
		D3DUSAGE_WRITEONLY,
		MIRRORVERTEX::FVF,
		D3DPOOL_MANAGED, &m_pMirrorVB, NULL)))
		return E_FAIL;
	MIRRORVERTEX* v;
	m_pMirrorVB->Lock(0, 0, (void**)&v, 0);
	v[0].p = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	v[1].p = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	v[2].p = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
	v[3].p = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	v[0].uv.x = v[0].uv.y = v[1].uv.y = v[2].uv.x = 0;
	v[1].uv.x = v[2].uv.y = v[3].uv.y = v[3].uv.x = 1;
	m_pMirrorVB->Unlock();
	D3DXMatrixOrthoLH(&m_pMatInvenBackProj,
		(float)g_pD3dApp->GetBackBufferDesc().Width,
		(float)g_pD3dApp->GetBackBufferDesc().Height,
		0.0f, 1.0f);
	pDataHeader = FindResource("w_wi11");
	if (!pDataHeader)
	{
		pDataHeader = FindResource("w_w11");
	}
	D3DXIMAGE_INFO SrcInfo;
	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(g_pD3dDev, (LPCVOID)pDataHeader->m_pData, pDataHeader->m_DataSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT,
		0, &SrcInfo, NULL, &m_pMirrorTexture)))
	{
		SAFE_RELEASE(m_pMirrorTexture);
		return E_FAIL;
	}
	return S_OK;
}
HRESULT CINFInven::RestoreDeviceObjects()
{
	FLOG("CINFInven::RestoreDeviceObjects()");
	m_pEqip->RestoreDeviceObjects();
	m_pInven->RestoreDeviceObjects();
	m_pItemSl->RestoreDeviceObjects();
	m_pSpi->RestoreDeviceObjects();
	m_pInvenBase->RestoreDeviceObjects();
	m_pInvenRotationBase->RestoreDeviceObjects();
	m_pMultiItemSelImage->RestoreDeviceObjects();
	int i; for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			m_pInvenDirection[i][j]->RestoreDeviceObjects();
		}
	}
	m_pScrollLine->RestoreDeviceObjects();
	m_pScrollButton->RestoreDeviceObjects();
	m_pFontSpi->RestoreDeviceObjects();
	m_pFontWeight[0]->RestoreDeviceObjects();
	m_pFontWeight[1]->RestoreDeviceObjects();
	m_pFontWeight[2]->RestoreDeviceObjects();
	for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		m_pFontItemNum[i]->RestoreDeviceObjects();
	}
	m_pWearPositionBlink->RestoreDeviceObjects();
	m_bRestored = TRUE;
	return S_OK;
}
HRESULT CINFInven::DeleteDeviceObjects()
{
	FLOG("CINFInven::DeleteDeviceObjects()");
	m_pEqip->DeleteDeviceObjects();
	SAFE_DELETE(m_pEqip);
	m_pInven->DeleteDeviceObjects();
	SAFE_DELETE(m_pInven);
	m_pInvenBase->DeleteDeviceObjects();
	SAFE_DELETE(m_pInvenBase);
	m_pMultiItemSelImage->DeleteDeviceObjects();
	SAFE_DELETE(m_pMultiItemSelImage);
	m_pItemSl->DeleteDeviceObjects();
	SAFE_DELETE(m_pItemSl);
	m_pSpi->DeleteDeviceObjects();
	SAFE_DELETE(m_pSpi);
	m_pScrollLine->DeleteDeviceObjects();
	SAFE_DELETE(m_pScrollLine);
	m_pScrollButton->DeleteDeviceObjects();
	SAFE_DELETE(m_pScrollButton);
	m_pFontSpi->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontSpi);
	m_pFontWeight[0]->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontWeight[0]);
	m_pFontWeight[1]->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontWeight[1]);
	m_pFontWeight[2]->DeleteDeviceObjects();
	SAFE_DELETE(m_pFontWeight[2]);
	int i; for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		m_pFontItemNum[i]->DeleteDeviceObjects();
		SAFE_DELETE(m_pFontItemNum[i]);
	}
	m_pWearPositionBlink->DeleteDeviceObjects();
	SAFE_DELETE(m_pWearPositionBlink);
	m_pInvenRotationBase->DeleteDeviceObjects();
	SAFE_DELETE(m_pInvenRotationBase);
	for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			m_pInvenDirection[i][j]->DeleteDeviceObjects();
			SAFE_DELETE(m_pInvenDirection[i][j]);
		}
	}
	m_bInvalidated = FALSE;
	SAFE_RELEASE(m_pMirrorVB);
	SAFE_RELEASE(m_pMirrorTexture);
	return S_OK;
}
HRESULT CINFInven::InvalidateDeviceObjects()
{
	FLOG("CINFInven::InvalidateDeviceObjects()");
	m_pEqip->InvalidateDeviceObjects();
	m_pInven->InvalidateDeviceObjects();
	m_pItemSl->InvalidateDeviceObjects();
	m_pSpi->InvalidateDeviceObjects();
	m_pInvenBase->InvalidateDeviceObjects();
	m_pMultiItemSelImage->InvalidateDeviceObjects();
	m_pScrollLine->InvalidateDeviceObjects();
	m_pScrollButton->InvalidateDeviceObjects();
	m_pFontSpi->InvalidateDeviceObjects();
	m_pFontWeight[0]->InvalidateDeviceObjects();
	m_pFontWeight[1]->InvalidateDeviceObjects();
	m_pFontWeight[2]->InvalidateDeviceObjects();
	int i; for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		m_pFontItemNum[i]->InvalidateDeviceObjects();
	}
	m_pWearPositionBlink->InvalidateDeviceObjects();
	m_pInvenRotationBase->InvalidateDeviceObjects();
	for (i = 0; i < INVEN_ROTATION_STATE; i++)
	{
		for (int j = 0; j < INVEN_ROTATION_NUM; j++)
		{
			m_pInvenDirection[i][j]->InvalidateDeviceObjects();
		}
	}
	m_bRestored = FALSE;
	m_bInvalidated = TRUE;
	return S_OK;
}
void CINFInven::Tick()
{
	FLOG("CINFInven::Tick()");
	if (m_bTurnLeft)		g_pShuttleChild->MirrorTurnUp();
	if (m_bTurnRight)	g_pShuttleChild->MirrorTurnDown();
	if (m_bTurnUp)		g_pShuttleChild->MirrorTurnLeft();
	if (m_bTurnDown)		g_pShuttleChild->MirrorTurnRight();
	if (m_iWearPosition == POS_INVALID_POSITION)
		return;
	m_tBlinkTime.Tick(g_pD3dApp->GetElapsedTime());
	if (m_tBlinkTime.IsOverEnd())
		m_tBlinkTime.Start();
}
#define WEIGHT_START_X			58 
#define WEIGHT_START_Y			249 
#define WEIGHT_INTERVAL			13
void CINFInven::Render()
{
	FLOG("CINFInven::Render()");
	int nWindowPosY = g_pGameMain->m_nLeftWindowY;
	RenderInvenBack();
	g_pShuttleChild->RenderMirror();
	m_pInvenBase->Move(9, nWindowPosY + 30);
	m_pInvenBase->Render();
	if (m_nButtonState == ROTATION_NONE || m_nRotationState == ROTATION_STATE_N)
	{
		m_pInvenRotationBase->Move(16, nWindowPosY + 207);
		m_pInvenRotationBase->Render();
	}
	else
	{
		m_pInvenDirection[m_nButtonState][m_nRotationState]->Move(16, nWindowPosY + 207);
		m_pInvenDirection[m_nButtonState][m_nRotationState]->Render();
	}
	if (g_pStoreData->m_mapItemUniqueNumber.size() > INVEN_X_NUMBER)
	{
		m_pScrollLine->Move(INVEN_SCROLL_LINE_START_X, nWindowPosY + INVEN_SCROLL_LINE_START_Y);
		m_pScrollLine->Render();
		m_pScrollButton->Move(INVEN_SCROLL_BUTTON_START_X, nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + m_nInvenCurrentScrollPosition);
		m_pScrollButton->Render();
	}
	CINFIcon* pIconInfo = ((CINFGameMain*)m_pParent)->m_pIcon;
	int i; for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		for (int j = 0; j < INVEN_X_NUMBER; j++)
		{
			if (m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j])
			{
				char buf[64];
				strcpy(buf, m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->IconName);
				pIconInfo->SetIcon(buf,
					INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 1,
					nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i + 1, 1.0f);
				pIconInfo->Render();
				if (IS_COUNTABLE_ITEM(m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->pItem->Kind))
				{
					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(
						m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->pItem->UniqueNumber);
					if (pItemInfo->CurrentCount > 1)
					{
						char buf[128];
						wsprintf(buf, "%d", pItemInfo->CurrentCount);
						int len = strlen(buf) - 1;
						m_pFontItemNum[i]->DrawText(INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 21 - len * 6,
							nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i - 1,
							QSLOT_COUNTERBLE_NUMBER, buf, 0L);
					}
				}
			}
		}
	}
	for (i = 0; i < MAX_EQUIP_POS; i++)
	{
		if (m_pWearDisplayInfo[i])
		{
			char buf[64];
			strcpy(buf, m_pWearDisplayInfo[i]->IconName);
			switch (i)
			{
			case POS_PROW:
				pIconInfo->SetIcon(buf, POS_PROW_X + 1, nWindowPosY + POS_PROW_Y + 1, 1.0f);
				break;
			case POS_PROWIN:
				pIconInfo->SetIcon(buf, POS_PROWIN_X + 1, nWindowPosY + POS_PROWIN_Y + 1, 1.0f);
				break;
			case POS_PROWOUT:
				pIconInfo->SetIcon(buf, POS_PROWOUT_X + 1, nWindowPosY + POS_PROWOUT_Y + 1, 1.0f);
				break;
			case POS_WINGIN:
				pIconInfo->SetIcon(buf, POS_WINGIN_X + 1, nWindowPosY + POS_WINGIN_Y + 1, 1.0f);
				break;
			case POS_WINGOUT:
				pIconInfo->SetIcon(buf, POS_WINGOUT_X + 1, nWindowPosY + POS_WINGOUT_Y + 1, 1.0f);
				break;
			case POS_CENTER:
				pIconInfo->SetIcon(buf, POS_CENTER_X + 1, nWindowPosY + POS_CENTER_Y + 1, 1.0f);
				break;
			case POS_REAR:
				pIconInfo->SetIcon(buf, POS_REAR_X + 1, nWindowPosY + POS_REAR_Y + 1, 1.0f);
				break;
			case POS_ACCESSORY_UNLIMITED:
				pIconInfo->SetIcon(buf, POS_ATTACHMENT_X + 1, nWindowPosY + POS_ATTACHMENT_Y + 1, 1.0f);
				break;
			case POS_ACCESSORY_TIME_LIMIT:
				pIconInfo->SetIcon(buf, POS_PET_X + 1, nWindowPosY + POS_PET_Y + 1, 1.0f);
				break;
			}
			pIconInfo->Render();
		}
	}
	if (m_iWearPosition != POS_INVALID_POSITION)
	{
		if (m_tBlinkTime.IsUnderMiddle())
		{
			RenderWearPosition(m_iWearPosition);
		}
	}
	char temp1[64];
	char temp2[64];
	SIZE size;
	wsprintf(temp1, "%d", m_nItemSpi);
	MakeCurrencySeparator(temp2, temp1, 3, ',');
	size = m_pFontSpi->GetStringSize(temp2);
	m_pFontSpi->DrawText(INVEN_SPI_START_X - size.cx, nWindowPosY + INVEN_SPI_START_Y, GUI_FONT_COLOR_BM, temp2, 0L);
	wsprintf(temp1, "%d", g_pShuttleChild->m_myShuttleInfo.WarPoint);
	MakeCurrencySeparator(temp2, temp1, 3, ',');
	size = m_pFontSpi->GetStringSize(temp2);
	m_pFontSpi->DrawText(INVEN_SPI_START_X - size.cx, nWindowPosY + INVEN_WARPOINT_Y, GUI_FONT_COLOR_BM, temp2, 0L);
	char buff[64];
	wsprintf(buff, "(%3d%%)",
		(int)(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo) * 100.0f));
	m_pFontWeight[0]->DrawText(WEIGHT_START_X + 10, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, STRMSG_C_INTERFACE_0026, 0);
	m_pFontWeight[1]->DrawText(WEIGHT_START_X + FONTWEITGHT_X1, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, buff, 0);
	wsprintf(buff, "%d/%d", (int)(g_pStoreData->GetTotalUseInven()), CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, g_pShuttleChild->m_myShuttleInfo.GetAddedPermanentInventoryCount()) - 1);
	m_pFontWeight[2]->DrawText(WEIGHT_START_X + FONTWEITGHT_X2, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, buff, 0);
}
void CINFInven::RenderSpi(int x, int y)
{
	m_pSpi->Move(x, y);
	m_pSpi->Render();
	char temp1[64];
	char temp2[64];
	wsprintf(temp1, "%d", m_nItemSpi);
	MakeCurrencySeparator(temp2, temp1, 3, ',');
	m_pFontSpi->DrawText(x + SPI_START_X, y + SPI_START_Y, GUI_FONT_COLOR, temp2, 0L);
}
void CINFInven::RenderWearPosition(int m_iWearPosition)
{
	int posx, posy;
	int nWindowPosY = g_pGameMain->m_nLeftWindowY;
	int nPosShop = 0;
	if (g_pD3dApp->m_dwGameState == _SHOP)
		nPosShop = CITY_BASE_NPC_BOX_START_X;
	switch (m_iWearPosition)
	{
	case POS_PROW:
		posx = POS_PROW_X + 1 + nPosShop;
		posy = nWindowPosY + POS_PROW_Y + 1;
		break;
	case POS_PROWIN:
		posx = POS_PROWIN_X + 1 + nPosShop;
		posy = nWindowPosY + POS_PROWIN_Y + 1;
		break;
	case POS_PROWOUT:
		posx = POS_PROWOUT_X + 1 + nPosShop;
		posy = nWindowPosY + POS_PROWOUT_Y + 1;
		break;
	case POS_WINGIN:
		posx = POS_WINGIN_X + 1 + nPosShop;
		posy = nWindowPosY + POS_WINGIN_Y + 1;
		break;
	case POS_WINGOUT:
		posx = POS_WINGOUT_X + 1 + nPosShop;
		posy = nWindowPosY + POS_WINGOUT_Y + 1;
		break;
	case POS_CENTER:
		posx = POS_CENTER_X + 1 + nPosShop;
		posy = nWindowPosY + POS_CENTER_Y + 1;
		break;
	case POS_REAR:
		posx = POS_REAR_X + 1 + nPosShop;
		posy = nWindowPosY + POS_REAR_Y + 1;
		break;
	case POS_ACCESSORY_UNLIMITED:
		posx = POS_ATTACHMENT_X + 1 + nPosShop;
		posy = nWindowPosY + POS_ATTACHMENT_Y + 1;
		break;
	case POS_ACCESSORY_TIME_LIMIT:
		posx = POS_PET_X + 1 + nPosShop;
		posy = nWindowPosY + POS_PET_Y + 1;
		break;
	}
	m_pWearPositionBlink->Move(posx, posy);
	DWORD dwColor = 0x00FFFFFF;
	DWORD alpha = 255 * m_tBlinkTime.GetPositionRate();
	alpha = alpha << 24;
	m_pWearPositionBlink->SetColor(alpha | dwColor);
	m_pWearPositionBlink->Render();
}
void CINFInven::RenderOnCityBase()
{
	FLOG("CINFInven::RenderOnCityBase()");
	g_pGameMain->m_pInfWindow->m_nLeftWindowInfo = 1;
	g_pGameMain->m_nLeftWindowY = CITY_SHOP_INVEN_START_Y;
	int nWindowPosY = g_pGameMain->m_nLeftWindowY;
	int x = CITY_BASE_NPC_BOX_START_X;
	int y = nWindowPosY;
	int cx = 426;
	int cy = 275;
	g_pGameMain->m_pInfWindow->m_pBoxImage[0]->Move(x, y);
	g_pGameMain->m_pInfWindow->m_pBoxImage[0]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[1]->Move(x + 19, y);
	g_pGameMain->m_pInfWindow->m_pBoxImage[1]->SetScale(cx - 38, 1);
	g_pGameMain->m_pInfWindow->m_pBoxImage[1]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[2]->Move(x + cx - 19, y);
	g_pGameMain->m_pInfWindow->m_pBoxImage[2]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[3]->Move(x, y + 19);
	g_pGameMain->m_pInfWindow->m_pBoxImage[3]->SetScale(1, cy - 38);
	g_pGameMain->m_pInfWindow->m_pBoxImage[3]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[5]->Move(x + cx - 19, y + 19);
	g_pGameMain->m_pInfWindow->m_pBoxImage[5]->SetScale(1, cy - 38);
	g_pGameMain->m_pInfWindow->m_pBoxImage[5]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[6]->Move(x, y + cy - 19);
	g_pGameMain->m_pInfWindow->m_pBoxImage[6]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[7]->Move(x + 19, y + cy - 19);
	g_pGameMain->m_pInfWindow->m_pBoxImage[7]->SetScale(cx - 38, 1);
	g_pGameMain->m_pInfWindow->m_pBoxImage[7]->Render();
	g_pGameMain->m_pInfWindow->m_pBoxImage[8]->Move(x + cx - 19, y + cy - 19);
	g_pGameMain->m_pInfWindow->m_pBoxImage[8]->Render();
	g_pShuttleChild->SetRenderInven(TRUE);
	RenderInvenBack(TRUE);
	g_pShuttleChild->RenderMirror();
	m_pInvenBase->Move(CITY_BASE_NPC_BOX_START_X + 9, nWindowPosY + 30);
	m_pInvenBase->Render();
	if (m_nButtonState == ROTATION_NONE || m_nRotationState == ROTATION_STATE_N)
	{
		m_pInvenRotationBase->Move(CITY_BASE_NPC_BOX_START_X + 16, nWindowPosY + 207);
		m_pInvenRotationBase->Render();
	}
	else
	{
		m_pInvenDirection[m_nButtonState][m_nRotationState]->Move(CITY_BASE_NPC_BOX_START_X + 16, nWindowPosY + 207);
		m_pInvenDirection[m_nButtonState][m_nRotationState]->Render();
	}
	if (g_pStoreData->m_mapItemUniqueNumber.size() > INVEN_X_NUMBER)
	{
		m_pScrollLine->Move(INVEN_SCROLL_LINE_START_X + CITY_BASE_NPC_BOX_START_X, nWindowPosY + INVEN_SCROLL_LINE_START_Y);
		m_pScrollLine->Render();
		m_pScrollButton->Move(INVEN_SCROLL_BUTTON_START_X + CITY_BASE_NPC_BOX_START_X, nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + m_nInvenCurrentScrollPosition);
		m_pScrollButton->Render();
	}
	CINFIcon* pIconInfo = ((CINFGameMain*)m_pParent)->m_pIcon;
	int i; for (i = 0; i < INVEN_Y_NUMBER; i++)
	{
		for (int j = 0; j < INVEN_X_NUMBER; j++)
		{
			m_pItemSl->Move(CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j,
				CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i);
			m_pItemSl->Render();
			if (m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j])
			{
				BOOL bMultiSel = FALSE;
				if (g_pD3dApp->CheckMultItemSel(m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->pItem->UniqueNumber))
				{
					bMultiSel = TRUE;
				}
				char buf[64];
				strcpy(buf, m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->IconName);
				pIconInfo->SetIcon(buf,
					CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 1,
					CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i + 1, 1.0f);
				pIconInfo->Render();
				if (IS_COUNTABLE_ITEM(m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->pItem->Kind))
				{
					CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByUniqueNumber(
						m_pInvenDisplayInfo[i * INVEN_X_NUMBER + j]->pItem->UniqueNumber);
					if (pItemInfo->CurrentCount > 1)
					{
						char buf[128];
#if defined(LANGUAGE_ENGLISH) || defined(LANGUAGE_VIETNAM)|| defined(LANGUAGE_THAI) 
						wsprintf(buf, "%d", pItemInfo->CurrentCount);
						int len = strlen(buf) - 1;
						m_pFontItemNum[i]->DrawText(CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 21 - len * 6,
							CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i - 1,
							QSLOT_COUNTERBLE_NUMBER, buf, 0L);
#else
						wsprintf(buf, "%d", pItemInfo->CurrentCount);
						int len = strlen(buf) - 1;
						m_pFontItemNum[i]->DrawText(CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 21 - len * 6,
							CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i - 1,
							QSLOT_COUNTERBLE_NUMBER, buf, 0L);
#endif
					}
				}
				if (bMultiSel)
				{
					float fXPos, fYPos;
					fXPos = CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * j + 1;
					fYPos = CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * i + 1;
					m_pMultiItemSelImage->Move(fXPos, fYPos);
					m_pMultiItemSelImage->Render();
				}
			}
		}
	}
	for (i = 0; i << MAX_EQUIP_POS; i++)
	{
		if (m_pWearDisplayInfo[i])
		{
			char buf[64];
			strcpy(buf, m_pWearDisplayInfo[i]->IconName);
			switch (i)
			{
			case POS_PROW:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_PROW_X + 1, CITY_SHOP_INVEN_START_Y + POS_PROW_Y + 1, 1.0f);
				break;
			case POS_PROWIN:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X + 1, CITY_SHOP_INVEN_START_Y + POS_PROWIN_Y + 1, 1.0f);
				break;
			case POS_PROWOUT:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X + 1, CITY_SHOP_INVEN_START_Y + POS_PROWOUT_Y + 1, 1.0f);
				break;
			case POS_WINGIN:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X + 1, CITY_SHOP_INVEN_START_Y + POS_WINGIN_Y + 1, 1.0f);
				break;
			case POS_WINGOUT:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X + 1, CITY_SHOP_INVEN_START_Y + POS_WINGOUT_Y + 1, 1.0f);
				break;
			case POS_CENTER:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_CENTER_X + 1, CITY_SHOP_INVEN_START_Y + POS_CENTER_Y + 1, 1.0f);
				break;
			case POS_REAR:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_REAR_X + 1, CITY_SHOP_INVEN_START_Y + POS_REAR_Y + 1, 1.0f);
				break;
			case POS_ACCESSORY_UNLIMITED:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X + 1, CITY_SHOP_INVEN_START_Y + POS_ATTACHMENT_Y + 1, 1.0f);
				break;
			case POS_ACCESSORY_TIME_LIMIT:
				pIconInfo->SetIcon(buf, CITY_BASE_NPC_BOX_START_X + POS_PET_X + 1, CITY_SHOP_INVEN_START_Y + POS_PET_Y + 1, 1.0f);
				break;
			}
			pIconInfo->Render();
		}
	}
	if (m_iWearPosition != POS_INVALID_POSITION)
	{
		if (m_tBlinkTime.IsUnderMiddle())
		{
			RenderWearPosition(m_iWearPosition);
		}
	}
	char temp1[64];
	char temp2[64];
	SIZE size;
	wsprintf(temp1, "%d", m_nItemSpi);
	MakeCurrencySeparator(temp2, temp1, 3, ',');
	size = m_pFontSpi->GetStringSize(temp2);
	m_pFontSpi->DrawText(INVEN_SPI_START_X + CITY_BASE_NPC_BOX_START_X - size.cx, nWindowPosY + INVEN_SPI_START_Y, GUI_FONT_COLOR_BM, temp2, 0L);
	wsprintf(temp1, "%d", g_pShuttleChild->m_myShuttleInfo.WarPoint);
	MakeCurrencySeparator(temp2, temp1, 3, ',');
	size = m_pFontSpi->GetStringSize(temp2);
	m_pFontSpi->DrawText(INVEN_SPI_START_X + CITY_BASE_NPC_BOX_START_X - size.cx, nWindowPosY + INVEN_WARPOINT_Y, GUI_FONT_COLOR_BM, temp2, 0L);
	char buff[64];
	wsprintf(buff, "(%3d%%)",
		(int)(g_pStoreData->GetTotalWeight() / CAtumSJ::GetTransport(&g_pShuttleChild->m_myShuttleInfo) * 100.0f));
	m_pFontWeight[0]->DrawText(WEIGHT_START_X + CITY_BASE_NPC_BOX_START_X + 10, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, STRMSG_C_INTERFACE_0026, 0);
	m_pFontWeight[1]->DrawText(WEIGHT_START_X + CITY_BASE_NPC_BOX_START_X + FONTWEITGHT_X1, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, buff, 0);
	wsprintf(buff, "%d/%d", (int)(g_pStoreData->GetTotalUseInven()), CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, g_pShuttleChild->m_myShuttleInfo.GetAddedPermanentInventoryCount()) - 1);
	m_pFontWeight[2]->DrawText(WEIGHT_START_X + CITY_BASE_NPC_BOX_START_X + FONTWEITGHT_X2, nWindowPosY + WEIGHT_START_Y - WEIGHT_INTERVAL, GUI_FONT_COLOR_BM, buff, 0);
}
void CINFInven::SetAllIconInfo()
{
	FLOG("CINFInven::SetAllIconInfo()");
	InitInvenIconInfo();
	InitWearIconInfo();
	CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.begin();
	while (it != g_pStoreData->m_mapItemWindowPosition.end())
	{
		CItemInfo* pItemInfo = it->second;
		int nMinIndex = m_nInvenCurrentScroll * INVEN_X_NUMBER;
		int nMaxIndex = (m_nInvenCurrentScroll * INVEN_X_NUMBER) + (INVEN_X_NUMBER * INVEN_Y_NUMBER);
		if (pItemInfo->Wear > WEAR_NOT_ATTACHED && pItemInfo->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
		{
			SetSingleWearIconInfo(pItemInfo);
		}
		else if (pItemInfo->ItemWindowIndex >= POS_ITEMWINDOW_OFFSET + nMinIndex &&
			pItemInfo->ItemWindowIndex < POS_ITEMWINDOW_OFFSET + nMaxIndex)
		{
			if (IS_COUNTABLE_ITEM(pItemInfo->Kind))
			{
				if (pItemInfo->CurrentCount > 0)
					SetSingleInvenIconInfo(pItemInfo);
#ifdef _DEBUG
				else
				{
					DBGOUT("CINFInven::SetAllIconInfo(), Do not rendering : item[%d] count == 0 \n", pItemInfo->ItemNum);
				}
#endif
			}
			else
			{
				SetSingleInvenIconInfo(pItemInfo);
			}
			if (pItemInfo->CurrentCount < 0)
			{
				char buf[256];
				wsprintf(buf, "DB ERROR : item(%d) count = %d", pItemInfo->ItemNum, pItemInfo->CurrentCount);
				g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
			}
		}
		it++;
	}
}
BOOL CINFInven::IsTradingItem(ITEM_GENERAL* pItem)
{
	FLOG("CINFInven::IsTradingItem(ITEM_GENERAL* pItem)");
	if (((CINFGameMain*)m_pParent)->m_nLeftWindowInfo == LEFT_WINDOW_TRANS)
	{
		vector<CItemInfo*>::iterator it = ((CINFGameMain*)m_pParent)->m_pTrade->m_vecMyStore.begin();
		while (it != ((CINFGameMain*)m_pParent)->m_pTrade->m_vecMyStore.end())
		{
			if ((*it)->UniqueNumber == pItem->UniqueNumber)
			{
				return TRUE;
			}
			it++;
		}
	}
	return FALSE;
}
void CINFInven::InitInvenIconInfo()
{
	FLOG("CINFInven::InitInvenIconInfo()");
	int i; for (i = 0; i < INVEN_NUMBER; i++)
	{
		SAFE_DELETE(m_pInvenDisplayInfo[i]);
	}
}
void CINFInven::InitWearIconInfo()
{
	FLOG("CINFInven::InitWearIconInfo()");
	int i; for (i = 0; i < MAX_EQUIP_POS; i++)
	{
		SAFE_DELETE(m_pWearDisplayInfo[i]);
	}
}
void CINFInven::SetSingleInvenIconInfo(CItemInfo* pItemInfo)
{
	char buf[20];
	ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pItemInfo->GetShapeItemNum());
	int tempSourceIndex = NULL;
	if (pShapeItem)
	{
		tempSourceIndex = pShapeItem->SourceIndex;
	}
	else
	{
		tempSourceIndex = pItemInfo->ItemInfo->SourceIndex;
	}
	wsprintf(buf, "%08d", tempSourceIndex);
	int index = (pItemInfo->ItemWindowIndex - POS_ITEMWINDOW_OFFSET) - (m_nInvenCurrentScroll * INVEN_X_NUMBER);
	if (index >= 0 && index < INVEN_NUMBER)
	{
		if (m_pInvenDisplayInfo[index])
		{
		}
		SAFE_DELETE(m_pInvenDisplayInfo[index]);
		m_pInvenDisplayInfo[index] = new INVEN_DISPLAY_INFO;
		memset(m_pInvenDisplayInfo[index], 0x00, sizeof(INVEN_DISPLAY_INFO));
		strcpy(m_pInvenDisplayInfo[index]->IconName, buf);
		ITEM* pITEM = pItemInfo->GetItemInfo();
		if (pITEM && strlen(pITEM->ItemName) > 0)
			strcpy(m_pInvenDisplayInfo[index]->Name, pITEM->ItemName);
		m_pInvenDisplayInfo[index]->pItem = (ITEM_BASE*)pItemInfo;
	}
}
void CINFInven::SetSingleWearIconInfo(CItemInfo* pItemInfo)
{
	char buf[20];
	ITEM* pShapeItem = g_pDatabase->GetServerItemInfo(pItemInfo->GetShapeItemNum());
	int tempSourceInfo = NULL;
	if (pShapeItem)
	{
		tempSourceInfo = pShapeItem->SourceIndex;
	}
	else
	{
		tempSourceInfo = pItemInfo->ItemInfo->SourceIndex;
	}
	wsprintf(buf, "%08d", tempSourceInfo);
	if (pItemInfo->Wear > WEAR_NOT_ATTACHED && pItemInfo->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
	{
		SAFE_DELETE(m_pWearDisplayInfo[pItemInfo->ItemWindowIndex]);
		m_pWearDisplayInfo[pItemInfo->ItemWindowIndex] = new INVEN_DISPLAY_INFO;
		memset(m_pWearDisplayInfo[pItemInfo->ItemWindowIndex], 0x00, sizeof(INVEN_DISPLAY_INFO));
		strcpy(m_pWearDisplayInfo[pItemInfo->ItemWindowIndex]->IconName, buf);
		ITEM* pITEM = pItemInfo->GetItemInfo();
		if (pITEM)
			strcpy(m_pWearDisplayInfo[pItemInfo->ItemWindowIndex]->Name, pITEM->ItemName);
		m_pWearDisplayInfo[pItemInfo->ItemWindowIndex]->pItem = (ITEM_BASE*)pItemInfo;
	}
}
void CINFInven::SetItemInfo(INVEN_DISPLAY_INFO* pItemInfo, int x, int y)
{
	FLOG("CINFInven::SetItemInfo(INVEN_DISPLAY_INFO *pItemInfo, int x, int y)");
	if (m_pCurrentItemInfo != pItemInfo)
	{
		if (pItemInfo)
		{
			CItemInfo* pItem = g_pStoreData->FindItemInInventoryByUniqueNumber(pItemInfo->pItem->UniqueNumber);
			((CINFGameMain*)m_pParent)->SetItemInfo(pItemInfo->pItem->UniqueNumber, pItemInfo->pItem->ItemNum, x, y, 0);
		}
		else
		{
			((CINFGameMain*)m_pParent)->SetItemInfo(0, 0, 0, 0);
		}
		m_pCurrentItemInfo = pItemInfo;
	}
}
int CINFInven::WndProcOnCityBase(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FLOG("CINFInven::WndProcOnCityBase(UINT uMsg, WPARAM wParam, LPARAM lParam)");
	ProcessRotationUnitWnd(uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_RBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();
		int i = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
		if (pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
			i >= 0 && i < INVEN_X_NUMBER &&
			pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
		{
			int j = (pt.y - CITY_SHOP_INVEN_START_Y - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			if (j >= 0 &&
				j < INVEN_Y_NUMBER &&
				m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i] && pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				if (m_bEnchantState && m_pEnchantItem)
				{
					m_bEnchantState = FALSE;
					((CINFGameMain*)m_pParent)->m_pInfWindow->DeleteMsgBox(_Q_USE_ENCHANT_ITEM_CARD);
				}
				else
				{
					if (nSelItemCnt <= 0)
					{
						SetMultiSelectItem(NULL);
						SetSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
					}
				}
			}
		}
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
			((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS &&
			g_pInterface->m_pBazaarShop == NULL)
		{
			if (IS_ITEM_SHOP_TYPE(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				CItemInfo* pItemInfo = (CItemInfo*)((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem;
				if (pItemInfo->Wear == WEAR_NOT_ATTACHED)
				{
					if (IS_COUNTABLE_ITEM(pItemInfo->Kind))
					{
						char buf[128];
						ITEM* pITEM = pItemInfo->GetItemInfo();
						if (pITEM)
						{
							char temp1[64];
							char temp2[64];
							wsprintf(temp1, "%d", CAtumSJ::GetItemSellingPriceAtShop(pITEM));
							MakeCurrencySeparator(temp2, temp1, 3, ',');
							wsprintf(buf, STRMSG_C_SHOP_0007, pITEM->ItemName, pITEM->MinTradeQuantity, temp2);
							if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ENERGY))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ENERGY);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_MULTI_SELL_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_MULTI_SELL_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_PUT_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_PUT_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_GET_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_GET_ITEM);
							}
							g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_SHOP_SELL_ENERGY,
								(DWORD)pItemInfo,
								pItemInfo->CurrentCount);
						}
						else
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_SERVER_0004, COLOR_SYSTEM);
						}
					}
					else
					{
						char buf[128];
						ITEM* pITEM = pItemInfo->GetItemInfo();
						if (pITEM)
						{
							char temp1[64];
							char temp2[64];
							wsprintf(temp1, "%d", CAtumSJ::GetItemSellingPriceAtShop(pITEM));
							MakeCurrencySeparator(temp2, temp1, 3, ',');
							wsprintf(buf, STRMSG_C_SHOP_0009, pITEM->ItemName, temp2);
							if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_SELL_ENERGY))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_SELL_ENERGY);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_SHOP_MULTI_SELL_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_SHOP_MULTI_SELL_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_PUT_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_PUT_ITEM);
							}
							else if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_MULTI_GET_ITEM))
							{
								g_pGameMain->m_pInfWindow->DeleteMsgBox(_Q_STORE_MULTI_GET_ITEM);
							}
							g_pGameMain->m_pInfWindow->AddMsgBox(buf, _Q_SHOP_SELL_ITEM,
								(DWORD)pItemInfo);
						}
						else
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_SHOP_0008, COLOR_SYSTEM);
						}
					}
				}
				else
				{
					g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_SHOP_0010, _MESSAGE);
				}
			}
		}
		else if (nSelItemCnt > 0)
		{
			int nPosX = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
			if ((pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X
				&& nPosX >= 0
				&& nPosX < INVEN_X_NUMBER))
			{
				int nPosY = (pt.y - CITY_SHOP_INVEN_START_Y - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
				BOOL bPopup = FALSE;
				if (nPosY >= 0
					&& nPosY < INVEN_Y_NUMBER
					&& m_pInvenDisplayInfo[nPosY * INVEN_X_NUMBER + nPosX]
					&& pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y
					&& pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * nPosY + INVEN_SLOT_SIZE)
				{
					if (g_pD3dApp->CheckMultItemSel(m_pInvenDisplayInfo[nPosY * INVEN_X_NUMBER + nPosX]->pItem->UniqueNumber))
					{
						bPopup = TRUE;
						((CINFGameMain*)m_pParent)->PopupMultiItemSelect();
					}
				}
				if (!bPopup)
				{
					SetMultiSelectItem(NULL);
				}
			}
			else
			{
				SetMultiSelectItem(NULL);
			}
		}
		SetSelectItem(NULL);
		SetItemInfo(NULL, 0, 0);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);
			if (pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
			pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * INVEN_X_NUMBER &&
			pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * INVEN_Y_NUMBER)
		{
			if (GetScrollLine() > 0)
			{
				if ((int)wParam < 0)
				{
					m_nInvenCurrentScroll++;
					if (m_nInvenCurrentScroll > GetScrollLine())
					{
						m_nInvenCurrentScroll = GetScrollLine();
						m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					}
					else
					{
						m_nInvenCurrentScrollPosition = GetScrollLineInterval(m_nInvenCurrentScroll);
					}
					SetAllIconInfo();
					SetItemInfo(NULL, 0, 0);
				}
				else
				{
					m_nInvenCurrentScroll--;
					if (m_nInvenCurrentScroll < 0)
					{
						m_nInvenCurrentScroll = 0;
						m_nInvenCurrentScrollPosition = 0;
					}
					else
					{
						m_nInvenCurrentScrollPosition = GetScrollLineInterval(m_nInvenCurrentScroll);
					}
					SetAllIconInfo();
					SetItemInfo(NULL, 0, 0);
				}
			}
			return INF_MSGPROC_BREAK;
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		((CINFGameMain*)m_pParent)->SetToolTip(NULL, 0, 0);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
			((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem)
		{
			m_bSelectItem = TRUE;
		}
		if (m_bScrollLock && GetScrollLine() > 0)
		{
			if (pt.x >= INVEN_SCROLL_LINE_START_X + CITY_BASE_NPC_BOX_START_X - 20 &&
				pt.x <= INVEN_SCROLL_LINE_START_X + CITY_BASE_NPC_BOX_START_X + 20)
			{
				if (pt.y >= nWindowPosY + INVEN_SCROLL_LINE_START_Y &&
					pt.y <= nWindowPosY + INVEN_SCROLL_LINE_START_Y + INVEN_SCROLL_LINE_LENGTH)
				{
					int nMove = pt.y - m_ptMouse.y;
					m_nInvenCurrentScrollPosition += nMove;
					m_nInvenCurrentScroll = GetScrollLineNumber(m_nInvenCurrentScrollPosition);
					if (m_nInvenCurrentScrollPosition < 0)
					{
						m_nInvenCurrentScroll = 0;
						m_nInvenCurrentScrollPosition = 0;
					}
					else if (m_nInvenCurrentScrollPosition > INVEN_SCROLL_LINE_MOVE_LENGTH)
					{
						m_nInvenCurrentScroll = GetScrollLine();
						m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					}
					if (nMove != 0)
					{
						m_ptMouse = pt;
						SetAllIconInfo();
					}
				}
				else if (pt.y < nWindowPosY + INVEN_SCROLL_LINE_START_Y)
				{
					m_nInvenCurrentScroll = 0;
					m_nInvenCurrentScrollPosition = 0;
					SetAllIconInfo();
				}
				else if (pt.y > nWindowPosY + INVEN_SCROLL_LINE_START_Y + INVEN_SCROLL_LINE_LENGTH)
				{
					m_nInvenCurrentScroll = GetScrollLine();
					m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					SetAllIconInfo();
				}
			}
		}
		if (!m_bSelectItem)
		{
			SetItemInfo(NULL, 0, 0);
			if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROW_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROW_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_PROW_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_PROW_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROW])
					SetItemInfo(m_pWearDisplayInfo[POS_PROW], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0015);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_PROWIN_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_PROWIN_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROWIN])
					SetItemInfo(m_pWearDisplayInfo[POS_PROWIN], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0016);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_PROWOUT_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROWOUT])
					SetItemInfo(m_pWearDisplayInfo[POS_PROWOUT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0017);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_WINGIN_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_WINGIN_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_WINGIN])
					SetItemInfo(m_pWearDisplayInfo[POS_WINGIN], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_060720_0100);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_WINGOUT_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_WINGOUT])
					SetItemInfo(m_pWearDisplayInfo[POS_WINGOUT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0018);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_CENTER_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_CENTER_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_CENTER_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_CENTER_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_CENTER])
					SetItemInfo(m_pWearDisplayInfo[POS_CENTER], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0019);
				return INF_MSGPROC_BREAK;
			}
			else if (m_pWearDisplayInfo[POS_REAR] &&
				pt.x > CITY_BASE_NPC_BOX_START_X + POS_REAR_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_REAR_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_REAR_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_REAR_Y + INVEN_SLOT_SIZE)
			{
				SetItemInfo(m_pWearDisplayInfo[POS_REAR], pt.x, pt.y);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_ATTACHMENT_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED])
					SetItemInfo(m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0020);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PET_X &&
				pt.x<CITY_BASE_NPC_BOX_START_X + POS_PET_X + INVEN_SLOT_SIZE &&
				pt.y>CITY_SHOP_INVEN_START_Y + POS_PET_Y &&
				pt.y < CITY_SHOP_INVEN_START_Y + POS_PET_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT])
					SetItemInfo(m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0020);
				return INF_MSGPROC_BREAK;
			}
			else
			{
				int i = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
				if (pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
					i >= 0 && i < INVEN_X_NUMBER &&
					pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
				{
					int j = (pt.y - CITY_SHOP_INVEN_START_Y - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
					if (pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
						j >= 0 && j < INVEN_Y_NUMBER &&
						pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
					{
						if (m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i])
						{
							char buf[256];
							ITEM* item = NULL;
							if (COMPARE_RACE(g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.Race, RACE_OPERATION | RACE_GAMEMASTER))
							{
								if (IS_COUNTABLE_ITEM(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->Kind))
								{
									item = g_pDatabase->GetServerItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum);
									if (item)
										wsprintf(buf, STRMSG_C_TOOLTIP_0014, item->ItemName, m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum, (int)((ITEM_GENERAL*)m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem)->UniqueNumber,
											((ITEM_GENERAL*)m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem)->CurrentCount);
								}
								else
								{
									item = g_pDatabase->GetServerItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum);
									if (item)
										wsprintf(buf, "%s(%8d)(%08d)", item->ItemName, m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum, (int)((ITEM_GENERAL*)m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem)->UniqueNumber);
								}
							}
							else
							{
								if (IS_COUNTABLE_ITEM(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->Kind))
								{
									item = g_pDatabase->GetServerItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum);
									if (item)
										wsprintf(buf, STRMSG_C_TOOLTIP_0013, item->ItemName, ((ITEM_GENERAL*)m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem)->CurrentCount);
								}
								else
								{
									item = g_pDatabase->GetServerItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->ItemNum);
									if (item)
										wsprintf(buf, "%s", item->ItemName);
								}
							}
							if (item)
							{
								m_iWearPosition = item->Position;
							}
							else
							{
								m_iWearPosition = POS_INVALID_POSITION;
							}
							SetItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i], pt.x, pt.y);
						}
						else
						{
							m_iWearPosition = POS_INVALID_POSITION;
							SetItemInfo(NULL, 0, 0);
						}
					}
				}
				else
				{
					m_iWearPosition = POS_INVALID_POSITION;
					if (!m_pSelectItem)
					{
						SetItemInfo(NULL, 0, 0);
					}
				}
			}
		}
		else
		{
			if (!m_pSelectItem)
			{
				SetItemInfo(NULL, 0, 0);
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		m_bMouseDownState = TRUE;
		if (g_pStoreData->m_mapItemUniqueNumber.size() > INVEN_X_NUMBER &&
			pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_SCROLL_BUTTON_START_X &&
			pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_SCROLL_BUTTON_START_X + INVEN_SCROLL_BUTTON_WIDTH &&
			pt.y > nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + m_nInvenCurrentScrollPosition &&
			pt.y < nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + INVEN_SCROLL_BUTTON_HEIGHT + m_nInvenCurrentScrollPosition)
		{
			m_bScrollLock = TRUE;
			m_ptMouse = pt;
		}
		else
		{
			m_bScrollLock = FALSE;
		}
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
			((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS)
		{
			m_bSelectItem = TRUE;
		}
		if (m_pWearDisplayInfo[POS_PROW] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROW_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROW_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROW_Y && pt.y < nWindowPosY + POS_PROW_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_PROW_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROW_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROW]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_PROWIN] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROWIN_Y && pt.y < nWindowPosY + POS_PROWIN_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROWIN_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROWIN]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_PROWOUT] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROWOUT_Y && pt.y < nWindowPosY + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROWOUT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROWOUT]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_WINGIN] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_WINGIN_Y && pt.y < nWindowPosY + POS_WINGIN_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_WINGIN_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_WINGIN]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_WINGOUT] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_WINGOUT_Y && pt.y < nWindowPosY + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_WINGOUT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_WINGOUT]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_CENTER] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_CENTER_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_CENTER_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_CENTER_Y && pt.y < nWindowPosY + POS_CENTER_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_CENTER_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_CENTER_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_CENTER]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_REAR] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_REAR_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_REAR_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_REAR_Y && pt.y < nWindowPosY + POS_REAR_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_REAR_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_REAR_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_REAR]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_ATTACHMENT_Y && pt.y < nWindowPosY + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_ATTACHMENT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED]);
			m_bSelectWearItem = TRUE;
			ClearMultiSeletItem();
			break;
		}
		else if (m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT] && pt.x > CITY_BASE_NPC_BOX_START_X + POS_PET_X && pt.x<CITY_BASE_NPC_BOX_START_X + POS_PET_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PET_Y && pt.y < nWindowPosY + POS_ATTACHMENT_Y + POS_PET_Y + INVEN_SLOT_SIZE)
		{
			if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
				ClearMultiSeletItem();
				break;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + POS_PET_X);
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PET_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT]);
			m_bSelectWearItem = TRUE;
			ClearMultiSeletItem();
			break;
		}
		int i = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
		if (pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
			i >= 0 && i < INVEN_X_NUMBER /*&&*/
			/*pt.x < INVEN_ITEM_SLOT_START_X+INVEN_SLOT_INTERVAL*i+INVEN_SLOT_SIZE*/)
		{
			int j = (pt.y - nWindowPosY - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			if (j >= 0 &&
				j < INVEN_Y_NUMBER &&
				m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i] && pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
				pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				m_nRenderMoveIconIntervalWidth = pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + (INVEN_SLOT_INTERVAL * i));
				m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + INVEN_ITEM_SLOT_START_Y + (INVEN_SLOT_INTERVAL * j));
				m_bSelectWearItem = FALSE;
				if (m_bEnchantState && m_pEnchantItem)
				{
					m_bEnchantState = FALSE;
					((CINFGameMain*)m_pParent)->m_pInfWindow->DeleteMsgBox(_Q_USE_ENCHANT_ITEM_CARD);
				}
				else
				{
					BOOL bMuitiItemSel = FALSE;
					if (g_pD3dApp->GetCtrlBtnClick())
					{
						GUI_BUILDINGNPC* pNpc = g_pInterface->m_pCityBase->GetCurrentBuildingNPC();
						if (pNpc)
						{
							if (IS_ITEM_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
								|| IS_STORE_SHOP_TYPE(pNpc->buildingInfo.BuildingKind)
								|| IS_WARPOINT_SHOP_TYPE(pNpc->buildingInfo.BuildingKind))
							{
								bMuitiItemSel = TRUE;
							}
						}
					}
					if (!bMuitiItemSel)
					{
						int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();
						BOOL bMultiDragSel = FALSE;
						if (g_pD3dApp->CheckMultItemSel(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem->UniqueNumber))
						{
							bMultiDragSel = TRUE;
						}
						if (!bMultiDragSel)
						{
							SetMultiSelectItem(NULL);
						}
						if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem == NULL)
						{
							SetSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
						}
					}
					else
					{
						if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem == NULL)
						{
							SetMultiSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
							SetItemInfo(NULL, 0, 0);
							return INF_MSGPROC_BREAK;
						}
						else
						{
							SetMultiSelectItem(NULL);
						}
					}
				}
			}
			else
			{
				ClearMultiSeletItem();
			}
		}
		else
		{
			ClearMultiSeletItem();
		}
		SetItemInfo(NULL, 0, 0);
		if (m_bSelectItem)
		{
			return INF_MSGPROC_BREAK;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		m_bScrollLock = FALSE;
		CItemInfo* pSelectItem = NULL;
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem)
		{
			pSelectItem = (CItemInfo*)(((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem);
		}
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType != ITEM_INVEN_POS)
		{
			break;
		}
		if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROW_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROW_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_PROW_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_PROW_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_PROW);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWIN_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_PROWIN_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_PROWIN_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_PROWIN);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_PROWOUT_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_PROWOUT_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_PROWOUT);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGIN_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_WINGIN_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_WINGIN_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_WINGIN);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_WINGOUT_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_WINGOUT_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_WINGOUT);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_CENTER_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_CENTER_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_CENTER_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_CENTER_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_CENTER);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_REAR_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_REAR_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_REAR_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_REAR_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_REAR);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_ATTACHMENT_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_ACCESSORY_UNLIMITED);
		}
		else if (pt.x > CITY_BASE_NPC_BOX_START_X + POS_PET_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + POS_PET_X + INVEN_SLOT_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + POS_PET_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + POS_PET_Y + INVEN_SLOT_SIZE)
		{
			SendChangeWearWindowPos(POS_ACCESSORY_TIME_LIMIT);
		}
		else if (pSelectItem &&
			pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * INVEN_X_NUMBER &&
			pt.y>CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * INVEN_Y_NUMBER)
		{
			int i = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
			int j = (pt.y - CITY_SHOP_INVEN_START_Y - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			int nWindowPosition = j * INVEN_X_NUMBER + i + m_nInvenCurrentScroll * INVEN_X_NUMBER + POS_ITEMWINDOW_OFFSET;
			if (nWindowPosition != pSelectItem->ItemWindowIndex &&
				pSelectItem->ItemWindowIndex >= POS_ITEMWINDOW_OFFSET &&
				pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
				i >= 0 && i < INVEN_X_NUMBER &&
				pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE &&
				pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
				j >= 0 && j < INVEN_Y_NUMBER &&
				pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				if (pSelectItem->ItemWindowIndex < nWindowPosition)
				{
					INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = m_pInvenDisplayInfo[pSelectItem->ItemWindowIndex];
					for (int i = pSelectItem->ItemWindowIndex + 1; i <= nWindowPosition; i++)
					{
						CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
						if (it != g_pStoreData->m_mapItemWindowPosition.end())
						{
							CItemInfo* pSwapItem = it->second;
							pSwapItem->ItemWindowIndex = i - 1;
							g_pStoreData->m_mapItemWindowPosition[i - 1] = pSwapItem;
							SetSingleInvenIconInfo(pSwapItem);
						}
						else {
							pSelectItem->ItemWindowIndex = i - 1;
							g_pStoreData->m_mapItemWindowPosition[i - 1] = pSelectItem;
							SetAllIconInfo();
							SetSelectItem(NULL);
							return INF_MSGPROC_NORMAL;
						}
					}
					pSelectItem->ItemWindowIndex = nWindowPosition;
					g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
					SetSingleInvenIconInfo(pSelectItem);
					SetAllIconInfo();
				}
				else if (pSelectItem->ItemWindowIndex > nWindowPosition)
				{
					INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = m_pInvenDisplayInfo[pSelectItem->ItemWindowIndex];
					for (int i = pSelectItem->ItemWindowIndex - 1; i >= nWindowPosition; i--)
					{
						CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
						if (it != g_pStoreData->m_mapItemWindowPosition.end())
						{
							CItemInfo* pSwapItem = it->second;
							pSwapItem->ItemWindowIndex = i + 1;
							g_pStoreData->m_mapItemWindowPosition[i + 1] = pSwapItem;
							SetSingleInvenIconInfo(pSwapItem);
						}
						else {
							DBGOUT("ERROR : CINFInven::WndProc() Item List crashed!!!!!\n");
							SetSelectItem(NULL);
							return INF_MSGPROC_NORMAL;
						}
					}
					pSelectItem->ItemWindowIndex = nWindowPosition;
					g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
					SetSingleInvenIconInfo(pSelectItem);
					SetAllIconInfo();
				}
			}
			else if (pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
			{
				if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
					break;
				}
				if (g_pInterface->m_pBazaarShop)
				{
					g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_060802_0000, COLOR_ERROR);
					break;
				}
				if (IsAbleReleaseItem(pSelectItem, pSelectItem->ItemWindowIndex))
				{
					MSG_FC_ITEM_CHANGE_WINDOW_POSITION* pMsg;
					char buffer[SIZE_MAX_PACKET];
					*(MessageType_t*)buffer = T_FC_ITEM_CHANGE_WINDOW_POSITION;
					pMsg = (MSG_FC_ITEM_CHANGE_WINDOW_POSITION*)(buffer + SIZE_FIELD_TYPE_HEADER);
					pMsg->CharacterUniqueNumber = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber;
					pMsg->FromItemUniqueNumber = pSelectItem->UniqueNumber;
					pMsg->FromItemWindowIndex = pSelectItem->ItemWindowIndex;
					pMsg->ToItemUniqueNumber = 0;
					int count = 0;
					int i; for (i = 0; i < MAX_EQUIP_POS; i++)
					{
						if (m_pWearDisplayInfo[i])
							count++;
					}
					i = POS_ITEMWINDOW_OFFSET + g_pStoreData->m_mapItemWindowPosition.size();
					pMsg->ToItemWindowIndex = i;
					if (pSelectItem)
					{
						g_pShuttleChild->m_pSkill->DeleteSkillFromWearItem(pSelectItem->Kind);
					}
					g_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(MSG_FC_ITEM_CHANGE_WINDOW_POSITION));
				}
				else
				{
					char buf[128];
					wsprintf(buf, STRMSG_C_ITEM_0007);
					g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
				}
			}
		}
		else if (pSelectItem &&
			pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_GARBAGE_START_X &&
			pt.x<CITY_BASE_NPC_BOX_START_X + INVEN_GARBAGE_START_X + INVEN_GARBAGE_SIZE &&
			pt.y>CITY_SHOP_INVEN_START_Y + INVEN_GARBAGE_START_Y &&
			pt.y < CITY_SHOP_INVEN_START_Y + INVEN_GARBAGE_START_Y + INVEN_GARBAGE_SIZE
			&& g_pD3dApp->GetMultiSelectItem() <= 0)
		{
			if (!((CINFGameMain*)m_pParent)->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE) &&
				!((CINFGameMain*)m_pParent)->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE_NUM))
			{
				char buf[256];
				ITEM* pITEM = pSelectItem->GetItemInfo();
				if (pITEM)
				{
					if (pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
					{
						((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(STRMSG_C_ITEM_0008, _MESSAGE);
						pSelectItem = NULL;
					}
					else
					{
						if (IS_COUNTABLE_ITEM(pSelectItem->Kind) && pSelectItem->CurrentCount > 1)
						{
							wsprintf(buf, STRMSG_C_ITEM_0009, pITEM->ItemName);
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(buf,
								_Q_ITEM_DELETE_NUM, (DWORD)pSelectItem, pSelectItem->CurrentCount);
						}
						else
						{
							wsprintf(buf, STRMSG_C_ITEM_0010, pITEM->ItemName);
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(buf, _Q_ITEM_DELETE);
						}
						SetDeleteItemInfo(pSelectItem);
					}
				}
			}
		}
		if (m_bSelectItem)
		{
			SetSelectItem(NULL);
			m_bSelectItem = FALSE;
			m_bSelectWearItem = FALSE;
			break;
		}
		((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
		SetItemInfo(NULL, 0, 0);
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		if (((CINFGameMain*)m_pParent)->m_pTrade->m_bTrading)
			return INF_MSGPROC_NORMAL;
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int i = (pt.x - (CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X)) / INVEN_SLOT_INTERVAL;
		if (pt.x > CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X &&
			i >= 0 && i < INVEN_X_NUMBER &&
			pt.x < CITY_BASE_NPC_BOX_START_X + INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
		{
			int j = (pt.y - CITY_SHOP_INVEN_START_Y - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			if (pt.y > CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y &&
				j >= 0 && j < INVEN_Y_NUMBER &&
				pt.y < CITY_SHOP_INVEN_START_Y + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				if (0 == m_struDeleteItem.UniqueNumber && m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i])
				{
					CItemInfo* pItemInfo = (CItemInfo*)m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem;
					ITEM* pITEM = pItemInfo->GetItemInfo();
					if (pITEM)
					{
						if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC())
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_080603_0100, COLOR_ERROR);
						}
						else
						{
							switch (pITEM->Position)
							{
							case POS_INVALID_POSITION:
							{
								g_pD3dApp->DelMultiItemList(TRUE);
								if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_PUT_COUNTABLE_ITEM))
								{
									break;
								}
								SendUseItem((ITEM_BASE*)pItemInfo);
							}
							break;
							case POS_PROWIN:
							case POS_PROWOUT:
							case POS_WINGIN:
							case POS_WINGOUT:
							case POS_PROW:
							case POS_CENTER:
							case POS_REAR:
							case POS_ACCESSORY_UNLIMITED:
							case POS_ACCESSORY_TIME_LIMIT:
							case POS_PET:
							{
								SetSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
								SendChangeWearWindowPos(pITEM->Position);
							}
							break;
							default:
							{
								char buf[128];
								wsprintf(buf, STRMSG_C_ITEM_0011, pItemInfo->ItemNum);
								g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
							}
							}
						}
					}
					SetSelectItem(NULL);
					((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
				}
			}
		}
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_CONTROL:
		{
			g_pD3dApp->OnCtrlBtnClick(TRUE);
		}
		break;
		}
	}
	break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_CONTROL:
		{
			g_pD3dApp->OnCtrlBtnClick(FALSE);
		}
		break;
		}
	}
	break;
	}
	return INF_MSGPROC_NORMAL;
}
int CINFInven::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FLOG("CINFInven::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)");
	ProcessRotationUnitWnd(uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		if (pt.x > INVEN_ITEM_SLOT_START_X &&
			pt.x < INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * INVEN_X_NUMBER &&
			pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
			pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * INVEN_Y_NUMBER)
		{
			if (GetScrollLine() > 0)
			{
				if ((int)wParam < 0)
				{
					m_nInvenCurrentScroll++;
					if (m_nInvenCurrentScroll > GetScrollLine())
					{
						m_nInvenCurrentScroll = GetScrollLine();
						m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					}
					else
					{
						m_nInvenCurrentScrollPosition = GetScrollLineInterval(m_nInvenCurrentScroll);
					}
					SetAllIconInfo();
					SetItemInfo(NULL, 0, 0);
				}
				else
				{
					m_nInvenCurrentScroll--;
					if (m_nInvenCurrentScroll < 0)
					{
						m_nInvenCurrentScroll = 0;
						m_nInvenCurrentScrollPosition = 0;
					}
					else
					{
						m_nInvenCurrentScrollPosition = GetScrollLineInterval(m_nInvenCurrentScroll);
					}
					SetAllIconInfo();
					SetItemInfo(NULL, 0, 0);
				}
			}
			return INF_MSGPROC_BREAK;
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		((CINFGameMain*)m_pParent)->SetToolTip(NULL, 0, 0);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
			((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS)
		{
			m_bSelectItem = TRUE;
		}
		if (m_bScrollLock && GetScrollLine() > 0)
		{
			if (pt.x >= INVEN_SCROLL_LINE_START_X - 20 &&
				pt.x <= INVEN_SCROLL_LINE_START_X + 20)
			{
				if (pt.y >= nWindowPosY + INVEN_SCROLL_LINE_START_Y &&
					pt.y <= nWindowPosY + INVEN_SCROLL_LINE_START_Y + INVEN_SCROLL_LINE_LENGTH)
				{
					int nMove = pt.y - m_ptMouse.y;
					m_nInvenCurrentScrollPosition += nMove;
					m_nInvenCurrentScroll = GetScrollLineNumber(m_nInvenCurrentScrollPosition);
					if (m_nInvenCurrentScrollPosition < 0)
					{
						m_nInvenCurrentScroll = 0;
						m_nInvenCurrentScrollPosition = 0;
					}
					else if (m_nInvenCurrentScrollPosition > INVEN_SCROLL_LINE_MOVE_LENGTH)
					{
						m_nInvenCurrentScroll = GetScrollLine();
						m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					}
					if (nMove != 0)
					{
						m_ptMouse = pt;
						SetAllIconInfo();
					}
				}
				else if (pt.y < nWindowPosY + INVEN_SCROLL_LINE_START_Y)
				{
					m_nInvenCurrentScroll = 0;
					m_nInvenCurrentScrollPosition = 0;
					SetAllIconInfo();
				}
				else if (pt.y > nWindowPosY + INVEN_SCROLL_LINE_START_Y + INVEN_SCROLL_LINE_LENGTH)
				{
					m_nInvenCurrentScroll = GetScrollLine();
					m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
					SetAllIconInfo();
				}
			}
		}
		if (!m_bSelectItem)
		{
			SetItemInfo(NULL, 0, 0);
			if (pt.x > POS_PROW_X &&
				pt.x<POS_PROW_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROW_Y &&
				pt.y < nWindowPosY + POS_PROW_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROW])
					SetItemInfo(m_pWearDisplayInfo[POS_PROW], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0015);
				return INF_MSGPROC_BREAK;
			}
			else if (
				pt.x > POS_PROWIN_X &&
				pt.x<POS_PROWIN_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROWIN_Y &&
				pt.y < nWindowPosY + POS_PROWIN_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROWIN])
					SetItemInfo(m_pWearDisplayInfo[POS_PROWIN], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0016);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_PROWOUT_X &&
				pt.x<POS_PROWOUT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROWOUT_Y &&
				pt.y < nWindowPosY + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_PROWOUT])
					SetItemInfo(m_pWearDisplayInfo[POS_PROWOUT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0017);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_WINGIN_X &&
				pt.x<POS_WINGIN_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_WINGIN_Y &&
				pt.y < nWindowPosY + POS_WINGIN_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_WINGIN])
					SetItemInfo(m_pWearDisplayInfo[POS_WINGIN], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_060720_0100);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_WINGOUT_X &&
				pt.x<POS_WINGOUT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_WINGOUT_Y &&
				pt.y < nWindowPosY + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_WINGOUT])
					SetItemInfo(m_pWearDisplayInfo[POS_WINGOUT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0018);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_CENTER_X &&
				pt.x<POS_CENTER_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_CENTER_Y &&
				pt.y < nWindowPosY + POS_CENTER_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_CENTER])
					SetItemInfo(m_pWearDisplayInfo[POS_CENTER], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0019);
				return INF_MSGPROC_BREAK;
			}
			else if (m_pWearDisplayInfo[POS_REAR] &&
				pt.x > POS_REAR_X &&
				pt.x<POS_REAR_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_REAR_Y &&
				pt.y < nWindowPosY + POS_REAR_Y + INVEN_SLOT_SIZE)
			{
				SetItemInfo(m_pWearDisplayInfo[POS_REAR], pt.x, pt.y);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_ATTACHMENT_X &&
				pt.x<POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_ATTACHMENT_Y &&
				pt.y < nWindowPosY + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED])
					SetItemInfo(m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_TOOLTIP_0020);
				return INF_MSGPROC_BREAK;
			}
			else if (pt.x > POS_PET_X && pt.x<POS_PET_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PET_Y && pt.y < nWindowPosY + POS_PET_Y + INVEN_SLOT_SIZE)
			{
				if (m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT])
					SetItemInfo(m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT], pt.x, pt.y);
				else
					((CINFGameMain*)m_pParent)->SetToolTip(pt.x, pt.y, STRMSG_C_061018_0102);
				return INF_MSGPROC_BREAK;
			}
			else
			{
				int i = (pt.x - INVEN_ITEM_SLOT_START_X) / INVEN_SLOT_INTERVAL;
				if (pt.x > INVEN_ITEM_SLOT_START_X &&
					i >= 0 && i < INVEN_X_NUMBER &&
					pt.x < INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
				{
					int j = (pt.y - nWindowPosY - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
					if (pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
						j >= 0 && j < INVEN_Y_NUMBER &&
						pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
					{
						if (m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i])
						{
							ITEM* item = NULL;
							if (item)
							{
								m_iWearPosition = item->Position;
							}
							else
							{
								m_iWearPosition = POS_INVALID_POSITION;
							}
							SetItemInfo(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i], pt.x, pt.y);
							return INF_MSGPROC_BREAK;
						}
						else
						{
							m_iWearPosition = POS_INVALID_POSITION;
							if (!m_pSelectItem)
							{
								SetItemInfo(NULL, 0, 0);
							}
						}
					}
				}
				else
				{
					m_iWearPosition = POS_INVALID_POSITION;
					if (!m_pSelectItem)
					{
						SetItemInfo(NULL, 0, 0);
					}
				}
			}
		}
		else
		{
			if (!m_pSelectItem)
			{
				SetItemInfo(NULL, 0, 0);
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		m_bMouseDownState = TRUE;
		if (g_pStoreData->m_mapItemUniqueNumber.size() > INVEN_X_NUMBER &&
			pt.x > INVEN_SCROLL_BUTTON_START_X &&
			pt.x < INVEN_SCROLL_BUTTON_START_X + INVEN_SCROLL_BUTTON_WIDTH &&
			pt.y > nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + m_nInvenCurrentScrollPosition &&
			pt.y < nWindowPosY + INVEN_SCROLL_BUTTON_START_Y + INVEN_SCROLL_BUTTON_HEIGHT + m_nInvenCurrentScrollPosition)
		{
			m_bScrollLock = TRUE;
			m_ptMouse = pt;
		}
		else
		{
			m_bScrollLock = FALSE;
		}
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem &&
			((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType == ITEM_INVEN_POS)
		{
			m_bSelectItem = TRUE;
		}
		if (m_pWearDisplayInfo[POS_PROW] && pt.x > POS_PROW_X && pt.x<POS_PROW_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROW_Y && pt.y < nWindowPosY + POS_PROW_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_PROW_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROW_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROW]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_PROWIN] && pt.x > POS_PROWIN_X && pt.x<POS_PROWIN_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROWIN_Y && pt.y < nWindowPosY + POS_PROWIN_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_PROWIN_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROWIN_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROWIN]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_PROWOUT] && pt.x > POS_PROWOUT_X && pt.x<POS_PROWOUT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PROWOUT_Y && pt.y < nWindowPosY + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_PROWOUT_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PROWOUT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_PROWOUT]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_WINGIN] && pt.x > POS_WINGIN_X && pt.x<POS_WINGIN_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_WINGIN_Y && pt.y < nWindowPosY + POS_WINGIN_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_WINGIN_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_WINGIN_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_WINGIN]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_WINGOUT] && pt.x > POS_WINGOUT_X && pt.x<POS_WINGOUT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_WINGOUT_Y && pt.y < nWindowPosY + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_WINGOUT_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_WINGOUT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_WINGOUT]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_CENTER] && pt.x > POS_CENTER_X && pt.x<POS_CENTER_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_CENTER_Y && pt.y < nWindowPosY + POS_CENTER_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_CENTER_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_CENTER_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_CENTER]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_REAR] && pt.x > POS_REAR_X && pt.x<POS_REAR_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_REAR_Y && pt.y < nWindowPosY + POS_REAR_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_REAR_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_REAR_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_REAR]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED] && pt.x > POS_ATTACHMENT_X && pt.x<POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_ATTACHMENT_Y && pt.y < nWindowPosY + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_ATTACHMENT_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_ATTACHMENT_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_ACCESSORY_UNLIMITED]);
			m_bSelectWearItem = TRUE;
		}
		else if (m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT] && pt.x > POS_PET_X && pt.x<POS_PET_X + INVEN_SLOT_SIZE &&
			pt.y>nWindowPosY + POS_PET_Y && pt.y < nWindowPosY + POS_PET_Y + INVEN_SLOT_SIZE)
		{
			if (g_pGameMain->m_pTrade->m_bTrading)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_TRADE_0004, _MESSAGE);
				return INF_MSGPROC_BREAK;
			}
			m_nRenderMoveIconIntervalWidth = pt.x - POS_PET_X;
			m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + POS_PET_Y);
			SetSelectItem(m_pWearDisplayInfo[POS_ACCESSORY_TIME_LIMIT]);
			m_bSelectWearItem = TRUE;
		}
		int i = (pt.x - INVEN_ITEM_SLOT_START_X) / INVEN_SLOT_INTERVAL;
		if (pt.x > INVEN_ITEM_SLOT_START_X &&
			i >= 0 && i < INVEN_X_NUMBER &&
			pt.x < INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
		{
			int j = (pt.y - nWindowPosY - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			if (j >= 0 &&
				j < INVEN_Y_NUMBER &&
				m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i] && pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
				pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				m_nRenderMoveIconIntervalWidth = pt.x - (INVEN_ITEM_SLOT_START_X + (INVEN_SLOT_INTERVAL * i));
				m_nRenderMoveIconIntervalHeight = pt.y - (nWindowPosY + INVEN_ITEM_SLOT_START_Y + (INVEN_SLOT_INTERVAL * j));
				m_bSelectWearItem = FALSE;
				if (m_bEnchantState && m_pEnchantItem)
				{
					m_bEnchantState = FALSE;
					((CINFGameMain*)m_pParent)->m_pInfWindow->DeleteMsgBox(_Q_USE_ENCHANT_ITEM_CARD);
				}
				else
				{
					if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem == NULL)
					{
						SetSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
					}
				}
			}
		}
		if (pt.x < INVEN_FULL_BACK_W &&
			pt.y>nWindowPosY &&
			pt.y < nWindowPosY + INVEN_FULL_BACK_H)
		{
			g_pInterface->SetWindowOrder(WNDLeftRightWindow);
			return INF_MSGPROC_BREAK;
		}
		SetItemInfo(NULL, 0, 0);
		if (m_bSelectItem)
		{
			return INF_MSGPROC_BREAK;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		m_bMouseDownState = FALSE;
		m_bScrollLock = FALSE;
		if (g_pGameMain->m_pQuickSlot->m_nItemType == QSLOT_ITEMTYPE_SKILL ||
			g_pGameMain->m_pQuickSlot->m_nItemType == QSLOT_ITEMTYPE_ITEM)
		{
			break;
		}
		CItemInfo* pSelectItem = NULL;
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem)
		{
			pSelectItem = (CItemInfo*)(((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem);
		}
		if (((CINFGameMain*)m_pParent)->m_stSelectItem.bySelectType != ITEM_INVEN_POS)
		{
			break;
		}
		if (g_pGameMain->m_pQuickSlot->LButtonUpQuickSlot(pt))
		{
			break;
		}
		else if (pSelectItem && pt.x > 16 && pt.x<295 && pt.y>nWindowPosY + 32 && pt.y < nWindowPosY + 202)
		{
			if (pt.x > POS_PROW_X && pt.x<POS_PROW_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROW_Y && pt.y < nWindowPosY + POS_PROW_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_PROW);
			}
			else if (pt.x > POS_PROWIN_X && pt.x<POS_PROWIN_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROWIN_Y && pt.y < nWindowPosY + POS_PROWIN_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_PROWIN);
			}
			else if (pt.x > POS_PROWOUT_X && pt.x<POS_PROWOUT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PROWOUT_Y && pt.y < nWindowPosY + POS_PROWOUT_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_PROWOUT);
			}
			else if (pt.x > POS_WINGIN_X && pt.x<POS_WINGIN_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_WINGIN_Y && pt.y < nWindowPosY + POS_WINGIN_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_WINGIN);
			}
			else if (pt.x > POS_WINGOUT_X && pt.x<POS_WINGOUT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_WINGOUT_Y && pt.y < nWindowPosY + POS_WINGOUT_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_WINGOUT);
			}
			else if (pt.x > POS_CENTER_X && pt.x<POS_CENTER_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_CENTER_Y && pt.y < nWindowPosY + POS_CENTER_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_CENTER);
			}
			else if (pt.x > POS_REAR_X && pt.x<POS_REAR_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_REAR_Y && pt.y < nWindowPosY + POS_REAR_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_REAR);
			}
			else if (pt.x > POS_ATTACHMENT_X && pt.x<POS_ATTACHMENT_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_ATTACHMENT_Y && pt.y < nWindowPosY + POS_ATTACHMENT_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_ACCESSORY_UNLIMITED);
			}
			else if (pt.x > POS_PET_X && pt.x<POS_PET_X + INVEN_SLOT_SIZE &&
				pt.y>nWindowPosY + POS_PET_Y && pt.y < nWindowPosY + POS_PET_Y + INVEN_SLOT_SIZE)
			{
				SendChangeWearWindowPos(POS_ACCESSORY_TIME_LIMIT);
			}
		}
		else if (pSelectItem && pt.x > INVEN_ITEM_SLOT_START_X && pt.x<INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * INVEN_X_NUMBER &&
			pt.y>nWindowPosY + INVEN_ITEM_SLOT_START_Y && pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * INVEN_Y_NUMBER)
		{
			int i = (pt.x - INVEN_ITEM_SLOT_START_X) / INVEN_SLOT_INTERVAL;
			int j = (pt.y - nWindowPosY - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			int nWindowPosition = j * INVEN_X_NUMBER + i + m_nInvenCurrentScroll * INVEN_X_NUMBER + POS_ITEMWINDOW_OFFSET;
			if (pSelectItem->ItemWindowIndex
				&& nWindowPosition != pSelectItem->ItemWindowIndex &&
				pSelectItem->ItemWindowIndex >= POS_ITEMWINDOW_OFFSET &&
				pt.x > INVEN_ITEM_SLOT_START_X &&
				i >= 0 && i < INVEN_X_NUMBER &&
				pt.x < INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE &&
				pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
				j >= 0 && j < INVEN_Y_NUMBER &&
				pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				if (pSelectItem->ItemWindowIndex < nWindowPosition)
				{
					INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = m_pInvenDisplayInfo[pSelectItem->ItemWindowIndex];
					for (int i = pSelectItem->ItemWindowIndex + 1; i <= nWindowPosition; i++)
					{
						CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
						if (it != g_pStoreData->m_mapItemWindowPosition.end())
						{
							CItemInfo* pSwapItem = it->second;
							pSwapItem->ItemWindowIndex = i - 1;
							g_pStoreData->m_mapItemWindowPosition[i - 1] = pSwapItem;
							SetSingleInvenIconInfo(pSwapItem);
						}
						else
						{
							pSelectItem->ItemWindowIndex = i - 1;
							g_pStoreData->m_mapItemWindowPosition[i - 1] = pSelectItem;
							SetAllIconInfo();
							SetSelectItem(NULL);
							return INF_MSGPROC_NORMAL;
						}
					}
					pSelectItem->ItemWindowIndex = nWindowPosition;
					g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
					SetSingleInvenIconInfo(pSelectItem);
					SetAllIconInfo();
				}
				else if (pSelectItem->ItemWindowIndex > nWindowPosition)
				{
					INVEN_DISPLAY_INFO* pSelectInvenDisplayInfo = m_pInvenDisplayInfo[pSelectItem->ItemWindowIndex];
					for (int i = pSelectItem->ItemWindowIndex - 1; i >= nWindowPosition; i--)
					{
						CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(i);
						if (it != g_pStoreData->m_mapItemWindowPosition.end())
						{
							CItemInfo* pSwapItem = it->second;
							pSwapItem->ItemWindowIndex = i + 1;
							g_pStoreData->m_mapItemWindowPosition[i + 1] = pSwapItem;
							SetSingleInvenIconInfo(pSwapItem);
						}
						else {
							DBGOUT("ERROR : CINFInven::WndProc() Item List crashed!!!!!\n");
							SetSelectItem(NULL);
							return INF_MSGPROC_NORMAL;
						}
					}
					pSelectItem->ItemWindowIndex = nWindowPosition;
					g_pStoreData->m_mapItemWindowPosition[nWindowPosition] = pSelectItem;
					SetSingleInvenIconInfo(pSelectItem);
					SetAllIconInfo();
				}
			}
			else if (pSelectItem->ItemWindowIndex >= 0 && pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
			{
				if (IsAbleReleaseItem(pSelectItem, pSelectItem->ItemWindowIndex))
				{
					MSG_FC_ITEM_CHANGE_WINDOW_POSITION* pMsg;
					char buffer[SIZE_MAX_PACKET];
					*(MessageType_t*)buffer = T_FC_ITEM_CHANGE_WINDOW_POSITION;
					pMsg = (MSG_FC_ITEM_CHANGE_WINDOW_POSITION*)(buffer + SIZE_FIELD_TYPE_HEADER);
					pMsg->CharacterUniqueNumber = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber;
					pMsg->FromItemUniqueNumber = pSelectItem->UniqueNumber;
					pMsg->FromItemWindowIndex = pSelectItem->ItemWindowIndex;
					pMsg->ToItemUniqueNumber = 0;
					int count = 0;
					int i; for (i = 0; i < MAX_EQUIP_POS; i++)
					{
						if (m_pWearDisplayInfo[i])
							count++;
					}
					i = POS_ITEMWINDOW_OFFSET + g_pStoreData->m_mapItemWindowPosition.size();
					pMsg->ToItemWindowIndex = i;
					if (pSelectItem)
					{
						g_pShuttleChild->m_pSkill->DeleteSkillFromWearItem(pSelectItem->Kind);
					}
					g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(MSG_FC_ITEM_CHANGE_WINDOW_POSITION));
				}
				else
				{
					char buf[128];
					wsprintf(buf, STRMSG_C_ITEM_0007);
					g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
				}
			}
		}
		else if (pSelectItem && pt.x > INVEN_GARBAGE_START_X && pt.x<INVEN_GARBAGE_START_X + INVEN_GARBAGE_SIZE &&
			pt.y>nWindowPosY + INVEN_GARBAGE_START_Y && pt.y < nWindowPosY + INVEN_GARBAGE_START_Y + INVEN_GARBAGE_SIZE)
		{
			if (!((CINFGameMain*)m_pParent)->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE) &&
				!((CINFGameMain*)m_pParent)->m_pInfWindow->IsExistMsgBox(_Q_ITEM_DELETE_NUM))
			{
				char buf[256];
				ITEM* item = g_pDatabase->GetServerItemInfo(pSelectItem->ItemNum);
				if (item)
				{
					if (pSelectItem->ItemWindowIndex < POS_ITEMWINDOW_OFFSET)
					{
						((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(STRMSG_C_ITEM_0008, _MESSAGE);
						pSelectItem = NULL;
					}
					else
					{
						if (IS_COUNTABLE_ITEM(pSelectItem->Kind) && pSelectItem->CurrentCount > 1)
						{
							wsprintf(buf, STRMSG_C_ITEM_0009, item->ItemName);
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(buf,
								_Q_ITEM_DELETE_NUM, (DWORD)pSelectItem, pSelectItem->CurrentCount);
						}
						else
						{
							wsprintf(buf, STRMSG_C_ITEM_0010, item->ItemName);
							((CINFGameMain*)m_pParent)->m_pInfWindow->AddMsgBox(buf, _Q_ITEM_DELETE);
						}
						SetDeleteItemInfo(pSelectItem);
					}
				}
			}
		}
		if (m_bSelectItem)
		{
			SetSelectItem(NULL);
			m_bSelectItem = FALSE;
			m_bSelectWearItem = FALSE;
			break;
		}
		((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
		SetItemInfo(NULL, 0, 0);
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		if (((CINFGameMain*)m_pParent)->m_pTrade->m_bTrading)
			return INF_MSGPROC_NORMAL;
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		CheckMouseReverse(&pt);
		int nWindowPosY = g_pGameMain->m_nLeftWindowY;
		int i = (pt.x - INVEN_ITEM_SLOT_START_X) / INVEN_SLOT_INTERVAL;
		if (pt.x > INVEN_ITEM_SLOT_START_X &&
			i >= 0 && i < INVEN_X_NUMBER &&
			pt.x < INVEN_ITEM_SLOT_START_X + INVEN_SLOT_INTERVAL * i + INVEN_SLOT_SIZE)
		{
			int j = (pt.y - nWindowPosY - INVEN_ITEM_SLOT_START_Y) / INVEN_SLOT_INTERVAL;
			if (pt.y > nWindowPosY + INVEN_ITEM_SLOT_START_Y &&
				j >= 0 && j < INVEN_Y_NUMBER &&
				pt.y < nWindowPosY + INVEN_ITEM_SLOT_START_Y + INVEN_SLOT_INTERVAL * j + INVEN_SLOT_SIZE)
			{
				if (0 == m_struDeleteItem.UniqueNumber && m_pInvenDisplayInfo[(j * INVEN_X_NUMBER) + i])
				{
					ITEM_BASE* pItem = m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]->pItem;
					ITEM* item = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
					if (item)
					{
						if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC())
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_080603_0100, COLOR_ERROR);
						}
						else
						{
							switch (item->Position)
							{
							case POS_INVALID_POSITION:
							{
								g_pD3dApp->DelMultiItemList(TRUE);
								if (g_pGameMain->m_pInfWindow->IsExistMsgBox(_Q_STORE_PUT_COUNTABLE_ITEM))
								{
									break;
								}
								SendUseItem(pItem);
							}
							break;
							case POS_PROWIN:
							case POS_PROWOUT:
							case POS_WINGIN:
							case POS_WINGOUT:
							case POS_PROW:
							case POS_CENTER:
							case POS_REAR:
							case POS_ACCESSORY_UNLIMITED:
							case POS_ACCESSORY_TIME_LIMIT:
							case POS_PET:
							{
								SetSelectItem(m_pInvenDisplayInfo[j * INVEN_X_NUMBER + i]);
								SendChangeWearWindowPos(item->Position);
							}
							break;
							default:
							{
								char buf[128];
								wsprintf(buf, STRMSG_C_ITEM_0011, pItem->ItemNum);
								g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
							}
							}
						}
					}
					SetSelectItem(NULL);
					((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
				}
			}
		}
	}
	break;
	}
	return INF_MSGPROC_NORMAL;
}
void CINFInven::DeleteSelectItem(int count)
{
	FLOG("CINFInven::DeleteSelectItem(int count)");
	if (0 == m_struDeleteItem.UniqueNumber)
	{
		return;
	}
	CItemInfo* pDeleteItem = g_pStoreData->FindItemInInventoryByUniqueNumber(m_struDeleteItem.UniqueNumber);
	if (NULL == pDeleteItem)
	{
		g_pD3dApp->m_pChat->CreateChatChild(STRERR_ERROR_0025, COLOR_ERROR);
		InitDeleteItemInfo();
		return;
	}
	if (IS_COUNTABLE_ITEM(pDeleteItem->Kind) && pDeleteItem->CurrentCount < count)
	{
		InitDeleteItemInfo();
		return;
	}
	MSG_FC_ITEM_THROW_AWAY_ITEM sMsg;
	memset(&sMsg, 0x00, sizeof(sMsg));
	char buffer[SIZE_MAX_PACKET];
	int nType = T_FC_ITEM_THROW_AWAY_ITEM;
	sMsg.Amount = count;
	sMsg.ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
	sMsg.ItemUniqueNumber = pDeleteItem->UniqueNumber;
	memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
	memcpy(buffer + SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
	g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
	InitDeleteItemInfo();
}
void CINFInven::SendUseItem(ITEM_BASE* pItem)
{
	FLOG("CINFInven::SendUseItem(ITEM_BASE* pItem)");
	switch (pItem->Kind)
	{
	case ITEMKIND_ENERGY:
	{
		bool bSend = false;
		ITEM* pIteminfo = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
		CAppEffectData* pEff = NULL;
		switch (pIteminfo->ArrDestParameter[0])
		{
		case DES_CASH_STAT_ALL_INITIALIZE:
		{
			MSG_FC_ITEM_USE_ENERGY sMsg;
			sMsg.ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENERGY, (char*)&sMsg, sizeof(sMsg));
			((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
			bSend = true;
		}
		break;
		case DES_CASH_STAT_PART_INITIALIZE:
		{
			bSend = true;
			if (g_pGameMain->m_pInfWindow->SearchMsgBox(_Q_PART_STAT_RESET))
				break;
			g_pGameMain->m_pInfWindow->RadioClear();
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_STAT_0001);
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_050517_0001);
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_STAT_0003);
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_STAT_0004);
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_STAT_0005);
			g_pGameMain->m_pInfWindow->AddRadioOption(STRMSG_C_STAT_0006);
			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_STAT_0007, _Q_PART_STAT_RESET, 0, 0, 0, pItem->UniqueNumber);
		}
		break;
		case DES_GRADUAL_EP_UP:
		case DES_IN_TIME_EP_UP:
		case DES_IMMEDIATE_EP_UP:
		{
		}
		break;
		case DES_GRADUAL_SP_UP:
		case DES_IN_TIME_SP_UP:
		case DES_IMMEDIATE_SP_UP:
			break;
		case DES_GRADUAL_HP_UP:
		case DES_IN_TIME_HP_UP:
		case DES_IMMEDIATE_HP_UP:
		{
		}
		break;
		case DES_GRADUAL_DP_UP:
		case DES_IN_TIME_DP_UP:
		case DES_IMMEDIATE_DP_UP:
		{
		}
		break;
		case DES_WARP:
		case DES_WARP_OUTPOST:
		{
			if (g_pShuttleChild
				&& g_pShuttleChild->m_pClientParty->IsFormationFlight())
			{
				if (g_pShuttleChild->m_pClientParty->GetPartyInfo().bPartyType == _PARTYMASTER)
				{
					g_pShuttleChild->m_pClientParty->TempPartyFormation(FLIGHT_FORM_NONE);
				}
				else
				{
					g_pShuttleChild->CheckFormationMoveDelete();
				}
			}
		}
		break;
		}
		if (!bSend)
		{
			MSG_FC_ITEM_USE_ENERGY sMsg;
			sMsg.ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_ENERGY, (char*)&sMsg, sizeof(sMsg));
			((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
		}
	}
	break;
	case ITEMKIND_CARD:
	{
		ITEM* pIteminfo = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
		if (NULL == pIteminfo)
		{
			return;
		}
		switch (pIteminfo->ArrDestParameter[0])
		{
		case DES_CASH_NORMAL_RESTORE:
		{
			MSG_FC_ITEM_USE_CARDITEM_RESTORE sMsg;
			memset(&sMsg, 0x00, sizeof(sMsg));
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_CARDITEM_RESTORE, (char*)&sMsg, sizeof(sMsg));
		}
		break;
		case DES_CASH_SUPER_RESTORE:
		{
			if (g_pGameMain && g_pGameMain->m_pInfWindow)
			{
				g_pGameMain->m_bChangeMousePoint = TRUE;
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0006, _Q_SUPER_RESTORE, 0, 0, 0, pItem->UniqueNumber);
			}
		}
		break;
		case DES_CASH_GUILD_ALL_MEMBERS_SUMMON:
		{
			MSG_FC_ITEM_USE_CARDITEM_GUILDSUMMON sMsg;
			memset(&sMsg, 0x00, sizeof(sMsg));
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_CARDITEM_GUILDSUMMON, (char*)&sMsg, sizeof(sMsg));
		}
		break;
		case DES_CASH_GUILD_MEMBER_SUMMON:
		{
			if (g_pGameMain && g_pGameMain->m_pInfWindow)
			{
				g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0007, _Q_MEMBER_SUMMON, 0, 0, 0, pItem->UniqueNumber);
			}
		}
		break;
		case DES_CASH_GUILD:
		{
			if (g_pGameMain && g_pGameMain->m_pInfWindow)
			{
				MSG_FC_ITEM_USE_CARDITEM_GUILD sMsg;
				sMsg.ItemUniqueNumber = pItem->UniqueNumber;
				g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_CARDITEM_GUILD, (char*)&sMsg, sizeof(sMsg));
			}
		}
		break;
		case DES_CASH_MONSTER_SUMMON:
		{
			MSG_FC_ITEM_USE_CARDITEM_MONSTERSUMMON sMsg;
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_CARDITEM_MONSTERSUMMON, (char*)&sMsg, sizeof(sMsg));
		}
		break;
		case DES_CASH_CHANGE_CHARACTERNAME:
		{
			g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_CARD_0008, _Q_USEITEM_NAME_CHANGE, 0, 0, 0, pItem->UniqueNumber);
		}
		break;
		case DES_KILLMARK_EXP:
		{
			char chMsg[256] = { 0, };
			wsprintf(chMsg, STRMSG_C_081230_0207, pItem->ItemInfo->ItemName);
			g_pGameMain->m_pInfWindow->AddMsgBox(chMsg, _Q_USEITEM_KILL_MARK, 0, 0, 0, pItem->UniqueNumber);
		}
		break;
		case DES_CHAT_ALL_INFLUENCE:
		{
			DoModalChatAllInfluence(pItem->UniqueNumber);
		}
		break;
		case DES_CASH_CHANGE_PILOTFACE:
		{
			if (g_pD3dApp->m_bCharacter)
			{
				g_pGameMain->m_pChangeCharactor->SetActive(TRUE, pItem->UniqueNumber);
			}
			else
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_071121_0100, COLOR_SYSTEM);
			}
		}
		break;
		default:
		{
			MSG_FC_ITEM_USE_ITEM sMsg;
			memset(&sMsg, 0x00, sizeof(sMsg));
			char buffer[SIZE_MAX_PACKET];
			sMsg.ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
			sMsg.ItemUniqueNumber = pItem->UniqueNumber;
			int nType = T_FC_ITEM_USE_ITEM;
			memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
			memcpy(buffer + SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
			g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
			((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
		}
		break;
		}
	}
	break;
	case ITEMKIND_INFLUENCE_BUFF:
	{
		MSG_FC_ITEM_USE_INFLUENCE_BUFF sMsg;
		sMsg.ItemUniqueNumber = pItem->UniqueNumber;
		g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_INFLUENCE_BUFF, (char*)&sMsg, sizeof(sMsg));
	}
	break;
	case ITEMKIND_INFLUENCE_GAMEEVENT:
	{
		MSG_FC_ITEM_USE_INFLUENCE_GAMEEVENT sMsg;
		sMsg.ItemUniqueNumber = pItem->UniqueNumber;
		g_pFieldWinSocket->SendMsg(T_FC_ITEM_USE_INFLUENCE_GAMEEVENT, (char*)&sMsg, sizeof(sMsg));
	}
	break;
	case ITEMKIND_BULLET:
	{
		if (g_pShuttleChild->m_pPrimaryWeapon || g_pShuttleChild->m_pSecondaryWeapon)
		{
			bool bSend = false;
			BOOL bPrimaryFullWeapon, bSecondaryFullWeapon;
			bPrimaryFullWeapon = bSecondaryFullWeapon = FALSE;
			CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(POS_PROWOUT);
			if (it != g_pStoreData->m_mapItemWindowPosition.end())
			{
				ITEM* p = it->second->GetItemInfo();
				ITEM* p2 = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
				if (p && p2 && (RECHARGE_BULLET_ALL_CALIBER == p2->Caliber || p->Caliber == p2->Caliber))
				{
					int nMaxCharge = p->Charging;
					nMaxCharge = g_pGameMain->GetMaxBulletItem(p->Charging, it->second->ColorCode);
					if (it->second->CurrentCount == nMaxCharge)
					{
						bPrimaryFullWeapon = TRUE;
						g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_ITEM_0012, COLOR_SYSTEM);
					}
					else
					{
						bSend = true;
					}
				}
			}
			{
				it = g_pStoreData->m_mapItemWindowPosition.find(POS_WINGOUT);
				if (it != g_pStoreData->m_mapItemWindowPosition.end())
				{
					ITEM* p = it->second->GetItemInfo();
					ITEM* p2 = g_pDatabase->GetServerItemInfo(pItem->ItemNum);
					if (p && p2 && (RECHARGE_BULLET_ALL_CALIBER == p2->Caliber || p->Caliber == p2->Caliber))
					{
						int nMaxCharge = p->Charging;
						nMaxCharge = g_pGameMain->GetMaxBulletItem(p->Charging, it->second->ColorCode);
						if (it->second->CurrentCount == nMaxCharge)
						{
							g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_ITEM_0013, COLOR_SYSTEM);
							bSecondaryFullWeapon = TRUE;
						}
						else
						{
							bSend = true;
						}
					}
				}
			}
			bSend = true;
			if ((TRUE == bPrimaryFullWeapon)
				&& (TRUE == bSecondaryFullWeapon))
			{
				bSend = false;
			}
			if (bSend)
			{
				MSG_FC_ITEM_USE_ITEM sMsg;
				memset(&sMsg, 0x00, sizeof(sMsg));
				char buffer[SIZE_MAX_PACKET];
				sMsg.ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
				sMsg.ItemUniqueNumber = pItem->UniqueNumber;
				int nType = T_FC_ITEM_USE_ITEM;
				memcpy(buffer, &nType, SIZE_FIELD_TYPE_HEADER);
				memcpy(buffer + SIZE_FIELD_TYPE_HEADER, &sMsg, sizeof(sMsg));
				g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(sMsg));
				((CINFGameMain*)m_pParent)->SetToolTip(0, 0, NULL);
			}
			else if ((TRUE != bPrimaryFullWeapon) && (TRUE != bSecondaryFullWeapon))
			{
				g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_ITEM_0014, COLOR_SYSTEM);
			}
		}
		else
		{
			g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_ITEM_0015, COLOR_SYSTEM);
		}
	}
	break;
	case ITEMKIND_ENCHANT:
	case ITEMKIND_GAMBLE:
	{
	}
	break;
	case ITEMKIND_TANK:
		break;
	case ITEMKIND_RANDOMBOX:
	{
		INIT_MSG_WITH_BUFFER(MSG_FC_ITEM_USE_RANDOMBOX, T_FC_ITEM_USE_RANDOMBOX, pSRandombox, Sendbuf);
		pSRandombox->ItemUID = pItem->UniqueNumber;
		g_pD3dApp->m_pFieldWinSocket->Write(Sendbuf, MSG_SIZE(MSG_FC_ITEM_USE_RANDOMBOX));
		g_pD3dApp->m_bRequestEnable = FALSE;
	}
	break;
	case ITEMKIND_SKILL_SUPPORT_ITEM:
	{
		INIT_MSG_WITH_BUFFER(MSG_FC_ITEM_USE_SKILL_SUPPORT_ITEM, T_FC_ITEM_USE_SKILL_SUPPORT_ITEM, pSkillSupportItem, Sendbuf);
		pSkillSupportItem->ClientIndex = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex;
		pSkillSupportItem->ItemUniqueNumber = pItem->UniqueNumber;
		g_pD3dApp->m_pFieldWinSocket->Write(Sendbuf, MSG_SIZE(MSG_FC_ITEM_USE_SKILL_SUPPORT_ITEM));
	}
	break;
	default:
	{
		char buf[128];
		wsprintf(buf, STRMSG_C_ITEM_0016, pItem->ItemInfo->ItemName);
		g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
	}
	break;
	}
}
void CINFInven::SendChangeWearWindowPos(int nWindowPosition)
{
	int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();
	if (nSelItemCnt > 0)
	{
		return;
	}
	FLOG("CINFInven::SendChangeWearWindowPos(int nWindowPosition)");
	if (g_pInterface->m_pCityBase->GetCurrentBuildingNPC() && IS_TUNING_COLOR_SHOP(g_pInterface->m_pCityBase->GetCurrentBuildingNPC()->buildingInfo.BuildingKind))
	{
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_051207_0003, COLOR_ERROR);
		return;
	}
	if (g_pInterface->m_pBazaarShop)
	{
		g_pD3dApp->m_pChat->CreateChatChild(STRMSG_C_060802_0000, COLOR_ERROR);
		return;
	}
	CItemInfo* pSelectItem = NULL;
	if (((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem)
	{
		pSelectItem = (CItemInfo*)(((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem->pItem);
	}
	if (pSelectItem && pSelectItem->ItemWindowIndex != nWindowPosition)
	{
		if (pSelectItem->GetItemInfo()->Position != nWindowPosition)
		{
			return;
		}
		CItemInfo* pWearItemInfo = NULL;
		CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(nWindowPosition);
		if (it != g_pStoreData->m_mapItemWindowPosition.end())
		{
			pWearItemInfo = it->second;
		}
		if (m_pWearDisplayInfo[nWindowPosition] == NULL ||
			(pWearItemInfo && IsAbleChangeItem(pSelectItem, pWearItemInfo, nWindowPosition)))
		{
			if (g_pTutorial->IsTutorialMode() &&
				g_pTutorial->GetLesson() == L6)
			{
			}
			else
			{
				MSG_FC_ITEM_CHANGE_WINDOW_POSITION* pMsg;
				char buffer[SIZE_MAX_PACKET];
				*(MessageType_t*)buffer = T_FC_ITEM_CHANGE_WINDOW_POSITION;
				pMsg = (MSG_FC_ITEM_CHANGE_WINDOW_POSITION*)(buffer + SIZE_FIELD_TYPE_HEADER);
				pMsg->CharacterUniqueNumber = g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.CharacterUniqueNumber;
				pMsg->FromItemUniqueNumber = pSelectItem->UniqueNumber;
				pMsg->FromItemWindowIndex = pSelectItem->ItemWindowIndex;
				if (m_pWearDisplayInfo[nWindowPosition])
				{
					pMsg->ToItemUniqueNumber = m_pWearDisplayInfo[nWindowPosition]->pItem->UniqueNumber;
				}
				else
				{
					pMsg->ToItemUniqueNumber = 0;
				}
				pMsg->ToItemWindowIndex = nWindowPosition;
				if (m_pWearDisplayInfo[nWindowPosition])
				{
					g_pShuttleChild->m_pSkill->DeleteSkillFromWearItem(m_pWearDisplayInfo[nWindowPosition]->pItem->Kind);
				}
				g_pD3dApp->m_pFieldWinSocket->Write(buffer, SIZE_FIELD_TYPE_HEADER + sizeof(MSG_FC_ITEM_CHANGE_WINDOW_POSITION));
			}
		}
		else
		{
			char buf[128];
			wsprintf(buf, STRMSG_C_ITEM_0007);
			g_pD3dApp->m_pChat->CreateChatChild(buf, COLOR_ERROR);
		}
	}
}
void CINFInven::SetSelectItem(INVEN_DISPLAY_INFO* pDisplayInfo)
{
	FLOG("CINFInven::SetSelectItem(INVEN_DISPLAY_INFO* pDisplayInfo)");
	POINT ptIcon;
	ptIcon.x = m_nRenderMoveIconIntervalWidth;
	ptIcon.y = m_nRenderMoveIconIntervalHeight;
	((CINFGameMain*)m_pParent)->SetSelectItem(pDisplayInfo, ptIcon, ITEM_INVEN_POS);
}
void CINFInven::SetMultiSelectItem(INVEN_DISPLAY_INFO* pDisplayInfo)
{
	POINT ptIcon;
	ptIcon.x = m_nRenderMoveIconIntervalWidth;
	ptIcon.y = m_nRenderMoveIconIntervalHeight;
	((CINFGameMain*)m_pParent)->SetMultiSelectItem(pDisplayInfo,
		((CINFGameMain*)m_pParent)->m_stSelectItem.pSelectItem, ptIcon);
}
void CINFInven::DeleteEnchantItem(UINT nUniqueNumber)
{
	FLOG("CINFInven::DeleteEnchantItem(UINT nUniqueNumber)");
	m_pEnchantItem = NULL;
}
BOOL CINFInven::IsAbleSetItem(CItemInfo* pItemInfo, int nWindowPosition)
{
	GEAR_STAT sGearStat = g_pShuttleChild->m_myShuttleInfo.TotalGearStat;
	ITEM* pItem = pItemInfo->GetRealItemInfo();
	int nArrParamCnt = 0;
	for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
	{
		SetGearStatByItem(sGearStat,
			pItem->ArrDestParameter[nArrParamCnt],
			pItem->ArrParameterValue[nArrParamCnt]);
	}
	CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(POS_REAR);
	if (it != g_pStoreData->m_mapItemWindowPosition.end())
	{
		CItemInfo* pWearItemInfo = it->second;
		if (pWearItemInfo->GetRealItemInfo()->ReqGearStat.AttackPart > sGearStat.AttackPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.DefensePart > sGearStat.DefensePart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.DodgePart > sGearStat.DodgePart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.FuelPart > sGearStat.FuelPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.ShieldPart > sGearStat.ShieldPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.SoulPart > sGearStat.SoulPart)
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CINFInven::IsAbleReleaseItem(CItemInfo* pItemInfo, int nWindowPosition)
{
	if (nWindowPosition == POS_REAR)
	{
		return TRUE;
	}
	GEAR_STAT sGearStat = g_pShuttleChild->m_myShuttleInfo.TotalGearStat;
	ITEM* pItem = pItemInfo->GetRealItemInfo();
	int nArrParamCnt = 0;
	for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
	{
		ReleaseGearStatByItem(sGearStat, pItem->ArrDestParameter[nArrParamCnt],
			pItem->ArrParameterValue[nArrParamCnt]);
	}
	CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(POS_REAR);
	if (it != g_pStoreData->m_mapItemWindowPosition.end())
	{
		CItemInfo* pWearItemInfo = it->second;
		if (pWearItemInfo->GetRealItemInfo()->ReqGearStat.AttackPart > sGearStat.AttackPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.DefensePart > sGearStat.DefensePart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.DodgePart > sGearStat.DodgePart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.FuelPart > sGearStat.FuelPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.ShieldPart > sGearStat.ShieldPart ||
			pWearItemInfo->GetRealItemInfo()->ReqGearStat.SoulPart > sGearStat.SoulPart)
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CINFInven::IsAbleChangeItem(CItemInfo* pSetItemInfo, CItemInfo* pReleaseItemInfo, int nWindowPosition)
{
	GEAR_STAT sGearStat = g_pShuttleChild->m_myShuttleInfo.TotalGearStat;
	ITEM* pSetItem = pSetItemInfo->GetRealItemInfo();
	ITEM* pReleaseItem = pReleaseItemInfo->GetRealItemInfo();
	int nArrParamCnt = 0;
	for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
	{
		SetGearStatByItem(sGearStat, pSetItem->ArrDestParameter[nArrParamCnt],
			pSetItem->ArrParameterValue[nArrParamCnt]);
	}
	for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++)
	{
		ReleaseGearStatByItem(sGearStat, pReleaseItem->ArrDestParameter[nArrParamCnt],
			pReleaseItem->ArrParameterValue[nArrParamCnt]);
	}
	ITEM* pItem = NULL;
	if (nWindowPosition == POS_REAR)
	{
		pItem = pSetItemInfo->GetRealItemInfo();
	}
	else
	{
		CMapItemWindowInventoryIterator it = g_pStoreData->m_mapItemWindowPosition.find(POS_REAR);
		if (it != g_pStoreData->m_mapItemWindowPosition.end())
		{
			pItem = it->second->GetRealItemInfo();
		}
		else
		{
			return TRUE;
		}
	}
	if (pItem->ReqGearStat.AttackPart > sGearStat.AttackPart ||
		pItem->ReqGearStat.DefensePart > sGearStat.DefensePart ||
		pItem->ReqGearStat.DodgePart > sGearStat.DodgePart ||
		pItem->ReqGearStat.FuelPart > sGearStat.FuelPart ||
		pItem->ReqGearStat.ShieldPart > sGearStat.ShieldPart ||
		pItem->ReqGearStat.SoulPart > sGearStat.SoulPart)
	{
		return FALSE;
	}
	return TRUE;
}
void CINFInven::SetGearStatByItem(GEAR_STAT& sGearStat, int nDestParam, float fDestValue)
{
	switch (nDestParam)
	{
	case DES_ATTACK_PART:
		sGearStat.AttackPart += fDestValue;
		break;
	case DES_DEFENSE_PART:
		sGearStat.DefensePart += fDestValue;
		break;
	case DES_FUEL_PART:
		sGearStat.FuelPart += fDestValue;
		break;
	case DES_SOUL_PART:
		sGearStat.SoulPart += fDestValue;
		break;
	case DES_SHIELD_PART:
		sGearStat.ShieldPart += fDestValue;
		break;
	case DES_DODGE_PART:
		sGearStat.DodgePart += fDestValue;
		break;
	}
}
void CINFInven::ReleaseGearStatByItem(GEAR_STAT& sGearStat, int nDestParam, float fDestValue)
{
	switch (nDestParam)
	{
	case DES_ATTACK_PART:
		sGearStat.AttackPart -= fDestValue;
		break;
	case DES_DEFENSE_PART:
		sGearStat.DefensePart -= fDestValue;
		break;
	case DES_FUEL_PART:
		sGearStat.FuelPart -= fDestValue;
		break;
	case DES_SOUL_PART:
		sGearStat.SoulPart -= fDestValue;
		break;
	case DES_SHIELD_PART:
		sGearStat.ShieldPart -= fDestValue;
		break;
	case DES_DODGE_PART:
		sGearStat.DodgePart -= fDestValue;
		break;
	}
}
int	CINFInven::GetScrollLine()
{
	int nInvenTotalNum = g_pStoreData->m_mapItemUniqueNumber.size();
	int nWearItemNum = GetWearItemNum();
	int ntemp = int((nInvenTotalNum - nWearItemNum) / INVEN_X_NUMBER) - 6;
	if (0 != ((nInvenTotalNum - nWearItemNum) % INVEN_X_NUMBER))
		ntemp++;
	if (ntemp < 0)
	{
		ntemp = 0;
	}
	return ntemp;
}
int	CINFInven::GetScrollLineNumber(float nLength)
{
	int ntemp = GetScrollLine();
	float fLineNum = (nLength * ntemp) / INVEN_SCROLL_LINE_MOVE_LENGTH;
	return (int)fLineNum;
}
int CINFInven::ProcessRotationUnitWnd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_pD3dApp->GetHwnd(), &pt);
		CheckMouseReverse(&pt);
		if (ProcessRotationUnit(pt, uMsg) == INF_MSGPROC_BREAK)
			return INF_MSGPROC_BREAK;
	}
	break;
	}
	return INF_MSGPROC_NORMAL;
}
int CINFInven::ProcessRotationUnit(POINT pt, UINT uMsg)
{
	if (uMsg == WM_LBUTTONUP)	SetByPushButton(FALSE);
	m_nRotationState = ROTATION_NONE;
	m_nButtonState = ROTATION_STATE_N;
	m_bTurnLeft = FALSE;
	m_bTurnRight = FALSE;
	m_bTurnUp = FALSE;
	m_bTurnDown = FALSE;
	int PosX = 16;
	int PosY = g_pGameMain->m_nLeftWindowY + 207;
	if (g_pD3dApp->m_dwGameState == _SHOP)
	{
		PosX += CITY_BASE_NPC_BOX_START_X;
	}
	if (pt.x > PosX && pt.x< PosX + 12 && pt.y>PosY + 8 && pt.y < PosY + 30)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			m_nRotationState = ROTATION_LEFT;
			m_nButtonState = ROTATION_STATE_O;
			if (m_bByPush[0] == TRUE)
			{
				m_nRotationState = ROTATION_LEFT;
				m_nButtonState = ROTATION_STATE_P;
				m_bTurnLeft = TRUE;
			}
			break;
		case WM_LBUTTONDOWN:
			m_nRotationState = ROTATION_LEFT;
			m_nButtonState = ROTATION_STATE_P;
			m_bTurnLeft = TRUE;
			m_bByPush[0] = TRUE;
			return INF_MSGPROC_BREAK;
		case WM_LBUTTONUP:
			m_nRotationState = ROTATION_LEFT;
			m_nButtonState = ROTATION_STATE_N;
			return INF_MSGPROC_BREAK;
		}
	}
	else if (pt.x > PosX + 34 && pt.x< PosX + 46 && pt.y>PosY + 8 && pt.y < PosY + 30)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			m_nRotationState = ROTATION_RIGHT;
			m_nButtonState = ROTATION_STATE_P;
			m_bTurnRight = TRUE;
			m_bByPush[1] = TRUE;
			return INF_MSGPROC_BREAK;
		case WM_MOUSEMOVE:
			m_nRotationState = ROTATION_RIGHT;
			m_nButtonState = ROTATION_STATE_O;
			if (m_bByPush[1] == TRUE)
			{
				m_nRotationState = ROTATION_RIGHT;
				m_nButtonState = ROTATION_STATE_P;
				m_bTurnRight = TRUE;
			}
			break;
		case WM_LBUTTONUP:
			m_nRotationState = ROTATION_RIGHT;
			m_nButtonState = ROTATION_STATE_N;
			return INF_MSGPROC_BREAK;
		}
	}
	else if (pt.x > PosX + 13 && pt.x< PosX + 33 && pt.y>PosY && pt.y < PosY + 10)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			m_nRotationState = ROTATION_UP;
			m_nButtonState = ROTATION_STATE_O;
			if (m_bByPush[2] == TRUE)
			{
				m_nRotationState = ROTATION_UP;
				m_nButtonState = ROTATION_STATE_P;
				m_bTurnUp = TRUE;
			}
			break;
		case WM_LBUTTONDOWN:
			m_nRotationState = ROTATION_UP;
			m_nButtonState = ROTATION_STATE_P;
			m_bTurnUp = TRUE;
			m_bByPush[2] = TRUE;
			return INF_MSGPROC_BREAK;
		case WM_LBUTTONUP:
			m_nRotationState = ROTATION_UP;
			m_nButtonState = ROTATION_STATE_N;
			return INF_MSGPROC_BREAK;
		}
	}
	else if (pt.x > PosX + 13 && pt.x<PosX + 33 && pt.y>PosY + 26 && pt.y < PosY + 38)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			m_nRotationState = ROTATION_DOWN;
			m_nButtonState = ROTATION_STATE_O;
			if (m_bByPush[3] == TRUE)
			{
				m_nRotationState = ROTATION_DOWN;
				m_nButtonState = ROTATION_STATE_P;
				m_bTurnDown = TRUE;
			}
			break;
		case WM_LBUTTONDOWN:
			m_nRotationState = ROTATION_DOWN;
			m_nButtonState = ROTATION_STATE_P;
			m_bTurnDown = TRUE;
			m_bByPush[3] = TRUE;
			return INF_MSGPROC_BREAK;
		case WM_LBUTTONUP:
			m_nRotationState = ROTATION_DOWN;
			m_nButtonState = ROTATION_STATE_N;
			return INF_MSGPROC_BREAK;
		}
	}
	else if (pt.x > PosX + 13 && pt.x<PosX + 33 && pt.y>PosY + 13 && pt.y < PosY + 25)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			m_nRotationState = ROTATION_ORIGINAL;
			m_nButtonState = ROTATION_STATE_O;
			break;
		case WM_LBUTTONDOWN:
			m_nRotationState = ROTATION_ORIGINAL;
			m_nButtonState = ROTATION_STATE_P;
			g_pShuttleChild->MirrorTurnOrig();
			return INF_MSGPROC_BREAK;
		case WM_LBUTTONUP:
			m_nRotationState = ROTATION_ORIGINAL;
			m_nButtonState = ROTATION_STATE_N;
			return INF_MSGPROC_BREAK;
		}
	}
	return INF_MSGPROC_NORMAL;
}
void CINFInven::SetByPushButton(BOOL bFlag)
{
	int i; for (i = 0; i < 4; i++)
	{
		m_bByPush[i] = FALSE;
	}
}
int CINFInven::GetWearItemNum()
{
	int count = 0;
	for (int i = 0; i < SIZE_MAX_POS; i++)
	{
		if (m_pWearDisplayInfo[i])
		{
			count++;
		}
	}
	return count;
}
float CINFInven::GetScrollLineInterval(int nScrollLineNum)
{
	int ntemp = GetScrollLine();
	float fLineNum = 0.0f;
	if (ntemp)
		fLineNum = (INVEN_SCROLL_LINE_MOVE_LENGTH * nScrollLineNum) / ntemp;
	return fLineNum;
}
void CINFInven::SetScrollEndLine()
{
	m_nInvenCurrentScroll = GetScrollLine();
	m_nInvenCurrentScrollPosition = INVEN_SCROLL_LINE_MOVE_LENGTH;
}
void CINFInven::InitInven()
{
	m_nInvenCurrentScroll = 0;
	m_nInvenCurrentScrollPosition = 0;
	SetAllIconInfo();
}
void CINFInven::RenderInvenBack(BOOL bShop)
{
	D3DXMATRIX pTemp, pMatrix, pMatBack, pMatOldProj, pMatOldView, pMatPresView;
	D3DXMatrixIdentity(&pMatrix);
	D3DXMatrixIdentity(&pMatBack);
	D3DXMatrixIdentity(&pTemp);
	D3DXMatrixIdentity(&pMatOldView);
	D3DXMatrixIdentity(&pMatPresView);
	RECT WindowRect;
	GetClientRect(g_pD3dApp->GetHwnd(), &WindowRect);
	D3DXMatrixOrthoLH(&m_pMatInvenBackProj,
		(float)WindowRect.right,
		(float)WindowRect.bottom,
		0.0f, 100.0f);
	g_pD3dDev->GetTransform(D3DTS_VIEW, &pMatOldView);
	g_pD3dDev->GetTransform(D3DTS_PROJECTION, &pMatOldProj);
	g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatPresView);
	g_pD3dDev->SetTransform(D3DTS_PROJECTION, &m_pMatInvenBackProj);
	g_pD3dDev->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x00);
	g_pD3dDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	int nScreenWidth = -(int)g_pD3dApp->GetBackBufferDesc().Width / 2;
	int nScreenHeight = ((float)g_pD3dApp->GetBackBufferDesc().Height / 2) + 0.5f;
	int nWindowPosY = g_pGameMain->m_nLeftWindowY;
	D3DXMatrixScaling(&pMatBack, INVEN_BACK_W, INVEN_BACK_H, 1.0f);
	if (bShop)
	{
		int nStart = CITY_BASE_NPC_BOX_START_X;
		D3DXMatrixTranslation(&pTemp,
			nScreenWidth + nStart + INVEN_BACK_POS_X,
			nScreenHeight - (nWindowPosY + INVEN_BACK_POS_Y),
			0.0f);
	}
	else
	{
		D3DXMatrixTranslation(&pTemp,
			nScreenWidth + INVEN_BACK_POS_X,
			nScreenHeight - (nWindowPosY + INVEN_BACK_POS_Y),
			0.0f);
	}
	pMatBack = pMatBack * pTemp;
	g_pD3dDev->SetTransform(D3DTS_WORLD, &pMatBack);
	g_pD3dDev->SetTexture(0, m_pMirrorTexture);
	g_pD3dDev->SetStreamSource(0, m_pMirrorVB, 0, sizeof(MIRRORVERTEX));
	g_pD3dDev->SetFVF(MIRRORVERTEX::FVF);
	g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3dDev->SetTransform(D3DTS_VIEW, &pMatOldView);
	g_pD3dDev->SetTransform(D3DTS_PROJECTION, &pMatOldProj);
}
void CINFInven::ReSetScrollCurrentLine()
{
	int nCurrentLine = GetScrollLine();
	if (m_nInvenCurrentScroll > nCurrentLine)
	{
		m_nInvenCurrentScroll = nCurrentLine;
		m_nInvenCurrentScrollPosition = GetScrollLineInterval(m_nInvenCurrentScroll);
	}
}
int CINFInven::GetInvenFreeSize()
{
	int nFreeSize = (CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, g_pShuttleChild->m_myShuttleInfo.GetAddedPermanentInventoryCount()) - 1)
		- g_pStoreData->GetTotalUseInven();
	return nFreeSize;
}
void CINFInven::ClearMultiSeletItem()
{
	int nSelItemCnt = g_pD3dApp->GetMultiSelectItem();
	if (nSelItemCnt > 0)
	{
		SetMultiSelectItem(NULL);
	}
}
BOOL CINFInven::IsInvenHave(int nItemNum)
{
	BOOL bHave = FALSE;
	for (int nCnt = 0; nCnt < INVEN_NUMBER; nCnt++)
	{
		if (NULL == m_pInvenDisplayInfo[nCnt])
		{
			continue;
		}
		if (nItemNum == m_pInvenDisplayInfo[nCnt]->pItem->ItemNum)
		{
			bHave = TRUE;
			break;
		}
	}
	return bHave;
}
void CINFInven::InitDeleteItemInfo()
{
	memset(&m_struDeleteItem, 0x00, sizeof(structDelItemInfo));
}
void CINFInven::SetDeleteItemInfo(CItemInfo* pSelectItem)
{
	if (NULL == pSelectItem)
	{
		InitDeleteItemInfo();
		return;
	}
	m_struDeleteItem.UniqueNumber = pSelectItem->UniqueNumber;
	m_struDeleteItem.CurrentCount = pSelectItem->CurrentCount;
}
void CINFInven::DoModalChatAllInfluence(UID64_t		UniqueNumber)
{
	if (g_pGameMain && g_pGameMain->m_pInfWindow)
	{
		g_pGameMain->m_bChangeMousePoint = TRUE;
		g_pGameMain->m_pInfWindow->AddMsgBox(STRMSG_C_070810_0201,
			_Q_USEITEM_SPEAKER_MSG,
			0,
			0,
			0,
			UniqueNumber);
	}
}

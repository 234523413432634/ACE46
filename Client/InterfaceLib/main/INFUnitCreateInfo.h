// INFUnitCreateInfo.h: interface for the CUnitCreateInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFUNITCREATEINFO_H__1244E4AD_A756_471B_8F2A_F05AA9642656__INCLUDED_)
#define AFX_INFUNITCREATEINFO_H__1244E4AD_A756_471B_8F2A_F05AA9642656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INFBase.h"

// 2011. 10. 10 by jskim UI시스템 변경
enum
{
	FADE_IN,
	FADE_OUT,
	FADE_NORMAL
};																								  

class CINFImage;
class CINFImageEx;		// 2011. 10. 10 by jskim UI시스템 변경

class CINFUnitCreateInfo : public CINFBase
{
public:
	CINFUnitCreateInfo(CAtumNode* pParent);
	virtual ~CINFUnitCreateInfo();


	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();


	void Render( int nUnitKind );
	virtual void Tick( float fElapedTime );
	void StartView( );
	void SetFadeMode(int nMode);		// 2011. 10. 10 by jskim UI시스템 변경
	void SetBkAlpha(DWORD dwColor) { m_dwBkAlpha = dwColor; }		// 2011. 10. 10 by jskim UI시스템 변경
	void SetSelGear(int num) { m_nSelGear = num; }		// 2011. 10. 10 by jskim UI시스템 변경

protected:
	BOOL			m_bRestored;
	//img data
	CINFImageEx*	m_pBGear;		// 2011. 10. 10 by jskim UI시스템 변경
	CINFImageEx*	m_pIGear;		// 2011. 10. 10 by jskim UI시스템 변경
	CINFImageEx*	m_pMGear;		// 2011. 10. 10 by jskim UI시스템 변경
	CINFImageEx*	m_pAGear;		// 2011. 10. 10 by jskim UI시스템 변경

	CINFImageEx*	m_pBack;		// 2011. 10. 10 by jskim UI시스템 변경
	CINFImageEx*	m_pGearInfo[4];		// 2011. 10. 10 by jskim UI시스템 변경				   

	//tick data
	CRemainTime     m_tRemainTime;
	float			m_fIncreaseViewingTime;
// 2011. 10. 10 by jskim UI시스템 변경
	int				m_nFadeMode;		
	float			m_fFadeInTime;
	DWORD			m_dwBkAlpha;

	int				m_nSelGear;
// end 2011. 10. 10 by jskim UI시스템 변경														  
};

#endif // !defined(AFX_INFUNITCREATEINFO_H__1244E4AD_A756_471B_8F2A_F05AA9642656__INCLUDED_)

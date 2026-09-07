// 2006-09-05 by cmkwon
// #include "LocalizationDefineCommon.h"
#include "Contents.h"

#ifndef _LOCALIZATION_DEFINE_COMMON_H_
#define _LOCALIZATION_DEFINE_COMMON_H_


///////////////////////////////////////////////////////////////////////////////
// 2006-09-04 by cmkwon, ľđľî°Ł ´Ů¸Ą Define ¸®˝şĆ®

///////////////////////////////////////////////////////////////////////////////
// 2008-01-08 by cmkwon, ľđľî Ľ­şń˝ş °ü·Ă Á¤ŔÇ¸¦ ÇĎłŞ·Î ĹëŔĎÇÔ - SERVICE_TYPE_XXX ·Î ĹëŔĎÇÔ, Preprocessor definitions Ŕş »çżëÇĎÁö ľĘŔ˝
// 2008-04-25 by cmkwon, Áöżř ľđľî/Ľ­şń˝ş Ăß°ˇ˝Ă ˛Ŕ Ăß°ˇ µÇľîľß ÇĎ´Â »çÇ× - [Ľ­şń˝ş-ÇĘĽö]Ľ­şń˝ş Ĺ¸ŔÔ(SERVICE_TYPE_XXX) Á¤ŔÇ ÇĎ±â
//  SERVICE_TYPE_KOREAN_SERVER_1		==> ÇŃ±ą	Masangsoft		- Kor	100
//  SERVICE_TYPE_KOREAN_SERVER_2		==>	ÇŃ±ą	Yedang			- Kor	1000
//  SERVICE_TYPE_ENGLISH_SERVER_1		==>	ÄłłŞ´Ů	Wikigames		- Eng	2000
//  SERVICE_TYPE_ENGLISH_SERVER_2		==>	żµ±ą	Gameforge4D		- Eng	5000	
//  SERVICE_TYPE_GERMAN_SERVER_1		==>	µ¶ŔĎ	Gameforge4D		- Deu	5100	// 2008-04-11 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ - Gameforge4D µ¶ŔĎľî 
//  SERVICE_TYPE_CHINESE_SERVER_1		==>	Áß±ą	Yetime			- Chn	3000
//  SERVICE_TYPE_VIETNAMESE_SERVER_1	==>	şŁĆ®ł˛	VTC-Intecom		- Viet	4000
//  SERVICE_TYPE_THAI_SERVER_1			==>	ĹÂ±ą	WinnerOnline	- Tha	6000	// 2008-04-11 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ - WinnerOnline ĹÂ±ąľî
//	SERVICE_TYPE_SINGAPORE_1			==> żµľî		 WinnerOnline- Sgp	6100	// 2010-12-07 by shcho,	 Áöżř Ľ­şń˝ş Ăß°ˇ(WinnerOnline żµľî) - 
//	SERVICE_TYPE_INDONESIA_SERVER_1		==> ŔÎµµł×˝ĂľĆľî WinnerOnline- IND	6200	// 2010-01-11 by shcho,	 Áöżř Ľ­şń˝ş Ăß°ˇ(WinnerOnline ŔÎµµł×˝ĂľĆľî) - 
//  SERVICE_TYPE_RUSSIAN_SERVER_1		==>	·Ż˝ĂľĆ	Innova			- Rus	7000	// 2008-05-29 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Innova_Rus ·Ż˝ĂľĆľî Ăß°ˇ) - 
//  SERVICE_TYPE_TAIWANESE_SERVER_1		==>	´ë¸¸	Newpower		- Tpe	8000	// 2008-09-23 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(´ë¸¸ Netpower_Tpe) - 
//  SERVICE_TYPE_JAPANESE_SERVER_1		==> ŔĎş»	Arario			- Jpn	10000	// 2008-12-03 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(ŔĎş» Arario_Jpn) - 
//  SERVICE_TYPE_TURKISH_SERVER_1		==> ĹÍĹ°	Gameforge4D		- Tur	5200	// 2008-12-22 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ĹÍĹ°ľĆ, şŇľî, ŔĚĹ»¸®ľĆľî) - 
//  SERVICE_TYPE_ITALIAN_SERVER_1		==> ŔĚĹ»¸®ľĆľî	Gameforge4D	- Ita	5400	// 2008-12-22 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ĹÍĹ°ľĆ, şŇľî, ŔĚĹ»¸®ľĆľî) - 
//  SERVICE_TYPE_FRENCH_SERVER_1		==> ÇÁ¶ű˝şľî	Gameforge4D	- Fra	5500	// 2008-12-22 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ĹÍĹ°ľĆ, şŇľî, ŔĚĹ»¸®ľĆľî) - 
//  SERVICE_TYPE_POLISH_SERVER_1		==> Ćú¶őµĺľî	Gameforge4D	- Pol	5600	// 2009-06-04 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D Ćú¶őµĺľî, ˝şĆäŔÎľî) - 
//  SERVICE_TYPE_SPANISH_SERVER_1		==> ˝şĆäŔÎľî	Gameforge4D	- Esp	5700	// 2009-06-04 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D Ćú¶őµĺľî, ˝şĆäŔÎľî) - 
//	SERVICE_TYPE_ARGENTINA_SERVER_1		==> ľĆ¸ŁÇîĆĽłŞ	LIN			- Arg	11000	// 2010-11-01 by shcho,	 Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ˝şĆäŔÎľî, ľĆ¸ŁÇîĆĽłŞľî) -  

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 01
#ifdef S_140_SERVER_SETTING_HSSON
#define SERVICE_TYPE_KOREAN_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		100			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define SERVICE_TYPE_KOREAN_SERVER_2
#define SERVICE_UID_FOR_WORLD_RANKING		1000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define SERVICE_TYPE_JAPANESE_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		10000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define SERVICE_TYPE_ENGLISH_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		2000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define SERVICE_TYPE_RUSSIAN_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		7000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif


#ifdef S_VIE_SERVER_SETTING_HSSON
#define SERVICE_TYPE_VIETNAMESE_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		4000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif


// °ÔŔÓĆ÷Áö
#ifdef S_DEU_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_GERMAN_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5100			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_ENG_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_ENGLISH_SERVER_2
#define SERVICE_UID_FOR_WORLD_RANKING		5000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif				  
//
#ifdef S_ITA_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_ITALIAN_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5400			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif
// 
#ifdef S_ESP_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_SPANISH_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5700			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif
// 
#ifdef S_FRA_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_FRENCH_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5500			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif
// 
#ifdef S_POL_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_POLISH_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5600			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif
// 				
#ifdef S_TUR_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_TURKISH_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		5200			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_ARGENTINA_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		11000			// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - 
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define SERVICE_TYPE_CHINESE_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		3000
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define SERVICE_TYPE_GLOBAL_SERVER_1
#define SERVICE_UID_FOR_WORLD_RANKING		101
#endif

///////////////////////////////////////////////////////////////////////////////
// 2008-05-09 by cmkwon, CodePage Á¤ŔÇ Ăß°ˇ - 
//		Korean Extended Wansung			==> 949			: Korean(Masangsoft_Kor, Yedang_Kor)
//		Latin 1							==> 1252		: English(Yedang-Global_Eng, Gameforge4D_Eng), German(Gameforge4D_Deu), French(Gameforge4D_Fra), Italian(Gameforge4D_Ita), Spanish(Gameforge4D_Esp)
//		Japanese (Shift-JIS)			==> 932			
//		Chinese (PRC)					==> 936			: Chinese(Yetime_Chn)
//		Chinese (Taiwan, Hong Kong)		==> 950			: Taiwnaese(Netpower_Tpe)
//		Viet Nam						==> 1258		: Vietnamnese(VTC-Intecom_Viet)
//		Thai							==> 874			: Thai(WinnerOnline_Tha)
//		Cyrillic, Russian (Russia)		==> 1251		: Russian(Innova_Rus)
//		Turkish							==> 1254		: Turkish(Gameforge4D_Tur)
//		Central European, Eastern European	==> 1250	: Polish(Gameforge4D_Pol)

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 02
#ifdef S_140_SERVER_SETTING_HSSON
#define CODE_PAGE							949
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define CODE_PAGE							949
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define CODE_PAGE							932
#endif

#if defined(S_CAN_SERVER_SETTING_HSSON) || defined(S_DEU_SERVER_SETTING_JHAHN) || defined(S_ENG_SERVER_SETTING_JHAHN) || defined(S_ITA_SERVER_SETTING_JHAHN) || defined(S_FRA_SERVER_SETTING_JHAHN) || defined(S_POL_SERVER_SETTING_JHAHN) \
 || defined(S_ESP_SERVER_SETTING_JHAHN) || defined(S_TUR_SERVER_SETTING_JHAHN) || defined(S_GLOBAL_SERVER_SETTING_JHSEOL)	// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define CODE_PAGE							1252
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define CODE_PAGE							1251
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define CODE_PAGE							1258
#endif


#ifdef S_ARG_SERVER_SETTING_JHAHN
#define SERVICE_TYPE_ARGENTINA_SERVER_1
#define CODE_PAGE							1252
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define CODE_PAGE							936
#endif

///////////////////////////////////////////////////////////////////////////////
// 2009-04-20 by cmkwon, Gameforge4D "/´©±¸" ¸í·Éľî Á¦°ĹÇĎ±â - 
#if defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1)
	#define _DEFINED_GAMEFORGE4D_		// 2009-04-21 by cmkwon, ˝şĆç¸µ ĽöÁ¤ÇÔ.
#endif


///////////////////////////////////////////////////////////////////////////////
// 2009-04-29 by cmkwon, ÇŮ˝Żµĺ »çżë ż©şÎ #define Ŕ¸·Î Ăł¸® - 
// 2008-04-25 by cmkwon, Áöżř ľđľî/Ľ­şń˝ş Ăß°ˇ˝Ă ˛Ŕ Ăß°ˇ µÇľîľß ÇĎ´Â »çÇ× - [ľđľî-żÉĽÇ] Ăß°ˇ ľđľî˝Ăżˇ ÇŮ˝Żµĺ »çżë ż©şÎ Ăß°ˇ
// 2009-10-06 by cmkwon, şŁĆ®ł˛ °ÔŔÓ °ˇµĺ X-TRAPŔ¸·Î şŻ°ć - şŁĆ®ł˛ ÇŮ˝ŻµĺżˇĽ­ Á¦żÜ
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_KOREAN_SERVER_2) || defined(SERVICE_TYPE_ENGLISH_SERVER_1) || defined(SERVICE_TYPE_VIETNAMESE_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(_DEFINED_GAMEFORGE4D_)
// 2009-11-04 by cmkwon, ĹÂ±ą °ÔŔÓ°ˇµĺ Apex·Î şŻ°ć - ĹÂ±ą(SERVICE_TYPE_THAI_SERVER_1) Á¦°ĹÇÔ.
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_KOREAN_SERVER_2) || defined(SERVICE_TYPE_ENGLISH_SERVER_1) || defined(SERVICE_TYPE_THAI_SERVER_1) || defined(_DEFINED_GAMEFORGE4D_)

// 2012-11-27 by bckim, ÄłłŞ´Ů ÇŮ˝Żµĺ Á¦°Ĺ
//#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_KOREAN_SERVER_2) || defined(SERVICE_TYPE_ENGLISH_SERVER_1) || defined(_DEFINED_GAMEFORGE4D_)
#if defined(SERVICE_TYPE_KOREAN_SERVER_1) || defined(SERVICE_TYPE_KOREAN_SERVER_2) /*|| defined(SERVICE_TYPE_ENGLISH_SERVER_1)*/ || defined(_DEFINED_GAMEFORGE4D_)
// 2012-11-27 by bckim, ÄłłŞ´Ů ÇŮ˝Żµĺ Á¦°Ĺ. End

#ifdef GAMEGUARD_NOT_EXECUTE_HSSON	

#else
	 	#define _USING_HACKSHIELD_			// 2009-04-29 by cmkwon, ÇŮ˝Żµĺ »çżë ż©şÎ #define Ŕ¸·Î Ăł¸® - ÇŮ˝Żµĺ »çżë ż©şÎ Á¤ŔÇ
#endif

	#if defined(_ATUM_FIELD_SERVER)
		#pragma comment(lib, "AntiCpXSvr.lib")
	#endif // END - #if defined(_ATUM_FIELD_SERVER)
#endif 

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 03
#ifdef S_140_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)

#if defined(SC_LEVEL_EXPANSION_115_BCKIM_JWLEE)	// 2015-07-21 by bckim, ¸¸·ľ Č®Ŕĺ ( 110 >> 115 )
#define CHARACTER_MAX_LEVEL							115		
#else
#define CHARACTER_MAX_LEVEL							110		
#endif	// End. 2015-07-21 by bckim, ¸¸·ľ Č®Ŕĺ ( 110 >> 115 )

#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)

#if defined(SC_LEVEL_EXPANSION_115_BCKIM_JWLEE)	// 2015-07-21 by bckim, ¸¸·ľ Č®Ŕĺ ( 110 >> 115 )
#define CHARACTER_MAX_LEVEL							115		
#else
#define CHARACTER_MAX_LEVEL							110
#endif	// End. 2015-07-21 by bckim, ¸¸·ľ Č®Ŕĺ ( 110 >> 115 )

#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				40		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			20		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ ŔÎşŁĹä¸® Ä«żîĆ®
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				20		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ Ă˘°í Ä«żîĆ®
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	20		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ ĂÖ´ëż©´Üżř Ä«żîĆ®
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ ŔÎşŁĹä¸® Ä«żîĆ®
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ Ă˘°í Ä«żîĆ®
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30		// 2006-09-06 by cmkwon, ¸ăąö˝± Ľ­şń˝ş˝Ă Ăß°ˇ ĂÖ´ëż©´Üżř Ä«żîĆ®
#endif


#ifdef S_ARG_SERVER_SETTING_JHAHN
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define SIZE_MAX_INITIAL_GUILD_CAPACITY				30		// ĂĘ±â ±ćµĺ »ýĽş ˝Ă °ˇ´É ±ćµĺżř Ľö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć (40-->30)
#define SIZE_MAX_GUILD_CAPACITY						300		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ĂÖ´ë ±ćµĺżř Ľö
#define SIZE_MAX_ITEM_GENERAL						61		// Äł¸ŻĹÍŔÇ ŔÎşĄĹä¸®żˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö(1°ł´Â SPI ľĆŔĚĹŰŔÇ Ä«żîĆ®ŔĚ´Ů, Ĺ¬¶óŔĚľđĆ®´Â 60Ŕ» »çżëÇŃ´Ů.), // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(61-->41)
#define SIZE_MAX_ITEM_GENERAL_IN_STORE				101		// Ă˘°íżˇ ĽŇŔŻÇŇ Ľö ŔÖ´Â ľĆŔĚĹŰŔÇ ĂÖ´ë °łĽö, // 2006-09-06 by cmkwon, ÇŃ±ą Ľ­ąö¸¸ şŻ°ć(101-->51)
#define CHARACTER_MAX_LEVEL							110
#define COUNT_IN_MEMBERSHIP_ADDED_INVENTORY			40
#define COUNT_IN_MEMBERSHIP_ADDED_STORE				50
#define COUNT_IN_MEMBERSHIP_ADDED_GUILD_CAPACITY	30
#endif

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 04
#ifdef S_140_SERVER_SETTING_HSSON
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON 
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define STAT_BGEAR_ATTACK_PART						3
#define STAT_BGEAR_DEFENSE_PART						3
#define STAT_BGEAR_FUEL_PART						3
#define STAT_BGEAR_SOUL_PART						3
#define STAT_BGEAR_SHIELD_PART						3
#define STAT_BGEAR_DODGE_PART						3

#define STAT_MGEAR_ATTACK_PART						2
#define STAT_MGEAR_DEFENSE_PART						4
#define STAT_MGEAR_FUEL_PART						3
#define STAT_MGEAR_SOUL_PART						4
#define STAT_MGEAR_SHIELD_PART						3
#define STAT_MGEAR_DODGE_PART						2

#define STAT_AGEAR_ATTACK_PART						4
#define STAT_AGEAR_DEFENSE_PART						3
#define STAT_AGEAR_FUEL_PART						3
#define STAT_AGEAR_SOUL_PART						3
#define STAT_AGEAR_SHIELD_PART						4
#define STAT_AGEAR_DODGE_PART						1

#define STAT_IGEAR_ATTACK_PART						4
#define STAT_IGEAR_DEFENSE_PART						2
#define STAT_IGEAR_FUEL_PART						3
#define STAT_IGEAR_SOUL_PART						3
#define STAT_IGEAR_SHIELD_PART						2
#define STAT_IGEAR_DODGE_PART						4
#endif

///////////////////////////////////////////////////////////////////////////////
// 2006-09-15 by cmkwon, 
// Kor_Masang140	==> 121.134.114.140:9979	// 2007-01-03 by cmkwon, ¸¶»ó ŔĚŔü
// Kor_Yedang		==> 192.168.10.40:9979		// 2007-01-03 by cmkwon, Yedang »çĽł IP
// Eng_Gala-Net		==> »ó°ü ľřŔ˝
// Viet_VTC-Intecom	==> »ó°ü ľřŔ˝
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 05
#ifdef S_140_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif															   

#ifdef S_KOR_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"192.168.10.40"
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"192.168.10.40"
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"192.168.10.40"
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define MSBILLING_DB_SERVER_IP						"115.144.35.140"
#endif

#define MSBILLING_DB_SERVER_PORT					9979

// Kor_Masang51		==> 1
// Kor_ETRI			==> 1
// Eng_Gala-Net		==> »ó°ü ľřŔ˝
// Viet_VTC-Intecom	==> »ó°ü ľřŔ˝
#define MSBILLING_GAMEUID							1

// 2006-09-22 by dhjin
// Kor_Masang51		==> 201			// 2006-10-23 by cmkwon, şŻ°ć(101-->201)
// Kor_ETRI			==> 201			// 2006-10-23 by cmkwon, şŻ°ć(101-->201)
// Eng_Gala-Net		==> 201			// 2006-10-23 by cmkwon, şŻ°ć(101-->201)
// Viet_VTC-Intecom	==> 201			// 2006-10-23 by cmkwon, şŻ°ć(101-->201)
#define COUNT_IN_MEMBERSHIP_GUILDSTORE				201		// 2006-09-22 by dhjin, ¸âąö˝± Ľ­şń˝ş˝Ă ż©´Ü Ă˘°í Ä«żîĆ®


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 06
#ifdef S_140_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.aceonline.co.kr"
#endif															   

#ifdef S_KOR_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://ao.masangsoft.com"		// 2009-05-13 by cmkwon, żą´ç Č¨ĆäŔĚÁö µµ¸ŢŔÎ şŻ°ć - ±âÁ¸(aceonline.co.kr)
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.aceonline.jp"
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://ace.subagames.com"		// 2008-08-05 by cmkwon, WikiGames_Eng °ÔŔÓ Á¤ş¸ ĽöÁ¤ - ,"http://www.ace-onlines.com"
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.aceonline.ru"	// 2008-06-24 by cmkwon, ĽöÁ¤µĘ
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://phidoi.vtc.vn"		// 2007-10-05 by cmkwon, ĽöÁ¤(±âÁ¸ "http://caoboi.vtc.vn")
#endif

#if defined(S_DEU_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.de"
#endif

#if defined(S_ENG_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.net"
#endif

#if defined(S_ITA_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.it"
#endif

#if defined(S_FRA_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.fr"
#endif

#if defined(S_POL_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.pl"
#endif

#if defined(S_ESP_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.es"
#endif

#if defined(S_TUR_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.airrivals.org"
#endif

#if defined(S_ARG_SERVER_SETTING_JHAHN)
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://www.axeso5.com"
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://ao.masangsoft.com"
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define STRMSG_S_GAMEHOMEPAGE_DOMAIN			"http://ao.masangsoft.com"
#endif

///////////////////////////////////////////////////////////////////////////////
// 2006-05-22 by cmkwon, şńąř MD5·Î ŔÎÄÚµů˝Ăżˇ żř·ˇ şńąř ľŐżˇ Ăß°ˇµÉ ˝şĆ®¸µ - 
#define MD5_PASSWORD_ADDITIONAL_STRING			""

#define EXT_AUTH_GAME_NAME						"SCO"		// 2006-05-22 by cmkwon

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 07
#ifdef S_140_SERVER_SETTING_HSSON
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://www.masangsoft.com/SCGame/scLauncher.htm"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://www.masangsoft.com/SCGame/scLauncher.htm"		// 2006-08-04 by cmkwon, Ăß°ˇÇÔ


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif // S_140_SERVER_SETTING_HSSON

#ifdef S_KOR_SERVER_SETTING_HSSON
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"10.10.90."			// 2007-07-04 by cmkwon, ¸¶»óĽŇÇÁĆ®(VPN Á˘ĽÓ »çĽł IP)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"183.110.249."		// 2012-07-06 by hskim, YD IDC ŔĚŔü ŔŰľ÷ - // 2010-02-10 by cmkwon, żÍŔĚµđ Áöżř Ľ­ąö IDC IP - 
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )
///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Main/index.php"			// 2009-05-13 by cmkwon, żą´ç Č¨ĆäŔĚÁö µµ¸ŢŔÎ şŻ°ć - ±âÁ¸(aceonline.co.kr), // 2006-09-12 by cmkwon, ĽöÁ¤
#define TESTSERVER_LAUNCHER_WEB_URL				"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Test/index.php"		// 2009-05-13 by cmkwon, żą´ç Č¨ĆäŔĚÁö µµ¸ŢŔÎ şŻ°ć - ±âÁ¸(aceonline.co.kr), // 2006-12-25 by cmkwon, ĽöÁ¤
///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"dpdltm!@eoqkr"
#endif // S_KOR_SERVER_SETTING_HSSON



#ifdef S_JPN_SERVER_SETTING_HSSON
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://www.aceonline.co.kr/SCGame/scLauncher.htm"			// 2006-09-12 by cmkwon, ĽöÁ¤
#define TESTSERVER_LAUNCHER_WEB_URL				"http://www.aceonline.co.kr/SCGame/scLauncher_test.htm"		// 2006-12-25 by cmkwon, ĽöÁ¤


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"dpdltm!@eoqkr"
#endif // S_JPN_SERVER_SETTING_HSSON




#ifdef S_CAN_SERVER_SETTING_HSSON
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"10.10.111."		// 2008-08-05 by cmkwon, WikiGames_Eng °ÔŔÓ Á¤ş¸ ĽöÁ¤ - ş»Ľ· Private IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"74.200.6.2"		// 2008-08-05 by cmkwon, WikiGames_Eng °ÔŔÓ Á¤ş¸ ĽöÁ¤ - ş»Ľ· Public IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"208.68.90."		// 2008-08-05 by cmkwon, WikiGames_Eng °ÔŔÓ Á¤ş¸ ĽöÁ¤ - Ĺ×Ľ· Public IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
												|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://ace.subagames.com/launcher.aspx"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://ace.subagames.com/launcher.aspx"


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif // S_CAN_SERVER_SETTING_HSSON




#ifdef S_RUS_SERVER_SETTING_HSSON
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"89.249.25."		// 2008-07-29 by cmkwon, Innova_Rus ĽöÁ¤ - Ľ­ąö °řŔÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"172.29."			// 2008-07-29 by cmkwon, Innova_Rus ĽöÁ¤ - Ľ­ąö »çĽł IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"89.208.20.170"		// 2008-07-29 by cmkwon, Innova_Rus ĽöÁ¤ - ş»şÎŔĺ´Ô ·Ż˝ĂľĆżˇĽ­ żäĂ»(AdminTool ±łŔ°Ŕ» Ŕ§ÇŘ)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://launcher.aceonline.ru"		// 2008-06-24 by cmkwon, ĽöÁ¤µĘ
#define TESTSERVER_LAUNCHER_WEB_URL				"http://launcher.aceonline.ru"		// 2008-06-24 by cmkwon, ĽöÁ¤µĘ


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"dpdltm!@eoqkr"
#endif // S_RUS_SERVER_SETTING_HSSON


#ifdef S_VIE_SERVER_SETTING_HSSON
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35.14"	// 2007-01-03 by cmkwon, ł»şÎ Ĺ×Ľ·±ş
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35.15"	// 2007-01-03 by cmkwon, ÇÁ·Î±×·ĄĆŔ±ş
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2008-05-19 by cmkwon, ¸¶»ó ¸đµÎ, // 2007-01-03 by cmkwon, ±âČą,żîżµĆŔ±ş
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"222.255.15.252"	// 2008-05-19 by cmkwon, Hoa żäĂ», // 2007-01-03 by cmkwon, »çŔĺ´Ô,ş»şÎŔĺ´Ô
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"117.103.192.64"	// 2008-04-28 by cmkwon, VTC-Intecom Monitor PC1
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"222.255.15.15"		// 2008-04-28 by cmkwon, VTC-Intecom Monitor PC1(<==222.255.15.252)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"127.0.0.1"			// 2007-03-06 by cmkwon, VTC-Intecom Monitor PC2(delete 203.162.1.223)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"222.255.15.248"	// 2007-01-03 by cmkwon, Masang Remote PC in VTC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"222.255.15.37"		// 2007-01-03 by cmkwon, VTC Main Pre Server public IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"10.10.1."			// 2007-01-03 by cmkwon, VTC Server private IP
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://phidoi.vtc.vn/notice.asp"		// 2007-10-05 by cmkwon, ĽöÁ¤(±âÁ¸ "http://caoboi.vtc.vn/notice.asp")
#define TESTSERVER_LAUNCHER_WEB_URL				"http://phidoi.vtc.vn/notice.asp"		// 2007-10-05 by cmkwon, ĽöÁ¤(±âÁ¸ "http://caoboi.vtc.vn/notice.asp")


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif
// GameForge4D µ¶ŔĎ
//==========================================================================================================================================================================
#ifdef S_DEU_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
//#define LAUNCHER_WEB_URL						"http://airrivals.de/launcher/launcher.html"
//#define TESTSERVER_LAUNCHER_WEB_URL				"de.dev.airrivals.de"
#define LAUNCHER_WEB_URL						"http://airrivals.de/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://de.dev.airrivals.de/launcher/launcher.html"
// ~~~ DE(µ¶ŔĎ)~~~
// Live: http://airrivals.de/launcher/launcher.html
// Test: http://de.dev.airrivals.de/launcher/launcher.html


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

//żµ±ą
//==========================================================================================================================================================================
#ifdef S_ENG_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )


///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
//#define LAUNCHER_WEB_URL						"http://www.airrivals.net/launcher/launcher.html"
//#define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.net/launcher/launcher_test.html"
#define LAUNCHER_WEB_URL						"http://airrivals.net/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://en.dev.airrivals.de/launcher/launcher.html"
// ~~~ EN(żµ±ą) ~~~
// Live: http://airrivals.net/launcher/launcher.html
// Test: http://en.dev.airrivals.de/launcher/launcher.html



///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

//ŔĚĹ»¸®ľĆ
//==========================================================================================================================================================================
#ifdef S_ITA_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
//#define LAUNCHER_WEB_URL						"http://www.airrivals.it/launcher/launcher.html"
//#define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.it/launcher/launcher.html"
#define LAUNCHER_WEB_URL						"http://airrivals.it/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://it.dev.airrivals.de/launcher/launcher.html"
// ~~~ IT(ŔĚĹÂ¸®) ~~~
// Live: http://airrivals.it/launcher/launcher.html [^]
// Test: http://it.dev.airrivals.de/launcher/launcher.html [^]


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif


// ÇÁ¶ű˝ş
//==========================================================================================================================================================================
#ifdef S_FRA_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
//#define LAUNCHER_WEB_URL						"http://www.airrivals.fr/launcher/launcher.html"
//#define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.fr/launcher/launcher.html"
#define LAUNCHER_WEB_URL						"http://airrivals.fr/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://fr.dev.airrivals.de/launcher/launcher.html"
// ~~~ FR(ÇÁ¶ű˝ş) ~~~
// Live: http://airrivals.fr/launcher/launcher.html [^]
// Test: http://fr.dev.airrivals.de/launcher/launcher.html [^]



///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

// Ćú¶őµĺ
//==========================================================================================================================================================================
#ifdef S_POL_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
// #define LAUNCHER_WEB_URL						"http://www.airrivals.pl/launcher/launcher.html"
// #define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.pl/launcher/launcher_test.html"
#define LAUNCHER_WEB_URL						"http://airrivals.pl/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://pl.dev.airrivals.de/launcher/launcher.html"
// ~~~ PL(Ćú¶őµĺ) ~~~
// Live: http://airrivals.pl/launcher/launcher.html
// Test: http://pl.dev.airrivals.de/launcher/launcher.html


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

// ˝şĆäŔÎ
//==========================================================================================================================================================================
#ifdef S_ESP_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
// #define LAUNCHER_WEB_URL						"http://www.airrivals.net/launcher/launcher.html"
// #define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.net/launcher/launcher_test.html"
#define LAUNCHER_WEB_URL						"http://airrivals.es/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://es.dev.airrivals.de/launcher/launcher.html"
// ~~~ ES(˝şĆäŔÎ) ~~~
// Live: http://airrivals.es/launcher/launcher.html
// Test: http://es.dev.airrivals.de/launcher/launcher.html


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

// ĹÍĹ°
//==========================================================================================================================================================================
#ifdef S_TUR_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL

// 2012-11-13 by bckim, GameForge4D ·±Ăł URL ĂÖ˝ĹČ­
// #define LAUNCHER_WEB_URL						"http://www.airrivals.org/launcher/launcher.html"
// #define TESTSERVER_LAUNCHER_WEB_URL				"http://www.airrivals.org/launcher/launcher.html"
#define LAUNCHER_WEB_URL						"http://airrivals.org/launcher/launcher.html"
#define TESTSERVER_LAUNCHER_WEB_URL				"http://tr.dev.airrivals.de/launcher/launcher.html"
// ~~~ TR(ĹÍĹ°) ~~~
// Live: http://airrivals.org/launcher/launcher.html
// Test: http://tr.dev.airrivals.de/launcher/launcher.html


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif


#ifdef S_ARG_SERVER_SETTING_JHAHN
// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - MS140ąř Ľ­ąö¸¸ Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ IP ¸¦ Ć˛¸®°Ô(121.134.11.) ĽłÁ¤ ÇÔ
// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"83.141.22."		// 2008-01-22 by cmkwon, Gameforge4D IDC IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"85.115.3.230"		// 2008-01-04 by cmkwon, Gameforge4D Holger
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"91.6.248.205"		// 2008-01-16 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"91.6.212.252"	    // 2008-01-18 by cmkwon, S_P,S_F: Gameforge4D Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"192.168."			// 2008-02-01 by dhjin, Gameforge4D ł»şÎ IP
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"79.110.95.2"		// 2008-10-31 by cmkwon, Gameforge4D »çą«˝Ç IP şŻ°ć - AdminTool »çżë ÇŇ IP of PC
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )

///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"http://launchers.axeso5.com/aceonline/launcher" // 2012-03-28 by hskim, ľĆ¸ŁÇîĆĽłŞ ·±Ăł ĽöÁ¤ - URL şŻ°ć												
#define TESTSERVER_LAUNCHER_WEB_URL				"http://launchers.axeso5.com/aceonline/launcher" // 2012-03-28 by hskim, ľĆ¸ŁÇîĆĽłŞ ·±Ăł ĽöÁ¤ - URL şŻ°ć


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"callweb"
#endif

// Áß±ą
//==========================================================================================================================================================================
#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"10.10.90."			// 2007-07-04 by cmkwon, ¸¶»óĽŇÇÁĆ®(VPN Á˘ĽÓ »çĽł IP)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"183.110.249."		// 2012-07-06 by hskim, YD IDC ŔĚŔü ŔŰľ÷ - // 2010-02-10 by cmkwon, żÍŔĚµđ Áöżř Ľ­ąö IDC IP - 
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)		( 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)) \
|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)) )
///////////////////////////////////////////////////////////////////////////////
// 2006-05-02 by cmkwon, Launcher URL
#define LAUNCHER_WEB_URL						"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Main/index.php"			// 2009-05-13 by cmkwon, żą´ç Č¨ĆäŔĚÁö µµ¸ŢŔÎ şŻ°ć - ±âÁ¸(aceonline.co.kr), // 2006-09-12 by cmkwon, ĽöÁ¤
#define TESTSERVER_LAUNCHER_WEB_URL				"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Test/index.php"		// 2009-05-13 by cmkwon, żą´ç Č¨ĆäŔĚÁö µµ¸ŢŔÎ şŻ°ć - ±âÁ¸(aceonline.co.kr), // 2006-12-25 by cmkwon, ĽöÁ¤
///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon
#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
// Kor_Masang51		==> callweb
// Kor_Yedang		==> 2006-12-25 by cmkwon, ĽöÁ¤ÇÔ
#define BILLING_DBSERVER_USER_PWD				"dpdltm!@eoqkr"
#endif	// S_CHI_SERVER_SETTING_JHSEOL


// ±Ű·Îąú
//==========================================================================================================================================================================
#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP1		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP2		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP3		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP4		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP5		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP6		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP7		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP8		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP9		"115.144.35."
#define SCADMINTOOL_CONNECTABLE_PREFIX_IP10		"115.144.35."
#define IS_SCADMINTOOL_CONNECTABLE_IP(ip)	\
	(  0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP1,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP1)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP2,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP2)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP3,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP3)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP4,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP4)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP5,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP5)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP6,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP6)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP7,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP7)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP8,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP8)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP9,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP9)	) \
	|| 0 == strncmp((ip),SCADMINTOOL_CONNECTABLE_PREFIX_IP10,	strlen(SCADMINTOOL_CONNECTABLE_PREFIX_IP10)	) )

#define LAUNCHER_WEB_URL						"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Main/index.php"
#define TESTSERVER_LAUNCHER_WEB_URL				"https://ao.masangsoft.com/_AceOnline/_Luncher_/_Test/index.php"

#define BILLING_DBSERVER_DATABASE_NAME			"MS_Billing"
#define BILLING_DBSERVER_USER_ID				"atum"
#define BILLING_DBSERVER_USER_PWD				"dpdltm!@eoqkr"
#endif // S_GLOBAL_SERVER_SETTING_JHSEOL


///////////////////////////////////////////////////////////////////////////////
// 2007-02-13 by cmkwon

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 08
#ifdef S_140_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME						"±Ľ¸˛"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME						"±Ľ¸˛"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME						"MS PGothic"				// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							SHIFTJIS_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME						"Tahoma"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME						"Verdana"				// 2008-06-18 by cmkwon, ±ŰľľĂĽ, ·Ż˝ĂľĆ·ÎşÎĹÍ ąŢŔ˝
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME					"Times New Roman"		// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET						VIETNAMESE_CHARSET		// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT						FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define	SG_BOX_FONT_FACENAME					"Times New Roman"		// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET						VIETNAMESE_CHARSET		// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT						FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define	SG_BOX_FONT_FACENAME						"Tahoma"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define	SG_BOX_FONT_FACENAME						"Tahoma"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ 
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define	SG_BOX_FONT_FACENAME						"Tahoma"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define	SG_BOX_FONT_FACENAME						"Tahoma"					// 2007-02-12 by cmkwon, ±ŰľľĂĽ
#define	SG_BOX_FONT_CHARSET							ANSI_CHARSET			// 2007-02-12 by cmkwon, Äł¸ŻĹÍĽÂ
#define	SG_BOX_FONT_WEIGHT							FW_BOLD					// 2007-02-12 by cmkwon, ±ŰľľĂĽ µÎ±ú
#endif

// #define STR_XOR_KEY_STRING_PRE_SERVER_ADDRESS				"+-faNsf(^fP{)3>fnao??_+|23kdasf*^@`d{]s*&DS"	// 2008-04-23 by cmkwon, PreServer ÁÖĽŇ¸¦ IPżÍ µµ¸ŢŔÎ µŃ´Ů Áöżř - 




// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 09
#if defined(S_140_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	// 61.39.170.140							==>	"1D1C4852775D571F6E48614F19"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
	// 61.39.170.151							==>	"1D1C4852775D571F6E48614E1C"
	// 61.39.170.169							==> "1D1C4852775D571F6E48614D10"
	// 115.144.35.11							==> "1A1C534F7F4752066D537E4A18"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1D1C4852775D571F6E48614F19"

	#define REGISTRY_BASE_PATH						"SpaceCowboy(Masang51)"
	#define EXE_1_FILE_NAME							"SpaceCowboy.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"SpaceCowboy.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"

#elif defined(S_140_SERVER_SETTING_HSSON)
	// ş» Ľ­ąö·Î ip ĽĽĆĂ
	// 121.134.114.140							==>	"1A1F574F7F4052066F57645518070E"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1F574F7F4052066F57645518070E"

	#define REGISTRY_BASE_PATH						"SpaceCowboy(Masang51)_Test"
	#define EXE_1_FILE_NAME							"SpaceCowboy.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"SpaceCowboy_Test.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif



#if defined(S_KOR_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	// 182.162.137.7								==> "1A15544F7F4554066F5567551E"// 2012-10-04 by hskim, ÇŃ±ą ŔÚĂĽ Ľ­şń˝ş (°ˇşńľĆ IDC)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A15544F7F4554066F5567551E"

	#define REGISTRY_BASE_PATH		 				"ACEonline_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher_Test.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_KOR_SERVER_SETTING_HSSON)
	// ş» Ľ­ąö·Î ip ĽĽĆĂ
	// aceonlineloginsvr001.masangsoft.com			==> "4A4E030E201F0F463B0A3F1C405D4D101C515F0E11324A0F535D0C170E1512043D2F0D"			// 2012-10-04 by hskim, ÇŃ±ą ŔÚĂĽ Ľ­şń˝ş (°ˇşńľĆ IDC)
	// aceonlineloginsvrtest.masangsoft.com			==> "4A4E030E201F0F463B0A3F1C405D4D101C150A4C4B71461D41520503121C005E70230F09"			// 2012-10-04 by hskim, ÇŃ±ą ŔÚĂĽ Ľ­şń˝ş (°ˇşńľĆ IDC)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"4A4E030E201F0F463B0A3F1C405D4D101C515F0E11324A0F535D0C170E1512043D2F0D"


	#define REGISTRY_BASE_PATH						"ACEonline"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif



#if defined(S_JPN_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)

	#ifdef S_EP4_TEST_SERVER_HSKIM
		#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1D1C48537E45481969527E4E1B"
	#else
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	//61.206.174.229(Arario_Ĺ×Ľ·)				==>	"1D1C48537E45481969527E491B0A"	// 2009-10-26 by cmkwon, Ľ­ąö±şĹëÇŐ, IDC ŔĚŔü ŔŰľ÷ - ±âÁ¸ 119.75.233.55
	//61.206.174.70(Arario_Ĺ×Ľ·)şŻ°ćµĘ			==>	"1D1C48537E45481969527E4C193A"	// 2010-10-18 by shcho, ŔĎş» Ĺ×Ľ· IP / PORTąřČŁ şŻ°ć - ±âÁ¸ 229¸¦ 70Ŕ¸·Î şŻ°ć 
	//61.206.174.179(Arario_Ĺ×Ľ·)şŻ°ćµĘ			==>	"1D1C48537E45481969527E4A1E0A"	// 2013-09-12 by jekim, ŔĎş» Ĺ×Ľ· IP şŻ°ć.
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1D1C48537E45481969527E4A1E0A"
	#endif

	#define REGISTRY_BASE_PATH						"ACEOnline(JPN)_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_JPN_SERVER_SETTING_HSSON)

	// 61.206.174.81(Arario_ş»Ľ·)				==>	"1D1C48537E45481969527E4318"	// 2009-10-26 by cmkwon, Ľ­ąö±şĹëÇŐ, IDC ŔĚŔü ŔŰľ÷ - ±âÁ¸ 119.75.233.81
	// 61.206.174.160(Arario_ş»Ľ·)				==> "1D1C48537E45481969527E4A1F03"	// 2010. 10. 04. by hsLee.	Ľ­ąö IPşŻ°ć. - ±âş» 61.206.174.81
	// 61.39.170.222(¸¶»ó ł»şÎ Ľ­ąö)			==> "1D1C4852775D571F6E4862491B"	// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1D1C48537E45481969527E4A1F03"

	#define REGISTRY_BASE_PATH						"ACEOnline(JPN)"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif



#if defined(S_CAN_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
//XOR KEY: +-faNsf(^fP{)3>fnao??_+|23kdasf*^@`d{]s*&DS

#ifdef S_EP4_TEST_SERVER_HSKIM
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1F514F7E5D56066F"	 //	IP 127.0.0.1 //2021 by Inet
#else
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5F4815153D16145E3B147E0940455F0A1D4C0A4950335E085B5C054A0F1612"	// 2013-11-06 by jekim, 68.179.106.27 ÄłłŞ´Ů Ĺ×Ľ· ľĆŔĚÇÇ şŻ°ć // IP 66.207.198.252 - ÄłłŞ´Ů ł»şÎ Ĺ×Ľ·
	//#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C534F7F4752066D537E4A1D01"	// IP 115.144.35.142 - ¸¶»ó ł»şÎ ÄłłŞ´Ů Ĺ×Ľ·
#endif

#define REGISTRY_BASE_PATH						"NewRivalsEvolution"
#define EXE_1_FILE_NAME							"46.exe"
#define LAUNCHER_FILE_NAME						"Updater.exe"
#ifdef _WIN_XP
#define CLIENT_EXEUTE_FILE_NAME					"EngineXP.atm"
#else
#define CLIENT_EXEUTE_FILE_NAME					"Engine.exe"
#endif
#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, 수정함

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_CAN_SERVER_SETTING_HSSON)

#ifdef S_EP4_TEST_SERVER_HSKIM
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1F514F7E5D56066F"	//	IP 127.0.0.1 //2021 by Inet
#else
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5F4815153D16145E3B147E0940455F0A1D4C0A4950335E085B5C054A0F1612"	// 2013-11-06 by jekim, 68.179.106.27 ÄłłŞ´Ů Ĺ×Ľ· ľĆŔĚÇÇ şŻ°ć // IP 66.207.198.252 - ÄłłŞ´Ů ł»şÎ Ĺ×Ľ·
//#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C534F7F4752066D537E4A1D01"	// IP 115.144.35.142 - ¸¶»ó ł»şÎ ÄłłŞ´Ů Ĺ×Ľ·
#endif

#define REGISTRY_BASE_PATH						"NewRivalsEvolution"
#define EXE_1_FILE_NAME							"46.exe"
#define LAUNCHER_FILE_NAME						"Updater.exe"
#ifdef _WIN_XP
#define CLIENT_EXEUTE_FILE_NAME					"EngineXP.atm"
#else
#define CLIENT_EXEUTE_FILE_NAME					"Engine.exe"
#endif
#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, 수정함

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif



#if defined(S_RUS_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
#ifdef S_EP4_TEST_SERVER_HSKIM
	/// 2012-05-05 by jhsel, ·Ż˝ĂľĆ EP4 Ĺ×Ľ·ŔĎ °ćżě 109.105.134.130 ·Î ÁöÁ¤ ==> "1A1D5F4F7F4353066F55645518000E"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1D5F4F7F4353066F55645518000E"
#else
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	// 109.105.134.130								==>	"1A1D5F4F7F4353066F55645518000E"			// 2010-04-26 by cmkwon, ·Ż˝ĂľĆ Innova Ĺ×Ľ· IP şŻ°ć - ±âÁ¸(89.249.25.58)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1D5F4F7F4353066F55645518000E"
#endif
	#define REGISTRY_BASE_PATH						"ACEonline(RU)_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_RUS_SERVER_SETTING_HSSON)
#ifdef S_EP4_TEST_SERVER_HSKIM
	/// 2012-05-05 by jhsel, ·Ż˝ĂľĆ EP4 Ĺ×Ľ·ŔĎ °ćżě 109.105.134.130 ·Î ÁöÁ¤ ==> "1A1D5F4F7F4353066F55645518000E"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1D5F4F7F4353066F55645518000E"
#else
	// 109.105.134.133							==>	"1A1D5F4F7F4353066F55645518000D"			// 2008-06-24 by cmkwon, ĽöÁ¤µĘ
	// 61.39.170.220							==> "1D1C4852775D571F6E48624919"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1D5F4F7F4353066F55645518000D"
#endif
	#define REGISTRY_BASE_PATH						"ACEonline(RU)"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ
	
	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

#if defined(S_VIE_SERVER_SETTING_HSSON) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)

#ifdef S_EP4_TEST_SERVER_HSKIM
	// 117.103.198.170								==> "1A1C514F7F4355066F5F685518040E"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C514F7F4355066F5F685518040E"	 // 2012-04-09 by hskim, EP4 Ŕü´Ţżë Ľ­ąö Ăß°ˇ
#else
// 2011-08-16 by shcho, şŁĆ®ł˛ IPşŻ°ćµĘ - preServer(±âÁ¸:222.235.15.54 -> şŻ°ć:117.103.198.155)
// 222.255.15.54								==>	"191F544F7C4653066F537E4E1D"
// 117.103.198.155								==> "1A1C514F7F4355066F5F685518060B"
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C514F7F4355066F5F685518060B"
#endif

#define REGISTRY_BASE_PATH						"PhiDoi_Test"
#define EXE_1_FILE_NAME							"PhiDoi.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"PhiDoi.atm"
#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_VIE_SERVER_SETTING_HSSON)
// 117.103.194.77					        ==>	"1A1C514F7F4355066F5F64551E04"		// 2011-08-16 by shcho, şŁĆ®ł˛ IPşŻ°ćµĘ 
// MasangTest(61.39.170.143)				==> "1D1C4852775D571F6E48614F1A"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)

// 2012-10-15 by bckim, şŁĆ®ł˛ EP4 ˝ĹĽ­ąö şôµĺ ĆĐÄˇżäĂ» °ü·Ă Á¤ş¸ Ăß°ˇ ľ÷ą«
// şŻ°ćŔü : 117.103.194.77 (1A1C514F7F4355066F5F64551E04) --> şŻ°ćČÄ : 117.103.194.68 (1A1C514F7F4355066F5F64551F0B)  
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C514F7F4355066F5F64551F0B"
// end 2012-10-15 by bckim, şŁĆ®ł˛ EP4 ˝ĹĽ­ąö şôµĺ ĆĐÄˇżäĂ» °ü·Ă Á¤ş¸ Ăß°ˇ ľ÷ą« 

#define REGISTRY_BASE_PATH						"PhiDoi"
#define EXE_1_FILE_NAME							"PhiDoi.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"PhiDoi.atm"
#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// µ¶ŔĎ
#if defined(S_DEU_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
	// 79.110.95.47								==> "1C1448507F4348116B48644C"	// 2010-03-25 by cmkwon, Gameforge4D_Deu Ĺ×Ľ· Ľ­ąö IP şŻ°ć(±âÁ¸:79.110.95.9) - 
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B48644C"

	#define REGISTRY_BASE_PATH						"AirRivalsDe_Test"
	#define EXE_1_FILE_NAME							"AirRivals.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_DEU_SERVER_SETTING_JHAHN)
	// prokyon.airrivals.de						==> "5B5F090A371C08063F0F220940455F0A1D4F0B5A"		// 2009-03-04 by cmkwon, Gameforge żµ±ą,µ¶ŔĎ ş»Ľ· PreServer, DBServer Domain Ŕ¸·Î şŻ°ć - ±âÁ¸(83.141.22.113)
	// 61.39.170.147							==>	"1D1C4852775D571F6E48614F1E"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5B5F090A371C08063F0F220940455F0A1D4F0B5A"

	#define REGISTRY_BASE_PATH						"AirRivalsDe"
	#define EXE_1_FILE_NAME							"AirRivals.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"
	
	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// żµ±ą
#if defined(S_ENG_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// 79.110.95.47								==> "1C1448507F4348116B48644C"	// 2010-03-25 by cmkwon, Gameforge4D_Deu Ĺ×Ľ· Ľ­ąö IP şŻ°ć(±âÁ¸:79.110.95.9) - 
// 79.110.95.6									"1C1448507F4348116B4866"		// 2008-10-30 by cmkwon, Gameforge4D_Eng Ĺ×Ľ· Ľ­ąö IP şŻ°ć - , // 85.115.19.228							==>	"131848507F4648196748624911"
// 79.110.95.62								==> "1C1448507F4348116B486649"	// 2012-11-21 by jhseol, Gameforge4D żµ±ą EP4 Ĺ×Ľ· Ăß°ˇ	
// 79.110.88.24								==> "1C1448507F4348106648624F"	// 2013-02-14 by jhseol, żµ±ą EP4 Ĺ×Ľ· Á¤ş¸ şŻ°ć
#ifdef S_EP4_TEST_SERVER_HSKIM
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348106648624F"
#else
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B4866"		//	79.110.95.6
#endif

#define REGISTRY_BASE_PATH						"AirRivals_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher_Test.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_ENG_SERVER_SETTING_JHAHN)
// zion.airrivals.net						==>	"5144090F60120F5A2C0F261A454010080B15"		// 2009-03-04 by cmkwon, Gameforge żµ±ą,µ¶ŔĎ ş»Ľ· PreServer, DBServer Domain Ŕ¸·Î şŻ°ć - ±âÁ¸(83.141.22.23)
// 61.39.170.145							==> "1D1C4852775D571F6E48614F1C"	// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#ifdef S_EP4_TEST_SERVER_HSKIM
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5144090F60120F5A2C0F261A454010080B15"
#else
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5144090F60120F5A2C0F261A454010080B15"
#endif

#define REGISTRY_BASE_PATH						"AirRivals"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// ÇÁ¶ű˝ş
#if defined(S_FRA_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// 79.110.95.20									"1C1448507F4348116B48624B"		// 2009-03-03 by cmkwon, Áöżř Ľ­şń˝ş Ăß°ˇ(Gameforge4D ÇÁ¶ű˝şľî °ü·Ă) - 
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B48624B"

#define REGISTRY_BASE_PATH						"AirRivalsFR_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_FRA_SERVER_SETTING_JHAHN)
// s1.airrivals.fr							==>	"581C48002701144128073C0807554C"
// 61.39.170.225							==> "1D1C4852775D571F6E4862491C"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"581C48002701144128073C0807554C"

#define REGISTRY_BASE_PATH						"AirRivalsFR"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// ˝şĆäŔÎ
#if defined(S_ESP_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// 79.110.95.6									"1C1448507F4348116B4866"		// 2008-10-30 by cmkwon, Gameforge4D_Eng Ĺ×Ľ· Ľ­ąö IP şŻ°ć - , // 85.115.19.228							==>	"131848507F4648196748624911"

// 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 
// #define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5F48151560120F5A2C0F261A454010031D"
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B48624E"			//	79.110.95.25
// End. 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 

#define REGISTRY_BASE_PATH						"AirRivalsEs_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"

#elif defined(S_ESP_SERVER_SETTING_JHAHN)
// zion.airrivals.net						==>	"5144090F60120F5A2C0F261A454010080B15"		// 2009-03-04 by cmkwon, Gameforge żµ±ą,µ¶ŔĎ ş»Ľ· PreServer, DBServer Domain Ŕ¸·Î şŻ°ć - ±âÁ¸(83.141.22.23)
// 61.39.170.145							==> "1D1C4852775D571F6E48614F1C"	// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"581C48002701144128073C0807564D"

#define REGISTRY_BASE_PATH						"AirRivalsEs"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// ĹÍĹ°
#if defined(S_TUR_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// 79.110.95.14									"1C1448507F4348116B48614F"		// 2009-01-09 by dhjin

// 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 
//#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B48614F"		//	79.110.95.14
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B486349"		//	79.110.95.32
// End. 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 

#define REGISTRY_BASE_PATH						"AirRivalsTR_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_TUR_SERVER_SETTING_JHAHN)
// solus.airrivals.org						==>	"58420A143D5D07412C14390D485F4D48011308"
// 61.39.170.223							==> "1D1C4852775D571F6E4862491A"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"58420A143D5D07412C14390D485F4D48011308"

#define REGISTRY_BASE_PATH						"AirRivalsTR"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

// Ćú¶őµĺ
#if defined(S_POL_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// test.airrivals.pl							"5F48151560120F5A2C0F261A4540101602"

// 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 
//#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"5F48151560120F5A2C0F261A4540101602"
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B486249"	//	79.110.95.22
// End. 2014-06-11 by bckim, °ÔŔÓĆ÷Áö Ĺ×Ľ·żë ˝ÇÇŕĆÄŔĎ ľĆŔĚÇÇ şŻ°ć 

#define REGISTRY_BASE_PATH						"AirRivalsPl_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_POL_SERVER_SETTING_JHAHN)
// s1.airrivals.pl							==>	"581C48002701144128073C08074352"
// 61.39.170.226							==> "1D1C4852775D571F6E4862491F"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"581C48002701144128073C08074352"

#define REGISTRY_BASE_PATH						"AirRivalsPl"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

//ŔĚĹ»¸®ľĆ
#if defined(S_ITA_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
// 79.110.95.16									"1C1448507F4348116B48614D"		// 2009-01-20 by cmkwon, ŔĚĹ»¸®ľĆ Ĺ×Ľ· Á¤ş¸ ąŢľĆĽ­ ĽöÁ¤ - 
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1C1448507F4348116B48614D"		//	79.110.95.16

#define REGISTRY_BASE_PATH						"AirRivalsIT_Test"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_ITA_SERVER_SETTING_JHAHN)
// s1.airrivals.it							==>	"581C48002701144128073C08075A4A"
	// 61.39.170.224							==> "1D1C4852775D571F6E4862491D"		// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"581C48002701144128073C08075A4A"

	#define REGISTRY_BASE_PATH						"AirRivalsIT"
#define EXE_1_FILE_NAME							"AirRivals.exe"
#define LAUNCHER_FILE_NAME						"Launcher.atm"

#define CLIENT_EXEUTE_FILE_NAME					"AirRivals.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"
	
	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

//ľĆ¸ŁÇîĆĽłŞ 
#if defined(S_ARG_SERVER_SETTING_JHAHN) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)
#ifdef S_EP4_TEST_SERVER_HSKIM
	/// 2012-04-19 by jhsel, ľĆ¸ŁÇîĆĽłŞ EP4 Ĺ×Ľ·ŔĎ °ćżě 209.251.185.12 ·Î ÁöÁ¤ ==> "191D5F4F7C4657066F5E65551801"
#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"191D5F4F7C4657066F5E65551801"
#else
// Lin_Arg_Test_PreServer(209.251.187.187)	==>	"191D5F4F7C4657066F5E6755180B09"	// 2010-11-01 by shcho
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"191D5F4F7C4657066F5E6755180B09"
#endif
	#define REGISTRY_BASE_PATH						"ACEonline_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"Sign_up.htm"

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_ARG_SERVER_SETTING_JHAHN)
#ifdef S_EP4_TEST_SERVER_HSKIM
	/// 2012-04-19 by jhsel, ľĆ¸ŁÇîĆĽłŞ EP4 Ĺ×Ľ·ŔĎ °ćżě 209.251.185.12 ·Î ÁöÁ¤ ==> "191D5F4F7C4657066F5E65551801"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"191D5F4F7C4657066F5E65551801"
#else
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"191D5F4F7C4657066F5E6755180B0D"
#endif
	#define REGISTRY_BASE_PATH						"ACEonline"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
#define URL_REGISTER_PAGE						"Sign_up.htm"

#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif


//Áß±ą
#if defined(S_CHN_SERVER_SETTING_JHSEOL) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)			// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	// 115.144.35.146								==> "1A1C534F7F4752066D537E4A1D05"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C534F7F4752066D537E4A1D05"

	#define REGISTRY_BASE_PATH		 				"ACEonline_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher_Test.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_CHN_SERVER_SETTING_JHSEOL)
	// ş» Ľ­ąö·Î ip ĽĽĆĂ
	// 54.250.154.14								==> "1E1948537B4348196B527E4A1D"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1E1948537B4348196B527E4A1D"


	#define REGISTRY_BASE_PATH						"ACEonline"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif


//±Ű·Îąú
#if defined(S_GLOBAL_SERVER_SETTING_JHSEOL) && defined(S_ACCESS_INTERNAL_SERVER_HSSON)			// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
	// ł»şÎ Ľ­ąö·Î ip ĽĽĆĂ
	// 115.144.35.146								==> "1A1C534F7F4752066D537E4A1D05"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1A1C534F7F4752066D537E4A1D05"

	#define REGISTRY_BASE_PATH		 				"ACEonline_Test"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher_Test.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#elif defined(S_GLOBAL_SERVER_SETTING_JHSEOL)
	// ş» Ľ­ąö·Î ip ĽĽĆĂ
	// 54.250.154.14								==> "1E1948537B4348196B527E4A1D"
	#define CHOICE_PRE_SERVER_IP_OR_DOMAIN_IN_XOR	"1E1948537B4348196B527E4A1D"


	#define REGISTRY_BASE_PATH						"ACEonline"
	#define EXE_1_FILE_NAME							"ACEonline.exe"
	#define LAUNCHER_FILE_NAME						"Launcher.atm"

	#define CLIENT_EXEUTE_FILE_NAME					"ACEonline.atm"
	#define URL_REGISTER_PAGE						"reg.asp"				// 2006-04-05 by cmkwon, ĽöÁ¤ÇÔ

	#define WEB_START_REGISTRY_VALUE_NAME			"InstallPath"
#endif

///////////////////////////////////////////////////////////////////////////////
// 2006-12-22 by cmkwon, °ÔŔÓŔĚ¸§ şŻ°ćÇĎ¸éĽ­ 

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 10
#ifdef S_140_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"SpaceCowboy Online"
#define STRMSG_REG_STRING_CLIENT_VERSION			"SpaceCowboyVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Masang Soft"
#endif


#ifdef S_KOR_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Yedang Online"
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Arario"
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Wikigames"				// 2008-07-31 by cmkwon, Yedang-Global_Eng ¸¦ Wikigames_Eng ·Î şŻ°ć ÇÔ - 
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Innova"
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define STRMSG_WINDOW_TEXT							"PhiDoi"
#define STRMSG_REG_STRING_CLIENT_VERSION			"PhiDoiVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"VTC Game"
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define STRMSG_WINDOW_TEXT							"AirRivals"
#define STRMSG_REG_STRING_CLIENT_VERSION			"AirRivalsVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"Gameforge4d"
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"axeso5.com"
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"MasangSoft"
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define STRMSG_WINDOW_TEXT							"ACEonline"
#define STRMSG_REG_STRING_CLIENT_VERSION			"ACEonlineVersion"
#define STRMSG_REG_STRING_REGISTRYKEY_NAME			"MasangSoft Global"
#endif

///////////////////////////////////////////////////////////////////////////////
// 2007-06-27 by cmkwon, Áß±ą ąć˝ÉĂë ˝Ă˝şĹŰ ĽöÁ¤ - ąĚĽşłâŔÚ °ü·Ă
// Kor		- ¸¸ 20ĽĽ
// China	- ¸¸ 18ĽĽ
#define ADULT_YEARS									20			// 2007-06-29 by cmkwon,

///////////////////////////////////////////////////////////////////////////////
// 2007-07-06 by cmkwon, SCAdminToolżˇĽ­ OnlyServerAdmin°ü·Ă ĽöÁ¤ - °čÁ¤ Á¤ş¸
#define SCADMINTOOL_ONLY_SERVER_ADMIN_ACCOUNT_NAME		"SC_moniter"
#define SCADMINTOOL_ONLY_SERVER_ADMIN_PASSWORD			"cowboyWkd"

///////////////////////////////////////////////////////////////////////////////
// 2007-09-05 by cmkwon, EXE_1żˇ ·Î±×ŔÎ Ľ­ąö Ľ±ĹĂ ŔÎĹÍĆäŔĚ˝ş ĽöÁ¤ - »óŔ§ URL, żěĂř ÇĎ´Ü URL
#define EXE1_URL_1										"http://notice.aceonline.com.cn/ace2.htm"
#define EXE1_URL_2										"http://notice.aceonline.com.cn/ace1.htm"


///////////////////////////////////////////////////////////////////////////////
// 2008-12-19 by cmkwon, ÇŃ±ą Yedang ÇŮ˝Żµĺ ¸đ´ĎĹÍ¸µ Ľ­ąö ĽłÁ¤ Ăß°ˇ - IP°ˇ ""·Î ĽłÁ¤µÇ¸é ¸đ´ĎĹÍ¸µ Ľ­ąö¸¦ »çżëÇĎÁö ľĘ´Â °ÍŔÓ, ÇöŔç´Â Masang140°ú Yedang¸¸ »çżë ÇŇ °ÍŔÓ

// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 11
#ifdef S_140_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					"115.144.35.184"
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					""		// 2012-07-06 by hskim, YD IDC ŔĚŔü ŔŰľ÷ - YD ¸đ´ĎĹÍ¸µ Ľ­ąö »çżë ľČÇÔ
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef _DEFINED_GAMEFORGE4D_
	#ifdef S_ACCESS_INTERNAL_SERVER_HSSON
		#define GAME_GUARD_MONITORING_SERVER_IP					"79.110.95.63"		// 2013-07-15 by bckim, Ĺ×˝şĆ®żë ÇŮ˝Żµĺ ¸đ´ĎĹÍ¸µ 
	#else 
		#define GAME_GUARD_MONITORING_SERVER_IP					"79.110.88.27"		// 2013-07-15 by bckim, ¶óŔĚşężë ÇŮ˝Żµĺ ¸đ´ĎĹÍ¸µ 
	#endif
#endif

#ifdef S_ARG_SERVER_SETTING_JHAHN
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define GAME_GUARD_MONITORING_SERVER_IP					""
#endif

///////////////////////////////////////////////////////////////////////////////
// 2009-02-12 by cmkwon, EP3-3 żůµĺ·©Ĺ·˝Ă˝şĹŰ ±¸Çö - żůµĺ·©Ĺ· DB Ľ­ąö Á¤ş¸
// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 12
#ifdef S_140_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.135"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						9979
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef _DEFINED_GAMEFORGE4D_
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif


#ifdef S_ARG_SERVER_SETTING_JHAHN
#define WRK_DBSERVER_IP							"115.144.35.131"		// 2012-10-14 by hskim, ¸¶»ó »çĂĘµż »çą«˝Ç IP şŻ°ć - // 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"		// 2012-10-14 by hskim, ¸¶»ó »çĂĘµż »çą«˝Ç IP şŻ°ć - // 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define WRK_DBSERVER_IP							"115.144.35.131"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)
#define WRK_DBSERVER_PORT						1433
#define WRK_DBSERVER_DATABASE_NAME				"atum2_db_WorldRanking"
#define WRK_DBSERVER_ID							"atum"
#define WRK_DBSERVER_PWD						"callweb"
#define WRK_DBSERVER_IP_FOR_TEST_SERVER			"115.144.35.140"				// 2009-12-28 by cmkwon, ¸¶»óČ¸»ç IP şŻ°ć - ±âÁ¸(121.134.114.)// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#define WRK_DBSERVER_PORT_FOR_TEST_SERVER		9979					// 2009-06-01 by cmkwon, żůµĺ ·©Ĺ· ˝Ă˝şĹŰ Ĺ×˝şĆ® ±â´É ±¸Çö(for Ĺ×Ľ·) - 
#endif

///////////////////////////////////////////////////////////////////////////////
// 2009-03-31 by cmkwon, ĽĽ·ÂĂĘ±âČ­ ˝Ă˝şĹŰ ±¸Çö - 
#define MAX_INFLUENCE_PERCENT			53		// ĂÖ´ë 5% Â÷ŔĚ±îÁö¸¸ ĽĽ·Â Ľ±ĹĂŔĚ °ˇ´ÉÇŃ´Ů.


///////////////////////////////////////////////////////////////////////////////
// 2009-05-12 by cmkwon, (ŔĎş»żäĂ») ŔĎş»¸¸ ŔüÁř ±âÁöŔü ÁÖ±â 7ŔĎ·Î ĽöÁ¤ - ŔüÁř±âÁöŔü ÁÖ±â Ľ­şń˝şş°·Î ´Ů¸Ł°Ô ĽłÁ¤
// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 13
#ifdef S_140_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				7
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define	OUTPOST_NEXTWARGAP				5
#endif

// °ÔŔÓĆ÷Áö
#if defined(S_DEU_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_ENG_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_ITA_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_FRA_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_POL_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_ESP_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_TUR_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#if defined(S_ARG_SERVER_SETTING_JHAHN)
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_CHN_SERVER_SETTING_JHSEOL							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define	OUTPOST_NEXTWARGAP				5
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define	OUTPOST_NEXTWARGAP				5
#endif

///////////////////////////////////////////////////////////////////////////////
// 2009-07-08 by cmkwon, ŔüŔď °ü·Ă Á¤ŔÇ Ŕ§Äˇ ŔĚµż(LocalizationDefineCommon.h) - 
// ±ą°ˇ ĽŇ˝şĹëÇŐ ¶§ Ăß°ˇ µÇľîľß ÇŇ şÎşĐ ĽřĽ­ 14
#ifdef S_140_SERVER_SETTING_HSSON
#define	OUTPOST_WARTIME					20			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ	// 2013-01-21 by jhseol, NGC ŔüŔü±âÁö Ć®¸®°Ĺ ˝Ă˝şĹŰ - Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ ˝Ă°Ł 20şĐŔ¸·Î şŻ°ć
#define OUTPOST_WARTIME_FOR_TESTSERVER	20			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ	// 2013-01-21 by jhseol, NGC ŔüŔü±âÁö Ć®¸®°Ĺ ˝Ă˝şĹŰ - Ĺ×˝şĆ®¸¦ Ŕ§ÇŘ ˝Ă°Ł 20şĐŔ¸·Î şŻ°ć
#define PAY_MINIMUN_COUNT				2			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#ifdef S_KOR_SERVER_SETTING_HSSON
// 2013-05-21 by bckim, ŔüÁř±âÁöŔü ˝Ă°Ł 2˝Ă°ŁżˇĽ­ 1˝Ă°ŁŔ¸·Î şŻ°ć 
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#ifdef S_JPN_SERVER_SETTING_HSSON
// 2013-08-27 ŔĎş» Ăß°ˇ : EP4-3 ŔüÁř±âÁöŔü ˝Ă°Ł 2˝Ă°ŁżˇĽ­ 1˝Ă°ŁŔ¸·Î şŻ°ć 
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				3			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö	// 2014-08-21 by bckim, ŔĎş» ĂÖĽŇŔÎżř 10¸íżˇĽ­ 3¸íŔ¸·Î şŻ°ć
#endif

#ifdef S_CAN_SERVER_SETTING_HSSON
// 2013-09-03 ÄłłŞ´Ů Ăß°ˇ : EP4-3 ŔüÁř±âÁöŔü ˝Ă°Ł 2˝Ă°ŁżˇĽ­ 1˝Ă°ŁŔ¸·Î şŻ°ć 
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#ifdef S_RUS_SERVER_SETTING_HSSON
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#ifdef S_VIE_SERVER_SETTING_HSSON
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ			// 2014-03-04 şŁĆ®ł˛ Ăß°ˇ : EP4-3 ˝Ă°ŁşŻ°ć 120->60
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_DEU_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_ENG_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_ITA_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_FRA_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_POL_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_ESP_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_TUR_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_ARG_SERVER_SETTING_JHAHN)
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ		// 2014-03-04 ľĆ¸ŁÇî Ăß°ˇ : EP4-3 ˝Ă°ŁşŻ°ć 120->60
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#if defined(S_CHN_SERVER_SETTING_JHSEOL)							// 2013-07-02 by jhseol, Áß±ą Ĺ×˝şĆ®Ľ­ąö ±¸Ăŕ
#define	OUTPOST_WARTIME					120			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł			==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 120şĐ
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł	==> ¸¶»ó 10şĐ, łŞ¸ÓÁö´Â 60şĐ
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö ==> ¸¶»ó 1¸í, łŞ¸ÓÁö´Â 10¸í
#endif

#ifdef S_GLOBAL_SERVER_SETTING_JHSEOL						// 2013-09-04 by jhseol, ±Ű·Îąú Ľ­ąö şôµĺżÉĽÇ Ăß°ˇ
#define	OUTPOST_WARTIME					60			// ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł
#define OUTPOST_WARTIME_FOR_TESTSERVER	60			// Ĺ×Ľ·Ŕş ŔüÁř±âÁöŔü ÁřÇŕ ˝Ă°Ł
#define PAY_MINIMUN_COUNT				10			// ¸đĽ±Ŕü,ŔüÁř±âÁöŔü,°ĹÁˇŔü °łŔÎ ş¸»óŔ» Ŕ§ÇŃ ĂÖĽŇ ŔÎżřĽö
#endif

///////////////////////////////////////////////////////////////////////////////
// 2009-11-02 by cmkwon, Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö - 
#define SIZE_MAX_ADDABLE_INVENTORY_COUNT		50		// ±âş»°ú ÇÁ¸®ąĚľöŔ» Á¦żÜÇŃ Ăß°ˇ·Î °ˇ´ÉÇŃ ĂÖ´ë ŔÎşĄ Ăß°ˇ °łĽö
#define SIZE_MAX_ADDABLE_STORE_COUNT			50		// ±âş»°ú ÇÁ¸®ąĚľöŔ» Á¦żÜÇŃ Ăß°ˇ·Î °ˇ´ÉÇŃ ĂÖ´ë Ă˘°í Ăß°ˇ °łĽö


////////////////////////////////////////////////////////////////////////////////
// ÄÁĹŮĂ÷ ąöÁŻ °ü¸®żë µđĆÄŔÎ. by hsLee. 
#define __CONTENTS_SHOW_INFINITY_DIFFICULTY_EDIT_WND__		// ŔÎÇÇ´ĎĆĽ ł­ŔĚµµ Á¶Á¤ UI ş¸ŔĚ±â.	2010. 07. 27. by hsLee.
////////////////////////////////////////////////////////////////////////////////


#endif // end_#ifndef _LOCALIZATION_DEFINE_COMMON_H_

#include "stdafx.h"
#include "atumparam.h"
#include "atumprotocol.h"
#include "AtumError.h"				// 2006-10-02 by cmkwon
#include <fstream>					// 2007-05-10 by cmkwon

#ifdef _ATUM_FIELD_SERVER
#include "FieldIOCPSocket.h"
#endif

#ifndef DBGOUT
#define	DBGOUT	(void(0))
#endif

///////////////////////////////////////////////////////////////////////////////
// Config Variables
///////////////////////////////////////////////////////////////////////////////
char	CONFIG_ROOT[1024];

///////////////////////////////////////////////////////////////////////////////
// String Conversion Functions
const char *DBG_GET_POSITION_STRING(D3DXVECTOR3& vec, string& str)
{
#ifdef _DEBUG
	char buffer[256];
	sprintf(buffer, "(%d, %d, %d)", (int)vec.x, (int)vec.y, (int)vec.z);
	str += buffer;

	return str.c_str();
#else
	return NULL;
#endif
}

const char *GetMapString(MAP_CHANNEL_INDEX &refMapChannelIndex, string &str)
{
	char buffer[512];
	sprintf(buffer, "%d(%d)", refMapChannelIndex.MapIndex, refMapChannelIndex.ChannelIndex);
	str += buffer;

	return str.c_str();
}

const char *GetCharacterString(const CHARACTER *pCharac, string &str)
{
	if (pCharac == NULL)
	{
		return "[NULL]";
	}

	char buffer[512];
	sprintf(buffer, "[A(%s:%d) C(%s:%d) CI(%d)]",
		pCharac->AccountName, pCharac->AccountUniqueNumber, pCharac->CharacterName, pCharac->CharacterUniqueNumber, pCharac->ClientIndex);
// 2007-07-18 by cmkwon, 정보 수정 - 위와 같이 수정
//	sprintf(buffer, "[A: %s, CH: %s, CI: %d, UN: %d]",
//		pCharac->AccountName, pCharac->CharacterName, pCharac->ClientIndex, pCharac->CharacterUniqueNumber);
	str += buffer;

	return str.c_str();
}

const char *GetItemString(const ITEM_BASE *pItem, string &str)
{
	if (pItem == NULL)
	{
		return "[NULL]";
	}

	char buffer[512];
	sprintf(buffer, "[N: %s, IN: %d, UN: %I64d]",
		pItem->ItemInfo->ItemName, pItem->ItemNum, pItem->UniqueNumber);
	str += buffer;

	return str.c_str();
}

const char *GetItemGeneralString(const ITEM_GENERAL *pItem, string &str)
{
	if (pItem == NULL)
	{
		return "[NULL]";
	}

	char buffer[512];
	// 2008-02-14 by cmkwon, 순서 변경 함
	sprintf(buffer, "[IN:%d N:%10s UN:%I64d C:%d]",
		pItem->ItemNum, pItem->ItemInfo->ItemName, pItem->UniqueNumber, pItem->CurrentCount);
	str += buffer;

	return str.c_str();
}

const char *GetTimeString(string &str)
{
	char buffer[128];
	time_t ltime;
	struct tm *today = NULL;

	time(&ltime);
	today = localtime(&ltime);
	strftime(buffer, 128, "[%m-%d %H:%M:%S]", today );
	str += buffer;

	return str.c_str();
}

const char *GetItemSkillString(const ITEM_SKILL *pItem, string &str)
{
	if (pItem == NULL)
	{
		return "[NULL]";
	}

	char buffer[512];
	// 2008-02-14 by cmkwon, 순서 변경 함
	sprintf(buffer, "[IN:%d N:%10s UN:%I64d]",
		pItem->ItemNum, pItem->ItemInfo->ItemName, pItem->UniqueNumber);
	str += buffer;

	return str.c_str();
}

const char *Int2String(int n, string &str)
{
	char buffer[64];
	sprintf(buffer, "%d", n);
	str += buffer;

	return str.c_str();
}

const char *GetRaceString(USHORT race)
{
	switch(race)
	{
	case RACE_BATTALUS:			return STRCMD_CS_COMMON_RACE_BATTALUS;
	case RACE_DECA:				return STRCMD_CS_COMMON_RACE_DECA;
	case RACE_PHILON:			return STRCMD_CS_COMMON_RACE_PHILON;
	case RACE_SHARRINE:			return STRCMD_CS_COMMON_RACE_SHARRINE;
// 2006-04-20 by cmkwon	case RACE_MONSTER1:			return STRCMD_CS_COMMON_RACE_MONSTER1;
	case RACE_INFLUENCE_LEADER:	return STRCMD_CS_COMMON_RACE_INFLUENCE_LEADER;			// 2006-04-20 by cmkwon
// 2006-12-08 by dhjin	case RACE_MONSTER2:			return STRCMD_CS_COMMON_RACE_MONSTER2;

// 2007-10-06 by cmkwon, 부지도자 2명의 호칭을 다르게 설정 -
//	case RACE_INFLUENCE_SUBLEADER: return STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER;	// 2006-12-08 by dhjin
//	case RACE_NPC:				return STRCMD_CS_COMMON_RACE_NPC;
	case RACE_INFLUENCE_SUBLEADER_1:	return STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_1;	// 2007-10-06 by cmkwon, 부지도자 2명의 호칭을 다르게 설정 -
	case RACE_INFLUENCE_SUBLEADER_2:	return STRCMD_CS_COMMON_RACE_INFLUENCE_SUBLEADER_2;	// 2007-10-06 by cmkwon, 부지도자 2명의 호칭을 다르게 설정 -
	case RACE_OPERATION:		return STRCMD_CS_COMMON_RACE_OPERATION;
	case RACE_GAMEMASTER:		return STRCMD_CS_COMMON_RACE_GAMEMASTER;
	case RACE_MONITOR:			return STRCMD_CS_COMMON_RACE_MONITOR;
	case RACE_GUEST:			return STRCMD_CS_COMMON_RACE_GUEST;
	case RACE_DEMO:				return STRCMD_CS_COMMON_RACE_DEMO;
	case RACE_ALL:				return STRCMD_CS_COMMON_RACE_ALL;
	}

	return STRCMD_CS_COMMON_RACE_UNKNOWN;
}

char* Underbar2Space(char *string)
{
	int len = strlen(string);
	for (int i = 0; i < len && i < 128; i++)
	{
		if (string[i] == '_')
		{
			string[i] = ' ';
		}
	}

	return string;
}

bool operator<(const VersionInfo &v1, const VersionInfo &v2)
{
	if (v1.GetVersion()[0] != v2.GetVersion()[0])
	{
		return v1.GetVersion()[0] < v2.GetVersion()[0];
	}
	if (v1.GetVersion()[1] != v2.GetVersion()[1])
	{
		return v1.GetVersion()[1] < v2.GetVersion()[1];
	}
	if (v1.GetVersion()[2] != v2.GetVersion()[2])
	{
		return v1.GetVersion()[2] < v2.GetVersion()[2];
	}
	if (v1.GetVersion()[3] != v2.GetVersion()[3])
	{
		return v1.GetVersion()[3] < v2.GetVersion()[3];
	}
	
	return FALSE;
}

void PrintExchangeMsg(BYTE SendOrRecv, MessageType_t nType, char *peerIP, ENServerType st, BYTE printLevel)
{
#ifdef _DEBUG
	int bPrint = FALSE;

/*	if (HIBYTE(nType) == T0_FC_BATTLE)
	{
		// 무조건 찍기!
		bPrint = TRUE;
	} else*/
	if (printLevel == PRINTLEVEL_NO_MSG)
	{
		return;
	}
	else if (printLevel == PRINTLEVEL_ALL)
	{
		bPrint = TRUE;
	}
	else if (printLevel == PRINTLEVEL_WO_ALIVE)
	{
		if (IS_ALIVE_MSG(nType))
		{
			return;
		}

		bPrint = TRUE;
	}
	else if (printLevel == PRINTLEVEL_WO_MOVE_BATTLE &&
		HIBYTE(nType) != T0_FC_MOVE &&
		HIBYTE(nType) != T0_FN_MOVE &&
		HIBYTE(nType) != T0_FN_MONSTER &&
		HIBYTE(nType) != T0_FC_BATTLE &&
		HIBYTE(nType) != T0_FN_BATTLE &&
		nType != T_FC_CHARACTER_CHANGE_CURRENTHPDPSPEP &&
		nType != T_FC_CHARACTER_CHANGE_BODYCONDITION &&
		nType != T_FN_MONSTER_CHANGE_HP &&
		nType != T_FC_STRING_128
	)
	{
		bPrint = TRUE;
	}
	else if (printLevel == PRINTLEVEL_A_FEW &&
//		HIBYTE(nType) == T0_FC_TRADE
//*
		(HIBYTE(nType) == T0_FC_PARTY ||
		HIBYTE(nType) == T0_IC_PARTY ||
		HIBYTE(nType) == T0_FI_PARTY)
		&&
		!(nType >= T_FC_PARTY_UPDATE_MEMBER_INFO_ALL &&
		nType <= T_FC_PARTY_UPDATE_MEMBER_INFO_BODYCONDITION)
//*/
	)
	{
		// PARTY 관련만 찍기
		bPrint = TRUE;
	}
	else if (printLevel == PRINTLEVEL_FEW &&
		HIBYTE(nType) == T0_FI_ADMIN
	)
	{
		// ADMIN 만 찍기
		bPrint = TRUE;
	}
	else
	{
		return;
	}

	// bPrint 확인하기
	if (!bPrint) return;

	char szTime[128];
	time_t ltime;
	struct tm *today = NULL;
	const char *st_string;

	st_string = GGetENServerTypeString(st);

	if (SendOrRecv == RECV_TYPE)
	{
		time(&ltime);
		today = localtime(&ltime);
		strftime(szTime, 128, "[%m-%d %H:%M:%S]", today );
		DBGOUT("%s RECV TYPE: %s(%#04x) from %s[%s]\n", szTime, GetProtocolTypeString(nType), nType, st_string, peerIP);
	}
	else if (SendOrRecv == SEND_TYPE)
	{
		time(&ltime);
		today = localtime(&ltime);
		strftime(szTime, 128, "[%m-%d %H:%M:%S]", today );
		DBGOUT("%s SEND TYPE: %s(%#04x) to %s[%s]\n", szTime, GetProtocolTypeString(nType), nType, st_string, peerIP);
	}
#endif

}

BYTE* GetIPAddressInBytes(char *i_szIPAddress, BYTE *o_pIPArray)
{
	char *token;
	char seps[] = ".";
	char buffer[SIZE_MAX_VERSION_STRING];
	STRNCPY_MEMSET(buffer, i_szIPAddress, SIZE_MAX_VERSION_STRING);

	token = strtok(buffer, seps); o_pIPArray[0] = (BYTE)atoi(token);
	token = strtok(NULL, seps); o_pIPArray[1] = (BYTE)atoi(token);
	token = strtok(NULL, seps); o_pIPArray[2] = (BYTE)atoi(token);
	token = strtok(NULL, seps); o_pIPArray[3] = (BYTE)atoi(token);

	return o_pIPArray;
}

const char *GetIPAddressString(BYTE *i_pIPArray, STRNBUF &i_strnbuf)
{
	sprintf(i_strnbuf.GetBuffer(), "%d.%d.%d.%d", i_pIPArray[0], i_pIPArray[1], i_pIPArray[2], i_pIPArray[3]);
	return i_strnbuf.GetBuffer();
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			char *GetMonsterBellString(BYTE i_byMonBell)
/// \brief		
/// \author		cmkwon
/// \date		2006-12-13 ~ 2006-12-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *GetMonsterBellString(BYTE i_byMonBell)
{
	switch(i_byMonBell)
	{
	case BELL_ATATTACK:						return "BELL_ATATTACK";
	case BELL_DEFENSE:						return "BELL_DEFENSE";
	case BELL_RETREAT:						return "BELL_RETREAT";
	case BELL_TAGETATATTACK:				return "BELL_TAGETATATTACK";
	case BELL_NOTATTACK:					return "BELL_NOTATTACK";
	case BELL_ATTACK_OUTPOST_PROTECTOR:		return "BELL_ATTACK_OUTPOST_PROTECTOR";

	case BELL_INFLUENCE_VCN:				return "BELL_INFLUENCE_VCN";
	case BELL_INFLUENCE_ANI:				return "BELL_INFLUENCE_ANI";
	case BELL_STRATEGYPOINT_VCN:			return "BELL_STRATEGYPOINT_VCN";
	case BELL_STRATEGYPOINT_ANI:			return "BELL_STRATEGYPOINT_ANI";
	case BELL_OUTPOST_PROTECTOR_VCN:		return "BELL_OUTPOST_PROTECTOR_VCN";
	case BELL_OUTPOST_PROTECTOR_ANI:		return "BELL_OUTPOST_PROTECTOR_ANI";
	case BELL_OUTPOST_RESET_VCN:			return "BELL_OUTPOST_RESET_VCN";
	case BELL_OUTPOST_RESET_ANI:			return "BELL_OUTPOST_RESET_ANI";
	case BELL_ONEY_ATTACK_INFLUENCE_VCN:	return "BELL_ONEY_ATTACK_INFLUENCE_VCN";
	case BELL_ONEY_ATTACK_INFLUENCE_ANI:	return "BELL_ONEY_ATTACK_INFLUENCE_ANI";
	}
	
	return "UNKNOWN_BELL";
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			const char *GetItemDesParameter(BYTE i_byDesParameter)
/// \brief		
/// \author		cmkwon
/// \date		2005-11-25 ~ 2005-11-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
const char *GetItemDesParameter(DestParam_t i_byDesParameter)		// 2011-08-01 by hskim, 파트너 시스템 2차 - 자료형 변경 (DestParameter - 255 -> 32767 지원)
{
	switch(i_byDesParameter)
	{
	GET_CASERETURN_STRING_BY_TYPE(DES_NULL);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACK_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_DEFENSE_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_FUEL_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_SOUL_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_SHIELD_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_DODGE_PART);
	GET_CASERETURN_STRING_BY_TYPE(DES_BODYCONDITION);
	GET_CASERETURN_STRING_BY_TYPE(DES_ENDURANCE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_ENDURANCE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_CHARGING_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_CHARGING_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_PROPENSITY);
	GET_CASERETURN_STRING_BY_TYPE(DES_HP);
	GET_CASERETURN_STRING_BY_TYPE(DES_DP);
	GET_CASERETURN_STRING_BY_TYPE(DES_SP);
	GET_CASERETURN_STRING_BY_TYPE(DES_EP);
	GET_CASERETURN_STRING_BY_TYPE(DES_SPRECOVERY);
	GET_CASERETURN_STRING_BY_TYPE(DES_HPRECOVERY);
	GET_CASERETURN_STRING_BY_TYPE(DES_MINATTACK_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_MAXATTACK_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_MINATTACK_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_MAXATTACK_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACKPROBABILITY_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACKPROBABILITY_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_DEFENSE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_DEFENSE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_DEFENSEPROBABILITY_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_DEFENSEPROBABILITY_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILLPROBABILITY_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILLPROBABILITY_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTION_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTION_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTIONRESISTANCE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTIONRESISTANCE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_SPEED);
	GET_CASERETURN_STRING_BY_TYPE(DES_TRANSPORT);
	GET_CASERETURN_STRING_BY_TYPE(DES_MATERIAL);
	GET_CASERETURN_STRING_BY_TYPE(DES_REATTACKTIME_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_REATTACKTIME_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_ABRASIONRATE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_ABRASIONRATE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_RANGE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_RANGE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_RANGEANGLE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_RANGEANGLE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_MULTITAGET_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_MULTITAGET_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_EXPLOSIONRANGE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_EXPLOSIONRANGE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_UNIT);
	GET_CASERETURN_STRING_BY_TYPE(DES_REVISION);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTIONPROBABILITY_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_FACTIONPROBABILITY_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_SHOTNUM_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_SHOTNUM_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_MULTINUM_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_MULTINUM_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACKTIME_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACKTIME_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_TIME_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_TIME_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_OVERHITTIME_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_OVERHITTIME_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_UNITKIND);
	GET_CASERETURN_STRING_BY_TYPE(DES_ITEMKIND);
	GET_CASERETURN_STRING_BY_TYPE(DES_SUMMON);
	GET_CASERETURN_STRING_BY_TYPE(DES_GRADUAL_HP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_GRADUAL_DP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_GRADUAL_SP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_GRADUAL_EP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IN_TIME_HP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IN_TIME_DP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IN_TIME_SP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IN_TIME_EP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IMMEDIATE_HP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IMMEDIATE_DP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IMMEDIATE_SP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_IMMEDIATE_EP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_GROUNDMODE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SIEGEMODE);
	GET_CASERETURN_STRING_BY_TYPE(DES_WEIGHT_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_WEIGHT_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_BULLET_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_BULLET_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_PRIMARY_WEAPON);
	GET_CASERETURN_STRING_BY_TYPE(DES_SECONDARY_WEAPON);
	GET_CASERETURN_STRING_BY_TYPE(DES_ALL_WEAPON);
	GET_CASERETURN_STRING_BY_TYPE(DES_CRITICALHITRATE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_CRITICALHITRATE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_WARP);
	GET_CASERETURN_STRING_BY_TYPE(DES_REACTION_RANGE);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_NONE);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_PREFIX);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_SUFFIX);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_BOTH);
	GET_CASERETURN_STRING_BY_TYPE(DES_REQ_MIN_LEVEL);
	GET_CASERETURN_STRING_BY_TYPE(DES_REQ_MAX_LEVEL);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_STAT_ALL_INITIALIZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_STAT_PART_INITIALIZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_PREFIX_INITIALIZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_FIX_SUFFIX_INITIALIZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_ENCHANT_INITIALIZE);				// 2007-04-03 by cmkwon

	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_STEALTH);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_HP_AND_DP_UP);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_GUILD_ALL_MEMBERS_SUMMON);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_GUILD_MEMBER_SUMMON);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_NORMAL_RESTORE);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_SUPER_RESTORE);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_GUILD);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_MONSTER_SUMMON);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_CHANGE_CHARACTERNAME);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_SKILL_INITIALIZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_CHANGE_PILOTFACE);
	GET_CASERETURN_STRING_BY_TYPE(DES_CASH_CRACKER_CARD);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_REDUCE_SHIELD_DAMAGE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_INVINCIBLE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_BIG_BOOM);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_HALLUCINATION);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_RANGEDOWN_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_RANGEDOWN_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_PROHIBITION_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_PROHIBITION_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACK_RANGE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_ATTACK_RANGE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_INVISIBLE);
// 2005-12-02 by cmkwon	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_HYPERMOVING);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_DEFENSE_01);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_DEFENSE_02);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_FREEZINGBODY);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_REVERSECONTROL);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_GRADUAL_HPSHIELD_DOWN);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_SLOWMOVING);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_BOOSTEROFF);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_COLLISIONDAMAGE_DOWN);
//	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_CAMOUFLAGE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_RANDOMTELEPORT);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_ATTACK_ALL);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_SCANNING);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_REVERSEENGINE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_LOCKDOWN);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_STEALSP);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_SMARTSP);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_SUMMON_FORMATION_MEMBER);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_CANCEL_MAGIC);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_RANDOM_CANCEL);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_STOPMAGIC);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_CANCELALL);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_REACTIONSPEED);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_ENGINEANGLE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_ENGINEBOOSTERANGLE);
	GET_CASERETURN_STRING_BY_TYPE(DES_DROP_EXP);
	GET_CASERETURN_STRING_BY_TYPE(DES_DROP_SPI);
	GET_CASERETURN_STRING_BY_TYPE(DES_DROP_ITEM);
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_ITEM_DROP_RATE);	// 238,2010-11-30 by shcho, 레어아이템 드랍 확률 증가 아이템 구현
	GET_CASERETURN_STRING_BY_TYPE(DES_HP_REPAIR_RATE_FLIGHTING);
	GET_CASERETURN_STRING_BY_TYPE(DES_DP_REPAIR_RATE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SP_REPAIR_RATE);
	GET_CASERETURN_STRING_BY_TYPE(DES_BAZAAR_SELL);
	GET_CASERETURN_STRING_BY_TYPE(DES_BAZAAR_BUY);
	GET_CASERETURN_STRING_BY_TYPE(DES_KILLMARK_EXP);
	GET_CASERETURN_STRING_BY_TYPE(DES_HYPER_BOOSTER);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_CHAFF_HP);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_AIR_BOMBING);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_NO_WARNING);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_ROLLING_TIME);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_FULL_RECOVERY);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_CAMOUFLAGE);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_BARRIER);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_HYPERSHOT);
	GET_CASERETURN_STRING_BY_TYPE(DES_SKILL_SHIELD_PARALYZE);
	GET_CASERETURN_STRING_BY_TYPE(DES_WARHEAD_SPEED);			// 176, 2007-06-11 by cmkwon, 탄두의 속도
	GET_CASERETURN_STRING_BY_TYPE(DES_CHAT_ALL_INFLUENCE);		// 177		// 2007-08-09 by cmkwon, 모든 세력에 채팅 전송하기 - desparam 추가, 유료아이템
	// 2008-09-22 by dhjin, 신규 인첸트
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_BOOSTER_TIME_UP);		// 178		// 2008-09-22 by dhjin, 부스터 시간 증가
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_MAX_SPEED_UP);			// 179		// 2008-09-22 by dhjin, 엔진 일반속도(최대) 증가
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_MIN_SPEED_UP);			// 180		// 2008-09-22 by dhjin, 엔진 일반속도(최소) 증가
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_BOOSTER_SPEED_UP);		// 181		// 2008-09-22 by dhjin, 엔진 부스터속도 증가
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_GROUND_SPEED_UP);		// 182		// 2008-09-22 by dhjin, 엔진 지상속도 증가
	GET_CASERETURN_STRING_BY_TYPE(DES_RADAR_OBJECT_DETECT_RANGE);	// 183		// 2008-09-22 by dhjin, 레이더 물체 감지 반경
	GET_CASERETURN_STRING_BY_TYPE(DES_PIERCE_UP_01);				// 184		// 2008-09-22 by dhjin, 기본무기 피어스율 증가 카드
	GET_CASERETURN_STRING_BY_TYPE(DES_PIERCE_UP_02);				// 185		// 2008-09-22 by dhjin, 고급무기 피어스율 증가 카드
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_ANGLE_UP);				// 186		// 2008-09-30 by dhjin, 엔진 회전각 증가 카드
	GET_CASERETURN_STRING_BY_TYPE(DES_ENGINE_BOOSTERANGLE_UP);		// 187		// 2008-09-30 by dhjin, 엔진 부스터 회전각 증가 카드
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_ITEM_PARTNER_SPEED);		// 239,		// 2010-12-21 by jskim, 마을 이동 속도 증가 아이템 구현
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_ITEM_PARTNER_DAMAGE);	// 240,		// 2010-12-21 by jskim, 파트너 데미지 증가 아이템 구현
	GET_CASERETURN_STRING_BY_TYPE(DES_RARE_ITEM_HPDP);				// 241,		// 2010-12-21 by jskim, HP, DP 키트 적용량 증가 아이템 구현
	GET_CASERETURN_STRING_BY_TYPE(DES_SUMMON_POSITION_X);			// 242,		// 2011-05-02 by hskim, 인피니티 3차 - 소환시 위치 변경 (상대값)
	GET_CASERETURN_STRING_BY_TYPE(DES_SUMMON_POSITION_Y);			// 243,		// 2011-05-02 by hskim, 인피니티 3차 - 소환시 위치 변경 (상대값)
	GET_CASERETURN_STRING_BY_TYPE(DES_SUMMON_POSITION_Z);			// 244,		// 2011-05-02 by hskim, 인피니티 3차 - 소환시 위치 변경 (상대값)

	GET_CASERETURN_STRING_BY_TYPE(DES_MAPBUFF_RANDOM_ADD_REATTACKTIME);		// 245,		// 2011-09-01 by hskim, EP4 [트리거 시스템] - 화산재 / 모래 폭풍 - 맵버프 추가 시간 적용
	GET_CASERETURN_STRING_BY_TYPE(DES_MAPBUFF_RANDOM_ADD_TIME);				// 246,		// 2011-09-01 by hskim, EP4 [트리거 시스템] - 화산재 / 모래 폭풍 - 맵버프 추가 시간 적용
	GET_CASERETURN_STRING_BY_TYPE(DES_ENCHANT_ONCE_APPLY);					// 247,		// 2011-09-01 by hskim, EP4 [Free to play] - 10회 인첸트 아이템 기능 구현
	GET_CASERETURN_STRING_BY_TYPE(DES_GAMBLE_RARE_ITEM_FIX);				// 248,		// 2011-09-01 by hskim, EP4 [Free to play] - 접두/접미 고정 옵션

	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SOCKET_ITEM_AUTOKIT);		// 23000,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 파트너 시스템 소켓류 - 자동 키트
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SOCKET_ITEM_AUTOSKILL);	// 23001,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 파트너 시스템 소켓류 - 자동 스킬

	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOKIT_HP);			// 239000,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 키트 HP
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOKIT_SHIELD);		// 239001,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 키트 Shield
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOKIT_SP);			// 239002,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 키트 SP

	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOSKILL_AGEAR);		// 239010,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 스킬 A 기어
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOSKILL_BGEAR);		// 239011,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 스킬 B 기어
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOSKILL_IGEAR);		// 239012,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 스킬 I 기어
	GET_CASERETURN_STRING_BY_TYPE(DES_PET_SLOT_ITEM_AUTOSKILL_MGEAR);		// 239013,	// 2011-09-01 by hskim, 파트너 시스템 2차 - 자동 스킬 M 기어
	}

	return "UNKNOWN_DES_PARAMETER";
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL GIsValidInfuenceType(BYTE i_byInfluenceTy)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-03 ~ 2005-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL GIsValidInfuenceType(BYTE i_byInfluenceTy)
{
	switch(i_byInfluenceTy)
	{
	case INFLUENCE_TYPE_NORMAL:
	case INFLUENCE_TYPE_VCN:
	case INFLUENCE_TYPE_ANI:
	case INFLUENCE_TYPE_RRP:
		break;
	default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			char *GGetLowerCase(char *o_szLowercaseSting, char *i_szString, int nMaxStringSize)
/// \brief		
/// \author		cmkwon
/// \date		2006-05-22 ~ 2006-05-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *GGetLowerCase(char *o_szLowercaseSting, char *i_szString, int nMaxStringSize)
{
	STRNCPY_MEMSET(o_szLowercaseSting, i_szString, nMaxStringSize);
	_strlwr(o_szLowercaseSting);
	return o_szLowercaseSting;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			int GDelimiterIndex(char *i_szStr, int i_nMinIndex)
/// \brief		
/// \author		cmkwon
/// \date		2006-04-12 ~ 2006-04-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int GDelimiterIndex(char *i_szStr, int i_nMaxCount)
{
	if(i_nMaxCount >= strlen(i_szStr))
	{
		return strlen(i_szStr);
	}

	char *pBeforeStr = i_szStr;
	char *pCurStr = CharNext(pBeforeStr);
	while(pCurStr)
	{
		pBeforeStr	= pCurStr;
		pCurStr		= CharNext(pCurStr);
		int nCurCount = pCurStr - i_szStr;
		if(nCurCount == i_nMaxCount)
		{
			return nCurCount;
		}
		else if(nCurCount > i_nMaxCount)
		{
			return pBeforeStr - i_szStr;
		}
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 2006-10-02 by cmkwon
int		g_nargvCount;
char	g_szArrargv[SIZE_MAX_ARGV_COUNT][SIZE_MAX_ARGV_PARAM_STRING_SIZE];

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GSetExcuteParameterList(int i_nParamCount, char ** i_ppszParamString)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-02 ~ 2006-10-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GSetExcuteParameterList(int i_nParamCount, char ** i_ppszParamString)
{
	if(SIZE_MAX_ARGV_COUNT < i_nParamCount)
	{
		return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
	}

	g_nargvCount			= i_nParamCount;
	int i; for(i=0; i < g_nargvCount; i++)
	{
		STRNCPY_MEMSET(g_szArrargv[i], i_ppszParamString[i], SIZE_MAX_ARGV_PARAM_STRING_SIZE);
	}

	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-02 ~ 2006-10-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList(SEXCUTE_PARAMETER *io_pExeParam)
{
#ifdef _DEBUG
	return ERR_NO_ERROR;
#endif

	// 2008-04-25 by cmkwon, 지원 언어/서비스 추가시 꼭 추가 되어야 하는 사항 - [서비스-필수] C_Exe1, C_Exe2 실행시 실행 파라미터 처리 함수 추가

#ifdef SERVICE_TYPE_KOREAN_SERVER_1
	return GCheckExcuteParameterList_Kor(io_pExeParam);
#endif

#ifdef SERVICE_TYPE_KOREAN_SERVER_2
	return GCheckExcuteParameterList_Kor2(io_pExeParam);
#endif

#ifdef SERVICE_TYPE_ENGLISH_SERVER_1
	return GCheckExcuteParameterList_Eng(io_pExeParam);
#endif

// 2007-12-21 by cmkwon, Gameforge(Europe) 프로젝트 추가 - 추가함
// 2008-12-22 by cmkwon, 지원 서비스 추가(Gameforge4D 터키아, 불어, 이탈리아어) - 아래와 같이 3개 언어 추가
//#if defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1)		// 2008-04-25 by cmkwon, Gameforge4D_Deu 추가함 - 
// 2009-06-04 by cmkwon, 지원 서비스 추가(Gameforge4D 폴란드어, 스페인어) - 
//#if defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1)
// 2010-11-01 by shcho,	 지원 서비스 추가(Gameforge4D 스페인어, 아르헨티나어) -  
//#if defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1)
#if defined(SERVICE_TYPE_ENGLISH_SERVER_2) || defined(SERVICE_TYPE_GERMAN_SERVER_1) || defined(SERVICE_TYPE_TURKISH_SERVER_1) || defined(SERVICE_TYPE_FRENCH_SERVER_1) || defined(SERVICE_TYPE_ITALIAN_SERVER_1) || defined(SERVICE_TYPE_POLISH_SERVER_1) || defined(SERVICE_TYPE_SPANISH_SERVER_1) || defined(SERVICE_TYPE_ARGENTINA_SERVER_1)
	return GCheckExcuteParameterList_Eng_Server_2(io_pExeParam);
#endif

#ifdef SERVICE_TYPE_VIETNAMESE_SERVER_1
	return GCheckExcuteParameterList_Viet(io_pExeParam);
#endif

	// 2007-05-15 by cmkwon, 중국 서버	
#ifdef SERVICE_TYPE_CHINESE_SERVER_1
	return GCheckExcuteParameterList_Chn(io_pExeParam);
#endif

// 2008-05-02 by cmkwon, WinnerOnline_Tha 추가하기 - 
#if defined(SERVICE_TYPE_THAI_SERVER_1) || defined(SERVICE_TYPE_SINGAPORE_1) || defined(SERVICE_TYPE_INDONESIA_SERVER_1)
	return GCheckExcuteParameterList_Tha(io_pExeParam);
#endif

// 2008-05-29 by cmkwon, 지원 서비스 추가(Innova_Rus 러시아어 추가) - 
#ifdef SERVICE_TYPE_RUSSIAN_SERVER_1
	return GCheckExcuteParameterList_Rus(io_pExeParam);
#endif

// 2008-09-23 by cmkwon, 지원 서비스 추가(대만 Netpower_Tpe) - 
#ifdef SERVICE_TYPE_TAIWANESE_SERVER_1
	return GCheckExcuteParameterList_Tpe(io_pExeParam);
#endif

// 2008-12-03 by cmkwon, 지원 서비스 추가(일본 Arario_Jpn) - 
#ifdef SERVICE_TYPE_JAPANESE_SERVER_1
	return GCheckExcuteParameterList_Jpn(io_pExeParam);
#endif
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Kor(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-09 ~ 2006-10-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Kor(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:

		// 2012-07-11 by hskim, 선택적 셧다운
		if(1 != g_nargvCount && 2 != g_nargvCount)
		{
			return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
		}

		if( 1 == g_nargvCount )
		{
			g_nargvCount++;
			STRNCPY_MEMSET(g_szArrargv[1], STR_DEFAULT_VALUE_SELECTIVE_SHUTDOWN_INFO, SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);
		}

		STRNCPY_MEMSET(io_pExeParam->o_szSelectiveShutdownInfo, g_szArrargv[1], SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);
		break;
		// 2012-07-11 by hskim, 선택적 셧다운

	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]: char[24]	// 2012-07-11 by hskim, 선택적 셧다운
			// argv[2]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount && 3 != g_nargvCount)		// 2012-07-11 by hskim, 선택적 셧다운
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}

			// 2012-07-11 by hskim, 선택적 셧다운
			if(3 == g_nargvCount)
			{
				STRNCPY_MEMSET(io_pExeParam->o_szSelectiveShutdownInfo, g_szArrargv[1], SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[2], SIZE_MAX_IPADDRESS);				
			}
			// end 2012-07-11 by hskim, 선택적 셧다운
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Kor2(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-09 ~ 2006-10-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Kor2(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[20]	AccountName
			// argv[2]:	char[33]	MD5PWD
			// argv[3]: int			Seed			[0~
			// argv[4]:	int			ExcuteType
			// argv[5]:	char[21]	2007-03-29 by cmkwon, Yedang 인증 Key
			// argv[6]:	char[20]	2007-06-05 by cmkwon, BirthYear - 출생년도
			// argv[7]: char[24]	// 2012-07-11 by hskim, 선택적 셧다운
			///////////////////////////////////////////////////////////////////////////////			
			if(7 != g_nargvCount && 8 != g_nargvCount)		// 2012-07-11 by hskim, 선택적 셧다운
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			// 2012-07-11 by hskim, 선택적 셧다운
			if( 7 == g_nargvCount )
			{
				g_nargvCount++;
				STRNCPY_MEMSET(g_szArrargv[7], STR_DEFAULT_VALUE_SELECTIVE_SHUTDOWN_INFO, SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);
			}

			STRNCPY_MEMSET(io_pExeParam->o_szSelectiveShutdownInfo, g_szArrargv[7], SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);
			// end 2012-07-11 by hskim, 선택적 셧다운

			// argv[1]
			_strlwr(g_szArrargv[1]);				// 2006-10-18 by cmkwon, 소문자로 변경
			STRNCPY_MEMSET(io_pExeParam->o_szAccountName0, g_szArrargv[1], SIZE_MAX_ACCOUNT_NAME);
			
			// argv[2], argv[3]
			char szEncryptMutexName[1024];
			sprintf(szEncryptMutexName, "%s%s", g_szArrargv[2], g_szArrargv[3]);
			HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, TRUE, szEncryptMutexName);
			if(hMutex == NULL)
			{		
				return ERR_INVALID_EXCUTE_PARAMETER;
			}	
			CloseHandle(hMutex);		// 2006-10-19 by cmkwon, 함수 변경(ReleaseMutex-->CloseHandle)

		}
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[20]	AccountName
			// argv[2]:	char[33]	MD5PWD
			// argv[3]: int			Seed			[0~
			// argv[4]:	int			ExcuteType
			// argv[5]:	char[21]	2007-03-29 by cmkwon, Yedang 인증 Key
			// argv[6]:	char[20]	2007-06-05 by cmkwon, BirthYear - 출생년도
			// argv[7]: char[24]	// 2012-07-11 by hskim, 선택적 셧다운
			// argv[8]: char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(8 != g_nargvCount && 9 != g_nargvCount)		// 2012-07-11 by hskim, 선택적 셧다운
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			// argv[1]
			_strlwr(g_szArrargv[1]);				// 2006-10-18 by cmkwon, 소문자로 변경
			STRNCPY_MEMSET(io_pExeParam->o_szAccountName0, g_szArrargv[1], SIZE_MAX_ACCOUNT_NAME);
			
			STRNCPY_MEMSET(io_pExeParam->o_szSelectiveShutdownInfo, g_szArrargv[7], SIZE_MAX_SELECTIVE_SHUTDOWN_INFO);		// 2012-07-11 by hskim, 선택적 셧다운

			// argv[2], argv[3]
			char szEncryptMutexName[1024];
			sprintf(szEncryptMutexName, "%s%s", g_szArrargv[2], g_szArrargv[3]);
			HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, TRUE, szEncryptMutexName);
			if(hMutex == NULL)
			{		
				return ERR_INVALID_EXCUTE_PARAMETER;
			}	
			CloseHandle(hMutex);		// 2006-10-19 by cmkwon, 함수 변경(ReleaseMutex-->CloseHandle)

			if(9 == g_nargvCount)		// 2012-07-11 by hskim, 선택적 셧다운
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[8], SIZE_MAX_IPADDRESS);		// 2012-07-11 by hskim, 선택적 셧다운
			}

			///////////////////////////////////////////////////////////////////////////////
			// 2007-06-05 by cmkwon, 출생년도를 넘긴다
			io_pExeParam->o_nBirthYear	= atoi(g_szArrargv[6]);
		}
		break;
	case EXCUTE_FILE_TYPE_SC_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-09 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]: char[16]	FieldServer IP
			// argv[2]: int			FieldServer Port
			// argv[3]: char[16]	IMServer IP
			// argv[4]: int			IMServer Port
			// argv[5]: Width		해상도 폭
			// argv[6]: Height		해상도 높이
			// argv[7]: Degree		사양 깊이 (0 저사양, 1 중사양 , 2 고사양, 3 충돌처리(유닛))
			// argv[8]:	char[20]	AccountName
			// argv[9]:	char[33]	MD5PWD
			// argv[10]:int			Seed			[0~
			// argv[11]:int			ExcuteType
			// argv[12]:char[20]	Reserve
			// argv[13]:char[20]	Reserve
			///////////////////////////////////////////////////////////////////////////////			
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Eng(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-09 ~ 2006-10-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Eng(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Eng_Server_2(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		// 2007-12-21 by cmkwon, Gameforge(Europe) 프로젝트 추가 - GCheckExcuteParameterList_Eng_Server_2() 추가
/// \author		cmkwon
/// \date		2007-12-21 ~ 2007-12-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Eng_Server_2(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Viet(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-09 ~ 2006-10-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Viet(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Chn(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		
/// \author		cmkwon
/// \date		2006-10-09 ~ 2006-10-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Chn(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}

			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Tha(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		// 2008-05-02 by cmkwon, WinnerOnline_Tha 추가하기 - GCheckExcuteParameterList_Tha() 추가
/// \author		cmkwon
/// \date		2008-05-02 ~ 2005-05-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Tha(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}
			
			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Rus(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		// 2008-05-29 by cmkwon, 지원 서비스 추가(Innova_Rus 러시아어 추가) - GCheckExcuteParameterList_Rus() 추가
/// \author		cmkwon
/// \date		2008-05-29 ~ 2005-05-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Rus(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}
			
			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Tpe(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		// 2008-09-23 by cmkwon, 지원 서비스 추가(대만 Netpower_Tpe) - 
/// \author		cmkwon
/// \date		2008-09-23 ~ 2008-09-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Tpe(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2006-10-02 by cmkwon
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 2 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}
			
			if(2 == g_nargvCount)
			{// 2007-05-15 by cmkwon, PreServer IP가 인자리스트에 있다
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[1], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	}
	return ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			Err_t GCheckExcuteParameterList_Jpn(SEXCUTE_PARAMETER *io_pExeParam)
/// \brief		// 2008-12-03 by cmkwon, 지원 서비스 추가(일본 Arario_Jpn) - 
/// \author		cmkwon
/// \date		2008-12-03 ~ 2008-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
Err_t GCheckExcuteParameterList_Jpn(SEXCUTE_PARAMETER *io_pExeParam)
{
	switch(io_pExeParam->i_nExcuteFileType)
	{
	case EXCUTE_FILE_TYPE_SC_EXE:	// ACEonline.atm
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2008-12-18 by cmkwon, 일본 Arario 런처 수정 - 
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[20]	AccountName
			// argv[2]:	char[33]	Password
			// argv[3]:	char[301]	Session Key
			///////////////////////////////////////////////////////////////////////////////			
			if(4 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}
			
			STRNCPY_MEMSET(io_pExeParam->o_szAccountName0, g_szArrargv[1], SIZE_MAX_ACCOUNT_NAME);
			STRNCPY_MEMSET(io_pExeParam->o_szPassword, g_szArrargv[2], SIZE_MAX_PASSWORD_MD5_STRING);
			STRNCPY_MEMSET(io_pExeParam->o_szSessionKey, g_szArrargv[3], SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);
		}
		break;
	case EXCUTE_FILE_TYPE_SC_LAUNCHER_ATM:	// Launcher.atm
		{
			///////////////////////////////////////////////////////////////////////////////
			// 2008-12-18 by cmkwon, 일본 Arario 런처 수정 - 
			// argv[0]: char[??]	실행 파일 이름
			// argv[1]:	char[20]	AccountName
			// argv[2]:	char[33]	Password
			// argv[3]:	char[301]	Session Key
			// argv[4]:	char[16]	2007-05-15 by cmkwon, PreServer IP
			///////////////////////////////////////////////////////////////////////////////			
			if(1 != g_nargvCount && 5 != g_nargvCount)
			{
				return ERR_INVALID_EXCUTE_PARAMETER_COUNT;
			}
			
			if(5 == g_nargvCount)
			{
				STRNCPY_MEMSET(io_pExeParam->o_szAccountName0, g_szArrargv[1], SIZE_MAX_ACCOUNT_NAME);
				STRNCPY_MEMSET(io_pExeParam->o_szPassword, g_szArrargv[2], SIZE_MAX_PASSWORD_MD5_STRING);
				STRNCPY_MEMSET(io_pExeParam->o_szSessionKey, g_szArrargv[3], SIZE_MAX_WEBLOGIN_AUTHENTICATION_KEY);
				STRNCPY_MEMSET(io_pExeParam->o_szPreServerIP0, g_szArrargv[4], SIZE_MAX_IPADDRESS);
			}
		}
		break;
	case EXCUTE_FILE_TYPE_SC_ATM:
		break;
	}
	return ERR_NO_ERROR;
}

Err_t GReadVersionInfoFile(VersionInfo *o_pLauncherVerInfo, SREG_DATA_EXE_2 *o_pRegDataExe2, char *i_szVersionInfoFileName)
{
	o_pLauncherVerInfo->SetVersion(0,0,0,0);
	o_pRegDataExe2->resetREG_DATA_EXE_2();


	const int	BUFF_SIZE = 1024;
	char		buff[BUFF_SIZE];
	const char	*token;
	char		seps[] = " \t";
	ifstream	fin;

	fin.open(i_szVersionInfoFileName);
	if (!fin.is_open())
	{
		return ERR_VERSIONINFO_FILE_NOT_FOUND;
	}

	while(TRUE)
	{
		memset(buff, 0x00, BUFF_SIZE);
		BOOL bFlagFileEnd = fin.getline(buff, BUFF_SIZE).eof();
		if (bFlagFileEnd == TRUE)
		{
			break;
		}

		token = strtok(buff, seps);
		if (token == NULL						// ignore blank lines
			|| strncmp(token, "#", 1) == 0		// ignore comments
			|| strncmp(token, "$", 1) == 0)		// ignore user defined variables
		{
			continue;
		}

		if (0 == stricmp(token, COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION))	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분	STRMSG_REG_KEY_NAME_LAUNCHER_VERSION->COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION
		{
			token = strtok(NULL, seps);
			if (token)
			{
				o_pLauncherVerInfo->SetVersion(token);
			}
		}
		else if (0 == stricmp(token, COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION))	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_CLIENT_VERSION->COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION  
		{
			char *token = strtok(NULL, seps);
			if (token)
			{
				o_pRegDataExe2->ClientVersion.SetVersion(token);
			}
		}
		else if (0 == stricmp(token, COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW))	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW
		{
// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 - 아래와 같이 수정
//			char *token = strtok(NULL, seps);
//			if (token)
//			{
//				o_pRegDataExe2->WindowDegree = atoi(token);
//			}
			char *token = strtok(NULL, "\t");
			if (token)
			{
				STRNCPY_MEMSET(o_pRegDataExe2->WindowDegree, token, SIZE_MAX_WINDOW_DEGREE_NAME);
			}
		}
		else if (0 == stricmp(token, COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE))	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_WINDOWMODE->COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE
		{// 2008-01-03 by cmkwon, 윈도우모드 상태 저장하기 - GReadVersionInfoFile() 에 처리 추가
			char *token = strtok(NULL, seps);
			if (token)
			{
				int nMode = atoi(token);
				if(0 != nMode)
				{
					o_pRegDataExe2->IsWindowMode	= TRUE;
				}
			}
		}
		else if (0 == stricmp(token, COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME))	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_ACCOUNT_NAME->COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME
		{
			char *token = strtok(NULL, seps);
			if (token)
			{
				STRNCPY_MEMSET(o_pRegDataExe2->BeforeAccountName, token, SIZE_MAX_ACCOUNT_NAME);
			}
		}
		else if (0 == stricmp(token, STRMSG_REG_KEY_NAME_SERVER_GROUP_NAME))
		{
			char *token = strtok(NULL, seps);
			if (token)
			{
				STRNCPY_MEMSET(o_pRegDataExe2->SelectedServerGroupName, token, SIZE_MAX_SERVER_NAME);
			}
		}
	}
	fin.close();

	return ERR_NO_ERROR;
}

Err_t GLoadExe1VersionInfo(VersionInfo *o_pLauncherVerInfo, char *i_szVersionInfoFileName)
{
	VersionInfo lver;
	SREG_DATA_EXE_2 regDataExe2;
	regDataExe2.resetREG_DATA_EXE_2();
	Err_t errCode = GReadVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
	if(errCode != ERR_NO_ERROR)
	{
		return errCode;
	}

	*o_pLauncherVerInfo = lver;
	return ERR_NO_ERROR;
}

Err_t GLoadExe2VersionInfo(SREG_DATA_EXE_2 *o_pRegDataEXE2, char *i_szVersionInfoFileName)
{
	VersionInfo lver;
	SREG_DATA_EXE_2 regDataExe2;
	regDataExe2.resetREG_DATA_EXE_2();
	Err_t errCode = GReadVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
	if(errCode != ERR_NO_ERROR)
	{
		return errCode;
	}

	*o_pRegDataEXE2		= regDataExe2;
	return ERR_NO_ERROR;
}


Err_t GWriteVersionInfoFile(VersionInfo *i_pLauncherVerInfo, SREG_DATA_EXE_2 *i_pRegDataEXE2, char *i_szVersionInfoFileName)
{
	// 지우고 새로 저장
	DeleteFile(i_szVersionInfoFileName);
	int nErr = GetLastError();
	SetLastError(0);
	
	// 파일 생성
	HANDLE hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile(i_szVersionInfoFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		int nErr = GetLastError();
		SetLastError(0);
		return ERR_VERSIONINFO_FILE_CAN_NOT_CREATE;
	}

	const int	BUFF_SIZE = 1024;
	char		buff[BUFF_SIZE];
	DWORD		nWritten;

	sprintf(buff, "###############################################################################\r\n");
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	sprintf(buff, "#  %s			0.0.0.0\r\n#\r\n", COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분	STRMSG_REG_KEY_NAME_LAUNCHER_VERSION->COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	sprintf(buff, "#  %s			0.0.0.0\r\n", COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION);		// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_CLIENT_VERSION->COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION  
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 - 아래와 같이 수정
//	sprintf(buff, "#  %s				0\r\n#\r\n", STRMSG_REG_KEY_NAME_WINDOWDEGREE);
	sprintf(buff, "#  %s			1024x768 (high)\r\n#\r\n", COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	
	// 2008-01-03 by cmkwon, 윈도우모드 상태 저장하기 - 
	sprintf(buff, "#  %s				0\r\n#\r\n", COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_WINDOWMODE->COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);

	sprintf(buff, "#  %s				Mir\r\n", COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_ACCOUNT_NAME->COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	sprintf(buff, "#  %s			ACE\r\n", STRMSG_REG_KEY_NAME_SERVER_GROUP_NAME);
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	sprintf(buff, "###############################################################################\r\n\r\n");
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);

	sprintf(buff, "%s\t\t\t%s\r\n\r\n", COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION, i_pLauncherVerInfo->GetVersionString());	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분	STRMSG_REG_KEY_NAME_LAUNCHER_VERSION->COMMON_STRMSG_REG_KEY_NAME_LAUNCHER_VERSION
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);

	sprintf(buff, "%s\t\t\t%s\r\n", COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION, i_pRegDataEXE2->ClientVersion.GetVersionString());		// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_CLIENT_VERSION->COMMON_STRMSG_REG_KEY_NAME_CLIENT_VERSION  
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
// 2007-12-27 by cmkwon, 윈도우즈 모드 기능 추가 -
//	sprintf(buff, "%s\t\t\t%d\r\n\r\n", STRMSG_REG_KEY_NAME_WINDOWDEGREE, i_pRegDataEXE2->WindowDegree);
	sprintf(buff, "%s\t\t\t%s\r\n\r\n", COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW, i_pRegDataEXE2->WindowDegree);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 COMMON_STRMSG_REG_KEY_NAME_WINDOWDEGREE_NEW
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	
	// 2008-01-03 by cmkwon, 윈도우모드 상태 저장하기 - 
	sprintf(buff, "%s\t\t\t\t%d\r\n\r\n", COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE, i_pRegDataEXE2->IsWindowMode);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_WINDOWMODE->COMMON_STRMSG_REG_KEY_NAME_WINDOWMODE
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);

	sprintf(buff, "%s\t\t\t\t%s\r\n", COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME, i_pRegDataEXE2->BeforeAccountName);	// 2013-10-15 by bckim, 국가별 공용으로 쓰는 스트링 구분 STRMSG_REG_KEY_NAME_ACCOUNT_NAME->COMMON_STRMSG_REG_KEY_NAME_ACCOUNT_NAME
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	sprintf(buff, "%s\t\t\t%s\r\n", STRMSG_REG_KEY_NAME_SERVER_GROUP_NAME, i_pRegDataEXE2->SelectedServerGroupName);
	WriteFile(hFile, buff, strlen(buff), &nWritten, NULL);
	
	CloseHandle(hFile);		// 파일 닫기
	return ERR_NO_ERROR;
}

Err_t GSaveExe1VersionInfo(VersionInfo *i_pLauncherVerInfo, char *i_szVersionInfoFileName)
{
	VersionInfo lver;
	SREG_DATA_EXE_2 regDataExe2;
	regDataExe2.resetREG_DATA_EXE_2();
	Err_t errCode = GReadVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
		
	lver = *i_pLauncherVerInfo;
	return GWriteVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
}

Err_t GSaveExe2VersionInfo(SREG_DATA_EXE_2 *i_pRegDataEXE2, char *i_szVersionInfoFileName)
{
	VersionInfo lver;
	SREG_DATA_EXE_2 regDataExe2;
	regDataExe2.resetREG_DATA_EXE_2();
	Err_t errCode = GReadVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
	
	regDataExe2 = *i_pRegDataEXE2;
	return GWriteVersionInfoFile(&lver, &regDataExe2, i_szVersionInfoFileName);
}


//
/////////////////////////////////////////////////////////////////////////
//// User-Friedly Map Name 할당 -> check: 시간나면 config로 옮기기!
/////////////////////////////////////////////////////////////////////////
//#define MAP_0101			"Stone's ruin"
//#define MAP_0102			"Tyrant of Giantjungle"
//#define MAP_0103			"Enigmatic sand"
//#define MAP_0104			"Steep Giantjungle"
//#define MAP_0105			"Land of Severe Cold"
//#define MAP_0106			"ATUM [Deca city]"
//#define MAP_NONAME			"무제"
//
//const char *GetUserFriendlyMapName(MapIndex_t MapIndex)
//{
//	if (MapIndex == 0101)
//	{
//		return MAP_0101;
//	}
//	else if (MapIndex == 0102)
//	{
//		return MAP_0102;
//	}
//	else if (MapIndex == 0103)
//	{
//		return MAP_0103;
//	}
//	else if (MapIndex == 0104)
//	{
//		return MAP_0104;
//	}
//	else if (MapIndex == 0105)
//	{
//		return MAP_0105;
//	}
//	else if (MapIndex == 0106)
//	{
//		return MAP_0106;
//	}
//	else
//	{
//		return MAP_NONAME;
//	}
//
//	return MAP_NONAME;
//}

///////////////////////////////////////////////////////////////////////////////
// 2008-04-25 by cmkwon, 지원 언어/서비스 추가시 꼭 추가 되어야 하는 사항 - [서비스-필수] AdminTool, DBTool, Monitor 에 보여질 서버군 리스트 정의

// 2006-01-09 by cmkwon, EP2를 위해서 DB명을 변경중
//						atum_db_account			--> atum2_db_account
//						atum_db_[n]				--> atum2_db_[n]
//						atum_db_account_test	--> atum2_db_account_test
//						atum_db_test			--> atum2_db_test
#ifndef _ATUM_CLIENT
GAME_SERVER_INFO_FOR_ADMIN g_arrGameServers[] = 
{
	// 2007-11-07 by cmkwon, 로그 DB 서버 따로 구축하기 - 필드 4개 추가, LogDBIP, LogDBPort, LogDBUID, LogDBPWD
	// {Name, XOREncodedPreServerIP, XOREncodedDBServerIP, DBServerPort, XOREncodedDBUserID, XOREncodedDBPWD, DB Name, LogDBIP, LogDBPort, LogDBUID, LogDBPWD, PreServerIP, DBServerIP, DBUserID, DBPWD}
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - Key
	// #define STR_XOR_KEY_STRING_SERVER_INFO			"~8xANs(^fP{)34$(fcbTN$(C-=x"	// 2007-10-24 by cmkwon, 서버 정보 암호화 - DB Server Password XOR Key

// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
//#ifdef _ATUM_ADMIN_RELEASE

#if defined(SERVICE_TYPE_KOREAN_SERVER_1)
// PreServerIP		: 115.144.35.140	- 4F094D6F7F471C70556555180704
// DBServerIP		: 115.144.35.140	- 4F094D6F7F471C70556555180704
// DBServerPort		: 9979
// DB User			: atum			- 1F4C0D2C
// DB PWD			: callweb		- 1D59142D39164A
	{"MS140 (Masang_Kor)_(atum2_db_1)",				"4F094D6F7F471C70556555180704", "4F094D6F7F471C70556555180704", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C70556555180704", 9979, "1F4C0D2C",	"1D59142D39164A"},

// 	PreServerIP		: 115.144.35.238	- 4F094D6F7F471C705565551B000C  -- 4F094D6F7F471C705565551B000C	
// 	DBServerIP		: 115.144.35.238	- 4F094D6F7F471C705565551B000C  -- 4F094D6F7F471C705565551B000C	
// 	DBServerPort		: 9979
// 	DB User			: atum			- 1F4C0D2C
// 	DB PWD			: callweb		- 1D59142D39164A
	{"MS238(Masang_Kor)_(atum2_db_account)",				"4F094D6F7F471C705565551B000C", "4F094D6F7F471C705565551B000C", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C705565551B000C", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{"MS600( bckim_22 )_(atum2_db_account)",				"4F094D6F7F471C705565551B01", "4F094D6F7F471C705565551B01", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C705565551B01", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{
		// 115.144.35.238	4F094D6F7F471C705565551B000C
		// 115.144.35.239	4F094D6F7F471C705565551B000D
		// 115.144.35.19	4F094D6F7F471C70556555180A
		/* Server Name	*/	"종헌 개인서버용",
		/* PreServer IP	*/	"4F094D6F7F471C70556555180A",
		//----------------------------------------------------------------//
		/* DBServer IP	*/	"4F094D6F7F471C70556555180A",
		/* DBServer Port*/	9979,
		/* DB User ID	*/	"1F4C0D2C",
		/* DB User PW	*/	"1D59142D39164A",
		/* DB Name		*/	"atum2_db_1",
		//----------------------------------------------------------------//
		/* LogDB IP		*/	"4F094D6F7F471C70556555180A",
		/* LogDB Port	*/	9979,
		/* LogDB User ID*/	"1F4C0D2C",
		/* LogDB User PW*/	"1D59142D39164A"
	},
	{
		// 115.144.35.19	4F094D6F7F471C70556555180A
		/* Server Name	*/	"중언 개인서버용",
		/* PreServer IP	*/	"4F094D6F7F471C705565551B06",
		//----------------------------------------------------------------//
		/* DBServer IP	*/	"4F094D6F7F471C705565551B06",
		/* DBServer Port*/	1433,
		/* DB User ID	*/	"1F4C0D2C",
		/* DB User PW	*/	"1D59142D39164A",
		/* DB Name		*/	"atum2_db_1",
		//----------------------------------------------------------------//
		/* LogDB IP		*/	"4F094D6F7F471C705565551B06",
		/* LogDB Port	*/	1433,
		/* LogDB User ID*/	"1F4C0D2C",
		/* LogDB User PW*/	"1D59142D39164A"
	},

#endif // END - #if defined(SERVICE_TYPE_KOREAN_SERVER_1)


#ifdef SERVICE_TYPE_KOREAN_SERVER_2		// 2006-09-05 by cmkwon
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 112.107.1.13(4F094A6F7F431F70577E4A1A), 211.189.116.12(4C09496F7F4B117057614D070206), 211.189.116.16(4C09496F7F4B117057614D070202), atum(1F4C0D2C), dpdltm@@tjdrhdgkwk$$!!(1A481C2D3A1E681E123A1F5B5B504343110846706F05)
	// 2007-12-07 by cmkwon, 예당 서버군 리스트 순서 변경 함
	// 2008-04-16 by cmkwon, 예당 정책 변경으로 DB Server 가 모두 사설로 변경됨(192.168.2.64<==211.189.116.12, 192.168.2.63<==211.189.116.16)
	// 2008-04-30 by cmkwon, 예당 DB Server IP, Port 변경(<==192.168.2.64,9979)
	// 2009-04-15 by cmkwon, 예당 모든 DB Server IP 변경 - 192.168.21.66(<--192.168.2.66), 192.168.21.63(<--192.168.2.63)

	// 2012-07-06 by hskim, YD IDC 이전 작업 -
	// ACEONLINE       패치서버		112.107.1.20	=>	183.110.249.135
	// ACEONLINE       테스트디비	112.107.1.17 (4F094A6F7F431F70577E4A1E)	=>	183.110.249.185 (4F004B6F7F42187054644207020C11)
	// ACEONLINE       대회			112.107.1.19	=>	183.110.249.137
	// ACEONLINE       아레나		112.107.1.18	=>	183.110.249.138
	// ACEONLINE       로그인		112.107.1.13 (4F094A6F7F431F70577E4A1A)	=>	183.110.249.139 (4F004B6F7F4218705464420702071D)
	// ACEONLINE       테스트		112.107.1.11 (4F094A6F7F431F70577E4A18)	=>	183.110.249.186 (4F004B6F7F42187054644207020C12)
	// ACEONLINE       비숍			112.107.1.9		=>	183.110.249.141
	// ACEONLINE       데카			112.107.1.5		=>	183.110.249.142
	// end 2012-07-06 by hskim, YD IDC 이전 작업 -

	// PreServerIP		: 183.110.249.139			- 4F004B6F7F4218705464420702071D
	// DBServerIP		: 192.168.21.66				- 4F014A6F7F4510705461551F05
	// DBServerPort		: 1456
	// DB User			: superold					- 0D4D08243C1C443A				
	// DB PWD			: shdls$$rpdla!!			- 0D501C2D3D570C2C163417481215	
	// LogDBServerIP	: 192.168.21.63				- 4F014A6F7F4510705461551F00
	// LogDBServerPort	: 1456
	// DB User			: superold					- 0D4D08243C1C443A				
	// DB PWD			: shdls$$rpdlafhrm!!		- 0D501C2D3D570C2C16341748555C56454742
	//{"본섭 - 01_데카(atum2_db_1)",					"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_1",	"4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C16341748555C56454742"},
	//{"본섭 - 02_비숍(atum2_db_7)",					"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_7",	"4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C16341748555C56454742"},
	//{"본섭 - 대회서버(atum2_db_20)",				"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_20",	"4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C16341748555C56454742"},

	// PreServerIP		: 183.110.249.186				- 4F004B6F7F42187054644207020C12		
	// DBServerIP		: 183.110.249.185			- 4F004B6F7F42187054644207020C11			
	// DBServerPort		: 9979
	// DB User			: superoldtest				- 0D4D08243C1C443A1235085D					
	// DB PWD			: shdls$$rpdlaxptmxm!!		- 0D501C2D3D570C2C163417484B4450451E0E4375	
	// LogDBServerIP	: 183.110.249.185			- 4F004B6F7F42187054644207020C11			
	// LogDBServerPort	: 9979
	// DB User			: superoldtest				- 0D4D08243C1C443A1235085D					
	// DB PWD			: shdls$$rpdlaxptmxm!!		- 0D501C2D3D570C2C163417484B4450451E0E4375	
	//{"테섭 - 예당테스트서버1(atum2_db_1)",		"4F004B6F7F42187054644207020C12", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375",	"atum2_db_1", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375"},
	//{"테섭 - 예당테스트서버2(atum2_db_2)",		"4F004B6F7F42187054644207020C12", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375",	"atum2_db_2", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375"},
	//{"테섭 - 예당테스트서버3(atum2_db_3)",		"4F004B6F7F42187054644207020C12", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375",	"atum2_db_3", "4F004B6F7F42187054644207020C11", 9979, "0D4D08243C1C443A1235085D",	"0D501C2D3D570C2C163417484B4450451E0E4375"},

	//{"(예약)본섭 - 05_아툼(atum2_db_8)",			"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_8", "4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C16341748555C56454742"},
	//{"(예약)본섭 - 06_오딘(atum2_db_9)",			"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_9", "4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C16341748555C56454742"},

	// 2007-12-13 by cmkwon, 백업 DB 추가
	//{"(백업)본섭 - 01_데카(atum2_db_1_bak)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_1_bak", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},
	//{"(백업)본섭 - 02_비숍(atum2_db_7_bak)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_7_bak", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},

	//{"(백업)본섭 - <2007년01월이전>바탈러스(atum2_db_2)",	"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_2", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},
	//{"(백업)본섭 - <2007년01월이전>샤린(atum2_db_3)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_3", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},
	//{"(백업)본섭 - <2007년01월이전>에오피(atum2_db_4)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_4", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},
	//{"(백업)본섭 - <2008년03월이전>카오스(atum2_db_5)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_5", "4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},
	//{"(백업)본섭 - <2008년03월이전>스카디(atum2_db_6)",		"4F004B6F7F4218705464420702071D", "4F014A6F7F4510705461551F05", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_6", "4F014A6F7F4510705461551F00", 1456, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},

	// 2011-10-10 by shcho, QA_SERVER 정보 추가
	// PreServerIP		: 112.107.1.6				- 4F094A6F7F431F70577E4D
	// DBServerIP		: 112.107.1.7				- 4F094A6F7F431F70577E4C
	// DBServerPort		: 9979
	// DB User			: superold					- 0D4D08243C1C443A								// 2008-09-04 by cmkwon, 예당 본섭,테섭 DB 접근 정보 수정 - 
	// DB PWD			: shdls$$rpdla!!			- 0D501C2D3D570C2C163417481215					// 2008-09-04 by cmkwon, 예당 본섭,테섭 DB 접근 정보 수정 - 
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - Key
	// #define STR_XOR_KEY_STRING_SERVER_INFO			"~8xANs(^fP{)34$(fcbTN$(C-=x"	// 2007-10-24 by cmkwon, 서버 정보 암호화 - DB Server Password XOR Key
	
	// 2012-07-06 by hskim, YD IDC 이전 작업 - YD QA SERVER 없어짐 삭제
	
	// {Name,										XOREncodedPreServerIP,		XOREncodedDBServerIP, DBServerPort, XOREncodedDBUserID,		 XOREncodedDBPWD,							DB Name,			LogDBIP,			LogDBPort,  LogDBUID,	LogDBPWD, PreServerIP, DBServerIP, DBUserID, DBPWD}
	// {"YD_QA_SERVER_(atum2_db_account)",				"4F094A6F7F431F70577E4D", "4F094A6F7F431F70577E4C", 9979, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215",	"atum2_db_1",		"4F094A6F7F431F70577E4C", 9979, "0D4D08243C1C443A",	"0D501C2D3D570C2C163417481215"},		
	// end 2011-10-10 by shcho, QA_SERVER 정보 추가

	// ============================================================================================================================ //
	// ============================================================================================================================ //
	// ============================================================================================================================ //

	// PreServerIP		: 192.168.1.2				- 4F014A6F7F451070577E49
	// DBServerIP		: 192.168.1.5				- 4F014A6F7F451070577E4E
	// DBServerPort		: 9979
	// DB User			: aceonline					- 1F5B1D2E201F413003				
	// DB PWD			: dpdltm&&**((112233		- 1A481C2D3A1E0E784C7A53010205161A5550	

#ifdef S_KOR_BUSAN_MONITORTOOL_MAKE

#ifdef _ATUM_MONITORTOOL
	// 부산 운영팀의 경우에는 모니터 툴은 본섭 정보가 포함되지 않도록 한다 - 부산 운영팀은 테섭만 DB Insert, 혹시나 본섭에 반영하는 실수를 방지하기 위해서...
#else
	{"본섭 - 01_데카 (atum2_db_1)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_1",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
	{"본섭 - 02_비숍 (atum2_db_7)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_7",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
	{"본섭 - 03_필론 (atum2_db_6)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_6",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
#endif // _ATUM_MONITORTOOL

#else
	{"본섭 - 01_데카 (atum2_db_1)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_1",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
	{"본섭 - 02_비숍 (atum2_db_7)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_7",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
	{"본섭 - 03_필론 (atum2_db_6)",			"4F014A6F7F451070577E49", "4F014A6F7F451070577E4E", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550",	"atum2_db_6",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1A481C2D3A1E0E784C7A53010205161A5550"},
#endif // S_KOR_BUSAN_ADMINTOOL_MAKE

	// PreServerIP		: 192.168.1.7				- 4F014A6F7F451070577E4C
	// DBServerIP		: 192.168.1.7				- 4F014A6F7F451070577E4C
	// DBServerPort		: 9979
	// DB User			: aceonline					- 1F5B1D2E201F413003
	// DB PWD			: abc123!@#					- 1F5A1B707C40091E45	

	{"테섭 - 마상 #1 (atum2_db_1)",			"4F014A6F7F451070577E4C", "4F014A6F7F451070577E4C", 9979, "1F5B1D2E201F413003",	"1F5A1B707C40091E45",	"atum2_db_1",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1F5A1B707C40091E45"},
	{"테섭 - 마상 #2 (atum2_db_6)",			"4F014A6F7F451070577E4C", "4F014A6F7F451070577E4C", 9979, "1F5B1D2E201F413003",	"1F5A1B707C40091E45",	"atum2_db_6",	"4F014A6F7F4510705461551F00", 9979, "1F5B1D2E201F413003",	"1F5A1B707C40091E45"},


	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.141(4F0A496F7F401C7057614F07020015), 121.134.114.141(4F0A496F7F401C7057614F07020015), atum(1F4C0D2C), callweb(1D59142D39164A)
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 115.144.35.141(4F094D6F7F471C70556555180705)
	{"MS141(Masang_Kor)_(atum2_db_1)",				"4F094D6F7F471C70556555180705", "4F094D6F7F471C70556555180705", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C70556555180705", 9979, "1F4C0D2C",	"1D59142D39164A"},			// 2006-09-21 by cmkwon
	// 2008-08-20 by cmkwon, 제거함
	//{"MS141(Yedang)_Kor(atum2_db_account_test)",		"4F0A496F7F401C7057614F07020015", "4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A"},		// 2006-09-21 by cmkwon


#endif// #ifdef SERVICE_TYPE_KOREAN_SERVER_2
#ifdef SERVICE_TYPE_ENGLISH_SERVER_1
	{ "Main   - 4.6 (atum/callweb)",
		"0C5C5A0920505F572B1D37084B3F", "0C5C5A0920505F572B1D37084B3F", 1433, //IP,IP,PORT
		"0C5C5A0920505F572B1D37084B3F",	"0C5C5A0920505F572B1D37084B3F",//DBID,DBPWD
		"atum2_db_1",//DBNAME
		"0C5C5A0920505F572B1D37084B3F", 1433, //DBIP,DBPORT
		"0C5C5A0920505F572B1D37084B3F",	"0C5C5A0920505F572B1D37084B3F" },//DBUID,DBPWD
	{ "Main   - RZLeechers (dedi)",
	"0C5C5A0920505F572B1D37084B3F", "0C5C5A0920505F572B1D37084B3F", 1433, //IP,IP,PORT
	"0C5C5A0920505F572B1D37084B3F",	"0C5C5A0920505F572B1D37084B3F",//DBID,DBPWD
	"atum2_db_1",//DBNAME
	"0C5C5A0920505F572B1D37084B3F", 1433, //DBIP,DBPORT
	"0C5C5A0920505F572B1D37084B3F",	"0C5C5A0920505F572B1D37084B3F" },//DBUID,DBPWD
#endif// #ifdef SERVICE_TYPE_ENGLISH_SERVER_1

#ifdef SERVICE_TYPE_VIETNAMESE_SERVER_1
	// 2006-08-28 by cmkwon, Changed PreServerIP(10.10.1.10 --> 10.10.1.28)
	// 2006-11-02 by cmkwon, Changed PreServerIP(10.10.1.28 --> 222.255.15.37), DBServerIP(10.10.1.14 --> 222.255.15.61)
	// 2007-10-22 by cmkwon, Changed passowrd(<--"tmzk!@Wkd")
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 222.255.15.37(4F0A496F7F401C7057614F07020014), 222.255.15.61(4C0A4A6F7C461D705765551F02), atum(1F4C0D2C), \/tcb3gin08(22170C222C404F37086043)
	// 2009-01-07 by cmkwon, 베트남 업데이트 시스템 변경 - 이름 변경하고 임시용 두개 추가함.
	// 2009-01-08 by cmkwon, 베트남 db 정보 사설 IP로 변경 - 
	// 2009-01-15 by cmkwon, 베트남 DB PWD 변경, public ip로 다시 변경 - 
	// 2010-02-04 by cmkwon, 베트남 서버군 추가(atum2_db_18 Eternal) - 
	// 2011-08-16 by shcho, 베트남 IP변경 PreServer(117.103.194.77), DBServer(117.103.194.76)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2012-07-04 by hskim, 베트남 서버 통합
	// 기존
	// PreServerIP		: 117.103.194.77			- 4F094F6F7F431B7057694F070403 (이전 4C0A4A6F7C461D705765551A04)
	// DBServerIP		: 117.103.194.76			- 4F094F6F7F431B7057694F070402 (이전 4C0A4A6F7C461D705765551F02)
	// DBServerIP		: 117.103.194.75			- 4F094F6F7F431B7057694F070401 (서버분류:dragon,legend)
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: !ace2012||\/tc!			- 5F591B247C43196C1A2C2706475705      => OLD PASSWORD (\/tcR3vive2011 -  22170C221C405E37103549190205)	// OLD PASSWORD (v\/tcb3gin08	- 086457352D111B390F3E4B11)2006-06-02 by shcho
	// 2011-03-17 by shcho, 베트남 IP변경건 - atum2_db_16번 서버 IP변경 117.103.194.75(4F094F6F7F431B7057694F070401)
	// {"Main      - Phoenix   (atum2_db_14)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_14", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			// 2011-01-20 by shcho, 베트남 서버 추가 증설
	// {"Main      - Akron     (atum2_db_15)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_15", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			// 2006-04-26 by cmkwon
	// {"Main      - Legend    (atum2_db_16)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_16", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			// 2011-12-08 by shcho, DB서버 IP,PW변경
	// {"Main      - Dragon    (atum2_db_17)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_17", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			// 2011-12-08 by shcho, DB서버 IP,PW변경
	// {"Main      - Eternal   (atum2_db_18)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_18", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			// 2006-04-26 by cmkwon
	// {"Main      - PANDORA   (atum2_db_19)",		"4F094F6F7F431B7057694F070403", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_19", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},			/
	// 수정 

	// PreServerIP		: 117.103.194.68			- 4F094F6F7F431B7057694F07050C		//	2013-01-28 by khkim	베트남 PreServer IP 변경에 따른 툴 정보 변경 사항 
	// DBServerIP		: 117.103.194.76			- 4F094F6F7F431B7057694F070402
	// DBServerIP		: 117.103.194.75			- 4F094F6F7F431B7057694F070401
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: !ace2012||\/tc!			- 5F591B247C43196C1A2C2706475705

	// 2013-11-26 by bckim, 베트남 디비 서버 아이디 패스워드 변경 
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: |(acevn2013)\/tc23|		- 021019222B05466C566148006F1B504B54501E 

 	{"Main      - Akron     (atum2_db_15)",		"4F094F6F7F431B7057694F07050C", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_15", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},
 	{"Main      - Legend    (atum2_db_16)",		"4F094F6F7F431B7057694F07050C", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_16", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},
 	{"Main      - Vatallus  (atum2_db_17)",		"4F094F6F7F431B7057694F07050C", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_17", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},
 	{"Main      - Dragon    (atum2_db_18)",		"4F094F6F7F431B7057694F07050C", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_18", "4F094F6F7F431B7057694F070402", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 2012-10-15 by bckim, 베트남 EP4 신서버 빌드 패치요청 관련 정보 추가 업무 
	// PreServerIP		: 117.103.194.68			- 4F094F6F7F431B7057694F07050C
	// DBServerIP		: 117.103.194.75			- 4F094F6F7F431B7057694F070401
	// DB User			: aceonlinevn				- 1F5B1D2E201F4130032615
	// DB PWD			: AceR3viv@l|2012ep4|\/tc	- 3F5B1D137D054128263C071B0305164D16571E0861504B
    //{"Main      - ServerName   (DB Name)","PreServerIP",					"DBServerIP",					9979, "DB User",				"DB PWD",												"DB Name",		"DBServerIP",					9979, "DB User",				"DB PWD"},											
	{"Main      - NGC   (atum2_db_1)",		"4F094F6F7F431B7057694F07050C", "4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_1",	"4F094F6F7F431B7057694F070401", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},
	// end 2012-10-15 by bckim, 베트남 EP4 신서버 빌드 패치요청 관련 정보 추가 업무 

	
// 2009-01-07 by cmkwon, 베트남 업데이트 시스템 변경 - 삭제함
//	{"VTC(Viet) - old_Alioth    (atum2_db_1)",	"4C0A4A6F7C461D705765551A04", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043",		"atum2_db_1", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043"},			// 2006-04-26 by cmkwon
//	{"VTC(Viet) - old_Blackburn (atum2_db_11)",	"4C0A4A6F7C461D705765551A04", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043",		"atum2_db_11", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043"},			// 2006-04-26 by cmkwon
//	{"VTC(Viet) - old_Chaos     (atum2_db_12)",	"4C0A4A6F7C461D705765551A04", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043",		"atum2_db_12", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043"},			// 2006-04-26 by cmkwon
//	{"VTC(Viet) - old_Denebola  (atum2_db_13)",	"4C0A4A6F7C461D705765551A04", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043",		"atum2_db_13", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043"},			// 2006-04-26 by cmkwon
//	{"VTC(Viet) - old_Edmont    (atum2_db_14)",	"4C0A4A6F7C461D705765551A04", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043",		"atum2_db_14", "4C0A4A6F7C461D705765551F02", 9979, "1F4C0D2C",	"22170C222C404F37086043"},			// 2006-04-26 by cmkwon

	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 222.255.15.54(4C0A4A6F7C461D705765551C07), 222.255.15.61(4C0A4A6F7C461D705765551F02), atum(1F4C0D2C), Scovn2007?><(1D59142D39164A)
	// 2009-01-07 by cmkwon, 베트남 업데이트 시스템 변경 - 테섭 DB 서버 정보 변경
	// 2009-01-08 by cmkwon, 베트남 db 정보 사설 IP로 변경 - 
	// 2009-01-15 by cmkwon, 베트남 DB PWD 변경, public ip로 다시 변경 - 
	// 2011-08-16 by shcho, 베트남 IP변경 - testServer PreServer(117.103.198.155), DBServer(117.103.198.170)
	// PreServerIP		: 117.103.198.155				- 4F094F6F7F431B7057694307020111
	// DBServerIP		: 117.103.198.170				- 4F094F6F7F431B7057694307020314
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: !ace2012||\/tc!				- 5F591B247C43196C1A2C2706475705
	
	//{"Test      - TestServ1 (atum2_db_1)",		"4F094F6F7F431B7057694307020111", "4F094F6F7F431B7057694307020314", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705",		"atum2_db_1", "4F094F6F7F431B7057694307020314", 9979, "1F4C0D2C",	"5F591B247C43196C1A2C2706475705"},		// 2012-07-04 by hskim, 베트남 서버 통합
	//{"Test      - TestServ1 (atum2_db_1)",		"4F094F6F7F431B7057694307020111", "4F094F6F7F431B7057694307020314", 9979, "1F5B1D3720",	"56591B247C43196D4F2C075F4757161B",		"atum2_db_1", "4F094F6F7F431B7057694307020314", 9979, "1F5B1D3720",	"56591B247C43196D4F2C075F4757161B"},

	// 2012-04-09 by hskim, EP4 전달용 서버 추가
	// PreServerIP		: 117.103.198.170				- 4F094F6F7F431B7057694307020314
	// DBServerIP		: 117.103.198.170				- 4F094F6F7F431B7057694307020314
	// DB User			: aceonlinevn				- 1F5B1D2E201F4130032615
	// DB PWD			: AceR3viv@l|2012ep4|\/tc	- 3F5B1D137D054128263C071B0305164D16571E0861504B
	
	// 2013-11-26 by bckim, 베트남 디비 서버 아이디 패스워드 변경 
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: |(acevn2013)\/tc23|		- 021019222B05466C566148006F1B504B54501E 
	{"Test_EP4  - TestServ2 (atum2_db_1)",		"4F094F6F7F431B7057694307020314", "4F094F6F7F431B7057694307020314", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E",		"atum2_db_1", "4F094F6F7F431B7057694307020314", 9979, "1F4C0D2C",	"021019222B05466C566148006F1B504B54501E"},

	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.143(4F0A496F7F401C7057614F07020017), 121.134.114.143(4F0A496F7F401C7057614F07020017), atum(1F4C0D2C), callweb(1D59142D39164A)
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 115.144.35.143(4F094D6F7F471C70556555180707) //61.39.170.143(48095672775D1969567E4A1D00)
	{"MS143(VTC-Intecom_Viet)_(atum2_db_account)",		"4F094D6F7F471C70556555180707", "4F094D6F7F471C70556555180707", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C70556555180707", 9979, "1F4C0D2C",	"1D59142D39164A"},			// 2006-03-24 by cmkwon
	// 2008-08-20 by cmkwon, 제거함
	//{"MS143(VTC-Intecom)_Viet(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020017", "4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A"},		// 2006-03-24 by cmkwon

	// 2012-04-09 by hskim, EP4 전달용 서버 추가
	// PreServerIP		: 61.39.170.238				- 48095672775D1969567E491A0B
	// DBServerIP		: 61.39.170.238				- 48095672775D1969567E491A0B
	// DB Port			: 9143
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: callweb					- 1D59142D39164A
	{"MS143_EP4(VTC-Intecom_Viet)_(atum2_db_account)",		"48095672775D1969567E491A0B", "48095672775D1969567E491A0B", 9143, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491A0B", 9143, "1F4C0D2C",	"1D59142D39164A"},
#endif// #ifdef SERVICE_TYPE_VIETNAMESE_SERVER_1

#ifdef SERVICE_TYPE_ENGLISH_SERVER_2

//영국	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2008-01-04 by cmkwon, Gameforge4D 추가함 - 
	// 2008-04-16 by cmkwon, Gameforge4D 서버군 추가(db_3, db_4, db_5)
	// 2009-03-04 by cmkwon, Gameforge 영국,독일 본섭 PreServer, DBServer Domain 으로 변경 - 기존(83.141.22.23, 83.141.22.20)
	// PreServerIP	: zion.airrivals.net		- 4A08014B6348172B582B370812151B0C4C21
	// DBServerIP	: speicher.airrivals.net	- 43110B4C2E411B2B0423281B0C0F4303452663104C51
	// DBServerPort	: 9981		// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 영국
	// DB User		: atum						- 51151B48
	// DB PWD		: ZF-UJ96aVpzKqL9!u7nmAs	- 6A274370071048387C323B220F2A0C435C6223136856			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 영국
	{"Main      - HELIX   (atum2_db_1)",		"4A08014B6348172B582B370812151B0C4C21", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856"},
	{"Main      - ORION   (atum2_db_2)",		"4A08014B6348172B582B370812151B0C4C21", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856"},
	{"Main      - AURIGA  (atum2_db_3)",		"4A08014B6348172B582B370812151B0C4C21", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856"},
	{"Main      - Reserve1(atum2_db_4)",		"4A08014B6348172B582B370812151B0C4C21", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856"},
	{"Main      - Reserve1(atum2_db_5)",		"4A08014B6348172B582B370812151B0C4C21", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F4303452663104C51", 9981, "51151B48",	"6A274370071048387C323B220F2A0C435C6223136856"},

	// 2008-01-26 by cmkwon, Gameforge4D 테섭 추가함 - 
	// 2008-10-30 by cmkwon, Gameforge4D_Eng 테섭 서버 IP 변경 - (<--85.115.19.228)
	// PreServerIP	: 79.110.95.6		- 075840147C1950601F6C77
	// DBServerIP	: 79.110.95.6		- 075840147C1950601F6C77		// 2008-09-19 by cmkwon, 변경함(<--85.115.19.229)
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 영국 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9980 )
	{"Test [EN] - TestServer(atum2_db_1)",	"075840147C1950601F6C77", "0151401D7D07476D047172", 9980, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9980, "51151B48",	"530002493A4C1C"},
	
	// 2012-07-30 by jhseol, Gameforge4D 영국 테섭 추가
	// EntryName	: TEST - SQLPROXY(atum2_db_1)
	// PreServerIP	: 79.110.95.6			- 075840147C1950601F6C77
	// DBServerIP	: 79.110.88.176			- 075840147C195061126C705E48
	// DB User		: gf_dev				- 57073141285F
	// DB PWD		: x0k3qLN2m8BnUgeHTP7t	- 485105163C65306B477A03072B01502A7D057A0A
	// DB Port		: 8801
	{"TEST - SQLPROXY(atum2_db_1)",	"075840147C1950601F6C77", "075840147C195061126C705E48", 8801, "57073141285F",	"485105163C65306B477A03072B01502A7D057A0A",		"atum2_db_1", "075840147C195061126C705E48", 8801, "57073141285F",	"485105163C65306B477A03072B01502A7D057A0A"},

	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2008-01-04 by cmkwon, Gameforge4D 추가함 - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.145(0650401674074F6E1A6C705D4B)
	// PreServerIP	: 115.144.35.145		- 01505B0B7C1D4A7719776F584A53
	// DBServerIP	: 115.144.35.145		- 01505B0B7C1D4A7719776F584A53
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"Masang145(Gameforge4D_Eng)_(atum2_db_account)",		"01505B0B7C1D4A7719776F584A53", "01505B0B7C1D4A7719776F584A53", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F584A53", 9979, "51151B48",	"530002493A4C1C"},
	// 2008-08-20 by cmkwon, 제거함
	//{"MS145(Gameforge4D_Eng)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020011", "4F0A496F7F401C7057614F07020011", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_test",	"4F0A496F7F401C7057614F07020011", 9979, "51151B48",	"530002493A4C1C"},

	// 2012-11-21 by jhseol, Gameforge4D 영국 EP4 테섭 추가
	{"[ ------- EP4 TEST Server List ------ ]",	"", "", 0, "", "", "", "", 0, "", ""},
	// EntryName	: TEST - SQLPROXY(atum2_db_1)
	// PreServerIP	: 79.110.95.62			- 075840147C1950601F6C775B
	// DBServerIP	: 79.110.95.62			- 075840147C1950601F6C775B
	// DB User		: atum					- 51151B48
	// DB PWD		: callweb				- 530002493A4C1C
	// DB Port		: 9979
	// 2013-02-14 by jhseol, 영국 EP4 테섭 정보 변경
	// PreServerIP	: 79.110.88.24			- 075840147C195061126C735D
	// DBServerIP	: 79.110.88.125			- 075840147C195061126C705B4B
	// DB Port		: 9982
	{"EP4 Test - EP4 TestServer(atum2_db_1)",	"075840147C195061126C735D", "075840147C195061126C705B4B", 9982, "51151B48",	"530002493A4C1C",		"atum2_db_1", "075840147C195061126C705B4B", 9982, "51151B48",	"530002493A4C1C"},

	// 2012-11-21 by jhseol, Gameforge4D 영국 EP4 마상 내부 테섭 추가
	// EntryName	: EP4 MS145(Gameforge4D_Eng)
	// PreServerIP	: 115.144.35.145		- 01505B0B7C1D4A7719776F584A53
	// DBServerIP	: 115.144.35.216		- 01505B0B7C1D4A7719776F5B4F50
	// DB User		: atum					- 51151B48
	// DB PWD		: callweb				- 530002493A4C1C
	// DB Port		: 9979
	//{"EP4 MS145(Gameforge4D_Eng)",	"01505B0B7C1D4A7719776F584A53", "01505B0B7C1D4A7719776F5B4F50", 9979, "51151B48",	"530002493A4C1C",		"atum2_db_1", "01505B0B7C1D4A7719776F5B4F50", 9979, "51151B48",	"530002493A4C1C"},

#endif// #ifdef SERVICE_TYPE_ENGLISH_SERVER_2

// 2008-04-25 by cmkwon, 지원 언어에 독일어 추가 - 
#if defined(SERVICE_TYPE_GERMAN_SERVER_1)
//독일	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2008-07-10 by cmkwon, 독일 본섭 추가함.
	// 2008-08-13 by cmkwon, 독일 Gameforge4D_Deu DB Server IP changed - 83.141.22.216 <-- 83.141.22.210
	// 2009-03-04 by cmkwon, Gameforge 영국,독일 본섭 PreServer, DBServer Domain 으로 변경 - 기존(83.141.22.113, 83.141.22.216)
	// PreServerIP	: prokyon.airrivals.de		- 4013014E344610774B2B331B1710540E5A7B291B
	// DBServerIP	: speicher.airrivals.de		- 43110B4C2E411B2B0423281B0C0F43034526631A4C
	// DBServerPort	: 9979		// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 독일
	// DB User		: atum						- 51151B48
	// DB PWD		: BnaPyrf!fGAtYT8sHv-607	- 720F0F75345B18784C05001D27320D11612360481912			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 독일
	// 2008-07-10 by cmkwon, 독일 본섭 추가함.
	// 2008-07-22 by cmkwon, Gameforge4D_Deu Pandora(Server2) 추가 함
	{"Main      - Prokyon  (atum2_db_1)",		"4013014E344610774B2B331B1710540E5A7B291B", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912"},
	{"Main      - Pandora  (atum2_db_2)",		"4013014E344610774B2B331B1710540E5A7B291B", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912"},
	{"Main      - Antares  (atum2_db_3)",		"4013014E344610774B2B331B1710540E5A7B291B", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912"},
	{"Main      - Reserve4 (atum2_db_4)",		"4013014E344610774B2B331B1710540E5A7B291B", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912"},
	{"Main      - Reserve5 (atum2_db_5)",		"4013014E344610774B2B331B1710540E5A7B291B", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F43034526631A4C", 9979, "51151B48",	"720F0F75345B18784C05001D27320D11612360481912"},

	// 2008-04-25 by cmkwon, 지원 언어에 독일어 추가 - 
	// 2008-10-30 by cmkwon, Gameforge4D_Deu 테섭 서버 IP 변경 - (<--85.115.19.232)
	// 2010-03-25 by cmkwon, Gameforge4D_Deu 테섭 서버 IP 변경(기존:79.110.95.9) - 
	// PreServerIP	: 79.110.95.47		- 075840147C1950601F6C755E
	// DBServerIP	: 79.110.95.47		- 075840147C1950601F6C755E
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 독일 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9979 )
	{"Test    - TestServer(atum2_db_1)",	"075840147C1950601F6C755E", "0151401D7D07476D047172", 9979, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9979, "51151B48",	"530002493A4C1C"},

	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2008-04-25 by cmkwon, Gameforge4D_Deu 추가함 - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.147(0650401674074F6E1A6C705D49)
	// PreServerIP	: 115.144.35.147		- 01505B0B7C1D4A7719776F584A51
	// DBServerIP	: 115.144.35.147		- 01505B0B7C1D4A7719776F584A51
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS147(Gameforge4D_Deu)_(atum2_db_account)",		"01505B0B7C1D4A7719776F584A51", "01505B0B7C1D4A7719776F584A51", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F584A51", 9979, "51151B48",	"530002493A4C1C"},
	
	// 2009-10-16 by cmkwon, 필요 없으므로 제거
	//	{"MS147(Gameforge4D_Deu_MySQL)_(atum2_db_account)",	"4F0A496F7F401C7057614F07020013", "4F0A496F7F401C7057614F07020013", 3306, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"4F0A496F7F401C7057614F07020013", 3306, "51151B48",	"530002493A4C1C"},
	
	// 2008-08-20 by cmkwon, 제거함
	//{"MS147(Gameforge4D_Deu)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020013", "4F0A496F7F401C7057614F07020013", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_test",	"4F0A496F7F401C7057614F07020013", 9979, "51151B48",	"530002493A4C1C"},

#endif// #ifdef SERVICE_TYPE_GERMAN_SERVER_1

// 2008-05-07 by cmkwon, WinnerOnline_Tha 추가하기 - 
#if defined(SERVICE_TYPE_THAI_SERVER_1)
	// 2008-05-07 by cmkwon, WinnerOnline_Tha 추가하기 - 
	// 2008-11-19 by cmkwon, 태국 WinnerOnline_Tha 서버그룹 추가 - 
	// 2009-09-23 by cmkwon, 태국 서버군 통합 관련 서버군 정리 - 
	// PreServerIP	: 202.43.33.65		- 4C084A6F7A40066D557E4D1C
	// DBServerIP	: 202.43.33.66		- 4C084A6F7A40066D557E4D1F
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: winner9sohk		- 0951162F2B01112D093810
	{"Main  - Enterprise  (atum2_db_11<8+9>)",	"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_11", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main  - Orion       (atum2_db_12<7+10>)",	"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_12", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main  - Colony      (atum2_db_13)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_13", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main  - Reserve1    (atum2_db_14)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_14", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main  - Reserve2    (atum2_db_15)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_15", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main    - old_Omega   (atum2_db_7)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_7", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main    - old_Fornax  (atum2_db_8)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_8", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main    - old_Tucana  (atum2_db_9)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_9", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
	{"Main    - old_Phoenix (atum2_db_10)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_10", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 2009-09-23 by cmkwon, 태국 서버군 통합 관련 서버군 정리 - 
// 	{"Main      - Serv1   (atum2_db_1)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_1", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv2   (atum2_db_2)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_2", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv3	  (atum2_db_3)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_3", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv4	  (atum2_db_4)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_4", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv5   (atum2_db_5)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_5", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv6	  (atum2_db_6)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_6", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv7	  (atum2_db_7)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_7", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv8   (atum2_db_8)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_8", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv9	  (atum2_db_9)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_9", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},
// 	{"Main      - Serv10  (atum2_db_10)",		"4C084A6F7A40066D557E4D1C", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_10", "4C084A6F7A40066D557E4D1F", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},

	// 2008-05-07 by cmkwon, WinnerOnline_Tha 추가하기 - 
	// 2008-06-20 by cmkwon, WinnerOnline_Tha 테섭 PreServer,DBServerIP 변경 - 기존(// PreServerIP	: 202.176.82.16, // DBServerIP	: 58.136.118.37)
	// 2009-08-28 by cmkwon, 태국 테섭 지원 PC IP 변경 - 
	// PreServerIP	: 202.170.113.40	- 4C084A6F7F441870576148070704
	// DBServerIP	: 202.170.113.39	- 4C084A6F7F44187057614807000D
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: winner9sohk		- 0951162F2B01112D093810
	{"Test    - TestServer(atum2_db_1)",	"4C084A6F7F441870576148070704", "4C084A6F7F44187057614807000D", 1433, "1F4C0D2C",	"0951162F2B01112D093810",		"atum2_db_1", "4C084A6F7F44187057614807000D", 1433, "1F4C0D2C",	"0951162F2B01112D093810"},

	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2008-05-07 by cmkwon, WinnerOnline_Tha 추가하기 - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.148(48095672775D1969567E4A1D0B)
	// PreServerIP	: 61.39.170.148		- 48095672775D1969567E4A1D0B
	// DBServerIP	: 61.39.170.148		- 48095672775D1969567E4A1D0B
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS148(WinnerOnline_Tha)_(atum2_db_account)",		"48095672775D1969567E4A1D0B", "48095672775D1969567E4A1D0B", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E4A1D0B", 9979, "1F4C0D2C",	"1D59142D39164A"},
	// 2008-08-20 by cmkwon, 제거함
	//{"MS148(WinnerOnline_Tha)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F0702001C", "4F0A496F7F401C7057614F0702001C", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
#endif// #ifdef SERVICE_TYPE_THAI_SERVER_1

// 2008-06-27 by cmkwon, Innova_Rus AdminTool,DBTool 에 DB 정보 추가 - 
#if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)
	// 2008-06-27 by cmkwon, Innova_Rus AdminTool,DBTool 에 DB 정보 추가 - 
	// 2009-12-11 by cmkwon, 러시아 PreServer,DBServer IP변경 - 
	// PreServerIP	: 109.105.134.133	- 4F08416F7F431D7057634F07020717
	// DBServerIP	: 109.105.134.140	- 4F08416F7F431D7057634F07020014
	// 2008-07-29 by cmkwon, 임시로 DB IP 89.249.25.69(460156737A4A066C537E4D10) 로 수정
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: ahr!$akfj			- 1F500A606A1243380C
	{"Main      - Serv1   (atum2_db_1)",		"4F08416F7F431D7057634F07020717", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_1", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	{"Main      - Serv2   (atum2_db_2)",		"4F08416F7F431D7057634F07020717", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_2", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	{"Main      - Reserve3(atum2_db_3)",		"4F08416F7F431D7057634F07020717", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_3", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	{"Main      - Reserve4(atum2_db_4)",		"4F08416F7F431D7057634F07020717", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_4", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	{"Main      - Reserve5(atum2_db_5)",		"4F08416F7F431D7057634F07020717", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_5", "4F08416F7F431D7057634F07020014", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	
	// 2008-10-29 by cmkwon, 러시아 Innova_Rus 테섭 정보 추가 
	// 2010-04-26 by cmkwon, 러시아 Innova 테섭 IP 변경 - 기존 Pre(89.249.25.58), 기존 DB(89.249.25.56)
	// 2010-06-30 by jskim //DBServerIP	: 109.105.134.130	- 4F08416F7F431D7057634F07020714
	// PreServerIP	: 109.105.134.130	- 4F08416F7F431D7057634F07020714
	// DBServerIP	: 109.105.134.141	- 4F08416F7F431D7057634F07020015
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: ahr!$akfj			- 1F500A606A1243380C
	{"Test      - TestServer1(atum2_db_1)",	"4F08416F7F431D7057634F07020714", "4F08416F7F431D7057634F07020015", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_1", "4F08416F7F431D7057634F07020015", 1433, "1F4C0D2C",	"1F500A606A1243380C"},
	
	// 2011-04-29 by shcho, 러시아 Innova_Rus 테섭 정보 추가 - 2번째 테섭
	// PreServerIP	: 10.33.152.121		- 4F0856727D5D196B547E4A1B02
	// DBServerIP	: 10.33.152.120		- 4F0856727D5D196B547E4A1B03
	// DBServerPort	: 1433									 (변경해야함 - 상호)
	// DB User		: atum				- 1F4C0D2C			 (변경해야함 - 상호)
	// DB PWD		: ahr!$akfj			- 1F500A606A1243380C (변경해야함 - 상호)
	{"Test      - TestServer2(atum2_db_1)",	"4F0856727D5D196B547E4A1B02", "4F0856727D5D196B547E4A1B03", 1433, "1F4C0D2C",	"1F500A606A1243380C",		"atum2_db_1", "4F0856727D5D196B547E4A1B03", 1433, "1F4C0D2C",	"1F500A606A1243380C"},

	/// 2012-05-05 by jhseol, 러시아 EP4 전달용 서버 추가
	// PreServerIP		: 10.33.152.111			- 4F0856727D5D196B547E4A1802
	// DBServerIP		: 10.33.152.110			- 4F0856727D5D196B547E4A1803
	// DBServerPort		: 1433
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	// LogDBServerIP	: 10.33.152.110			- 4F0856727D5D196B547E4A1803
	// LogDBServerPort	: 1433
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	{"Test_EP4 - TestServer1(atum2_db_1)",	"4F0856727D5D196B547E4A1802", "4F0856727D5D196B547E4A1803", 1433, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F0856727D5D196B547E4A1803", 1433, "1F4C0D2C",	"1D59142D39164A"},

	// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
	// 2008-06-19 by cmkwon, Innova_Rus 내부 테스트 서버 추가 - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.220(48095672775D1969567E491B03)
	// PreServerIP	: 61.39.170.220		- 48095672775D1969567E491B03
	// DBServerIP	: 61.39.170.220		- 48095672775D1969567E491B03
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS220(Innova_Rus)_(atum2_db_account)",		"48095672775D1969567E491B03", "48095672775D1969567E491B03", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491B03", 9979, "1F4C0D2C",	"1D59142D39164A"},
	// 2008-08-20 by cmkwon, 제거함
	//{"MS220(Innova_Rus)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07010614", "4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A"},

	/// 2012-05-05 by jhseol, 러시아 EP4 전달용 서버 추가 - 61.39.170.238(48095672775D1969567E491A0B)
	{"MS220_EP4(Innova_Rus)_(atum2_db_1)",		"48095672775D1969567E491A0B", "48095672775D1969567E491A0B", 9220, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491A0B", 9220, "1F4C0D2C",	"1D59142D39164A"},


#endif// END - #if defined(SERVICE_TYPE_RUSSIAN_SERVER_1)

///////////////////////////////////////////////////////////////////////////////
// 2008-09-23 by cmkwon, 지원 서비스 추가(대만 Netpower_Tpe) - 
#if defined(SERVICE_TYPE_TAIWANESE_SERVER_1)	
	// PreServerIP		: 202.153.171.70			- 4C084A6F7F461B7057674A070404
	// DBServerIP		: 10.10.50.31				- 4F0856707E5D1D6E48634A
	// DBServerPort		: 9979
	// DB User			: aceuser					- 1F5B1D343D165A
	// DB PWD			: aceu960@@##				- 1F5B1D347745181E267358
	// LogDBServerIP	: 10.10.50.31				- 4F0856707E5D1D6E48634A
	// LogDBServerPort	: 9978
	// DB User			: aceuser					- 1F5B1D343D165A
	// DB PWD			: aceu960@@##				- 1F5B1D347745181E267358
	{"Main      - Serv1   (atum2_db_1)",		"4C084A6F7F461B7057674A070404", "4F0856707E5D1D6E48634A", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_1", "4F0856707E5D1D6E48634A", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},
	{"Main      - Serv2   (atum2_db_2)",		"4C084A6F7F461B7057674A070404", "4F0856707E5D1D6E48634A", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_2", "4F0856707E5D1D6E48634A", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},
	{"Main      - Serv3   (atum2_db_3)",		"4C084A6F7F461B7057674A070404", "4F0856707E5D1D6E48634A", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_3", "4F0856707E5D1D6E48634A", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},
	{"Main      - Reserve4(atum2_db_4)",		"4C084A6F7F461B7057674A070404", "4F0856707E5D1D6E48634A", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_4", "4F0856707E5D1D6E48634A", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},
	{"Main      - Reserve5(atum2_db_5)",		"4C084A6F7F461B7057674A070404", "4F0856707E5D1D6E48634A", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_5", "4F0856707E5D1D6E48634A", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},
	
	// 2008-06-27 by cmkwon, Innova_Rus AdminTool,DBTool 에 DB 정보 추가 - 
	// PreServerIP		: 192.168.1.31				- 4F014A6F7F451070577E4818
	// DBServerIP		: 192.168.1.32				- 4F014A6F7F451070577E481B
	// DBServerPort		: 9979
	// DB User			: aceuser					- 1F5B1D343D165A
	// DB PWD			: aceu960@@##				- 1F5B1D347745181E267358
	// LogDBServerIP	: 192.168.1.32				- 4F014A6F7F451070577E481B
	// LogDBServerPort	: 9978
	// DB User			: aceuser					- 1F5B1D343D165A
	// DB PWD			: aceu960@@##				- 1F5B1D347745181E267358
	{"Test    - TestServer(atum2_db_1)",	"4F014A6F7F451070577E4818", "4F014A6F7F451070577E481B", 9979, "1F5B1D343D165A",	"1F5B1D347745181E267358",		"atum2_db_1", "4F014A6F7F451070577E481B", 9978, "1F5B1D343D165A",	"1F5B1D347745181E267358"},

	// 2008-09-23 by cmkwon, 지원 서비스 추가(대만 Netpower_Tpe) - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.221(48095672775D1969567E491B02)
	// PreServerIP	: 61.39.170.221		- 48095672775D1969567E491B02
	// DBServerIP	: 61.39.170.221		- 48095672775D1969567E491B02
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS221(Netpower_Tpe)_(atum2_db_account)",	"48095672775D1969567E491B02", "48095672775D1969567E491B02", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491B02", 9979, "1F4C0D2C",	"1D59142D39164A"},
#endif // END - #if defined(SERVICE_TYPE_TAIWANESE_SERVER_1)

///////////////////////////////////////////////////////////////////////////////
// 2008-12-03 by cmkwon, 지원 서비스 추가(일본 Arario_Jpn) - 
#if defined(SERVICE_TYPE_JAPANESE_SERVER_1)	
	// 2009-02-04 by cmkwon, 일본 Arario 본섭/테섭 PreServerIP, DBServerIP 변경 - 기존 PreServer:203.191.254.181, 기존 DBServer:203.191.254.228
	// 2009-02-10 by cmkwon, 일본 Arario AdminTool에 서버군명 수정 및 추가 - 포트도 변경됨(기존 43957)
	// 2009-10-26 by cmkwon, 서버군통합, IDC 이전 작업 - 기존 PreServer:119.75.233.81, 기존 DBServer:119.75.233.28
	// 2010. 10. 04. by hsLee.	일본 서버 IP변경.	- 기본 PreServer:61.206.174.81, 기존 DBServer:61.206.174.28
	// PreServerIP		: 61.206.174.160			- 480956737E45066F516455180504
	// PreServerIP		: 192.168.70.21				- 4F014A6F7F4510705160551B02			// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )
	// DBServerIP		: 61.206.174.12				- 480956737E45066F5164551801
	// DBServerIP		: 192.168.66.28				- 4F014A6F7F4510705066551B0B			// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )
	// DBServerIP		: 192.168.66.35				- 4F014A6F7F4510705066551A06			// 2013-09-12 by jekim, 일본 IP 변경 ( VPN )
	// DBServerPort		: 40996
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: No##d/XGY3				- 30575B622A5C70193F63
	// LogDBServerIP	: 61.206.174.12				- 480956737E45066F5164551801
	// LogDBServerIP	: 192.168.66.28				- 4F014A6F7F4510705066551B0B			// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )
	// LogServerIP		: 192.168.66.35				- 4F014A6F7F4510705066551A06			// 2013-09-12 by jekim, 일본 IP 변경 ( VPN )
	// LogDBServerPort	: 40996
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: No##d/XGY3				- 30575B622A5C70193F63
// 2009-10-26 by cmkwon, 서버군통합, IDC 이전 작업 - 
//	{"Main      - Strike  (atum2_db_1)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Delta   (atum2_db_2)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_2", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Cross   (atum2_db_3)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_3", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Sigma   (atum2_db_4)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_4", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Omega   (atum2_db_5)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_5", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Chronicle(atum2_db_6)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_6", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Reserve1(atum2_db_7)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_7", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main      - Reserve2(atum2_db_8)",		"4F09416F7946066C5563551102", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_8", "4F09416F7946066C5563551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	// 2010. 10. 04. by hsLee. ServerIP 변경. - 기존 - PreServer : 61.206.174.81, Luna Server : 61.206.174.82, Stella Server : 61.206.174.83, DB Server : 61.206.174.28 
	// 2009-10-26 by cmkwon, 서버군통합, IDC 이전 작업 - 
// 	{"Main  - Luna(atum2_db_7)  <db_1+db_5>",			"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_7", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
// 	{"Main  - Stella(atum2_db_8)<db_2+db_3+db_4>",		"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_8", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Backup_Strike(db_1)",					"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Backup_Delta (db_2)",					"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_2", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Backup_Cross (db_3)",					"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_3", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Backup_Sigma (db_4)",					"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_4", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Backup_Omega (db_5)",					"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_5", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Reserve3(atum2_db_9)",				"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_9", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Main      - Reserve3(atum2_db_10)",				"480956737E45066F5164551102", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_10", "480956737E45066F5164551B0B", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	// 2010. 10. 04. by hsLee. ServerIP 변경.
 	{"Main	- Luna(atum2_db_7)  <db_1+db_5>",			"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_7", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main	- Luna_VPN(atum2_db_7)  <db_1+db_5>",		"4F014A6F7F4510705160551B02", "4F014A6F7F4510705066551A06", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_7", "4F014A6F7F4510705066551A06", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},

 	{"Main	- Stella(atum2_db_8)<db_2+db_3+db_4>",		"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_8", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main	- Stella_VPN(atum2_db_8)<db_2+db_3+db_4>",	"4F014A6F7F4510705160551B02", "4F014A6F7F4510705066551A06", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_8", "4F014A6F7F4510705066551A06", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},

	{"Main      - Backup_Strike(db_1)",					"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Backup_Delta (db_2)",					"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_2", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Backup_Cross (db_3)",					"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_3", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Backup_Sigma (db_4)",					"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_4", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Backup_Omega (db_5)",					"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_5", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Reserve3(atum2_db_9)",				"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_9", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Main      - Reserve3(atum2_db_10)",				"480956737E45066F516455180504", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_10", "480956737E45066F5164551801", 40996, "1F4C0D2C",	"30575B622A5C70193F63"},
	
	
	
	// 2009-02-04 by cmkwon, 일본 Arario 본섭/테섭 PreServerIP, DBServerIP 변경 - 기존 PreServer:203.191.254.156, 기존 DBServer:203.191.254.231
	// 2009-10-26 by cmkwon, 서버군통합, IDC 이전 작업 - 기존 PreServer:119.75.233.55, 기존 DBServer:119.75.233.51
	// 2009-02-04 by cmkwon, 일본 Arario 본섭/테섭 PreServerIP, DBServerIP 변경 - 기존 PreServer:203.191.254.156, 기존 DBServer:203.191.254.231
	// 2010-10-18 by shcho, 일본 테섭 IP / PORT번호 변경 - 기존 43957을 1433으로 변경
	// PreServerIP		: 61.206.174.70			- 480956737E45066F5164551E03
	// PreServerIP		: 192.168.68.9			- 4F014A6F7F45107050685510		// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )	
	// PreServerIP		: 61.206.174.179		- 480956737E45066F51645518040D	// 2013-09-12 by jekim, 일본 IP 변경
	// PreServerIP		: 192.168.70.179		- 4F014A6F7F45107051605518040D	// 2013-09-12 by jekim, 일본 IP 변경
	// DBServerIP		: 61.206.174.66			- 480956737E45066F5164551F05
	// DBServerIP		: 192.168.68.5			- 4F014A6F7F4510705068551C		// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )	
	// DBServerPort		: 1433  
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: No##d/XGY3				- 30575B622A5C70193F63
	// LogDBServerIP	: 61.206.174.66			- 480956737E45066F5164551F05
	// LogDBServerIP	: 192.168.68.5			- 4F014A6F7F4510705068551C		// 2013-04-12 by bckim, 일본 IP 변경 및 추가  ( VPN )	
	// LogDBServerPort	: 43957
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: No##d/XGY3				- 30575B622A5C70193F63
	{"Test   - TestServer(atum2_db_1)",		"480956737E45066F51645518040D", "480956737E45066F5164551F05", 1433, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "480956737E45066F5164551F05", 43957, "1F4C0D2C",	"30575B622A5C70193F63"},
	{"Test   - TestServer_VPN(atum2_db_1)",	"4F014A6F7F45107051605518040D", "4F014A6F7F4510705068551C", 1433, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "4F014A6F7F4510705068551C", 43957, "1F4C0D2C",	"30575B622A5C70193F63"},


	// 2012-04-03 by hskim, EP4 전달용 서버 추가
	// PreServerIP		: 61.206.174.52			- 480956737E45066F5164551C01		// Public IP 61.206.174.52
	// DBServerIP		: 61.206.174.54			- 480956737E45066F5164551C07
	// DBServerPort		: 1433  
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: No##d/HTT7			- 30575B622A5C600A3267
	//{"Test_EP4  - TestServer(atum2_db_1)",	"480956737E45066F5164551C01", "480956737E45066F5164551C07", 1433, "1F4C0D2C",	"30575B622A5C600A3267",		"atum2_db_1", "480956737E45066F5164551C07", 43957, "1F4C0D2C",	"30575B622A5C600A3267"},



// 2008-12-31 by cmkwon, 일본 정상적으로 설치 완료 되었다고 함. 임시용은 삭제
//	// 2008-12-10 by cmkwon, 일본 임시용
//	{"Temp for Alpha - Serv1   (atum2_db_1)",	"4C084B6F7F4A197054654F07020C15", "4C084B6F7F4A197054654F07010715", 43957, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_1", "4C084B6F7F4A197054654F07010715", 43957, "1F4C0D2C",	"30575B622A5C70193F63"},
//	{"Temp for Alpha - Serv2   (atum2_db_2)",	"4C084B6F7F4A197054654F07020C15", "4C084B6F7F4A197054654F07010715", 43957, "1F4C0D2C",	"30575B622A5C70193F63",		"atum2_db_2", "4C084B6F7F4A197054654F07010715", 43957, "1F4C0D2C",	"30575B622A5C70193F63"},

	// 2008-12-03 by cmkwon, 지원 서비스 추가(일본 Arario_Jpn) - 
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.222(48095672775D1969567E491B01)
	// PreServerIP	: 115.144.35.222	- 4F094D6F7F471C705565551B0106				61.39.170.222		- 48095672775D1969567E491B01
	// DBServerIP	: 115.144.35.222	- 4F094D6F7F471C705565551B0106				61.39.170.222		- 48095672775D1969567E491B01
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS222(Arario_Jpn)_(atum2_db_account)",	"4F094D6F7F471C705565551B0106", "4F094D6F7F471C705565551B0106", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C705565551B0106", 9979, "1F4C0D2C",	"1D59142D39164A"},

	// 2012-04-03 by hskim, EP4 전달용 서버 추가
	// PreServerIP	: 61.39.170.238		- 48095672775D1969567E491A0B
	// DBServerIP	: 61.39.170.238		- 48095672775D1969567E491A0B
	// DBServerPort	: 9222
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	//{"MS222_EP4(Arario_Jpn)_(atum2_db_account)",	"48095672775D1969567E491A0B", "48095672775D1969567E491A0B", 9222, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491A0B", 9222, "1F4C0D2C",	"1D59142D39164A"},

#endif // END - #if defined(SERVICE_TYPE_JAPANESE_SERVER_1)

	// 2008-12-22 by cmkwon, 지원 서비스 추가(Gameforge4D 터키아, 불어, 이탈리아어) -
#if defined(SERVICE_TYPE_TURKISH_SERVER_1)
//터키	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2008-12-22 by cmkwon, 지원 서비스 추가(Gameforge4D 터키아, 불어, 이탈리아어) - 현재는 정보 없음
	// 2009-01-15 by cmkwon, 터키 본섭 정보 설정 - 메인 서버 IP를 Domain으로 설정
	// 2009-05-18 by cmkwon, 터기 본섭 DBServer Port 변경 - 기존(9979)
	// 2012-05-23 by jhseol, 터기 본섭 DBServer Port 변경 - 기존(9980)
	// PreServerIP		: solus.airrivals.org		- 430E02503E071F305830281F1F0A464C46272A
	// DBServerIP		: speicher.airrivals.org	- 43110B4C2E411B2B0423281B0C0F4303452663115B42
	// DBServerPort		: 9979			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 터키
	// DB User			: atum						- 51151B48
	// DB PWD			: owm!qYPRr2Iti!cD4Xus-KS	- 5F1603043C702E0B5870081D174756261D0D380D046E70			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 터키
	// LogDBServerIP	: speicher.airrivals.org	- 43110B4C2E411B2B0423281B0C0F4303452663115B42
	// LogDBServerPort	: 9979			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 터키
	// DB User			: atum						- 51151B48
	// DB PWD			: owm!qYPRr2Iti!cD4Xus-KS	- 5F1603043C702E0B5870081D174756261D0D380D046E70			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 터키
	{"Main      - Server_1 (atum2_db_1)",		"430E02503E071F305830281F1F0A464C46272A", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70"},
	{"Main      - Server_2 (atum2_db_2)",		"430E02503E071F305830281F1F0A464C46272A", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70"},
	{"Main      - Server_3 (atum2_db_3)",		"430E02503E071F305830281F1F0A464C46272A", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70"},
	{"Main      - Reserve1 (atum2_db_4)",		"430E02503E071F305830281F1F0A464C46272A", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70"},
	{"Main      - Reserve2 (atum2_db_5)",		"430E02503E071F305830281F1F0A464C46272A", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F4303452663115B42", 9979, "51151B48",	"5F1603043C702E0B5870081D174756261D0D380D046E70"},
	
	// 2009-01-09 by dhjin, 지원 서비스 추가(Gameforge4D 터키아, 불어, 이탈리아어)
	// 2010-03-19 by cmkwon, 터키 테섭 IP 변경 - 기존(79.110.95.14)
	// PreServerIP		: 79.110.95.32			- 075840147C1950601F6C725B
	// DBServerIP		: 79.110.95.32			- 075840147C1950601F6C725B
	// DBServerPort		: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C
	// LogDBServerIP	: 79.110.95.32			- 075840147C1950601F6C725B
	// LogDBServerPort	: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 터키 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9981 )
	{"Test    - TestServer(atum2_db_1)",	"075840147C1950601F6C725B", "0151401D7D07476D047172", 9981, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9981, "51151B48",	"530002493A4C1C"},
	
	// 2008-12-22 by cmkwon, 지원 서비스 추가(Gameforge4D 터키아, 불어, 이탈리아어) -
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.223(0650401674074F6E1A6C735B4D)
	// PreServerIP	: 115.144.35.223		- 01505B0B7C1D4A7719776F5B4C55
	// DBServerIP	: 115.144.35.223		- 01505B0B7C1D4A7719776F5B4C55
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS223(Gameforge4D_Tur)_(atum2_db_account)",		"01505B0B7C1D4A7719776F5B4C55", "01505B0B7C1D4A7719776F5B4C55", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F5B4C55", 9979, "51151B48",	"530002493A4C1C"},

#endif // END - #if defined(SERVICE_TYPE_TURKISH_SERVER_1)

	// 2009-01-13 by cmkwon, 이탈리아 서비스 지원 관련 추가 - AdminTool,DBTool,Monitor 에 서버 리스트 추가
#if defined(SERVICE_TYPE_ITALIAN_SERVER_1)
//이탈	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2009-01-13 by cmkwon, 이탈리아 서비스 지원 관련 추가 - 마상 내버 서버정보
	// PreServerIP		: s1.airrivals.it			- 43504044245B0C305C232D1A500F41
	// DBServerIP		: speicher.airrivals.it		- 43110B4C2E411B2B0423281B0C0F4303452663175D
	// DBServerPort		: 9980			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 이탈리아
	// DB User			: atum						- 51151B48
	// DB PWD			: MW-igv!hSAeuOAN-DLKMKoZ	- 7D36434C2A5F5F317903241C31277B4F6D190633624A79			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 이탈리아
	// LogDBServerIP	: speicher.airrivals.it		- 43110B4C2E411B2B0423281B0C0F4303452663175D
	// LogDBServerPort	: 9980			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 이탈리아
	// DB User			: atum						- 51151B48
	// DB PWD			: MW-igv!hSAeuOAN-DLKMKoZ	- 7D36434C2A5F5F317903241C31277B4F6D190633624A79			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 이탈리아
	{"Main      - Server_1 (atum2_db_1)",		"43504044245B0C305C232D1A500F41", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79"},
	{"Main      - Server_2 (atum2_db_2)",		"43504044245B0C305C232D1A500F41", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79"},
	{"Main      - Server_3 (atum2_db_3)",		"43504044245B0C305C232D1A500F41", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79"},
	{"Main      - Reserve1 (atum2_db_4)",		"43504044245B0C305C232D1A500F41", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79"},
	{"Main      - Reserve2 (atum2_db_5)",		"43504044245B0C305C232D1A500F41", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F4303452663175D", 9980, "51151B48",	"7D36434C2A5F5F317903241C31277B4F6D190633624A79"},
	
	// 2009-01-20 by cmkwon, 이탈리아 테섭 정보 받아서 수정 - 
	// PreServerIP		: 79.110.95.16			- 075840147C1950601F6C705F
	// DBServerIP		: 79.110.95.16			- 075840147C1950601F6C705F
	// DBServerPort		: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C
	// LogDBServerIP	: 79.110.95.16			- 075840147C1950601F6C705F
	// LogDBServerPort	: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 이탈리아 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9982 )
	{"Test    - TestServer(atum2_db_1)",	"075840147C1950601F6C705F", "0151401D7D07476D047172", 9982, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9982, "51151B48",	"530002493A4C1C"},
	
	// 2009-01-13 by cmkwon, 이탈리아 서비스 지원 관련 추가 - 마상 내버 서버정보
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.224(0650401674074F6E1A6C735B4A)
	// PreServerIP	: 115.144.35.224		- 01505B0B7C1D4A7719776F5B4C52
	// DBServerIP	: 115.144.35.224		- 01505B0B7C1D4A7719776F5B4C52
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS224(Gameforge4D_Ita)_(atum2_db_account)",		"01505B0B7C1D4A7719776F5B4C52", "01505B0B7C1D4A7719776F5B4C52", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F5B4C52", 9979, "51151B48",	"530002493A4C1C"},
	
#endif // END - #if defined(SERVICE_TYPE_ITALIAN_SERVER_1)

	// 2009-03-03 by cmkwon, 지원 서비스 추가(Gameforge4D 프랑스어 관련) - 
#if defined(SERVICE_TYPE_FRENCH_SERVER_1)
//프랑	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2009-03-03 by cmkwon, 지원 서비스 추가(Gameforge4D 프랑스어 관련) - 본섭 정보
	// PreServerIP		: s1.airrivals.fr			- 43504044245B0C305C232D1A500047
	// DBServerIP		: speicher.airrivals.fr		- 43110B4C2E411B2B0423281B0C0F4303452663185B
	// DBServerPort		: 9980			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 프랑스
	// DB User			: atum						- 51151B48
	// DB PWD			: xsOlF!T8vVSD6-Z7QsJgOm	- 481221490B082A615C14122D484B6F55782607196648			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 프랑스
	// LogDBServerIP	: speicher.airrivals.fr		- 43110B4C2E411B2B0423281B0C0F4303452663185B
	// LogDBServerPort	: 9980			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 프랑스
	// DB User			: atum						- 51151B48
	// DB PWD			: xsOlF!T8vVSD6-Z7QsJgOm	- 481221490B082A615C14122D484B6F55782607196648			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 프랑스
	{"Main      - Server_1 (atum2_db_1)",		"43504044245B0C305C232D1A500047", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648"},
	{"Main      - Server_2 (atum2_db_2)",		"43504044245B0C305C232D1A500047", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648"},
	{"Main      - Server_3 (atum2_db_3)",		"43504044245B0C305C232D1A500047", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648"},
	{"Main      - Reserve1 (atum2_db_4)",		"43504044245B0C305C232D1A500047", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648"},
	{"Main      - Reserve2 (atum2_db_5)",		"43504044245B0C305C232D1A500047", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F4303452663185B", 9980, "51151B48",	"481221490B082A615C14122D484B6F55782607196648"},
	
	// 2009-03-03 by cmkwon, 지원 서비스 추가(Gameforge4D 프랑스어 관련) - 테섭 정보
	// PreServerIP		: 79.110.95.20			- 075840147C1950601F6C7359
	// DBServerIP		: 79.110.95.20			- 075840147C1950601F6C7359
	// DBServerPort		: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C
	// LogDBServerIP	: 79.110.95.20			- 075840147C1950601F6C7359
	// LogDBServerPort	: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 프랑스 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9983 )
	{"Test    - TestServer(atum2_db_1)",	"075840147C1950601F6C7359", "0151401D7D07476D047172", 9983, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9983, "51151B48",	"530002493A4C1C"},
	
	// 2009-03-03 by cmkwon, 지원 서비스 추가(Gameforge4D 프랑스어 관련) - 마상 내부 서버 정보
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.225(0650401674074F6E1A6C735B4B)
	// PreServerIP	: 115.144.35.225		- 01505B0B7C1D4A7719776F5B4C53
	// DBServerIP	: 115.144.35.225		- 01505B0B7C1D4A7719776F5B4C53
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS225(Gameforge4D_Fra)_(atum2_db_account)",		"01505B0B7C1D4A7719776F5B4C53", "01505B0B7C1D4A7719776F5B4C53", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F5B4C53", 9979, "51151B48",	"530002493A4C1C"},
	
#endif // END - #if defined(SERVICE_TYPE_FRENCH_SERVER_1)

	// 2009-06-04 by cmkwon, 지원 서비스 추가(Gameforge4D 폴란드어, 스페인어) - 추가함
#if defined(SERVICE_TYPE_POLISH_SERVER_1)
//폴란	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2009-06-04 by cmkwon, 지원 서비스 추가(Gameforge4D 폴란드어, 스페인어) - 본섭 정보
	// PreServerIP		: s1.airrivals.pl			- 43504044245B0C305C232D1A501659
	// DBServerIP		: speicher.airrivals.pl		- 43110B4C2E411B2B0423281B0C0F43034526630E45
	// DBServerPort		: 9981			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 폴란드
	// DB User			: atum						- 51151B48
	// DB PWD			: thYzIA!6oX-71Qs!SdHOGyN	- 4409375F04685F6F451A6C5E4F3746437A3105316E5C6D			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 폴란드
	// LogDBServerIP	: speicher.airrivals.pl		- 43110B4C2E411B2B0423281B0C0F43034526630E45
	// LogDBServerPort	: 9981			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 폴란드
	// DB User			: atum						- 51151B48
	// DB PWD			: thYzIA!6oX-71Qs!SdHOGyN	- 4409375F04685F6F451A6C5E4F3746437A3105316E5C6D			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 폴란드
	{"Main      - Server_1 (atum2_db_1)",		"43504044245B0C305C232D1A501659", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D"},
	{"Main      - Server_2 (atum2_db_2)",		"43504044245B0C305C232D1A501659", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D"},
	{"Main      - Server_3 (atum2_db_3)",		"43504044245B0C305C232D1A501659", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D"},
	{"Main      - Reserve1 (atum2_db_4)",		"43504044245B0C305C232D1A501659", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D"},
	{"Main      - Reserve2 (atum2_db_5)",		"43504044245B0C305C232D1A501659", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F43034526630E45", 9981, "51151B48",	"4409375F04685F6F451A6C5E4F3746437A3105316E5C6D"},
	
	// 2009-06-04 by cmkwon, 지원 서비스 추가(Gameforge4D 폴란드어, 스페인어) - 테섭 정보
	// PreServerIP		: test.airrivals.pl		- 44041D516348172B582B370812151B1245
	// DBServerIP		: test.airrivals.pl		- 44041D516348172B582B370812151B1245
	// DBServerPort		: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C
	// LogDBServerIP	: test.airrivals.pl		- 44041D516348172B582B370812151B1245
	// LogDBServerPort	: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 폴란드 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9984 )
	{"Test    - TestServer(atum2_db_1)",	"44041D516348172B582B370812151B1245", "0151401D7D07476D047172", 9984, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9984, "51151B48",	"530002493A4C1C"},
	
	// 2009-03-03 by cmkwon, 지원 서비스 추가(Gameforge4D 프랑스어 관련) - 마상 내부 서버 정보
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.226(0650401674074F6E1A6C735B48)
	// PreServerIP	: 115.144.35.226		- 01505B0B7C1D4A7719776F5B4C50
	// DBServerIP	: 115.144.35.226		- 01505B0B7C1D4A7719776F5B4C50
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS226(Gameforge4D_Pol)_(atum2_db_account)",		"01505B0B7C1D4A7719776F5B4C50", "01505B0B7C1D4A7719776F5B4C50", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F5B4C50", 9979, "51151B48",	"530002493A4C1C"},
	
#endif // END - #if defined(SERVICE_TYPE_POLISH_SERVER_1)

	// 2009-07-10 by cmkwon, Gameforge4D 스페인 서비스 시작 준비 - 
#if defined(SERVICE_TYPE_SPANISH_SERVER_1)
//스페	//#define STR_XOR_KEY_STRING_SERVER_INFO						"0an%M)~Y*BAi~f5b)UM~)%#058v~#YVV~&#@"			// 2012-07-26 by jhseol, 게임포지 Key 변경
	// 2009-07-10 by cmkwon, Gameforge4D 스페인 서비스 시작 준비 - 본섭 정보
	// PreServerIP		: s1.airrivals.es			- 43504044245B0C305C232D1A500346
	// DBServerIP		: speicher.airrivals.es		- 43110B4C2E411B2B0423281B0C0F43034526631B5A
	// DBServerPort		: 9982			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 스페인
	// DB User			: atum						- 51151B48
	// DB PWD			: MvB-bfYSX7whEh77ta!EgH	- 7D172C082F4F270A727536013B0E02555D346C3B4E6D			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 스페인
	// LogDBServerIP	: speicher.airrivals.es		- 43110B4C2E411B2B0423281B0C0F43034526631B5A
	// LogDBServerPort	: 9982			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 포트정리 스페인
	// DB User			: atum						- 51151B48
	// DB PWD			: MvB-bfYSX7whEh77ta!EgH	- 7D172C082F4F270A727536013B0E02555D346C3B4E6D			// 2012-07-26 by jhseol, 게임포지 Key 변경 - 암호변경 스페인
	{"Main      - Server_1 (atum2_db_1)",		"43504044245B0C305C232D1A500346", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D",		"atum2_db_1", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D"},
	{"Main      - Server_2 (atum2_db_2)",		"43504044245B0C305C232D1A500346", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D",		"atum2_db_2", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D"},
	{"Main      - Server_3 (atum2_db_3)",		"43504044245B0C305C232D1A500346", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D",		"atum2_db_3", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D"},
	{"Main      - Reserve1 (atum2_db_4)",		"43504044245B0C305C232D1A500346", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D",		"atum2_db_4", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D"},
	{"Main      - Reserve2 (atum2_db_5)",		"43504044245B0C305C232D1A500346", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D",		"atum2_db_5", "43110B4C2E411B2B0423281B0C0F43034526631B5A", 9982, "51151B48",	"7D172C082F4F270A727536013B0E02555D346C3B4E6D"},
	
	// 2009-07-10 by cmkwon, Gameforge4D 스페인 서비스 시작 준비 - 테섭 정보
	// PreServerIP		: test.airrivals.es		- 44041D516348172B582B370812151B075A
	// DBServerIP		: test.airrivals.es		- 44041D516348172B582B370812151B075A
	// DBServerPort		: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C
	// LogDBServerIP	: test.airrivals.es		- 44041D516348172B582B370812151B075A
	// LogDBServerPort	: 9979
	// DB User			: atum					- 51151B48
	// DB PWD			: callweb				- 530002493A4C1C

	// 2013-12-06 by bckim, 테스트서버 DB서버IP,PORT 변경 // 스페인 ( IP : 10.80.94.33(0151401D7D07476D047172) ,  PORT : 9985 )
	{"Test    - TestServer(atum2_db_1)",	"44041D516348172B582B370812151B075A", "0151401D7D07476D047172", 9985, "51151B48",	"530002493A4C1C",		"atum2_db_1", "0151401D7D07476D047172", 9985, "51151B48",	"530002493A4C1C"},
	
	// 2009-07-10 by cmkwon, Gameforge4D 스페인 서비스 시작 준비 - 마상 내부 서버 정보
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.227(0650401674074F6E1A6C735B49)
	// PreServerIP	: 115.144.35.227		- 01505B0B7C1D4A7719776F5B4C51
	// DBServerIP	: 115.144.35.227		- 01505B0B7C1D4A7719776F5B4C51
	// DB User		: atum				- 51151B48
	// DB PWD		: callweb			- 530002493A4C1C
	{"MS227(Gameforge4D_Esp)_(atum2_db_account)",		"01505B0B7C1D4A7719776F5B4C51", "01505B0B7C1D4A7719776F5B4C51", 9979, "51151B48",	"530002493A4C1C",	"atum2_db_1",		"01505B0B7C1D4A7719776F5B4C51", 9979, "51151B48",	"530002493A4C1C"},
	
#endif // END - #if defined(SERVICE_TYPE_SPANISH_SERVER_1)

	// 2010-11-01 by shcho, Lin 아르헨티나 서비스 시작 준비 - 
#if defined(SERVICE_TYPE_ARGENTINA_SERVER_1)
	// 2010-11-01 by shcho, Lin 아르헨티나 서비스 시작 준비 - 본섭 정보
	// PreServerIP		: 209.251.187.183			- 4C08416F7C46197057684C07020C17
	// DBServerIP		: 10.122.174.214			- 4F0856707C41066F5164551B0200
	// DBServerPort		: 9979
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: callweb					- 1D59142D39164A
	// LogDBServerIP	: 10.122.174.214			- 4F0856707C41066F5164551B0200
	// LogDBServerPort	: 9979
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: callweb					- 1D59142D39164A
	
	// arg_main - // Lin_Arg_Main_PreServer(209.251.187.183) = 4C08416F7C46197057684C07020C17
	{"Main      - Server_1 (atum2_db_1)",		"4C08416F7C46197057684C07020C17", "4F0856707C41066F5164551B0200", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F0856707C41066F5164551B0200", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
	// 2010-11-01 by shcho, Lin 아르헨티나 서비스 시작 준비 - 테섭 정보
	// PreServerIP		: 209.251.187.187		- 4C08416F7C46197057684C07020C13
	// DBServerIP		: 209.251.187.187		- 4C08416F7C46197057684C07020C13
	// DBServerPort		: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	// LogDBServerIP	: test.airrivals.es		- 0A5D0B356012412C14390D485F470A4D15
	// LogDBServerPort	: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	// 10.121.60.141 4F0856707C420668567E4A1D02
	// arg_test - // Lin_Arg_Test_PreServer(209.251.187.187) = 4C08416F7C46197057684C07020C13
	{"Test    - TestServer(atum2_db_1)",	"4F0856707C420668567E4A1D02", "4F0856707C420668567E4A1D02", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F0856707C420668567E4A1D02", 9979, "1F4C0D2C",	"1D59142D39164A"},

	// PreServerIP	: 115.144.35.228		- 4F094D6F7F471C705565551B010C
	// DBServerIP	: 115.144.35.228		- 4F094D6F7F471C705565551B010C
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS228(Lin_Arg)_(atum2_db_account)",		"4F094D6F7F471C705565551B010C", "4F094D6F7F471C705565551B010C", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C705565551B010C", 9979, "1F4C0D2C",	"1D59142D39164A"},

	/// 2012-04-19 by jhseol, EP4 전달용 서버 추가
	// PreServerIP		: 209.251.185.12			- 4C08416F7C46197057684E070206				// Local IP = PreServerIP		: 10.121.60.161			- 4F0856707C420668567E4A1F02
	// DBServerIP		: 209.251.185.12			- 4C08416F7C46197057684E070206				// Local IP = DBServerIP		: 10.121.60.161			- 4F0856707C420668567E4A1F02
	// DBServerPort		: 9979
	// DB User			: sa					- 0D59
	// DB PWD			: pAkuK2C@				- 0E79133405416B1E
	// LogDBServerIP	: 209.251.185.12			- 4C08416F7C46197057684E070206
	// LogDBServerPort	: 9979
	// DB User			: sa					- 0D59
	// DB PWD			: pAkuK2C@				- 0E79133405416B1E
	{"Test_EP4 - TestServer(atum2_db_1)",	"4C08416F7C46197057684E070206", "4C08416F7C46197057684E070206", 9979, "0D59",	"0E79133405416B1E",		"atum2_db_1", "4C08416F7C46197057684E070206", 9979, "0D59",	"0E79133405416B1E"},

	// 2009-07-10 by cmkwon, Gameforge4D 스페인 서비스 시작 준비 - 마상 내부 서버 정보
	// 2009-12-28 by cmkwon, 마상회사 IP 변경 - 61.39.170.228(48095672775D1969567E491B0B)
	// PreServerIP	: 61.39.170.228		- 48095672775D1969567E491B0B
	// DBServerIP	: 61.39.170.228		- 48095672775D1969567E491B0B
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	// {"MS228(Lin_Ang)_(atum2_db_account)",		"48095672775D1969567E491B0B", "48095672775D1969567E491B0B", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491B0B", 9979, "1F4C0D2C",	"1D59142D39164A"},


	/// 2012-04-19 by jhseol, EP4 전달용 서버 추가 - 61.39.170.238(48095672775D1969567E491A0B)
	{"MS228_EP4(LIN_Arg)_(atum2_db_1)",		"48095672775D1969567E491A0B", "48095672775D1969567E491A0B", 9228, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491A0B", 9228, "1F4C0D2C",	"1D59142D39164A"},

	
#endif // END - #if defined(SERVICE_TYPE_ARGENTINA_SERVER_1)

#if defined(SERVICE_TYPE_SINGAPORE_1)	
	// 2010-12-07 by shcho,	 서비스 시작 준비 (WinnerOnline 영어) - 본섭 정보
	// PreServerIP		: 114.129.32.181(외부)		- 4F094C6F7F411170556255180B05
	// DBServerIP		: 114.129.32.170(외부)		- 4F094C6F7F411170556255180404 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// DBServerPort		: 9979
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: callweb					- 1D59142D39164A
	// LogDBServerIP	: 114.129.32.170(외부)		- 4F094C6F7F411170556255180404 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// LogDBServerPort	: 9979
	// DB User			: atum						- 1F4C0D2C
	// DB PWD			: callweb					- 1D59142D39164A
	{"Main      - Server_1 (atum2_db_1)",		"4F094C6F7F411170556255180B05", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{"Main      - Server_2 (atum2_db_2)",		"4F094C6F7F411170556255180B05", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_2", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{"Main      - Server_3 (atum2_db_3)",		"4F094C6F7F411170556255180B05", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_3", "4F094C6F7F411170556255180404", 9979, "1F4C0D2C",	"1D59142D39164A"},

	
	// 2010-12-07 by shcho,	 서비스 시작 준비 (WinnerOnline 영어) - 테섭 정보
	// PreServerIP		: 180.178.110.34(외부)	- 4F00486F7F44107057614B070000
	// DBServerIP		: 180.178.110.33(외부)	- 4F00486F7F44107057614B070007 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// DBServerPort		: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	// LogDBServerIP	: 180.178.110.33(외부)	- 4F00486F7F44107057614B070007 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// LogDBServerPort	: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	{"Test    - TestServer1(atum2_db_1)",	"4F00486F7F44107057614B070000", "4F00486F7F44107057614B070007", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F00486F7F44107057614B070007", 9979, "1F4C0D2C",	"1D59142D39164A"},
	// 2010-12-07 by shcho,	 서비스 시작 준비 (WinnerOnline 영어) - 테섭 정보
	// PreServerIP		: 114.129.32.166(외부)	- 4F094C6F7F411170556255180502
	// DBServerIP		: 114.129.32.165(외부)	- 4F094C6F7F411170556255180501 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// DBServerPort		: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	// LogDBServerIP	: 114.129.32.165(외부)	- 4F094C6F7F411170556255180501 // 2010-12-21 by shcho, 외부 IP로 변경 - 기존 내부 IP
	// LogDBServerPort	: 9979
	// DB User			: atum					- 1F4C0D2C
	// DB PWD			: callweb				- 1D59142D39164A
	{"Test    - TestServer2(atum2_db_1)",	"4F094C6F7F411170556255180502", "4F094C6F7F411170556255180501", 9979, "1F4C0D2C",	"1D59142D39164A",		"atum2_db_1", "4F094C6F7F411170556255180501", 9979, "1F4C0D2C",	"1D59142D39164A"},

	// 2010-12-07 by shcho,	 서비스 시작 준비 (WinnerOnline 영어) - 마상 내부 테섭 정보
	// PreServerIP	: 61.39.170.202		- 48095672775D1969567E491901
	// DBServerIP	: 61.39.170.202		- 48095672775D1969567E491901
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS202(WinnerOnline_Sgp)_(atum2_db_account)",		"48095672775D1969567E491901", "48095672775D1969567E491901", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491901", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
#endif // END - #if defined(SERVICE_TYPE_SINGAPORE_1)

#if defined(SERVICE_TYPE_INDONESIA_SERVER_1)
	
	// 2010-01-11 by shcho,	 지원 서비스 추가(WinnerOnline 인도네시아어) - 마상 내부 서버 셋팅
	// atum2_db_1
	// PreServerIP	: 180.178.110.70	- 4F00486F7F44107057614B070404
	// DBServerIP	: 180.178.110.72	- 4F00486F7F44107057614B070406
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A

	{"Main      - Server_1 (atum2_db_1)",		"4F00486F7F44107057614B070404", "4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",	"4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{"Main      - Server_2 (atum2_db_2)",		"4F00486F7F44107057614B070404", "4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_2",	"4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A"},
	{"Main      - Server_3 (atum2_db_3)",		"4F00486F7F44107057614B070404", "4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_3",	"4F00486F7F44107057614B070406", 9979, "1F4C0D2C",	"1D59142D39164A"},

	// 2010-01-11 by shcho,	 지원 서비스 추가(WinnerOnline 인도네시아어) - 마상 내부 서버 셋팅
	// atum2_db_1
	// PreServerIP	: 180.178.110.34	- 4F00486F7F44107057614B070000
	// DBServerIP	: 180.178.110.33	- 4F00486F7F44107057614B070007
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"Test      - Server_1 (atum2_db_1)",		"4F00486F7F44107057614B070000", "4F00486F7F44107057614B070007", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",	"4F00486F7F44107057614B070007", 9979, "1F4C0D2C",	"1D59142D39164A"},

	// 2010-01-11 by shcho,	 지원 서비스 추가(WinnerOnline 인도네시아어) - 마상 내부 서버 셋팅
	// PreServerIP	: 61.39.170.203		- 48095672775D1969567E491900
	// DBServerIP	: 61.39.170.203		- 48095672775D1969567E491900
	// DB User		: atum				- 1F4C0D2C
	// DB PWD		: callweb			- 1D59142D39164A
	{"MS203(WinnerOnline_Idn)_(atum2_db_account)",		"48095672775D1969567E491900", "48095672775D1969567E491900", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"48095672775D1969567E491900", 9979, "1F4C0D2C",	"1D59142D39164A"},
#endif //defined(SERVICE_TYPE_INDONESIA_SERVER_1)

#if defined(SERVICE_TYPE_CHINESE_SERVER_1)					// 2013-07-02 by jhseol, 중국 테스트서버 구축
	//중국
	// PreServerIP		: 172.31.3.242	- 4F0F4A6F7D42066D48624F1B
	// PreServerIP		: 54.250.154.14	- 4B0C56737B43066F5364551807
	// DBServerIP		: 54.250.154.14	- 4B0C56737B43066F5364551807
	// 127.0.0.1 - 4F0A4F6F7E5D187057
	// DBServerPort		: 9979
	// DB User			: atum			- 1F4C0D2C
	// DB PWD			: callweb		- 1D59142D39164A
	{"China Promotion Server_1 (atum2_db_1)",				"4F0F4A6F7D42066D48624F1B", "4F0A4F6F7E5D187057", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A4F6F7E5D187057", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
	// PreServerIP		: 115.144.35.146	- 4F094D6F7F471C70556555180702
	// DBServerIP		: 115.144.35.146	- 4F094D6F7F471C70556555180702
	// DBServerPort		: 9979
	// DB User			: atum			- 1F4C0D2C
	// DB PWD			: callweb		- 1D59142D39164A
	{"MS146 (YETIME_CHN)_(atum2_db_1)",					"4F094D6F7F471C70556555180702", "4F094D6F7F471C70556555180702", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C70556555180702", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
#endif // END - #if defined(SERVICE_TYPE_CHINESE_SERVER_1)

#if defined(SERVICE_TYPE_GLOBAL_SERVER_1)					// 2013-09-04 by jhseol, 글로벌 서버 빌드옵션 추가
	//글로벌
	// PreServerIP		: 172.31.3.242	- 4F0F4A6F7D42066D48624F1B
	// PreServerIP		: 54.250.154.14	- 4B0C56737B43066F5364551807
	// DBServerIP		: 54.250.154.14	- 4B0C56737B43066F5364551807
	// 127.0.0.1 - 4F0A4F6F7E5D187057
	// DBServerPort		: 9979
	// DB User			: atum			- 1F4C0D2C
	// DB PWD			: callweb		- 1D59142D39164A
	{"Masang Galbal Server_1 (atum2_db_1)",				"4F0F4A6F7D42066D48624F1B", "4F0A4F6F7E5D187057", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A4F6F7E5D187057", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
	// PreServerIP		: 115.144.35.146	- 4F094D6F7F471C70556555180702
	// DBServerIP		: 115.144.35.146	- 4F094D6F7F471C70556555180702
	// DBServerPort		: 9979
	// DB User			: atum			- 1F4C0D2C
	// DB PWD			: callweb		- 1D59142D39164A
	{"MS000 (MASAMG_GLOBAL)_(atum2_db_1)",					"4F094D6F7F471C70556555180702", "4F094D6F7F471C70556555180702", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F094D6F7F471C70556555180702", 9979, "1F4C0D2C",	"1D59142D39164A"},
	
#endif // END - #if defined(SERVICE_TYPE_CHINESE_SERVER_1)
	
// 2008-08-29 by cmkwon, AdminTool,DBTool 서버군 리스트 관련 수정(서비스별로 보이게) - 
// #else // _ATUM_ADMIN_RELEASE
// 	// 2008-08-20 by cmkwon, 추가 및 수정 함. - 아래의 이름 모두 수정 함.
// 
// 	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.140(4F0A496F7F401C7057614F07020014), 121.134.114.140(4F0A496F7F401C7057614F07020014), atum(1F4C0D2C), callweb(1D59142D39164A)
// 	{"MS140(Masang_Kor)_(atum2_db_account)",				"4F0A496F7F401C7057614F07020014", "4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS140(Masang)_Kor(atum2_db_account_test)",		"4F0A496F7F401C7057614F07020014", "4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 
// 	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.141(4F0A496F7F401C7057614F07020015), 121.134.114.141(4F0A496F7F401C7057614F07020015), atum(1F4C0D2C), callweb(1D59142D39164A)
// 	{"MS141(Yedang_Kor)_(atum2_db_account)",				"4F0A496F7F401C7057614F07020015", "4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A"},			// 2006-09-21 by cmkwon
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS141(Yedang)_Kor(atum2_db_account_test)",		"4F0A496F7F401C7057614F07020015", "4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020015", 9979, "1F4C0D2C",	"1D59142D39164A"},		// 2006-09-21 by cmkwon
// 
// 	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.142(4F0A496F7F401C7057614F07020016), 121.134.114.142(4F0A496F7F401C7057614F07020016), atum(1F4C0D2C), callweb(1D59142D39164A)
// 	{"MS142(Wikigames_Eng)_(atum2_db_account)",		"4F0A496F7F401C7057614F07020016", "4F0A496F7F401C7057614F07020016", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020016", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS142(Yedang-Global)_Eng(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020016", "4F0A496F7F401C7057614F07020016", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020016", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	
// 	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.143(4F0A496F7F401C7057614F07020017), 121.134.114.143(4F0A496F7F401C7057614F07020017), atum(1F4C0D2C), callweb(1D59142D39164A)
// 	{"MS143(VTC-Intecom_Viet)_(atum2_db_account)",		"4F0A496F7F401C7057614F07020017", "4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A"},			// 2006-03-24 by cmkwon
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS143(VTC-Intecom)_Viet(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020017", "4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020017", 9979, "1F4C0D2C",	"1D59142D39164A"},		// 2006-03-24 by cmkwon
// 	
// 	// 2007-10-24 by cmkwon, 서버 정보 암호화 - 121.134.114.146(4F0A496F7F401C7057614F07020012), 121.134.114.146(4F0A496F7F401C7057614F07020012), atum(1F4C0D2C), callweb(1D59142D39164A)
// 	{"MS146(Yetime_Chn)_(atum2_db_account)",				"4F0A496F7F401C7057614F07020012", "4F0A496F7F401C7057614F07020012", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020012", 9979, "1F4C0D2C",	"1D59142D39164A"},			// 2007-06-13 by dhjin
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS146(Yetime)_Chn(atum2_db_account_test)",		"4F0A496F7F401C7057614F07020012", "4F0A496F7F401C7057614F07020012", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020012", 9979, "1F4C0D2C",	"1D59142D39164A"},		// 2007-06-13 by dhjin
// 
// 	// 2008-01-04 by cmkwon, Gameforge4D 추가함 - 
// 	// PreServerIP	: 121.134.114.145	- 4F0A496F7F401C7057614F07020011
// 	// DBServerIP	: 121.134.114.145	- 4F0A496F7F401C7057614F07020011
// 	// DB User		: atum				- 1F4C0D2C
// 	// DB PWD		: callweb			- 1D59142D39164A
// 	{"MS145(Gameforge4D_Eng)_(atum2_db_account)",		"4F0A496F7F401C7057614F07020011", "4F0A496F7F401C7057614F07020011", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020011", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS145(Gameforge4D_Eng)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020011", "4F0A496F7F401C7057614F07020011", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020011", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 
// 	// 2008-04-25 by cmkwon, Gameforge4D_Deu 추가함 - 
// 	// PreServerIP	: 121.134.114.147	- 4F0A496F7F401C7057614F07020013
// 	// DBServerIP	: 121.134.114.147	- 4F0A496F7F401C7057614F07020013
// 	// DB User		: atum				- 1F4C0D2C
// 	// DB PWD		: callweb			- 1D59142D39164A
// 	{"MS147(Gameforge4D_Deu)_(atum2_db_account)",		"4F0A496F7F401C7057614F07020013", "4F0A496F7F401C7057614F07020013", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020013", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	{"MS147(Gameforge4D_Deu_MySQL)_(atum2_db_account)",	"4F0A496F7F401C7057614F07020013", "4F0A496F7F401C7057614F07020013", 3306, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020013", 3306, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS147(Gameforge4D_Deu)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07020013", "4F0A496F7F401C7057614F07020013", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020013", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 
// 	// 2008-05-07 by cmkwon, WinnerOnline_Tha 추가하기 - 
// 	// PreServerIP	: 121.134.114.148	- 4F0A496F7F401C7057614F0702001C
// 	// DBServerIP	: 121.134.114.148	- 4F0A496F7F401C7057614F0702001C
// 	// DB User		: atum				- 1F4C0D2C
// 	// DB PWD		: callweb			- 1D59142D39164A
// 	{"MS148(WinnerOnline_Tha)_(atum2_db_account)",		"4F0A496F7F401C7057614F0702001C", "4F0A496F7F401C7057614F0702001C", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS148(WinnerOnline_Tha)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F0702001C", "4F0A496F7F401C7057614F0702001C", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 
// 	// 2008-06-19 by cmkwon, Innova_Rus 내부 테스트 서버 추가 - 
// 	// PreServerIP	: 121.134.114.220	- 4F0A496F7F401C7057614F07010614
// 	// DBServerIP	: 121.134.114.220	- 4F0A496F7F401C7057614F07010614
// 	// DB User		: atum				- 1F4C0D2C
// 	// DB PWD		: callweb			- 1D59142D39164A
// 	{"MS220(Innova_Rus)_(atum2_db_account)",		"4F0A496F7F401C7057614F07010614", "4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 	// 2008-08-20 by cmkwon, 제거함
// 	//{"MS220(Innova_Rus)_(atum2_db_account_test)",	"4F0A496F7F401C7057614F07010614", "4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_test",	"4F0A496F7F401C7057614F07010614", 9979, "1F4C0D2C",	"1D59142D39164A"},
// 
// 	// 2008-04-30 by cmkwon, 테스트용으로 추가 한 것
// 	// PreServerIP	: 121.134.114.150	- 4F0A496F7F401C7057614F07020114
// 	{"MS150(Masang_Kor)_(atum2_db_account)",		"4F0A496F7F401C7057614F07020114", "4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A",	"atum2_db_1",		"4F0A496F7F401C7057614F07020014", 9979, "1F4C0D2C",	"1D59142D39164A"},
// #endif // _ATUM_ADMIN_RELEASE
	{NULL, NULL, NULL, 0, NULL, NULL}
	// ServerName, PreServerIP, DBIP, DBPort, DBUID, DBPWD, DBName
};
#else // _ATUM_CLIENT
GAME_SERVER_INFO_FOR_ADMIN g_arrGameServers[] = 
{
	{NULL, NULL, NULL, 0, NULL, NULL}
};
#endif // _ATUM_CLIENT


///////////////////////////////////////////////////////////////////////////////
/// \fn			void GDecryptGameServerInfoByXOR(void)
/// \brief		// 2007-10-24 by cmkwon, 서버 정보 암호화 -
/// \author		cmkwon
/// \date		2007-10-24 ~ 2007-10-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void GDecryptGameServerInfoByXOR(void)
{
#ifndef _ATUM_CLIENT

	for(int i = 0; g_arrGameServers[i].ServerName != NULL; i++)
	{
		BYTE byEncodedBinary[1024];

		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, g_arrGameServers[i].XOREncodedServerIP))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].ServerIP, byEncodedBinary, strlen(g_arrGameServers[i].XOREncodedServerIP)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}

		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, g_arrGameServers[i].XOREncodedDBIP))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].DBIP, byEncodedBinary, strlen(g_arrGameServers[i].XOREncodedDBIP)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}

		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, g_arrGameServers[i].XOREncodedDBUID))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].DBUID, byEncodedBinary, strlen(g_arrGameServers[i].XOREncodedDBUID)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}

		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, g_arrGameServers[i].XOREncodedDBPWD))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].DBPWD, byEncodedBinary, strlen(g_arrGameServers[i].XOREncodedDBPWD)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}
		
		///////////////////////////////////////////////////////////////////////////////		
		// 2007-11-07 by cmkwon, 로그 DB 서버 따로 구축하기 - GAME_SERVER_INFO_FOR_ADMIN 구초제에 4개 필드 추가
		// 2007-11-07 by cmkwon, LogDBIP, LogDBUID, LogDBPWD 의 필드는 인코딩 되어 있으므로 아래와 같이 복호화 해야 함
		char szEncryptData[1024];

		memset(szEncryptData, 0x00, 1024);
		strncpy(szEncryptData, g_arrGameServers[i].LogDBIP, 2*SIZE_MAX_ADDRESS - 1);		// 2009-01-15 by cmkwon, PreServer, DBServer 정보 DNS로 설정 가능하게 수정 - 기존(SIZE_MAX_IPADDRESS)
		memset(g_arrGameServers[i].LogDBIP, 0x00, 2*SIZE_MAX_ADDRESS);						// 2009-01-15 by cmkwon, PreServer, DBServer 정보 DNS로 설정 가능하게 수정 - 기존(SIZE_MAX_IPADDRESS)
		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, szEncryptData))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].LogDBIP, byEncodedBinary, strlen(szEncryptData)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}

		memset(szEncryptData, 0x00, 1024);
		strncpy(szEncryptData, g_arrGameServers[i].LogDBUID, 2*SIZE_MAX_DB_USER_ID - 1);
		memset(g_arrGameServers[i].LogDBUID, 0x00, 2*SIZE_MAX_DB_USER_ID);
		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, szEncryptData))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].LogDBUID, byEncodedBinary, strlen(szEncryptData)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}
		
		memset(szEncryptData, 0x00, 1024);
		strncpy(szEncryptData, g_arrGameServers[i].LogDBPWD, 2*SIZE_MAX_DB_USER_PWD - 1);
		memset(g_arrGameServers[i].LogDBPWD, 0x00, 2*SIZE_MAX_DB_USER_PWD);
		memset(byEncodedBinary, 0x00, 1024);
		if(XOR::XORString2Binary(byEncodedBinary, szEncryptData))
		{
			XOR::XOREncode((BYTE*)g_arrGameServers[i].LogDBPWD, byEncodedBinary, strlen(szEncryptData)/2, STR_XOR_KEY_STRING_SERVER_INFO);
		}
	}
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			char *GetDayOfWeekString(int i_nDayOfWeek)
/// \brief		// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 -
/// \author		cmkwon
/// \date		2007-10-30 ~ 2007-10-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *GetDayOfWeekString(int i_nDayOfWeek)
{
	switch(i_nDayOfWeek)
	{
	case DAY_OF_WEEK_SUMDAY:				return "Sunday";
	case DAY_OF_WEEK_MONDAY:				return "Monday";
	case DAY_OF_WEEK_TUESDAY:				return "Tuesday";
	case DAY_OF_WEEK_WEDNESDAY:				return "Wednesday";
	case DAY_OF_WEEK_THURSDAY:				return "Thursday";
	case DAY_OF_WEEK_FRIDAY:				return "Friday";
	case DAY_OF_WEEK_SATURDAY:				return "Saturday";
	case DAY_OF_WEEK_HAPPYHOUREVENT_PERIOD:	return "HappyHourEventPeriod";
	case DAY_OF_WEEK_PCBANG_HAPPYHOUREVENT:	return "PCBangHappyHourEventPeriod";
	}
	return "Unknown day of week";	
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			char *GetWPUTString(BYTE i_byWPUT)
/// \brief		// 2009-01-22 by cmkwon, 전쟁 관련 게임 로그 수정 - 
/// \author		cmkwon
/// \date		2009-01-22 ~ 2009-01-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *GetWPUTString(BYTE i_byWPUT)
{
	switch(i_byWPUT)
	{
	case WPUT_GENERAL:				return "WPUT_GENERAL";
	case WPUT_ARENA:				return "WPUT_ARENA";
	case WPUT_MOTHERSHIPWAR:		return "WPUT_MOTHERSHIPWAR";
	case WPUT_STRATEGYPOINTWAR:		return "WPUT_STRATEGYPOINTWAR";
	case WPUT_OUTPOSTWAR:			return "WPUT_OUTPOSTWAR";
	case WPUT_SHOP:					return "WPUT_SHOP";
	case WPUT_ADMIN_COMMAND:		return "WPUT_ADMIN_COMMAND";
	case WPUT_TRIGGER:				return "WPUT_TRIGGER";			// 2011-11-07 by hskim, EP4 [트리거 시스템] - NCG 거점전 시스템
	case WPUT_MARKET:				return "WPUT_MARKET";	 		// 2013-11-25 by jhseol&bckim, 거래소
	}

	return WPUT_UNKNOWN;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			char *LowercaseOnly_i_forTurkish(char *i_szString, int i_nMaxLen)
/// \brief		// 2009-01-23 by cmkwon, 터키 i,I 문자 문제 처리 -
/// \author		cmkwon
/// \date		2009-01-23 ~ 2009-01-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *LowercaseOnly_i_forTurkish(char *i_szString, int i_nMaxLen)
{
#if defined(SERVICE_TYPE_TURKISH_SERVER_1)
	char *	pCurChar	= i_szString;
	int		nLen		= min(i_nMaxLen, strlen(i_szString));
	while(pCurChar && pCurChar < i_szString + nLen)
	{
		// 2009-01-23 by cmkwon, I문자만을 i로 변경
		if('I' == *pCurChar)
		{
			*pCurChar = 'i';
		}
		
		pCurChar = CharNext(pCurChar);		// 2007-12-05 by cmkwon, 다음 문자로 이동
	}
#endif
	return i_szString;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int GetArrIdxByUnitMaskforWRK(INT i_unitMask)
/// \brief		// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
/// \author		cmkwon
/// \date		2009-02-18 ~ 2009-02-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int GetArrIdxByUnitMaskforWRK(INT i_unitMask)
{
	if(UNITKIND_ALL_MASK == i_unitMask)
	{
		return 0;
	}

	if(COMPARE_BIT_FLAG(i_unitMask, UNITKIND_BGEAR))
	{
		return 1;
	}
	if(COMPARE_BIT_FLAG(i_unitMask, UNITKIND_MGEAR))
	{
		return 2;
	}
	if(COMPARE_BIT_FLAG(i_unitMask, UNITKIND_AGEAR))
	{
		return 3;
	}
	if(COMPARE_BIT_FLAG(i_unitMask, UNITKIND_IGEAR))
	{
		return 4;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int GetUnitMaskByArrIdxforWRK(INT i_arrIdx)
/// \brief		// 2009-02-12 by cmkwon, EP3-3 월드랭킹시스템 구현 - 
/// \author		cmkwon
/// \date		2009-02-18 ~ 2009-02-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int GetUnitMaskByArrIdxforWRK(INT i_arrIdx)
{
	switch(i_arrIdx)
	{
	case 0:			return UNITKIND_ALL_MASK;
	case 1:			return UNITKIND_BGEAR;
	case 2:			return UNITKIND_MGEAR;
	case 3:			return UNITKIND_AGEAR;
	case 4:			return UNITKIND_IGEAR;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int GetArrayIndexByInfluenceType(BYTE i_byInflTy)
/// \brief		// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - GetArrayIndexByInfluenceType() 함수 추가
/// \author		cmkwon
/// \date		2007-11-01 ~ 2007-11-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int GetArrayIndexByInfluenceType(BYTE i_byInflTy)
{
	switch(i_byInflTy)
	{
	case INFLUENCE_TYPE_NORMAL:				return 0;
	case INFLUENCE_TYPE_VCN:				return 1;
	case INFLUENCE_TYPE_ANI:				return 2;
	case INFLUENCE_TYPE_ALL_MASK:			return 3;
	}

	return -1;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BYTE GetInfluenceTypeByArrayIndex(int i_nArrIdx)
/// \brief		// 2007-10-30 by cmkwon, 세력별 해피아워 이벤트 구현 - GetInfluenceTypeByArrayIndex() 함수 추가
/// \author		cmkwon
/// \date		2007-11-01 ~ 2007-11-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BYTE GetInfluenceTypeByArrayIndex(int i_nArrIdx)
{
	switch(i_nArrIdx)
	{
	case 0:			return INFLUENCE_TYPE_NORMAL;
	case 1:			return INFLUENCE_TYPE_VCN;
	case 2:			return INFLUENCE_TYPE_ANI;
	case 3:			return INFLUENCE_TYPE_ALL_MASK;
	}

	return INFLUENCE_TYPE_UNKNOWN;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2008-06-03 by cmkwon, AdminTool, DBTool 사용시 아이템 검색시 콤보박스에서 검색 기능 추가(K0000143) - 
/// \author		cmkwon
/// \date		2008-06-03 ~ 2008-06-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
char *StringCullingForValidChar(char *i_szSource, int i_nCullingSize)
{
	char *pCur	= i_szSource;
	char *pPrev = i_szSource;
	while('\0' != *pCur)
	{
		pCur = CharNext(pCur);
		int nGap = pCur - i_szSource;
		if(nGap == i_nCullingSize)
		{
			return pCur;
		}
		else if(nGap > i_nCullingSize)
		{
			return pPrev;
		}
		pPrev = pCur;
	}

	return pCur;
}


// operator overloading
MEX_ITEM_INFO& MEX_ITEM_INFO::operator=(const ITEM& rhs)
{
	this->ItemNum			= rhs.ItemNum;		
	this->Kind				= rhs.Kind;			
	this->AbilityMin		= rhs.AbilityMin;		
	this->AbilityMax		= rhs.AbilityMax;		
	this->ReqUnitKind		= rhs.ReqUnitKind;	
	this->ReqMinLevel		= rhs.ReqMinLevel;
	this->ReqMaxLevel		= rhs.ReqMaxLevel;
	this->HitRate			= rhs.HitRate;		
	this->Defense			= rhs.Defense;		
	this->SpeedPenalty		= rhs.SpeedPenalty;	
	this->Range				= rhs.Range;			
	this->Price				= rhs.Price;
	this->CashPrice			= rhs.CashPrice;
// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 아래와 같이 수정
// 	this->DestParameter1	= rhs.DestParameter1;	
// 	this->ParameterValue1	= rhs.ParameterValue1;
// 	this->DestParameter2	= rhs.DestParameter2;	
// 	this->ParameterValue2	= rhs.ParameterValue2;
// 	this->DestParameter3	= rhs.DestParameter3;	
// 	this->ParameterValue3	= rhs.ParameterValue3;
// 	this->DestParameter4	= rhs.DestParameter4;	
// 	this->ParameterValue4	= rhs.ParameterValue4;
	memcpy(this->ArrDestParameter, rhs.ArrDestParameter, sizeof(this->ArrDestParameter[0])*SIZE_MAX_DESPARAM_COUNT_IN_ITEM);
	memcpy(this->ArrParameterValue, rhs.ArrParameterValue, sizeof(this->ArrParameterValue[0])*SIZE_MAX_DESPARAM_COUNT_IN_ITEM);

	this->ReAttacktime		= rhs.ReAttacktime;	
	this->Time				= rhs.Time;			
	this->RepeatTime		= rhs.RepeatTime;			// 2006-12-08 by cmkwon, 추가함
	this->RangeAngle		= rhs.RangeAngle;		
	this->MultiTarget		= rhs.MultiTarget;	
	this->ExplosionRange	= rhs.ExplosionRange;	
	this->ReactionRange		= rhs.ReactionRange;	
	this->ShotNum			= rhs.ShotNum;		
	this->MultiNum			= rhs.MultiNum;		
	this->AttackTime		= rhs.AttackTime;		
	this->ReqSP				= rhs.ReqSP;			
	this->OrbitType			= rhs.OrbitType;		
	this->ItemAttribute		= rhs.ItemAttribute;	
	this->BoosterAngle		= rhs.BoosterAngle;	

	return *this;
}

ITEM& ITEM::operator=(const MEX_ITEM_INFO& rhs)
{
	this->ItemNum			= rhs.ItemNum;		
	this->Kind				= rhs.Kind;			
	this->AbilityMin		= rhs.AbilityMin;		
	this->AbilityMax		= rhs.AbilityMax;		
	this->ReqUnitKind		= rhs.ReqUnitKind;	
	this->ReqMinLevel		= rhs.ReqMinLevel;
	this->ReqMaxLevel		= rhs.ReqMaxLevel;
	this->HitRate			= rhs.HitRate;		
	this->Defense			= rhs.Defense;		
	this->SpeedPenalty		= rhs.SpeedPenalty;	
	this->Range				= rhs.Range;			
	this->Price				= rhs.Price;
	this->CashPrice			= rhs.CashPrice;
// 2009-04-21 by cmkwon, ITEM에 DesParam 필드 개수 8개로 늘리기 - 아래와 같이 수정
// 	this->DestParameter1	= rhs.DestParameter1;	
// 	this->ParameterValue1	= rhs.ParameterValue1;
// 	this->DestParameter2	= rhs.DestParameter2;	
// 	this->ParameterValue2	= rhs.ParameterValue2;
// 	this->DestParameter3	= rhs.DestParameter3;	
// 	this->ParameterValue3	= rhs.ParameterValue3;
// 	this->DestParameter4	= rhs.DestParameter4;	
// 	this->ParameterValue4	= rhs.ParameterValue4;
	memcpy(this->ArrDestParameter, rhs.ArrDestParameter, sizeof(this->ArrDestParameter[0])*SIZE_MAX_DESPARAM_COUNT_IN_ITEM);
	memcpy(this->ArrParameterValue, rhs.ArrParameterValue, sizeof(this->ArrParameterValue[0])*SIZE_MAX_DESPARAM_COUNT_IN_ITEM);

	this->ReAttacktime		= rhs.ReAttacktime;	
	this->Time				= rhs.Time;			
	this->RepeatTime		= rhs.RepeatTime;			// 2006-12-08 by cmkwon, 추가함
	this->RangeAngle		= rhs.RangeAngle;		
	this->MultiTarget		= rhs.MultiTarget;	
	this->ExplosionRange	= rhs.ExplosionRange;	
	this->ReactionRange		= rhs.ReactionRange;	
	this->ShotNum			= rhs.ShotNum;		
	this->MultiNum			= rhs.MultiNum;		
	this->AttackTime		= rhs.AttackTime;		
	this->ReqSP				= rhs.ReqSP;			
	this->OrbitType			= rhs.OrbitType;		
	this->ItemAttribute		= rhs.ItemAttribute;	
	this->BoosterAngle		= rhs.BoosterAngle;	

	return *this;
}

BOOL GCheckLimitLevel(EN_CHECK_TYPE i_checkType, int i_nLevel)
{
	switch(i_checkType)
	{
	case CHECK_TYPE_BATTLE_P2P_PK:
		{
			if(i_nLevel >= 10)
			{
				return TRUE;
			}
		}
		break;
	case CHECK_TYPE_BATTLE_PARTY_WAR:
		{
			if(i_nLevel >= 10)
			{
				return TRUE;
			}
		}
		break;
	case CHECK_TYPE_CHARACTER_MENT:
		{
			if(i_nLevel >= 30)
			{
				return TRUE;
			}
		}
		break;
//	case CHECK_TYPE_GUILD_CREATE:
//	case CHECK_TYPE_CHAT_SELL_ALL:		// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
	case CHECK_TYPE_GUILD_CREATE:
		{
			if(i_nLevel >= GuildCommanderMinLevel)		// 2008-10-13 by dhjin, 여단장 위임 가능 레벨 체크 추가.
			{
				return TRUE;
			}
		}
		break;
	case CHECK_TYPE_PENALTY_ON_DEAD:
		{
			if(i_nLevel > 15)
			{
				return TRUE;
			}
		}
		break;
	case CHECK_TYPE_LOWLEVEL_ADVANTAGE:			// 2006-03-15 by cmkwon
		{
			if(i_nLevel <= 5)
			{
				return TRUE;
			}
		}
		break;
	case CHECK_TYPE_CHAT_WAR:			// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
		{
			if(i_nLevel >= 20)
			{
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL GCheckRaceAndInfluenceType(EN_CHECK_TYPE i_checkType, USHORT i_usRace, BYTE i_byInfluenceTy, USHORT i_usPeerRace, BYTE i_byPeerInfluenceTy)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-06 ~ 2005-12-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL GCheckRaceAndInfluenceType(EN_CHECK_TYPE i_checkType
								, USHORT i_usRace, BYTE i_byInfluenceTy
								, USHORT i_usPeerRace, BYTE i_byPeerInfluenceTy)
{
	switch(i_checkType)
	{
	case CHECK_TYPE_TRADE:		// 관리자 관련 예외 처리 필요, 2005-12-06 by cmkwon
		{
			if ( COMPARE_RACE(i_usRace, RACE_DEMO|RACE_GUEST|RACE_MONITOR|RACE_GAMEMASTER)
				|| COMPARE_RACE(i_usPeerRace, RACE_DEMO|RACE_GUEST|RACE_MONITOR|RACE_GAMEMASTER) )
			{
				return FALSE;
			}

			if(COMPARE_RACE(i_usRace, RACE_OPERATION))
			{// 2005-12-06 by cmkwon, 관리자도 관리자 끼리만 거래 가능, 내가 관리자 일때
				if(FALSE == COMPARE_RACE(i_usPeerRace, RACE_OPERATION))
				{
					return FALSE;
				}
			}
			else if(COMPARE_RACE(i_usPeerRace, RACE_OPERATION))
			{// 2005-12-06 by cmkwon, 관리자도 관리자 끼리만 거래 가능, 상대방이 관리자 일때
				if(FALSE == COMPARE_RACE(i_usRace, RACE_OPERATION))
				{
					return FALSE;
				}
			}
			else
			{
// 2006-05-18 by cmkwon, 아래와 같이 같은 세력이 아니면 거래 불가
// 				if(COMPARE_INFLUENCE(i_byInfluenceTy, INFLUENCE_TYPE_ANI))
// 				{// 반란세력
// 					if(i_byInfluenceTy != i_byPeerInfluenceTy)
// 					{// 다른 세력하고는 거래 불가
// 						return FALSE;
// 					}
// 				}
// 				else
// 				{// 일반세력, 정규세력
// 					if(FALSE == COMPARE_INFLUENCE(i_byPeerInfluenceTy, INFLUENCE_TYPE_NORMAL|INFLUENCE_TYPE_VCN))
// 					{// 다른 세력하고는 거래 불가
// 						return FALSE;
// 					}
// 				}

				if(COMPARE_INFLUENCE(i_byInfluenceTy, INFLUENCE_TYPE_NORMAL))
				{// 일반세력은 거래 불가
					return FALSE;
				}

				if(COMPARE_INFLUENCE(i_byPeerInfluenceTy, INFLUENCE_TYPE_NORMAL))
				{// 일반세력은 거래 불가
					return FALSE;
				}
				
				if(i_byInfluenceTy != i_byPeerInfluenceTy)
				{// 2006-05-18 by cmkwon, 다른 세력임
					return FALSE;
				}
			}
		}
		break;
	case CHECK_TYPE_BAZAAR:		// 2006-08-07 by cmkwon
		{
			if ( COMPARE_RACE(i_usRace, RACE_DEMO|RACE_GUEST|RACE_MONITOR)
				|| COMPARE_RACE(i_usPeerRace, RACE_DEMO|RACE_GUEST|RACE_MONITOR) )
			{
				return FALSE;
			}

			if(COMPARE_RACE(i_usRace, RACE_OPERATION|RACE_GAMEMASTER)
				^ COMPARE_RACE(i_usPeerRace, RACE_OPERATION|RACE_GAMEMASTER))
			{// 2006-08-07 by cmkwon, 자신과 상대방 중에 한명만 관리자 혹은 운영자 일때
				return FALSE;
			}			
		}
		break;

	case CHECK_TYPE_GUILD_JOIN:		// 관리자 처리 필요 없음, 2005-12-07 by cmkwon
		{
			if(COMPARE_INFLUENCE(i_byPeerInfluenceTy, INFLUENCE_TYPE_NORMAL))
			{// 일반세력은 여단 참여 불가
				return FALSE;
			}
			if(i_byInfluenceTy != i_byPeerInfluenceTy)
			{// 세력이 다르면 여단 참여 불가
				return FALSE;
			}
		}
		break;

	case CHECK_TYPE_PARTY_JOIN:
	case CHECK_TYPE_CHAT_SELL_ALL:		// 2005-12-07 by cmkwon
	case CHECK_TYPE_CHAT_CASH_ALL:		// 2005-12-07 by cmkwon
	case CHECK_TYPE_BATTLE_PARTY_WAR:		// 2006-01-05 by cmkwon
	case CHECK_TYPE_BATTLE_GUILD_WAR:		// 2006-01-05 by cmkwon
	case CHECK_TYPE_CHAT_WAR:			// 2008-05-19 by dhjin, EP3 - 채팅 시스템 변경, 전쟁 채팅
		{// 같은 세력만 가능하다
			if(i_byInfluenceTy != i_byPeerInfluenceTy)
			{
				//////////////////////////////////////////////////////////////////////////
				// 2006-10-12 by dhjin, 다른 세력은 불가
				//if(FALSE == COMPARE_INFLUENCE(i_byInfluenceTy, INFLUENCE_TYPE_NORMAL|INFLUENCE_TYPE_VCN)
				//	|| FALSE == COMPARE_INFLUENCE(i_byPeerInfluenceTy, INFLUENCE_TYPE_NORMAL|INFLUENCE_TYPE_VCN))
				//{
				return FALSE;
				//}
			}
		}
		break;
	case CHECK_TYPE_CHAT_PTOP:		// 2005-12-07 by cmkwon
	case CHECK_TYPE_VOIP_1to1:		// 2005-12-07 by cmkwon
		{
			///////////////////////////////////////////////////////////////////////////////
			// 1. 한명이라도 관리자, 게임마스터는 무조건 가능
			// 2. 세력이 다르면 불가
			if(COMPARE_RACE(i_usRace, RACE_OPERATION|RACE_GAMEMASTER)
				|| COMPARE_RACE(i_usPeerRace, RACE_OPERATION|RACE_GAMEMASTER))
			{
				return TRUE;
			}
			if(i_byInfluenceTy != i_byPeerInfluenceTy)
			{
				if(COMPARE_INFLUENCE(i_byInfluenceTy, INFLUENCE_TYPE_NORMAL|INFLUENCE_TYPE_VCN)
					&& COMPARE_INFLUENCE(i_byPeerInfluenceTy, INFLUENCE_TYPE_NORMAL|INFLUENCE_TYPE_VCN))
				{
					return TRUE;
				}
				return FALSE;
			}
		}
		break;
	default:
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

const char * GET_ARENA_STATE_STRING(BYTE i_byAState)
{
	switch(i_byAState)
	{
	case ARENA_STATE_NONE:					return "ARENA_STATE_NONE";
	case ARENA_STATE_JOINING:				return "ARENA_STATE_JOINING";
	case ARENA_STATE_READY:					return "ARENA_STATE_READY";
	case ARENA_STATE_READY_FINISH:			return "ARENA_STATE_READY_FINISH";
	case ARENA_STATE_FIGHTING_WARREADY:		return "ARENA_STATE_FIGHTING_WARREADY";
	case ARENA_STATE_FIGHTING_WARING:		return "ARENA_STATE_FIGHTING_WARING";
	case ARENA_STATE_FIGHTING_WARFINISH:	return "ARENA_STATE_FIGHTING_WARFINISH";
	}

	return "ARENA_STATE_UNKNOWN";
}

char *GetLeader8SubLeaderString(BYTE i_byInflTy, USHORT i_usRace)
{
	switch(i_byInflTy)
	{
	case INFLUENCE_TYPE_VCN:
		{
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_LEADER))
			{
				return STRCMD_VCN_INFLUENCE_LEADER;
			}
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_SUBLEADER_1))
			{
				return STRCMD_VCN_INFLUENCE_SUBLEADER_1;
			}
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_SUBLEADER_2))
			{
				return STRCMD_VCN_INFLUENCE_SUBLEADER_2;
			}
		}
		break;
	case INFLUENCE_TYPE_ANI:
		{
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_LEADER))
			{
				return STRCMD_ANI_INFLUENCE_LEADER;
			}
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_SUBLEADER_1))
			{
				return STRCMD_ANI_INFLUENCE_SUBLEADER_1;
			}
			if(COMPARE_RACE(i_usRace, RACE_INFLUENCE_SUBLEADER_2))
			{
				return STRCMD_ANI_INFLUENCE_SUBLEADER_2;
			}
		}
		break;
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			INT CalcGradeByRate(float i_nMaxValue, float i_nValue, INT i_nRate)
/// \brief		모선전 정보 표시 기획안 - 비율에 따른 등급을 계산한다.
/// \author		dhjin
/// \date		2008-03-24 ~ 2008-03-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
INT CalcGradeByRate(float i_nMaxValue, float i_nValue, INT i_nRate)
{
	//////////////////////////////////////////////////////////////////////////
	//	i_nValue			 i_nRate
	//	-----------	 * 100 * -------  = i_nValue / i_nMaxValue * i_nRate
	//	i_nMaxValue			   100	
	//////////////////////////////////////////////////////////////////////////
	
	float CalcGrade = i_nValue / i_nMaxValue * i_nRate;
	INT	o_Grade = ceil(CalcGrade);

	return o_Grade;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-10-22 by cmkwon, 시간 제한형류 아이템 시스템 정리 - 
/// \author		cmkwon
/// \date		2009-10-22 ~ 2009-10-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
inline int GetNumberOfBits(INT64 i_n64Mask)
{
	int nBits = 0;
    while( i_n64Mask )
    {
        i_n64Mask = i_n64Mask & ( i_n64Mask - 1 ); 
        nBits++;
    }
    return nBits;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-11-02 by cmkwon, 캐쉬(인벤/창고 확장) 아이템 추가 구현 - 
/// \author		cmkwon
/// \date		2009-11-02 ~ 2009-11-02
/// \warning	
///
/// \param		BYTE i_enStorage		==> ITEM_IN_XXX
/// \return		
///////////////////////////////////////////////////////////////////////////////
BYTE CHARACTER::GetAddedPermanentInventoryCount(BYTE i_enStorage/*=ITEM_IN_CHARACTER*/)
{
	BYTE by4Ret = 0;
	switch(i_enStorage)
	{
	case ITEM_IN_CHARACTER:				by4Ret = RacingPoint & 0xFF;			return min(by4Ret, SIZE_MAX_ADDABLE_INVENTORY_COUNT);
	case ITEM_IN_STORE:					by4Ret = (RacingPoint & 0xFF00)>>8;		return min(by4Ret, SIZE_MAX_ADDABLE_STORE_COUNT);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-11-02 by cmkwon, 캐쉬(인벤/창고 확장) 아이템 추가 구현 - 
/// \author		cmkwon
/// \date		2009-11-03 ~ 2009-11-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CHARACTER::SetAddedPermanentInventoryCount(BYTE i_byAddCount, BYTE i_enStorage/*=ITEM_IN_CHARACTER*/)
{	
	switch(i_enStorage)
	{
	case ITEM_IN_CHARACTER:
		{
			RacingPoint &= ~(0xFF);
			RacingPoint |= i_byAddCount;
		}
		break;
	case ITEM_IN_STORE:
		{
			RacingPoint &= ~(0xFF00);
			RacingPoint |= ((int)i_byAddCount)<<8;
		}
		break;
	default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

struct find_if_INT		// 2010-01-18 by cmkwon, 아이템 사용시 Parameter 중복 체크 시스템 구현 - 
{
	find_if_INT(INT i_findValue): m_findValue(i_findValue){};
	bool operator()(const INT i_nValue)
	{
		return m_findValue == i_nValue;
	}
	INT m_findValue;
};

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-01-18 by cmkwon, 아이템 사용시 Parameter 중복 체크 시스템 구현 - 
/// \author		cmkwon
/// \date		2010-01-18 ~ 2010-01-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL ITEM::CheckParamOverlap(ITEM *i_pUsingItemInfo)
{
	if(NULL == this->pParamOverlapIdxList || NULL == i_pUsingItemInfo->pParamOverlapIdxList)
	{
		return TRUE;
	}
	
	vectINT *pMainVec	= this->pParamOverlapIdxList;
	vectINT *pSubVec	= i_pUsingItemInfo->pParamOverlapIdxList;
	if(this->pParamOverlapIdxList->size() > i_pUsingItemInfo->pParamOverlapIdxList->size())
	{
		pMainVec	= i_pUsingItemInfo->pParamOverlapIdxList;
		pSubVec		= this->pParamOverlapIdxList;
	}
	
	vectINT::iterator MItr(pMainVec->begin());
	for(; MItr != pMainVec->end(); MItr++)
	{
		INT OverlapIdx = *MItr;
		
		vectINT::iterator SItr = find_if(pSubVec->begin(), pSubVec->end(), find_if_INT(OverlapIdx));
		if(SItr != pSubVec->end())
		{
			return FALSE;
		}
	}
	
	return TRUE;
}


#ifdef _ATUM_SERVER
#include "VMemPool.h"

void * EVENTINFO::operator new(size_t size)
{
	return CVMemPool::vmObjectNew(size);
}

void EVENTINFO::operator delete(void* p)
{
	CVMemPool::vmObjectDelete(p, sizeof(EVENTINFO));
}

#ifndef _ATUM_ADMINTOOL		// 2005-11-26 by cmkwon
void * ITEM_GENERAL::operator new(size_t size)
{
	return CVMemPool::vmObjectNew(size);
}

void ITEM_GENERAL::operator delete(void* p)
{
	CVMemPool::vmObjectDelete(p, sizeof(ITEM_GENERAL));
}
#endif // end_#ifndef _ATUM_ADMINTOOL

void * ITEM_SKILL::operator new(size_t size)
{
	return CVMemPool::vmObjectNew(size);
}

void ITEM_SKILL::operator delete(void* p)
{
	CVMemPool::vmObjectDelete(p, sizeof(ITEM_SKILL));
}


// 2010-06-15 by shcho&hslee 펫시스템 - 메모리 풀 사용.
void * tPET_CURRENTINFO :: operator new(size_t size)
{
	return CVMemPool::vmObjectNew(size);
}

void tPET_CURRENTINFO :: operator delete(void* p)
{
	CVMemPool::vmObjectDelete(p, sizeof(tPET_CURRENTINFO));
}

// 2010-11 by dhjin, 아라리오 채널링 로그인.
char* GetConnectPublisherName(eCONNECT_PUBLISHER i_eCONNECT_PUBLISHER)
{
	switch(i_eCONNECT_PUBLISHER)
	{
	case CONNECT_PUBLISHER_NHN_JPN:
		{
			return "NHN";
		}
		break;
	default:
		{
			return "DEFAULT";
		}
	}
}
// 2011-11-18 by shcho, 서버다운 프리페어서버다운 제거 처리 - 추가 명령문 삭제
BOOL CheckAdminCommand(char *token)
{

#ifdef S_GM_COMMAND_USE_SHCHO
	return FALSE;
#endif

	if(
	// 여기 있는 명령어는 다 불가로 처리한다.
	0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyKIND)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyKIND_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyKIND_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTbyITEMNUMRANGE_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STATINIT)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STATINIT_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STATINIT_2)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STATINIT_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMETIME)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMETIME_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMETIME_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STRINGLEVEL_0)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STRINGLEVEL_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STRINGLEVEL_2)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_STRINGLEVEL_HELP)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTALL)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTALL_1)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTALL_HELP)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTWEAPON)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTWEAPON_1)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINSERTWEAPON_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_USERSINFOTOTAL)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_USERSINFOTOTAL_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_USERSINFOTOTAL_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTDBG)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTDBG_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTDBG_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGSETPARAMF)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGSETPARAMF_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_VARIABLESET)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_VARIABLESET_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_VARIABLESET_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGF_0)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGF_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGF_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P1EXP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P1SPI)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P1EXPR)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P1ITEM)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P1RARE)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_P2END)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_GAMEEVENT_HELP)
//	|| 0 == stricmp(token, STRCMD_CS_COMMAND_CITYWAR)
//	|| 0 == stricmp(token, STRCMD_CS_COMMAND_CITYWAR_1)
//	|| 0 == stricmp(token, STRCMD_CS_COMMAND_CITYWAR_PSTART)
//	|| 0 == stricmp(token, STRCMD_CS_COMMAND_CITYWAR_PEND)
//	|| 0 == stricmp(token, STRCMD_CS_COMMAND_CITYWAR_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_HAPPYHOUREVENT)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_HAPPYHOUREVENT_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_HAPPYHOUREVENT_PSTART)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_HAPPYHOUREVENT_PEND)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_HAPPYHOUREVENT_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGSETPARAMI)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGSETPARAMI_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_REGISTERADMIN)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_REGISTERADMIN_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_REGISTERADMIN_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGI_0)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGI_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGI_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_SERVERDOWN)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_SERVERDOWN_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_SERVERDOWN_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDSET)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDSET_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDSET_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDROLLBACK)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDROLLBACK_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDROLLBACK_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDLIST)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDLIST_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDLIST_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDENCRYPT)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDENCRYPT_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_PASSWORDENCRYPT_HELP)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_1)
	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ACCOUNTBLOCKLIST_HELP)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINMAP_0)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINMAP_1)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINMAP_2)
// 	|| 0 == stricmp(token, STRCMD_CS_COMMAND_ITEMINMAP_HELP)
	)
		{
			return TRUE;
		}
	return FALSE;
}

// 2013-03-13 by hskim, 웹 캐시 상점 - RawData 전송 기능 추가
char *StrCaseStr(const char *s, const char *find)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0) {
		c = tolower((unsigned char)c);
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((char)tolower((unsigned char)sc) != c);
		} while (_strnicmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

char *StrRCaseStr(const char *s, const char *find)
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0) {
		c = tolower((unsigned char)c);
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((char)tolower((unsigned char)sc) != c);
		} while (_strnicmp(s, find, len) != 0);
		s--;
	}

	return ((char *)s + len + 1);
}
// end 2013-03-13 by hskim, 웹 캐시 상점 - RawData 전송 기능 추가

//void * ENCHANT::operator new(size_t size)
//{
//	return CVMemPool::vmObjectNew(size);
//}
//
//void ENCHANT::operator delete(void* p)
//{
//	CVMemPool::vmObjectDelete(p, sizeof(ENCHANT));
//}
#endif // _ATUM_SERVER

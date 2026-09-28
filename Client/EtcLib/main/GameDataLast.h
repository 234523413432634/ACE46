// GameDataLast.h: interface for the CGameDataLast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEDATALAST1_H__A520AAFF_7E8F_4A82_813D_B008287952A3__INCLUDED_)
#define AFX_GAMEDATALAST1_H__A520AAFF_7E8F_4A82_813D_B008287952A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  MAXBUFF  65536


class TotalHeader  
{
public:
	int m_EncodeNum;
	int m_Identification;
	int m_FileSize;
	int m_DataNumber;
	int m_Parity;

	TotalHeader();
	~TotalHeader();
};


class DataHeader  
{
public:
	int m_EncodeNum;
	int m_DataSize;
	int m_Parity;
	char m_FileName[10];
	char *m_pData;

	DataHeader();
	~DataHeader();
};

///////////////////////////////////////////////////////////////////////////////
//  DO NOT CHANGE THE LAYOUT OF CGameData.
//
//  There are two declarations of this class - this one and
//  Common\GameDataLast.h, which the servers use - and which of them a client
//  translation unit gets depends on the include order in its project.
///////////////////////////////////////////////////////////////////////////////

class CGameData
{
public:
	CGameData();
	virtual ~CGameData();
	BOOL SetFile(char *filename, BOOL encode, char* encodeString, int encodeSize, BOOL bAllLoading = TRUE);
	int GetTotalNumber();
	DataHeader * GetStartPosition();
	DataHeader*  GetNext();
	void SetEncodeString(char* szEncode, int size) { memcpy( m_EncodeString, szEncode, size); }
	DataHeader* Find(char* strName);
	void SetEncode(BOOL encode) { m_bEncode = encode; }
	DataHeader* FindFromFile(char* strName);

	// 2007-04-05 by bhsohn 맵로드시, 체크섬 추가
	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 아해와 같이 수정됨
	//static BOOL GetCheckSum(UINT *o_puiCheckSum, int *o_pnFileSize, char* pFilePath);
	static BOOL GetCheckSum(BYTE o_byObjCheckSum[32], int *o_pnFileSize, char* pFilePath);

	// 2007-11-08 by bhsohn 인벤 이펙트 관련 처리
	char *GetZipFilePath();
	BOOL IsUITexture();				  // 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가

	///////////////////////////////////////////////////////////////////////////
	// Gets a resource ready to be read one image at a time - see FindFromFile()
	// in the .cpp for what that means and why it is worth doing up front.  Safe
	// to call from any thread and safe to call twice.
	static void PreloadForRandomAccess(const char *i_szPath);
	// The same for every texture the interface reads that way, across every
	// core.  The client calls this while the loading screen is up.
	static void PreloadInterfaceTextures();
	// A line for the start-up log: how many directories were indexed and what
	// they cost in memory.
	static void GetPreloadSummary(char *o_szBuffer, int i_nBufferSize);

private:
	TotalHeader* pTotal_header;
	char m_ZipFilePath[256];
	char m_EncodeStrFilePath[256];
	char m_EncodeString[256];
	BOOL m_bEncode;

	map<string,DataHeader*> m_mapDataHeader;
	map<string,DataHeader*>::iterator m_it;

protected:
	BOOL make_parse_file_ext();
};


#endif // !defined(AFX_GAMEDATALAST1_H__A520AAFF_7E8F_4A82_813D_B008287952A3__INCLUDED_)

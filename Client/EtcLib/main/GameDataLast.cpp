// GameDataLast.cpp: implementation of the CGameDataLast class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameDataLast.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>      /* Needed only for _O_RDWR definition */
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <stdio.h>
#include <vector>
#include "sha256.h"		// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
#include "ResourcePack.h"
#include "Parallel.h"

// A DataHeader record on disk stops right before m_pData, which is only ever
// filled in at run time.
#define SIZE_DATAHEADER_ON_DISK	offsetof(DataHeader, m_pData)


// 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가
#include "AtumApplication.h"		
#include "Interface.h"
#include "ShuttleChild.h"
// end 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가

///////////////////////////////////////////////////////////////////////////////
//  Reading one image out of a .tex without reading the .tex
//
//  A .tex is a directory of images with no directory: a header, then one { 24
//  byte record, image } pair after another, and the only way to the tenth
//  image is through the nine in front of it.
///////////////////////////////////////////////////////////////////////////////

namespace
{
	// The record exactly as it is on disk, rather than member by member.
	//
	// m_FileName is declared char[10], but the record has twelve bytes for it:
	// the two after it are padding in the struct, and .tex files really do use
	// them.
	const size_t SIZE_NAME_ON_DISK = SIZE_DATAHEADER_ON_DISK - offsetof(DataHeader, m_FileName);

	struct SImageRecord
	{
		BYTE				arrOnDisk[SIZE_DATAHEADER_ON_DISK];
		int					nDataSize;
		unsigned __int64	nDataOffset;		// where the image itself starts
	};

	typedef map<string, SImageRecord> CImageIndex;

	class CIndexCache
	{
	public:
		CIndexCache()	{ InitializeCriticalSection(&m_cs); }
		~CIndexCache()
		{
			for (map<string, CImageIndex*>::iterator it = m_map.begin(); it != m_map.end(); ++it)
			{
				delete it->second;
			}
			DeleteCriticalSection(&m_cs);
		}

		// NULL when the resource is not there or turns out not to be a .tex.
		const CImageIndex *Get(const char *i_szPath)
		{
			const string strKey = Key(i_szPath);

			EnterCriticalSection(&m_cs);
			map<string, CImageIndex*>::const_iterator it = m_map.find(strKey);
			const CImageIndex *pFound = (it != m_map.end()) ? it->second : NULL;
			LeaveCriticalSection(&m_cs);
			if (NULL != pFound)
			{
				return pFound->empty() ? NULL : pFound;
			}

			CImageIndex *pIndex = new CImageIndex;
			Build(i_szPath, *pIndex);			// outside the lock

			EnterCriticalSection(&m_cs);
			pair<map<string, CImageIndex*>::iterator, bool> inserted =
				m_map.insert(pair<string, CImageIndex*>(strKey, pIndex));
			if (!inserted.second)
			{	// another thread indexed the same file; keep its copy
				delete pIndex;
				pIndex = inserted.first->second;
			}
			LeaveCriticalSection(&m_cs);
			return pIndex->empty() ? NULL : pIndex;
		}

		void GetSummary(char *o_szBuffer, int i_nBufferSize)
		{
			int nFiles = 0, nImages = 0;
			EnterCriticalSection(&m_cs);
			for (map<string, CImageIndex*>::const_iterator it = m_map.begin();
				 it != m_map.end(); ++it)
			{
				nFiles++;
				nImages += (int)it->second->size();
			}
			LeaveCriticalSection(&m_cs);

			_snprintf_s(o_szBuffer, i_nBufferSize, _TRUNCATE,
						"%d texture(s) indexed, %d images, %d KB resident in archives",
						nFiles, nImages,
						(int)(CResourcePack::Instance().GetCachedBytes() / 1024));
		}

	private:
		static string Key(const char *i_szPath)
		{
			string strKey = i_szPath;
			for (size_t i = 0; i < strKey.size(); i++)
			{
				char c = strKey[i];
				if ('\\' == c)					{ strKey[i] = '/'; }
				else if (c >= 'a' && c <= 'z')	{ strKey[i] = (char)(c - 'a' + 'A'); }
			}
			return strKey;
		}

		static void Build(const char *i_szPath, CImageIndex &o_index)
		{
			CResourceFile file;
			if (!CResourcePack::Instance().Open(i_szPath, file))
			{
				return;
			}

			TotalHeader totalHeader;
			memset(&totalHeader, 0x00, sizeof(totalHeader));
			if (file.Read(0, &totalHeader, sizeof(totalHeader)) != sizeof(totalHeader))
			{
				return;
			}

			// A resource already in memory - a stored archive entry, or one the
			// preload pinned - is walked as memory rather than as a file.
			const BYTE *pWhole = file.GetData();

			unsigned __int64 nAt = sizeof(TotalHeader);
			for (int i = 0; i < totalHeader.m_DataNumber; i++)
			{
				if (nAt + SIZE_DATAHEADER_ON_DISK > file.GetSize())
				{
					break;						// truncated
				}

				DataHeader header;
				if (NULL != pWhole)
				{
					memcpy(&header, pWhole + nAt, SIZE_DATAHEADER_ON_DISK);
				}
				else if (file.Read(nAt, &header, SIZE_DATAHEADER_ON_DISK) != SIZE_DATAHEADER_ON_DISK)
				{
					break;
				}
				nAt += SIZE_DATAHEADER_ON_DISK;

				if (header.m_DataSize < 0
					|| nAt + (unsigned __int64)header.m_DataSize > file.GetSize())
				{
					break;						// truncated, or not a .tex at all
				}

				SImageRecord record;
				memcpy(record.arrOnDisk, &header, SIZE_DATAHEADER_ON_DISK);
				record.nDataSize	= header.m_DataSize;
				record.nDataOffset	= nAt;

				// the name as strcmp() would have read it: the field and the
				// padding behind it, up to the first NUL
				char szName[SIZE_DATAHEADER_ON_DISK];
				memcpy(szName, record.arrOnDisk + offsetof(DataHeader, m_FileName),
					   SIZE_NAME_ON_DISK);
				szName[SIZE_NAME_ON_DISK] = '\0';
				o_index.insert(pair<string, SImageRecord>(szName, record));

				nAt += header.m_DataSize;
			}
			// header.m_pData is never set above, so ~DataHeader frees nothing.
		}

		CRITICAL_SECTION			m_cs;
		map<string, CImageIndex*>	m_map;
	};

	CIndexCache	g_indexCache;

	// The textures the interface reads one image at a time.
	const char *const g_arrRandomAccessTexture[] =
	{
		".\\Res-Tex\\bigitem.tex",		// item icons, the busiest of them
		".\\Res-Tex\\shopnpc.tex",
		".\\Res-Tex\\npc.tex",
		".\\Res-Tex\\steff.tex",
		".\\Res-Tex\\mini.tex",
		// Res-Tex\load.tex is read this way too but only from the loading
		// screen, where a pause is what the player is already looking at, and
		// it is 56 MB - the biggest file in the game.  It stays on demand.
	};

	void PreloadWorker(int i_nIndex, void * /*i_pContext*/)
	{
		CGameData::PreloadForRandomAccess(g_arrRandomAccessTexture[i_nIndex]);
	}
}

void CGameData::PreloadForRandomAccess(const char *i_szPath)
{
	// Pinning first: for an archived, deflated resource this is what turns the
	// walk below - and every read after it - into a memory access rather than
	// an inflate.  For a loose file or a stored entry it does nothing at all.
	CResourcePack::Instance().Pin(i_szPath);
	g_indexCache.Get(i_szPath);
}

void CGameData::PreloadInterfaceTextures()
{
	AtumParallelFor((int)(sizeof(g_arrRandomAccessTexture) / sizeof(g_arrRandomAccessTexture[0])),
					PreloadWorker, NULL);
}

void CGameData::GetPreloadSummary(char *o_szBuffer, int i_nBufferSize)
{
	if (NULL != o_szBuffer && i_nBufferSize > 0)
	{
		g_indexCache.GetSummary(o_szBuffer, i_nBufferSize);
	}
}

//#include "DXUtil.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGameData::CGameData()
{
	//	FLOG( "CGameData()" );
	pTotal_header = NULL;
	memset(m_EncodeStrFilePath,0x00,sizeof(m_EncodeStrFilePath));
	memset(m_ZipFilePath,0x00,sizeof(m_ZipFilePath));
    memset(m_EncodeString,0x00,sizeof(m_EncodeString));
	m_it = m_mapDataHeader.begin();
	m_bEncode = FALSE;
}

CGameData::~CGameData()
{
	//	FLOG( "~CGameData()" );
	map<string,DataHeader*>::iterator it = m_mapDataHeader.begin();

	while(it != m_mapDataHeader.end())
	{
		DataHeader* pHeader = it->second;
		SAFE_DELETE (pHeader);
		it++;
	}
	SAFE_DELETE( pTotal_header );
}

// 2007-11-08 by bhsohn 인벤 이펙트 관련 처리
char *CGameData::GetZipFilePath()
{
	return m_ZipFilePath;

}


int CGameData::GetTotalNumber()
{
	//	FLOG( "CGameData::GetTotalNumber()" );
	// 2014-06-30 by ymjoo 텍스처 로드 익셉션 예외 처리 2014-07-31 by ymjoo 롤백
	if(NULL == pTotal_header)
	{
		//return 0;
	}
	// END 2014-06-30 by ymjoo 텍스처 로드 익셉션 예외 처리
	return pTotal_header->m_DataNumber;
}

DataHeader * CGameData::GetStartPosition()
{
	//	FLOG( "CGameData::GetStartPosition()" );
	m_it = m_mapDataHeader.begin();	
	// 2010. 07. 23 by dhkwon, jskim null 채크 구문 추가 
	if(m_it == m_mapDataHeader.end() )
		return NULL;

	return m_it->second;
}

DataHeader* CGameData::GetNext()
{
	m_it++;
	if(m_it == m_mapDataHeader.end() )
		return NULL;
	return m_it->second;

}

DataHeader* CGameData::Find(char* strName)
{
	//	FLOG( "CGameData::Find(char* strName)" );
	map<string,DataHeader*>::iterator it = m_mapDataHeader.find(strName);

	if(it != m_mapDataHeader.end())
	{
		return it->second;
	}	

	// 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가
	CHARACTER myShuttleInfo = g_pShuttleChild->GetMyShuttleInfo();		
	// 관리자 파일이 없을시
	if(g_pInterface && IsUITexture())
	{
		if(COMPARE_RACE(myShuttleInfo.Race,RACE_OPERATION|RACE_GAMEMASTER))
		{		
			return g_pInterface->GetDummyDataHeader(strName);	// 임시로 로드한 이미지를 보낸다.		
		}
//		else
//		{
//			// 일반 유저
//			DbgOut("NULL == CGameData::Find [%s] \n", strName);
//			//g_pD3dApp->NetworkErrorMsgBox(STRERR_C_RESOURCE_0001);		
//		}	
	}
	// end 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가

	return NULL;
}

BOOL CGameData::SetFile(char *filename,  BOOL encode, char* encodeString, int encodeSize, BOOL bAllLoading)
{
	//	FLOG( "CGameData::SetFile(char *filename,  BOOL encode, char* encodeString, int encodeSize)" );
	strcpy(m_ZipFilePath, filename);
	if(encode)
	{
		SetEncodeString(encodeString, encodeSize);
	}
	m_bEncode = encode;
	if(bAllLoading)
	{
		return make_parse_file_ext();
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		체크섬 처리
/// \author		// 2007-04-05 by bhsohn 맵로드시, 체크섬 추가
///				// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 함수 수정
/// \date		2007-04-06 ~ 2007-04-06
/// \warning	
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CGameData::GetCheckSum(BYTE o_byObjCheckSum[32], int *o_pnFileSize, char* pFilePath)
{
	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	//*o_puiCheckSum	= 0;			// 2007-05-28 by cmkwon
	memset(o_byObjCheckSum, 0x00, 32);	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	*o_pnFileSize	= 0;			// 2007-05-28 by cmkwon
	
	UINT uiCheckSum = 0;
	if(strlen(pFilePath) <=0)
	{
		return FALSE;
	}

	// Through CResourcePack, so that the digest the server checks is over the
	// same bytes whether the resource is loose or inside an archive - which is
	// the whole point: packing a client must not change what it reports.
	std::vector<BYTE> vectFileData;
	if(FALSE == CResourcePack::Instance().Read(pFilePath, vectFileData))
	{
		return FALSE;
	}

	const long lFileSize = (long)vectFileData.size();
	*o_pnFileSize = lFileSize;			// 2007-05-28 by cmkwon

	BYTE *pFileData = (lFileSize > 0) ? &vectFileData[0] : NULL;
// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
//	int i; for(i=0; i < lFileSize/sizeof(UINT); i++)
//	{
//		uiCheckSum ^= ((UINT*)pFileData)[i];
//	}
	///////////////////////////////////////////////////////////////////////////////
	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
	sha256_encode(pFileData, lFileSize, o_byObjCheckSum);

	// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 기존 소스
	//*o_puiCheckSum	= uiCheckSum;			// 2007-05-28 by cmkwon
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			DataHeader* CGameData::FindFromFile(char* strName)
/// \brief		파일에서 원하는 데이타를 찾아서 로딩한다.
/// \author		dhkwon
/// \date		2004-06-03 ~ 2004-06-03
/// \warning	m_bEncode가 TRUE이면 실패한다.
///				리턴된 pDataHeader는 외부에서 지워줘야 한다.
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
DataHeader* CGameData::FindFromFile(char* strName)
{
	if(m_bEncode)
	{
		return NULL;
	}
	// One map lookup instead of a walk down the whole file - see the note above
	// CIndexCache.
	const CImageIndex *pIndex = g_indexCache.Get(m_ZipFilePath);
	if(NULL == pIndex)
	{
		DBGOUT("====> Not Data File (%s)\n", strName);
		return NULL;
	}

	CImageIndex::const_iterator it = pIndex->find(strName);
	if(it == pIndex->end())
	{
		DBGOUT("====> Not Data File (%s)\n", strName);
		return NULL;
	}
	const SImageRecord &record = it->second;

	// Opened per call rather than kept: CGameData cannot grow a member without
	// breaking every library that compiles against Common\GameDataLast.h - see
	// the note at the top of GameDataLast.h.
	CResourceFile file;
	if(!CResourcePack::Instance().Open(m_ZipFilePath, file))
	{
		return NULL;
	}

	// The same 24 bytes the old code read straight off the disk into here, so
	// the header a caller gets back is byte for byte what it always was.
	DataHeader* pDataHeader = new DataHeader;
	memcpy(pDataHeader, record.arrOnDisk, SIZE_DATAHEADER_ON_DISK);

	// The +1 and the memset are what the callers expect: several of them treat
	// the image as a string.
	pDataHeader->m_pData = new char[pDataHeader->m_DataSize+1];
	memset(pDataHeader->m_pData, 0x00, pDataHeader->m_DataSize+1);	// 2006-04-03 by ispark
	if(file.Read(record.nDataOffset, pDataHeader->m_pData, record.nDataSize)
	   != (size_t)record.nDataSize)
	{
		SAFE_DELETE(pDataHeader);
		DBGOUT("====> Not Data File (%s)\n", strName);
		return NULL;
	}
	return pDataHeader;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		리소스 보호 체크를 할것인가?
/// \author		// 2008-10-15 by bhsohn 리소스 메모리 보호 기능 추가
/// \date		2004-06-03 ~ 2004-06-03
/// \warning	
///				
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CGameData::IsUITexture()
{
	char *pZipFileName = GetZipFilePath();
	if(!pZipFileName)
	{
		return FALSE;
	}
	char strSelectPath[256], strInterPath[256];	

	g_pD3dApp->LoadPath( strSelectPath, IDS_DIRECTORY_TEXTURE, "select.tex");
	g_pD3dApp->LoadPath( strInterPath, IDS_DIRECTORY_TEXTURE, "interface.tex");

	if((!stricmp(pZipFileName, strSelectPath))
		||(!stricmp(pZipFileName, strInterPath)))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CGameData::make_parse_file_ext()
{
	//	FLOG( "CGameData::make_parse_file_ext()" );
	int EncodeFile;
	int maxsize;
	// read encode string
	if( m_bEncode && strlen(m_EncodeStrFilePath) > 0 )
	{
		EncodeFile = open(m_EncodeStrFilePath, O_RDONLY );

		read( EncodeFile, m_EncodeString, sizeof(m_EncodeString) );
		close(EncodeFile);
	}
	pTotal_header = new TotalHeader;

	// The resource is a loose file or an entry in one of the mounted archives;
	// CResourcePack prefers the loose one when both exist, and this is by far
	// the most common way the client reads anything - one whole file, parsed
	// once - so an archived entry is inflated straight into this buffer with
	// nothing cached and nothing left behind.
	std::vector<BYTE> vectFileData;
	if(FALSE == CResourcePack::Instance().Read(m_ZipFilePath, vectFileData))
	{
		char buf[512];
		wsprintf(buf, "ERROR CGameData:: resource read(%s)",m_ZipFilePath);
//		FLOG( buf );
//		DBGOUT("ERROR CGameData:: resource read(%s)\n",m_ZipFilePath);
		SAFE_DELETE(pTotal_header);
		return FALSE;
	}

	maxsize = strlen(m_EncodeString);
	const size_t length = vectFileData.size();
	char* pTemp = length > 0 ? (char*)&vectFileData[0] : NULL;

	if(m_bEncode && maxsize > 0)
	{	// The key restarts every MAXBUFF bytes because the original decoded one
		// read() buffer at a time - keep that, the encoded files depend on it.
		for(size_t nAt = 0; nAt < length; nAt += MAXBUFF)
		{
			const size_t n = ((length - nAt) < MAXBUFF) ? (length - nAt) : MAXBUFF;
			for(size_t j = 0; j < n; j++)
			{
				pTemp[nAt + j] ^= m_EncodeString[j % maxsize];
			}
		}
	}

	if(length < sizeof(TotalHeader))
	{
		SAFE_DELETE(pTotal_header);
		return FALSE;
	}

	// read total header
	size_t readPointer = 0;
	memcpy( pTotal_header, &pTemp[readPointer], sizeof(TotalHeader) );
	readPointer += sizeof(TotalHeader);
	// read data header
	for( int i=0 ; i < (pTotal_header->m_DataNumber) ; i++)
	{
		if(readPointer + SIZE_DATAHEADER_ON_DISK > length)
		{
			break;						// truncated
		}
		DataHeader* pHeader = NULL;
		pHeader = new DataHeader;
		memcpy((void*) pHeader, &pTemp[readPointer], SIZE_DATAHEADER_ON_DISK);
		readPointer += SIZE_DATAHEADER_ON_DISK;
		if(pHeader->m_DataSize < 0 || readPointer + pHeader->m_DataSize > length)
		{
			SAFE_DELETE(pHeader);
			break;
		}
		pHeader->m_pData = new char[pHeader->m_DataSize+1];
		memset(pHeader->m_pData, 0x00, pHeader->m_DataSize+1);			// 2006-04-03 by ispark
		memcpy((void*) pHeader->m_pData, &pTemp[readPointer], pHeader->m_DataSize );
		readPointer += pHeader->m_DataSize;
		m_mapDataHeader[pHeader->m_FileName] = pHeader;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TotalHeader Class
//////////////////////////////////////////////////////////////////////

TotalHeader::TotalHeader()
{
	//	FLOG( "TotalHeader()" );
	m_EncodeNum = 1000;
	m_Identification = 1000;
	m_FileSize = 0;
	m_DataNumber = 0;
	m_Parity = 0;
}

TotalHeader::~TotalHeader()
{
	//	FLOG( "~TotalHeader()" );

}

//////////////////////////////////////////////////////////////////////
// DataHeader Class
//////////////////////////////////////////////////////////////////////

DataHeader::DataHeader()
{
	//	FLOG( "DataHeader()" );
	m_EncodeNum = 0;
	m_DataSize = 0;
	m_Parity = 0;
	m_pData = NULL;
	memset(m_FileName,0x00,sizeof(m_FileName));
}

DataHeader::~DataHeader()
{
	//	FLOG( "~DataHeader()" );
//	delete [] m_pData;
	SAFE_DELETE_ARRAY(m_pData);
}



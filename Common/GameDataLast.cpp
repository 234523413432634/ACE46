// GameDataLast.cpp: implementation of the CGameDataLast class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameDataLast.h"
#include "ResourcePack.h"
#include <vector>

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>      /* Needed only for _O_RDWR definition */
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <stdio.h>
#include "sha256.h"		// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 

//#include "DXUtil.h"
// A DataHeader record on disk stops right before the m_pData member, which is
// only ever filled in at runtime.  Subtracting sizeof(char*) keeps the record
// length identical (24 bytes) on Win32 and x64.
#define SIZE_DATAHEADER_ON_DISK	(sizeof(DataHeader) - sizeof(char*))

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

int CGameData::GetTotalNumber()
{
	//	FLOG( "CGameData::GetTotalNumber()" );
	return pTotal_header->m_DataNumber;
}

DataHeader * CGameData::GetStartPosition()
{
	//	FLOG( "CGameData::GetStartPosition()" );
	m_it = m_mapDataHeader.begin();

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
	// The resource may be a loose file or an entry in one of the .\map archives;
	// either way the digest is over the same bytes the client sees.
	std::vector<BYTE> vectFileData;
	if(FALSE == CResourcePack::Instance().Read(pFilePath, vectFileData))
	{
		return FALSE;
	}

	const long lFileSize = (long)vectFileData.size();
	*o_pnFileSize = lFileSize;			// 2007-05-28 by cmkwon

	BYTE *pFileData = lFileSize > 0 ? &vectFileData[0] : NULL;
// 2009-05-29 by cmkwon, Hash알고리즘 추가(SHA256) - 
//	for(int i=0; i < lFileSize/sizeof(UINT); i++)
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

	// Reads through CResourcePack so an archived resource behaves the same as a
	// loose one; the archive entry has to be inflated whole anyway, so there is
	// nothing to gain from seeking around inside it.
	std::vector<BYTE> vectFileData;
	if(FALSE == CResourcePack::Instance().Read(m_ZipFilePath, vectFileData))
	{
		return NULL;
	}

	const size_t nSize = vectFileData.size();
	if(nSize < sizeof(TotalHeader))
	{
		return NULL;
	}
	const char *pFileData = (const char*)&vectFileData[0];

	TotalHeader totalHeader;
	memcpy(&totalHeader, pFileData, sizeof(TotalHeader));

	size_t readPointer = sizeof(TotalHeader);
	DataHeader* pDataHeader = new DataHeader;
	for( int i=0 ; i < (totalHeader.m_DataNumber) ; i++)
	{
		if(readPointer + SIZE_DATAHEADER_ON_DISK > nSize)
		{
			break;
		}
		memset(pDataHeader, 0x00, sizeof(DataHeader) );
		memcpy(pDataHeader, pFileData + readPointer, SIZE_DATAHEADER_ON_DISK);
		readPointer += SIZE_DATAHEADER_ON_DISK;

		if(strcmp(pDataHeader->m_FileName, strName ) == 0)
		{
			if(readPointer + pDataHeader->m_DataSize > nSize)
			{
				break;
			}
			pDataHeader->m_pData = new char[pDataHeader->m_DataSize+1];
			memset(pDataHeader->m_pData, 0x00, pDataHeader->m_DataSize+1);	// 2006-04-03 by ispark
			memcpy(pDataHeader->m_pData, pFileData + readPointer, pDataHeader->m_DataSize);
			return pDataHeader;
		}
		readPointer += pDataHeader->m_DataSize;
	}

//	delete pDataHeader;
	SAFE_DELETE(pDataHeader);
	DBGOUT("====> Not Data File (%s)\n", strName);
	return NULL;
}

BOOL CGameData::make_parse_file_ext()
{
	//	FLOG( "CGameData::make_parse_file_ext()" );
	int EncodeFile;
	char Data[300],temp[300],Encode[1024];
	int maxsize;
	// read encode string
	if( m_bEncode && strlen(m_EncodeStrFilePath) > 0 )
	{
		memset(Encode,0x00,sizeof(Encode));
		EncodeFile = open(m_EncodeStrFilePath, O_RDONLY );

		read( EncodeFile, m_EncodeString, sizeof(m_EncodeString) );
		close(EncodeFile);
	}
	pTotal_header = new TotalHeader;
	memset(temp,0x00,sizeof(temp));
	memset(Data,0x00,sizeof(Data));

	// The resource is either a loose file or an entry in one of the .\map
	// archives - CResourcePack prefers the loose one when both exist.
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
	const long length = (long)vectFileData.size();
	char* pTemp = new char[length > 0 ? length : 1];
	int readPointer=0;
	if(length > 0)
	{
		memcpy(pTemp, &vectFileData[0], length);
	}

	if(m_bEncode && maxsize > 0)
	{	// The key restarts every MAXBUFF bytes because the original decoded one
		// read() buffer at a time - keep that, encoded files depend on it.
		for(readPointer = 0; readPointer < length; readPointer += MAXBUFF)
		{
			const int n = (length - readPointer) < MAXBUFF ? (length - readPointer) : MAXBUFF;
			for(int j=0; j<n; j++)
			{
				pTemp[readPointer+j] ^= m_EncodeString[j%maxsize];
			}
		}
	}

	// read total header
	readPointer = 0;
	memcpy( pTotal_header, &pTemp[readPointer], sizeof(TotalHeader) );
	readPointer += sizeof(TotalHeader);
	// read data header
	for( int i=0 ; i < (pTotal_header->m_DataNumber) ; i++)
	{
		DataHeader* pHeader = NULL;
		pHeader = new DataHeader;
		memcpy((void*) pHeader, &pTemp[readPointer], SIZE_DATAHEADER_ON_DISK);
		readPointer += SIZE_DATAHEADER_ON_DISK;
		pHeader->m_pData = new char[pHeader->m_DataSize+1];
		memset(pHeader->m_pData, 0x00, pHeader->m_DataSize+1);			// 2006-04-03 by ispark
		memcpy((void*) pHeader->m_pData, &pTemp[readPointer], pHeader->m_DataSize );
		readPointer += pHeader->m_DataSize;
		m_mapDataHeader[pHeader->m_FileName] = pHeader;
	}	
//	delete pTemp;
	SAFE_DELETE_ARRAY(pTemp);
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



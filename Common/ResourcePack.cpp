///////////////////////////////////////////////////////////////////////////////
//  ResourcePack.cpp : see ResourcePack.h
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "ResourcePack.h"
#include "Parallel.h"
#include "../Server/ZipArchive/zlib/zlib.h"

namespace
{
	const DWORD SIGNATURE_END_OF_CENTRAL_DIRECTORY			= 0x06054b50;
	const DWORD SIGNATURE_ZIP64_END_OF_CENTRAL_DIRECTORY	= 0x06064b50;
	const DWORD SIGNATURE_ZIP64_LOCATOR						= 0x07064b50;
	const DWORD SIGNATURE_CENTRAL_FILE_HEADER				= 0x02014b50;
	const DWORD SIGNATURE_LOCAL_FILE_HEADER					= 0x04034b50;

	const unsigned short METHOD_STORED	= 0;
	const unsigned short METHOD_DEFLATE	= 8;

	const size_t SIZE_END_OF_CENTRAL_DIRECTORY	= 22;
	const size_t SIZE_CENTRAL_FILE_HEADER		= 46;
	const size_t SIZE_LOCAL_FILE_HEADER			= 30;
	const size_t SIZE_MAX_ZIP_COMMENT			= 0xFFFF;

	const unsigned __int64 SIZE_DEFAULT_CACHE_BUDGET = 192ull * 1024 * 1024;

	inline unsigned short Read16(const BYTE *i_p)
	{
		return (unsigned short)(i_p[0] | (i_p[1] << 8));
	}

	inline DWORD Read32(const BYTE *i_p)
	{
		return (DWORD)i_p[0] | ((DWORD)i_p[1] << 8) | ((DWORD)i_p[2] << 16) | ((DWORD)i_p[3] << 24);
	}

	inline unsigned __int64 Read64(const BYTE *i_p)
	{
		return (unsigned __int64)Read32(i_p) | ((unsigned __int64)Read32(i_p + 4) << 32);
	}

	// Upper case, forward slashes; Windows paths are case insensitive and the
	// existing code already upper cases resource names for its checksum map.
	void CanonicaliseInPlace(std::string &io_str)
	{
		for (size_t i = 0; i < io_str.size(); i++)
		{
			char c = io_str[i];
			if ('\\' == c)			{ io_str[i] = '/'; }
			else if (c >= 'a' && c <= 'z')	{ io_str[i] = (char)(c - 'a' + 'A'); }
		}
	}

	// Absolute, upper case, '/' separated form used as the index key.
	BOOL NormalisePath(const char *i_szPath, std::string &o_strNormalised)
	{
		if (NULL == i_szPath || '\0' == i_szPath[0])
		{
			return FALSE;
		}

		char szFull[MAX_PATH * 2];
		DWORD dwLength = GetFullPathNameA(i_szPath, sizeof(szFull), szFull, NULL);
		if (0 == dwLength || dwLength >= sizeof(szFull))
		{
			return FALSE;
		}

		o_strNormalised = szFull;
		CanonicaliseInPlace(o_strNormalised);
		return TRUE;
	}

	BOOL FileExistsOnDisk(const char *i_szPath)
	{
		DWORD dwAttributes = GetFileAttributesA(i_szPath);
		return (INVALID_FILE_ATTRIBUTES != dwAttributes
				&& 0 == (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE;
	}

	// FILE_SHARE_WRITE and FILE_SHARE_DELETE as well as FILE_SHARE_READ: the
	// client rewrites Res-Tex\omi.tex in place while it runs, and holding a
	// resource open must not be what stops it.
	HANDLE OpenLooseFile(const char *i_szPath)
	{
		return CreateFileA(i_szPath, GENERIC_READ,
						   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	BOOL ReadWholeFile(const char *i_szPath, std::vector<BYTE> &o_vectData)
	{
		HANDLE hFile = OpenLooseFile(i_szPath);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return FALSE;
		}

		LARGE_INTEGER liSize;
		if (!GetFileSizeEx(hFile, &liSize) || liSize.QuadPart < 0)
		{
			CloseHandle(hFile);
			return FALSE;
		}

		o_vectData.resize((size_t)liSize.QuadPart);
		DWORD dwTotal = 0;
		while (dwTotal < (DWORD)liSize.QuadPart)
		{
			DWORD dwRead = 0;
			if (!ReadFile(hFile, &o_vectData[dwTotal], (DWORD)liSize.QuadPart - dwTotal, &dwRead, NULL)
				|| 0 == dwRead)
			{
				CloseHandle(hFile);
				return FALSE;
			}
			dwTotal += dwRead;
		}

		CloseHandle(hFile);
		return TRUE;
	}

	// One ReadFile at an offset, without disturbing the handle's file pointer,
	// so several threads could share a handle if they ever needed to.
	size_t ReadFileAt(HANDLE i_hFile, unsigned __int64 i_nOffset, void *o_pBuffer, size_t i_nBytes)
	{
		BYTE	*pOut	= (BYTE*)o_pBuffer;
		size_t	nDone	= 0;
		while (nDone < i_nBytes)
		{
			const unsigned __int64 nAt = i_nOffset + nDone;
			OVERLAPPED overlapped;
			memset(&overlapped, 0x00, sizeof(overlapped));
			overlapped.Offset		= (DWORD)(nAt & 0xFFFFFFFFull);
			overlapped.OffsetHigh	= (DWORD)(nAt >> 32);

			const size_t nWant = i_nBytes - nDone;
			DWORD dwRead = 0;
			if (!ReadFile(i_hFile, pOut + nDone,
						  (DWORD)((nWant > 0x40000000u) ? 0x40000000u : nWant),
						  &dwRead, &overlapped)
				|| 0 == dwRead)
			{
				break;					// end of file, or a real failure
			}
			nDone += dwRead;
		}
		return nDone;
	}

	void CloseArchiveHandles(const BYTE *i_pView, HANDLE i_hMapping, HANDLE i_hFile)
	{
		if (NULL != i_pView)		{ UnmapViewOfFile(i_pView); }
		if (NULL != i_hMapping)		{ CloseHandle(i_hMapping); }
		if (INVALID_HANDLE_VALUE != i_hFile && NULL != i_hFile)	{ CloseHandle(i_hFile); }
	}

	// Returned instead of NULL for an empty range, so that "no bytes" still
	// reads as success.
	const BYTE ARR_NO_BYTES[1] = { 0 };

	// Raw deflate.  A private z_stream per call, so this is safe on any thread.
	BOOL InflateRaw(const BYTE *i_pIn, unsigned __int64 i_nIn,
					BYTE *o_pOut, unsigned __int64 i_nOut)
	{
		if (0 == i_nOut)
		{
			return TRUE;
		}

		z_stream stream;
		memset(&stream, 0x00, sizeof(stream));
		if (Z_OK != inflateInit2(&stream, -MAX_WBITS))
		{
			return FALSE;
		}

		BOOL bResult = TRUE;
		unsigned __int64 nRemainingIn	= i_nIn;
		unsigned __int64 nRemainingOut	= i_nOut;
		const BYTE *pIn	= i_pIn;
		BYTE *pOut		= o_pOut;

		while (nRemainingOut > 0)
		{
			// uInt is 32 bit even on x64, so feed the stream in chunks
			const uInt uChunkIn  = (uInt)((nRemainingIn  > 0x40000000ull) ? 0x40000000ull : nRemainingIn);
			const uInt uChunkOut = (uInt)((nRemainingOut > 0x40000000ull) ? 0x40000000ull : nRemainingOut);

			stream.next_in	 = const_cast<Bytef*>(pIn);
			stream.avail_in	 = uChunkIn;
			stream.next_out	 = pOut;
			stream.avail_out = uChunkOut;

			const int nStatus = inflate(&stream, Z_NO_FLUSH);
			if (Z_OK != nStatus && Z_STREAM_END != nStatus && Z_BUF_ERROR != nStatus)
			{
				bResult = FALSE;
				break;
			}

			const uInt uConsumed = uChunkIn - stream.avail_in;
			const uInt uProduced = uChunkOut - stream.avail_out;
			pIn				+= uConsumed;
			nRemainingIn	-= uConsumed;
			pOut			+= uProduced;
			nRemainingOut	-= uProduced;

			if (Z_STREAM_END == nStatus)
			{
				break;
			}
			if (0 == uConsumed && 0 == uProduced)
			{
				bResult = FALSE;			// no progress, truncated entry
				break;
			}
		}

		if (0 != nRemainingOut)
		{
			bResult = FALSE;
		}

		inflateEnd(&stream);
		return bResult;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CResourceFile

CResourceFile::CResourceFile()
	: m_pPack(NULL)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_pEntry(NULL)
	, m_nSize(0)
{
}

CResourceFile::~CResourceFile()
{
	Close();
}

void CResourceFile::Close()
{
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	m_pPack		= NULL;
	m_pEntry	= NULL;
	m_nSize		= 0;
}

size_t CResourceFile::Read(unsigned __int64 i_nOffset, void *o_pBuffer, size_t i_nBytes)
{
	if (NULL == m_pPack || NULL == o_pBuffer || 0 == i_nBytes || i_nOffset >= m_nSize)
	{
		return 0;
	}
	if (i_nBytes > (size_t)(m_nSize - i_nOffset))
	{
		i_nBytes = (size_t)(m_nSize - i_nOffset);
	}

	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		return ReadFileAt(m_hFile, i_nOffset, o_pBuffer, i_nBytes);
	}
	return m_pPack->ReadEntryRange(*(const CResourcePack::SEntry*)m_pEntry,
								   i_nOffset, o_pBuffer, i_nBytes);
}

const BYTE *CResourceFile::GetData()
{
	// A loose file has nothing mapped; the caller reads it.
	if (NULL == m_pPack || NULL == m_pEntry)
	{
		return NULL;
	}

	// A stored entry in a mapped archive is already a block of memory.
	const CResourcePack::SEntry &entry = *(const CResourcePack::SEntry*)m_pEntry;
	if (METHOD_STORED == entry.usMethod)
	{
		const BYTE *pMapped = m_pPack->EntryData(entry);
		if (NULL != pMapped)
		{
			return pMapped;
		}
		// Not mapped: only a copy Pin() made answers, same as a deflated entry.
	}

	// Only what has already been made resident - GetData() must stay free of
	// surprises, so it never starts an inflate of its own.
	const CResourcePack::SBlock *pBlock = m_pPack->LookupBlock(entry, -1);
	return (NULL != pBlock && !pBlock->vectData.empty()) ? &pBlock->vectData[0] : NULL;
}

///////////////////////////////////////////////////////////////////////////////

CResourcePack &CResourcePack::Instance()
{
	static CResourcePack s_pack;
	return s_pack;
}

CResourcePack::CResourcePack()
	: m_nCachedBytes(0)
	, m_nCacheBudget(SIZE_DEFAULT_CACHE_BUDGET)
{
	InitializeCriticalSection(&m_csCache);
}

CResourcePack::~CResourcePack()
{
	Unmount();
	DeleteCriticalSection(&m_csCache);
}

void CResourcePack::Unmount()
{
	EnterCriticalSection(&m_csCache);
	for (std::map<SBlockKey, SBlock*>::iterator it = m_mapBlock.begin(); it != m_mapBlock.end(); ++it)
	{
		delete it->second;
	}
	m_mapBlock.clear();
	m_nCachedBytes = 0;
	LeaveCriticalSection(&m_csCache);

	for (size_t i = 0; i < m_vectArchive.size(); i++)
	{
		SArchive &archive = m_vectArchive[i];
		CloseArchiveHandles(archive.pView, archive.hMapping, archive.hFile);
	}
	m_vectArchive.clear();
	m_mapEntry.clear();
	m_vectMountRoot.clear();
}

///////////////////////////////////////////////////////////////////////////////
// mounting

BOOL CResourcePack::MountDirectory(const char *i_szDirectory)
{
	std::string strRoot;
	if (!NormalisePath(i_szDirectory, strRoot))
	{
		return FALSE;
	}
	if ('/' != strRoot[strRoot.size() - 1])
	{
		strRoot += '/';
	}

	for (size_t i = 0; i < m_vectMountRoot.size(); i++)
	{
		if (m_vectMountRoot[i] == strRoot)
		{
			return TRUE;			// already mounted
		}
	}
	m_vectMountRoot.push_back(strRoot);

	// Deterministic order: archives are indexed alphabetically, so when two of
	// them hold the same entry the one that sorts first wins, the same way it
	// would if the operator listed them by hand.
	std::vector<std::string> vectArchivePath;

	char szPattern[MAX_PATH * 2];
	_snprintf_s(szPattern, sizeof(szPattern), _TRUNCATE, "%s*.zip", strRoot.c_str());
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(szPattern, &findData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}
			char szArchive[MAX_PATH * 2];
			_snprintf_s(szArchive, sizeof(szArchive), _TRUNCATE, "%s%s",
						strRoot.c_str(), findData.cFileName);
			vectArchivePath.push_back(szArchive);
		}
		while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}

	std::sort(vectArchivePath.begin(), vectArchivePath.end());
	for (size_t i = 0; i < vectArchivePath.size(); i++)
	{
		AddArchive(vectArchivePath[i].c_str());
	}
	return TRUE;
}

BOOL CResourcePack::AddArchive(const char *i_szArchivePath)
{
	// The key every entry of this archive is filed under starts here.
	std::string strRoot = i_szArchivePath;
	CanonicaliseInPlace(strRoot);
	const size_t nSlash = strRoot.rfind('/');
	strRoot = (std::string::npos == nSlash) ? std::string() : strRoot.substr(0, nSlash + 1);

	SArchive archive;
	archive.strPath		= i_szArchivePath;
	archive.hFile		= INVALID_HANDLE_VALUE;
	archive.hMapping	= NULL;
	archive.pView		= NULL;
	archive.nSize		= 0;

	archive.hFile = CreateFileA(i_szArchivePath, GENERIC_READ, FILE_SHARE_READ, NULL,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == archive.hFile)
	{
		return FALSE;
	}

	LARGE_INTEGER liSize;
	if (!GetFileSizeEx(archive.hFile, &liSize)
		|| liSize.QuadPart < (LONGLONG)SIZE_END_OF_CENTRAL_DIRECTORY)
	{
		CloseHandle(archive.hFile);
		return FALSE;
	}
	archive.nSize = (unsigned __int64)liSize.QuadPart;

	// A 64 bit process maps the archive whole, however large it is, and every
	// read out of it is then a memcpy.
	if (sizeof(void*) >= 8)
	{
		archive.hMapping = CreateFileMappingA(archive.hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (NULL != archive.hMapping)
		{
			archive.pView = (const BYTE*)MapViewOfFile(archive.hMapping, FILE_MAP_READ, 0, 0, 0);
			if (NULL == archive.pView)
			{	// no contiguous room after all - read the archive instead
				CloseHandle(archive.hMapping);
				archive.hMapping = NULL;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// end of central directory, scanned backwards past a possible comment.
	///////////////////////////////////////////////////////////////////////////
	const size_t nTailSize = (size_t)((archive.nSize < SIZE_END_OF_CENTRAL_DIRECTORY + SIZE_MAX_ZIP_COMMENT)
									  ? archive.nSize
									  : SIZE_END_OF_CENTRAL_DIRECTORY + SIZE_MAX_ZIP_COMMENT);
	std::vector<BYTE> vectTail(nTailSize);
	if (!ArchiveRead(archive, archive.nSize - nTailSize, &vectTail[0], nTailSize))
	{
		CloseArchiveHandles(archive.pView, archive.hMapping, archive.hFile);
		return FALSE;
	}

	size_t nEndOfCentralDirectoryAt = nTailSize;
	for (size_t nBack = nTailSize - SIZE_END_OF_CENTRAL_DIRECTORY + 1; nBack-- > 0; )
	{
		if (SIGNATURE_END_OF_CENTRAL_DIRECTORY == Read32(&vectTail[nBack]))
		{
			nEndOfCentralDirectoryAt = nBack;
			break;
		}
	}
	if (nEndOfCentralDirectoryAt == nTailSize)
	{	// not a zip, or truncated - leave the archives already mounted alone
		CloseArchiveHandles(archive.pView, archive.hMapping, archive.hFile);
		return FALSE;
	}
	const BYTE *pEndOfCentralDirectory = &vectTail[nEndOfCentralDirectoryAt];

	unsigned __int64 nEntryCount			= Read16(pEndOfCentralDirectory + 10);
	unsigned __int64 nCentralDirectorySize	= Read32(pEndOfCentralDirectory + 12);
	unsigned __int64 nCentralDirectoryOffset= Read32(pEndOfCentralDirectory + 16);

	// Zip64, when the counts or offsets did not fit in the classic record
	if (0xFFFF == nEntryCount || 0xFFFFFFFFu == nCentralDirectorySize
		|| 0xFFFFFFFFu == nCentralDirectoryOffset)
	{
		if (nEndOfCentralDirectoryAt >= 20
			&& SIGNATURE_ZIP64_LOCATOR == Read32(pEndOfCentralDirectory - 20))
		{
			const unsigned __int64 nZip64At = Read64(pEndOfCentralDirectory - 20 + 8);
			BYTE arrZip64[56];
			if (ArchiveRead(archive, nZip64At, arrZip64, sizeof(arrZip64))
				&& SIGNATURE_ZIP64_END_OF_CENTRAL_DIRECTORY == Read32(arrZip64))
			{
				nEntryCount				= Read64(arrZip64 + 32);
				nCentralDirectorySize	= Read64(arrZip64 + 40);
				nCentralDirectoryOffset	= Read64(arrZip64 + 48);
			}
		}
	}

	if (nCentralDirectorySize > (unsigned __int64)(size_t)-1
		|| nCentralDirectoryOffset > archive.nSize
		|| nCentralDirectorySize > archive.nSize - nCentralDirectoryOffset)
	{
		CloseArchiveHandles(archive.pView, archive.hMapping, archive.hFile);
		return FALSE;
	}

	std::vector<BYTE> vectDirectory((size_t)nCentralDirectorySize);
	if (nCentralDirectorySize > 0
		&& !ArchiveRead(archive, nCentralDirectoryOffset, &vectDirectory[0],
						(size_t)nCentralDirectorySize))
	{
		CloseArchiveHandles(archive.pView, archive.hMapping, archive.hFile);
		return FALSE;
	}

	const int nArchiveIndex = (int)m_vectArchive.size();
	m_vectArchive.push_back(archive);

	///////////////////////////////////////////////////////////////////////////
	// central directory
	size_t nAt = 0;
	for (unsigned __int64 n = 0; n < nEntryCount; n++)
	{
		if (nAt + SIZE_CENTRAL_FILE_HEADER > vectDirectory.size()
			|| SIGNATURE_CENTRAL_FILE_HEADER != Read32(&vectDirectory[nAt]))
		{
			break;
		}

		const BYTE *pHeader = &vectDirectory[nAt];
		const unsigned short usFlags		= Read16(pHeader + 8);
		const unsigned short usMethod		= Read16(pHeader + 10);
		const unsigned short usNameLength	= Read16(pHeader + 28);
		const unsigned short usExtraLength	= Read16(pHeader + 30);
		const unsigned short usCommentLength= Read16(pHeader + 32);

		if (nAt + SIZE_CENTRAL_FILE_HEADER + usNameLength + usExtraLength + usCommentLength
			> vectDirectory.size())
		{
			break;						// truncated central directory
		}

		SEntry entry;
		entry.nArchive			= nArchiveIndex;
		entry.usMethod			= usMethod;
		entry.nCompressedSize	= Read32(pHeader + 20);
		entry.nUncompressedSize	= Read32(pHeader + 24);
		entry.nLocalHeaderOffset= Read32(pHeader + 42);
		entry.nChunkSize		= 0;

		std::string strName((const char*)(pHeader + SIZE_CENTRAL_FILE_HEADER), usNameLength);

		const BYTE *pExtra		= pHeader + SIZE_CENTRAL_FILE_HEADER + usNameLength;
		const BYTE *pExtraEnd	= pExtra + usExtraLength;
		while (pExtra + 4 <= pExtraEnd)
		{
			const unsigned short usTag	= Read16(pExtra);
			const unsigned short usSize	= Read16(pExtra + 2);
			if (pExtra + 4 + usSize > pExtraEnd)
			{
				break;						// malformed, stop looking
			}
			const BYTE *pField = pExtra + 4;

			// Zip64 extended information, when any of the three fields saturated
			if (0x0001 == usTag)
			{
				if (0xFFFFFFFFu == entry.nUncompressedSize && pField + 8 <= pExtra + 4 + usSize)
				{
					entry.nUncompressedSize = Read64(pField); pField += 8;
				}
				if (0xFFFFFFFFu == entry.nCompressedSize && pField + 8 <= pExtra + 4 + usSize)
				{
					entry.nCompressedSize = Read64(pField); pField += 8;
				}
				if (0xFFFFFFFFu == entry.nLocalHeaderOffset && pField + 8 <= pExtra + 4 + usSize)
				{
					entry.nLocalHeaderOffset = Read64(pField);
				}
			}
			// The chunk table a writer lays down: version, chunk size, count,
			// then where each chunk starts inside the entry's deflate stream.
			else if (ATUM_RESPACK_EXTRA_ID == usTag && usSize >= 10
					 && ATUM_RESPACK_EXTRA_VERSION == Read16(pField))
			{
				const unsigned int nChunkSize	= Read32(pField + 2);
				const unsigned int nChunkCount	= Read32(pField + 6);
				if (nChunkSize > 0 && nChunkCount > 0
					&& (unsigned int)(usSize - 10) / 8 >= nChunkCount)
				{
					entry.nChunkSize = nChunkSize;
					entry.vectChunkOffset.reserve(nChunkCount);
					for (unsigned int c = 0; c < nChunkCount; c++)
					{
						entry.vectChunkOffset.push_back(Read64(pField + 10 + c * 8));
					}
				}
			}
			pExtra += 4 + usSize;
		}

		nAt += SIZE_CENTRAL_FILE_HEADER + usNameLength + usExtraLength + usCommentLength;

		if (strName.empty() || '/' == strName[strName.size() - 1])
		{
			continue;						// directory entry
		}
		if (0 != (usFlags & 0x0001))
		{
			continue;						// encrypted, not supported
		}
		if (METHOD_STORED != usMethod && METHOD_DEFLATE != usMethod)
		{
			continue;						// bzip2/lzma/... not supported
		}
		// A chunk table only makes sense on a deflate stream, and the number of
		// chunks has to match what the entry says it holds.
		if (!entry.vectChunkOffset.empty())
		{
			const unsigned __int64 nWant =
				(entry.nUncompressedSize + entry.nChunkSize - 1) / entry.nChunkSize;
			if (METHOD_DEFLATE != usMethod || nWant != entry.vectChunkOffset.size())
			{
				entry.vectChunkOffset.clear();
				entry.nChunkSize = 0;
			}
		}

		CanonicaliseInPlace(strName);
		// The key is the path the entry would have had if it had been extracted next
		// to the archive, so a lookup is one map probe on the caller's own path and
		// it does not matter which directory the archive was mounted from.
		std::string strKey = strRoot + strName;

		// first archive to declare a name keeps it
		m_mapEntry.insert(std::pair<std::string, SEntry>(strKey, entry));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// lookup

const CResourcePack::SEntry *CResourcePack::FindEntry(const char *i_szPath) const
{
	if (m_mapEntry.empty())
	{
		return NULL;
	}

	std::string strNormalised;
	if (!NormalisePath(i_szPath, strNormalised))
	{
		return NULL;
	}

	std::map<std::string, SEntry>::const_iterator itr = m_mapEntry.find(strNormalised);
	return (itr != m_mapEntry.end()) ? &itr->second : NULL;
}

BOOL CResourcePack::ArchiveRead(const SArchive &i_archive, unsigned __int64 i_nAt,
								void *o_pBuffer, size_t i_nBytes) const
{
	if (i_nAt > i_archive.nSize || (unsigned __int64)i_nBytes > i_archive.nSize - i_nAt)
	{
		return FALSE;
	}
	if (0 == i_nBytes)
	{
		return TRUE;
	}
	if (NULL != i_archive.pView)
	{
		memcpy(o_pBuffer, i_archive.pView + i_nAt, i_nBytes);
		return TRUE;
	}
	return (ReadFileAt(i_archive.hFile, i_nAt, o_pBuffer, i_nBytes) == i_nBytes) ? TRUE : FALSE;
}

unsigned __int64 CResourcePack::EntryDataOffset(const SEntry &i_entry) const
{
	const SArchive &archive = m_vectArchive[i_entry.nArchive];

	// The local header repeats the name and extra field with its own lengths,
	// which is why the central directory alone does not say where the data is.
	BYTE arrLocal[SIZE_LOCAL_FILE_HEADER];
	if (!ArchiveRead(archive, i_entry.nLocalHeaderOffset, arrLocal, sizeof(arrLocal))
		|| SIGNATURE_LOCAL_FILE_HEADER != Read32(arrLocal))
	{
		return ATUM_RESPACK_NO_OFFSET;
	}

	const unsigned __int64 nDataOffset = i_entry.nLocalHeaderOffset + SIZE_LOCAL_FILE_HEADER
										 + Read16(arrLocal + 26) + Read16(arrLocal + 28);
	if (nDataOffset > archive.nSize || i_entry.nCompressedSize > archive.nSize - nDataOffset)
	{
		return ATUM_RESPACK_NO_OFFSET;
	}
	return nDataOffset;
}

const BYTE *CResourcePack::EntryData(const SEntry &i_entry) const
{
	const SArchive &archive = m_vectArchive[i_entry.nArchive];
	if (NULL == archive.pView)
	{
		return NULL;					// not mapped; the caller has to read it
	}

	const unsigned __int64 nDataOffset = EntryDataOffset(i_entry);
	return (ATUM_RESPACK_NO_OFFSET != nDataOffset) ? archive.pView + nDataOffset : NULL;
}

const BYTE *CResourcePack::EntryBytes(const SEntry &i_entry, unsigned __int64 i_nDataAt,
									  unsigned __int64 i_nFrom, unsigned __int64 i_nTo,
									  std::vector<BYTE> &io_vectTemp) const
{
	if (ATUM_RESPACK_NO_OFFSET == i_nDataAt || i_nFrom > i_nTo
		|| i_nTo > i_entry.nCompressedSize)
	{
		return NULL;
	}

	const SArchive &archive = m_vectArchive[i_entry.nArchive];
	if (NULL != archive.pView)
	{
		return archive.pView + i_nDataAt + i_nFrom;
	}

	const unsigned __int64 nBytes = i_nTo - i_nFrom;
	if (0 == nBytes)
	{
		return ARR_NO_BYTES;
	}
	if (nBytes > (unsigned __int64)(size_t)-1)
	{
		return NULL;
	}
	io_vectTemp.resize((size_t)nBytes);
	return ArchiveRead(archive, i_nDataAt + i_nFrom, &io_vectTemp[0], (size_t)nBytes)
		   ? &io_vectTemp[0] : NULL;
}

///////////////////////////////////////////////////////////////////////////////
// the inflated block cache

const CResourcePack::SBlock *CResourcePack::LookupBlock(const SEntry &i_entry, int i_nChunk) const
{
	SBlockKey key;
	key.pEntry	= &i_entry;
	key.nChunk	= i_nChunk;

	EnterCriticalSection(&m_csCache);
	std::map<SBlockKey, SBlock*>::const_iterator it = m_mapBlock.find(key);
	const SBlock *pFound = (it != m_mapBlock.end()) ? it->second : NULL;
	LeaveCriticalSection(&m_csCache);
	return pFound;
}

const CResourcePack::SBlock *CResourcePack::GetBlock(const SEntry &i_entry, int i_nChunk,
													 BOOL i_bPin) const
{
	SBlockKey key;
	key.pEntry	= &i_entry;
	key.nChunk	= i_nChunk;

	EnterCriticalSection(&m_csCache);
	std::map<SBlockKey, SBlock*>::iterator it = m_mapBlock.find(key);
	SBlock *pFound = (it != m_mapBlock.end()) ? it->second : NULL;
	if (NULL != pFound && i_bPin)
	{
		pFound->bPinned = TRUE;
	}
	LeaveCriticalSection(&m_csCache);
	if (NULL != pFound)
	{
		return pFound;
	}
	// A stored entry needs nothing done to it and is normally read straight out
	// of the mapping.
	if (METHOD_DEFLATE != i_entry.usMethod
		&& !(METHOD_STORED == i_entry.usMethod && i_bPin && i_nChunk < 0
			 && NULL == m_vectArchive[i_entry.nArchive].pView))
	{
		return NULL;					// nothing to inflate, nothing to keep
	}

	// What this block covers.
	unsigned __int64 nAt	= 0;
	unsigned __int64 nSize	= i_entry.nUncompressedSize;
	if (i_nChunk >= 0)
	{
		nAt = (unsigned __int64)i_nChunk * i_entry.nChunkSize;
		if (nAt >= i_entry.nUncompressedSize)
		{
			return NULL;
		}
		nSize = i_entry.nUncompressedSize - nAt;
		if (nSize > i_entry.nChunkSize)		{ nSize = i_entry.nChunkSize; }
	}

	// Room for it?  A pin ignores the budget - that is what a pin is for.
	if (!i_bPin)
	{
		EnterCriticalSection(&m_csCache);
		const BOOL bRoom = (m_nCachedBytes + nSize <= m_nCacheBudget) ? TRUE : FALSE;
		LeaveCriticalSection(&m_csCache);
		if (!bRoom)
		{
			return NULL;
		}
	}

	// Inflate outside the lock: two threads may do the same work once, which
	// costs less than making every read wait behind an unrelated inflate.
	SBlock *pBlock = new SBlock;
	pBlock->bPinned = i_bPin;
	pBlock->vectData.resize((size_t)nSize);

	BOOL bOK = TRUE;
	if (nSize > 0)
	{
		unsigned __int64 nFrom	= 0;
		unsigned __int64 nTo	= i_entry.nCompressedSize;
		if (i_nChunk >= 0)
		{
			nFrom	= i_entry.vectChunkOffset[i_nChunk];
			nTo		= ((size_t)i_nChunk + 1 < i_entry.vectChunkOffset.size())
					  ? i_entry.vectChunkOffset[i_nChunk + 1]
					  : i_entry.nCompressedSize;
		}

		std::vector<BYTE> vectTemp;
		const BYTE *pData = EntryBytes(i_entry, EntryDataOffset(i_entry), nFrom, nTo, vectTemp);
		if (NULL == pData)
		{
			bOK = FALSE;
		}
		else if (METHOD_STORED == i_entry.usMethod)
		{
			memcpy(&pBlock->vectData[0], pData, (size_t)nSize);
		}
		else
		{
			bOK = InflateRaw(pData, nTo - nFrom, &pBlock->vectData[0], nSize);
		}
	}

	if (!bOK)
	{
		delete pBlock;
		return NULL;
	}

	EnterCriticalSection(&m_csCache);
	std::pair<std::map<SBlockKey, SBlock*>::iterator, bool> inserted =
		m_mapBlock.insert(std::pair<SBlockKey, SBlock*>(key, pBlock));
	if (!inserted.second)
	{	// another thread got there first; keep its block, drop ours
		delete pBlock;
		pBlock = inserted.first->second;
		if (i_bPin)		{ pBlock->bPinned = TRUE; }
	}
	else
	{
		m_nCachedBytes += nSize;
	}
	LeaveCriticalSection(&m_csCache);
	return pBlock;
}

BOOL CResourcePack::PinEntry(const SEntry &i_entry) const
{
	if (METHOD_STORED == i_entry.usMethod && NULL != EntryData(i_entry))
	{
		return TRUE;					// already resident, in the mapping
	}
	return (NULL != GetBlock(i_entry, -1, TRUE)) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// reading

BOOL CResourcePack::ReadEntry(const SEntry &i_entry, std::vector<BYTE> &o_vectData) const
{
	o_vectData.resize((size_t)i_entry.nUncompressedSize);
	if (0 == i_entry.nUncompressedSize)
	{
		return TRUE;
	}
	return (ReadEntryRange(i_entry, 0, &o_vectData[0], (size_t)i_entry.nUncompressedSize)
			== (size_t)i_entry.nUncompressedSize) ? TRUE : FALSE;
}

size_t CResourcePack::ReadEntryRange(const SEntry &i_entry, unsigned __int64 i_nOffset,
									 void *o_pBuffer, size_t i_nBytes) const
{
	if (i_nOffset >= i_entry.nUncompressedSize)
	{
		return 0;
	}
	if (i_nBytes > (size_t)(i_entry.nUncompressedSize - i_nOffset))
	{
		i_nBytes = (size_t)(i_entry.nUncompressedSize - i_nOffset);
	}
	if (0 == i_nBytes)
	{
		return 0;
	}

	BYTE *pOut = (BYTE*)o_pBuffer;

	// Whatever Pin() made resident answers everything, at memcpy speed.
	const SBlock *pWhole = LookupBlock(i_entry, -1);
	if (NULL != pWhole && pWhole->vectData.size() >= i_nOffset + i_nBytes)
	{
		memcpy(pOut, &pWhole->vectData[(size_t)i_nOffset], i_nBytes);
		return i_nBytes;
	}

	const unsigned __int64 nDataAt = EntryDataOffset(i_entry);
	if (ATUM_RESPACK_NO_OFFSET == nDataAt)
	{
		return 0;
	}

	if (METHOD_STORED == i_entry.usMethod)
	{	// straight out of the archive: a memcpy when it is mapped, and the one
		// ReadFile the loose file would have cost when it is not
		return ArchiveRead(m_vectArchive[i_entry.nArchive], nDataAt + i_nOffset, pOut, i_nBytes)
			   ? i_nBytes : 0;
	}

	// Holds the compressed bytes only while the archive is not mapped; a mapped
	// one is read where it lies.
	std::vector<BYTE> vectCompressed;

	///////////////////////////////////////////////////////////////////////////
	// The whole entry at once is what most of the game does - a mesh, a map, most
	// textures - read once and parsed.
	///////////////////////////////////////////////////////////////////////////
	if (0 == i_nOffset && i_nBytes == (size_t)i_entry.nUncompressedSize)
	{
		if (i_entry.vectChunkOffset.empty())
		{
			const BYTE *pData = EntryBytes(i_entry, nDataAt, 0, i_entry.nCompressedSize,
										   vectCompressed);
			return (NULL != pData && InflateRaw(pData, i_entry.nCompressedSize, pOut, i_nBytes))
				   ? i_nBytes : 0;
		}

		for (size_t c = 0; c < i_entry.vectChunkOffset.size(); c++)
		{
			const unsigned __int64 nChunkAt	= (unsigned __int64)c * i_entry.nChunkSize;
			unsigned __int64 nChunkSize		= i_entry.nUncompressedSize - nChunkAt;
			if (nChunkSize > i_entry.nChunkSize)	{ nChunkSize = i_entry.nChunkSize; }

			const unsigned __int64 nFrom = i_entry.vectChunkOffset[c];
			const unsigned __int64 nTo	 = (c + 1 < i_entry.vectChunkOffset.size())
										   ? i_entry.vectChunkOffset[c + 1]
										   : i_entry.nCompressedSize;
			const BYTE *pData = EntryBytes(i_entry, nDataAt, nFrom, nTo, vectCompressed);
			if (NULL == pData
				|| !InflateRaw(pData, nTo - nFrom, pOut + nChunkAt, nChunkSize))
			{
				return (size_t)nChunkAt;
			}
		}
		return i_nBytes;
	}

	///////////////////////////////////////////////////////////////////////////
	// chunked deflate: only the chunks the read lands in
	if (!i_entry.vectChunkOffset.empty())
	{
		const unsigned __int64 nFirst = i_nOffset / i_entry.nChunkSize;
		const unsigned __int64 nLast  = (i_nOffset + i_nBytes - 1) / i_entry.nChunkSize;
		size_t nDone = 0;

		for (unsigned __int64 c = nFirst; c <= nLast; c++)
		{
			const unsigned __int64 nChunkAt		= c * i_entry.nChunkSize;
			unsigned __int64 nChunkSize			= i_entry.nUncompressedSize - nChunkAt;
			if (nChunkSize > i_entry.nChunkSize)	{ nChunkSize = i_entry.nChunkSize; }

			const unsigned __int64 nInChunk = (i_nOffset + nDone) - nChunkAt;
			size_t nWant = (size_t)(nChunkSize - nInChunk);
			if (nWant > i_nBytes - nDone)		{ nWant = i_nBytes - nDone; }

			const SBlock *pBlock = GetBlock(i_entry, (int)c, FALSE);
			if (NULL != pBlock)
			{
				memcpy(pOut + nDone, &pBlock->vectData[(size_t)nInChunk], nWant);
			}
			else
			{	// the budget is spent; inflate the chunk and throw it away
				const unsigned __int64 nFrom = i_entry.vectChunkOffset[(size_t)c];
				const unsigned __int64 nTo	 = ((size_t)c + 1 < i_entry.vectChunkOffset.size())
											   ? i_entry.vectChunkOffset[(size_t)c + 1]
											   : i_entry.nCompressedSize;
				const BYTE *pData = EntryBytes(i_entry, nDataAt, nFrom, nTo, vectCompressed);
				std::vector<BYTE> vectChunk((size_t)nChunkSize);
				if (NULL == pData
					|| !InflateRaw(pData, nTo - nFrom, &vectChunk[0], nChunkSize))
				{
					return nDone;
				}
				memcpy(pOut + nDone, &vectChunk[(size_t)nInChunk], nWant);
			}
			nDone += nWant;
		}
		return nDone;
	}

	///////////////////////////////////////////////////////////////////////////
	// One plain deflate stream, and only a part of it wanted, with nothing to
	// seek to.
	///////////////////////////////////////////////////////////////////////////
	const SBlock *pBlock = GetBlock(i_entry, -1, FALSE);
	if (NULL != pBlock && pBlock->vectData.size() >= i_nOffset + i_nBytes)
	{
		memcpy(pOut, &pBlock->vectData[(size_t)i_nOffset], i_nBytes);
		return i_nBytes;
	}

	// Out of budget: inflate as far as the read reaches and discard the rest.
	const BYTE *pData = EntryBytes(i_entry, nDataAt, 0, i_entry.nCompressedSize, vectCompressed);
	std::vector<BYTE> vectWhole((size_t)(i_nOffset + i_nBytes));
	if (NULL == pData
		|| !InflateRaw(pData, i_entry.nCompressedSize, &vectWhole[0], vectWhole.size()))
	{
		return 0;
	}
	memcpy(pOut, &vectWhole[(size_t)i_nOffset], i_nBytes);
	return i_nBytes;
}

BOOL CResourcePack::Read(const char *i_szPath, std::vector<BYTE> &o_vectData) const
{
	if (FileExistsOnDisk(i_szPath))
	{
		return ReadWholeFile(i_szPath, o_vectData);
	}

	const SEntry *pEntry = FindEntry(i_szPath);
	if (NULL == pEntry)
	{
		return FALSE;
	}
	return ReadEntry(*pEntry, o_vectData);
}

BOOL CResourcePack::Open(const char *i_szPath, CResourceFile &o_file) const
{
	o_file.Close();

	if (FileExistsOnDisk(i_szPath))
	{
		HANDLE hFile = OpenLooseFile(i_szPath);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			LARGE_INTEGER liSize;
			if (GetFileSizeEx(hFile, &liSize) && liSize.QuadPart >= 0)
			{
				o_file.m_pPack	= this;
				o_file.m_hFile	= hFile;
				o_file.m_pEntry	= NULL;
				o_file.m_nSize	= (unsigned __int64)liSize.QuadPart;
				return TRUE;
			}
			CloseHandle(hFile);
		}
		// The file is there but would not open - fall through to the archives
		// rather than failing, which is what a missing file would do.
	}

	const SEntry *pEntry = FindEntry(i_szPath);
	if (NULL == pEntry)
	{
		return FALSE;
	}
	o_file.m_pPack	= this;
	o_file.m_hFile	= INVALID_HANDLE_VALUE;
	o_file.m_pEntry	= pEntry;
	o_file.m_nSize	= pEntry->nUncompressedSize;
	return TRUE;
}

BOOL CResourcePack::EnsureLooseCopy(const char *i_szPath) const
{
	if (FileExistsOnDisk(i_szPath))
	{
		return TRUE;
	}

	std::vector<BYTE> vectData;
	const SEntry *pEntry = FindEntry(i_szPath);
	if (NULL == pEntry || !ReadEntry(*pEntry, vectData))
	{
		return FALSE;
	}

	// The folder this belongs in may not exist at all: packing Res-Tex and
	// deleting it is exactly what an operator would do.
	{
		std::string strDirectory = i_szPath;
		const size_t nSlash = strDirectory.find_last_of("\\/");
		if (std::string::npos != nSlash && nSlash > 0)
		{
			strDirectory.erase(nSlash);
			// One level is enough: the parent is the directory the game is
			// already running in.
			CreateDirectoryA(strDirectory.c_str(), NULL);
		}
	}

	// Written under a temporary name and moved into place, so that a failure
	// half way through cannot leave a short file where a whole one belongs.
	char szTemporary[MAX_PATH * 2];
	_snprintf_s(szTemporary, sizeof(szTemporary), _TRUNCATE, "%s.extracting", i_szPath);

	HANDLE hFile = CreateFileA(szTemporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
							   FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	BOOL	bWritten = TRUE;
	size_t	nDone	 = 0;
	while (nDone < vectData.size())
	{
		const size_t nWant = vectData.size() - nDone;
		DWORD dwWritten = 0;
		if (!WriteFile(hFile, &vectData[nDone],
					   (DWORD)((nWant > 0x10000000u) ? 0x10000000u : nWant), &dwWritten, NULL)
			|| 0 == dwWritten)
		{
			bWritten = FALSE;
			break;
		}
		nDone += dwWritten;
	}
	CloseHandle(hFile);

	if (!bWritten || !MoveFileExA(szTemporary, i_szPath, MOVEFILE_REPLACE_EXISTING))
	{
		DeleteFileA(szTemporary);
		return FALSE;
	}
	return TRUE;
}

BOOL CResourcePack::Exists(const char *i_szPath) const
{
	if (FileExistsOnDisk(i_szPath))
	{
		return TRUE;
	}
	return (NULL != FindEntry(i_szPath)) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// residency

void CResourcePack::SetCacheBudget(unsigned __int64 i_nBytes)
{
	EnterCriticalSection(&m_csCache);
	m_nCacheBudget = i_nBytes;
	LeaveCriticalSection(&m_csCache);
}

unsigned __int64 CResourcePack::GetCachedBytes() const
{
	EnterCriticalSection(&m_csCache);
	const unsigned __int64 nBytes = m_nCachedBytes;
	LeaveCriticalSection(&m_csCache);
	return nBytes;
}

BOOL CResourcePack::Pin(const char *i_szPath)
{
	if (FileExistsOnDisk(i_szPath))
	{
		return TRUE;			// the file system is the cache for a loose file
	}
	const SEntry *pEntry = FindEntry(i_szPath);
	if (NULL == pEntry)
	{
		return FALSE;
	}
	return PinEntry(*pEntry);
}

namespace
{
	struct SPinJob
	{
		CResourcePack		*pPack;
		const char *const	*arrPath;
	};

	void PinWorker(int i_nIndex, void *i_pContext)
	{
		SPinJob *pJob = (SPinJob*)i_pContext;
		pJob->pPack->Pin(pJob->arrPath[i_nIndex]);
	}
}

void CResourcePack::PinAll(const char *const *i_arrPath, int i_nCount)
{
	if (NULL == i_arrPath || i_nCount <= 0)
	{
		return;
	}
	SPinJob job;
	job.pPack	= this;
	job.arrPath	= i_arrPath;
	AtumParallelFor(i_nCount, PinWorker, &job);
}

///////////////////////////////////////////////////////////////////////////////
// listing

BOOL CResourcePack::IsMountedArchive(const char *i_szDirectory, const char *i_szFileName) const
{
	if (m_vectArchive.empty())
	{
		return FALSE;
	}

	char szPath[MAX_PATH * 2];
	_snprintf_s(szPath, sizeof(szPath), _TRUNCATE, "%s/%s", i_szDirectory, i_szFileName);

	std::string strNormalised;
	if (!NormalisePath(szPath, strNormalised))
	{
		return FALSE;
	}
	for (size_t i = 0; i < m_vectArchive.size(); i++)
	{
		std::string strArchive = m_vectArchive[i].strPath;
		CanonicaliseInPlace(strArchive);
		if (strArchive == strNormalised)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CResourcePack::ListDirectory(const char *i_szDirectory, std::vector<std::string> &o_vectNames) const
{
	std::vector<std::string> vectResult;
	std::map<std::string, bool> mapSeen;			// canonicalised name -> present
	BOOL bFound = FALSE;							// the directory exists somewhere

	///////////////////////////////////////////////////////////////////////////
	// loose files first, so they win a name clash
	char szPattern[MAX_PATH * 2];
	_snprintf_s(szPattern, sizeof(szPattern), _TRUNCATE, "%s/*.*", i_szDirectory);

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(szPattern, &findData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		bFound = TRUE;							// an empty directory still counts
		do
		{
			if (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}
			if (IsMountedArchive(i_szDirectory, findData.cFileName))
			{	// the archives are containers, not resources in their own right
				continue;
			}
			std::string strCanonical = findData.cFileName;
			CanonicaliseInPlace(strCanonical);
			if (mapSeen.insert(std::pair<std::string, bool>(strCanonical, true)).second)
			{
				vectResult.push_back(findData.cFileName);
			}
		}
		while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}

	///////////////////////////////////////////////////////////////////////////
	// then whatever the archives add
	if (!m_mapEntry.empty())
	{
		std::string strPrefix;
		if (NormalisePath(i_szDirectory, strPrefix))
		{
			if ('/' != strPrefix[strPrefix.size() - 1])
			{
				strPrefix += '/';
			}

			std::map<std::string, SEntry>::const_iterator itr = m_mapEntry.lower_bound(strPrefix);
			for (; itr != m_mapEntry.end(); ++itr)
			{
				const std::string &strKey = itr->first;
				if (strKey.size() <= strPrefix.size()
					|| 0 != strKey.compare(0, strPrefix.size(), strPrefix))
				{
					break;						// past the prefix
				}
				const std::string strName = strKey.substr(strPrefix.size());
				if (std::string::npos != strName.find('/'))
				{
					continue;					// lives in a subdirectory
				}
				bFound = TRUE;
				if (mapSeen.insert(std::pair<std::string, bool>(strName, true)).second)
				{
					vectResult.push_back(strName);
				}
			}
		}
	}

	o_vectNames.swap(vectResult);
	return bFound;
}

void CResourcePack::GetSummary(char *o_szBuffer, int i_nBufferSize) const
{
	if (NULL == o_szBuffer || i_nBufferSize <= 0)
	{
		return;
	}
	if (m_vectArchive.empty())
	{
		_snprintf_s(o_szBuffer, i_nBufferSize, _TRUNCATE, "no resource archives mounted");
		return;
	}

	int nWritten = _snprintf_s(o_szBuffer, i_nBufferSize, _TRUNCATE,
							   "%d archive(s), %d entries:",
							   (int)m_vectArchive.size(), (int)m_mapEntry.size());
	for (size_t i = 0; i < m_vectArchive.size() && nWritten > 0 && nWritten < i_nBufferSize; i++)
	{
		const SArchive &archive = m_vectArchive[i];
		const char *szName = strrchr(archive.strPath.c_str(), '/');
		const char *szBack = strrchr(archive.strPath.c_str(), '\\');
		if (NULL == szName || (NULL != szBack && szBack > szName))	{ szName = szBack; }
		szName = szName ? szName + 1 : archive.strPath.c_str();
		nWritten += _snprintf_s(o_szBuffer + nWritten, i_nBufferSize - nWritten, _TRUNCATE,
								" %s (%.1f MB%s)", szName,
								(double)archive.nSize / (1024.0 * 1024.0),
								(NULL != archive.pView) ? ", mapped" : ", read on demand");
	}
}

///////////////////////////////////////////////////////////////////////////////
// mount helpers

#ifdef _ATUM_SERVER

extern char CONFIG_ROOT[1024];

BOOL AtumMountMapArchives(char *o_szSummary, int i_nSummarySize)
{
	char szMapDirectory[1024];
	_snprintf_s(szMapDirectory, sizeof(szMapDirectory), _TRUNCATE, "%s../map/", CONFIG_ROOT);

	const BOOL bResult = CResourcePack::Instance().MountDirectory(szMapDirectory);
	if(NULL != o_szSummary && i_nSummarySize > 0)
	{
		CResourcePack::Instance().GetSummary(o_szSummary, i_nSummarySize);
	}
	return bResult;
}

#endif	// _ATUM_SERVER

BOOL AtumMountClientArchives(char *o_szSummary, int i_nSummarySize)
{
	// The working directory itself first, so .\resources.zip holding "Res-
	// Obj/00000123.obj" works, then each resource directory, so .\Res-
	// Obj\meshes.zip holding "00000123.obj" works as well.
	static const char *const s_arrDirectory[] =
	{
		".", ".\\Res-Eff", ".\\Res-Map", ".\\Res-Obj", ".\\Res-Tex"
	};

	BOOL bResult = FALSE;
	for (int i = 0; i < (int)(sizeof(s_arrDirectory) / sizeof(s_arrDirectory[0])); i++)
	{
		if (CResourcePack::Instance().MountDirectory(s_arrDirectory[i]))
		{
			bResult = TRUE;
		}
	}

	// A 32 bit client has under 2 GB of address space and the interface alone
	// already holds tens of megabytes of texture, so what the pack may keep on
	// top of that has to stay modest.
	CResourcePack::Instance().SetCacheBudget((sizeof(void*) >= 8)
											 ? (512ull * 1024 * 1024)
											 : (64ull * 1024 * 1024));

	if (NULL != o_szSummary && i_nSummarySize > 0)
	{
		CResourcePack::Instance().GetSummary(o_szSummary, i_nSummarySize);
	}
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
//  ResourcePack.cpp : see ResourcePack.h
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "ResourcePack.h"
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

	BOOL FileExistsOnDisk(const char *i_szPath)
	{
		DWORD dwAttributes = GetFileAttributesA(i_szPath);
		return (INVALID_FILE_ATTRIBUTES != dwAttributes
				&& 0 == (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE;
	}

	BOOL ReadWholeFile(const char *i_szPath, std::vector<BYTE> &o_vectData)
	{
		HANDLE hFile = CreateFileA(i_szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
								   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
}

///////////////////////////////////////////////////////////////////////////////

CResourcePack &CResourcePack::Instance()
{
	static CResourcePack s_pack;
	return s_pack;
}

CResourcePack::CResourcePack()
{
}

CResourcePack::~CResourcePack()
{
	Unmount();
}

void CResourcePack::Unmount()
{
	for (size_t i = 0; i < m_vectArchive.size(); i++)
	{
		SArchive &archive = m_vectArchive[i];
		if (archive.bHeapCopy)
		{
			delete[] const_cast<BYTE*>(archive.pView);
		}
		else
		{
			if (NULL != archive.pView)		{ UnmapViewOfFile(archive.pView); }
			if (NULL != archive.hMapping)	{ CloseHandle(archive.hMapping); }
		}
		if (INVALID_HANDLE_VALUE != archive.hFile && NULL != archive.hFile)
		{
			CloseHandle(archive.hFile);
		}
	}
	m_vectArchive.clear();
	m_mapEntry.clear();
	m_vectMountRoot.clear();
}

///////////////////////////////////////////////////////////////////////////////
// path handling

BOOL CResourcePack::NormalisePath(const char *i_szPath, std::string &o_strNormalised)
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

std::string CResourcePack::KeyForPath(const std::string &i_strNormalised) const
{
	for (size_t i = 0; i < m_vectMountRoot.size(); i++)
	{
		const std::string &strRoot = m_vectMountRoot[i];
		if (i_strNormalised.size() > strRoot.size()
			&& 0 == i_strNormalised.compare(0, strRoot.size(), strRoot))
		{
			return i_strNormalised.substr(strRoot.size());
		}
	}
	return std::string();
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
	SArchive archive;
	archive.strPath		= i_szArchivePath;
	archive.hFile		= INVALID_HANDLE_VALUE;
	archive.hMapping	= NULL;
	archive.pView		= NULL;
	archive.nSize		= 0;
	archive.bHeapCopy	= FALSE;

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

	archive.hMapping = CreateFileMappingA(archive.hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (NULL != archive.hMapping)
	{
		archive.pView = (const BYTE*)MapViewOfFile(archive.hMapping, FILE_MAP_READ, 0, 0, 0);
	}
	if (NULL == archive.pView)
	{	// Win32 can run out of contiguous address space for a large archive;
		// fall back to a heap copy so the behaviour stays the same.
		if (NULL != archive.hMapping)
		{
			CloseHandle(archive.hMapping);
			archive.hMapping = NULL;
		}
		if (archive.nSize > (unsigned __int64)0x7FFFFFFF)
		{
			CloseHandle(archive.hFile);
			return FALSE;
		}
		std::vector<BYTE> vectWhole;
		if (!ReadWholeFile(i_szArchivePath, vectWhole))
		{
			CloseHandle(archive.hFile);
			return FALSE;
		}
		BYTE *pCopy = new BYTE[(size_t)archive.nSize];
		memcpy(pCopy, &vectWhole[0], (size_t)archive.nSize);
		archive.pView		= pCopy;
		archive.bHeapCopy	= TRUE;
	}

	///////////////////////////////////////////////////////////////////////////
	// end of central directory, scanned backwards past a possible comment
	const BYTE *pBase = archive.pView;
	size_t nScanFrom = (size_t)archive.nSize - SIZE_END_OF_CENTRAL_DIRECTORY;
	size_t nScanTo	 = 0;
	if (archive.nSize > SIZE_END_OF_CENTRAL_DIRECTORY + SIZE_MAX_ZIP_COMMENT)
	{
		nScanTo = (size_t)archive.nSize - SIZE_END_OF_CENTRAL_DIRECTORY - SIZE_MAX_ZIP_COMMENT;
	}

	const BYTE *pEndOfCentralDirectory = NULL;
	for (size_t nAt = nScanFrom + 1; nAt-- > nScanTo; )
	{
		if (SIGNATURE_END_OF_CENTRAL_DIRECTORY == Read32(pBase + nAt))
		{
			pEndOfCentralDirectory = pBase + nAt;
			break;
		}
	}
	if (NULL == pEndOfCentralDirectory)
	{	// not a zip, or truncated - leave the archives already mounted alone
		if (archive.bHeapCopy)			{ delete[] const_cast<BYTE*>(archive.pView); }
		else
		{
			UnmapViewOfFile(archive.pView);
			CloseHandle(archive.hMapping);
		}
		CloseHandle(archive.hFile);
		return FALSE;
	}

	unsigned __int64 nEntryCount			= Read16(pEndOfCentralDirectory + 10);
	unsigned __int64 nCentralDirectoryOffset= Read32(pEndOfCentralDirectory + 16);

	// Zip64, when the counts or offsets did not fit in the classic record
	if (0xFFFF == nEntryCount || 0xFFFFFFFFu == nCentralDirectoryOffset)
	{
		const BYTE *pLocator = pEndOfCentralDirectory - 20;
		if (pLocator >= pBase && SIGNATURE_ZIP64_LOCATOR == Read32(pLocator))
		{
			unsigned __int64 nZip64At = Read64(pLocator + 8);
			if (nZip64At + 56 <= archive.nSize
				&& SIGNATURE_ZIP64_END_OF_CENTRAL_DIRECTORY == Read32(pBase + nZip64At))
			{
				nEntryCount				= Read64(pBase + nZip64At + 32);
				nCentralDirectoryOffset	= Read64(pBase + nZip64At + 48);
			}
		}
	}

	const int nArchiveIndex = (int)m_vectArchive.size();
	m_vectArchive.push_back(archive);

	///////////////////////////////////////////////////////////////////////////
	// central directory
	unsigned __int64 nAt = nCentralDirectoryOffset;
	for (unsigned __int64 n = 0; n < nEntryCount; n++)
	{
		if (nAt + SIZE_CENTRAL_FILE_HEADER > archive.nSize
			|| SIGNATURE_CENTRAL_FILE_HEADER != Read32(pBase + nAt))
		{
			break;
		}

		const BYTE *pHeader = pBase + nAt;
		const unsigned short usFlags		= Read16(pHeader + 8);
		const unsigned short usMethod		= Read16(pHeader + 10);
		const unsigned short usNameLength	= Read16(pHeader + 28);
		const unsigned short usExtraLength	= Read16(pHeader + 30);
		const unsigned short usCommentLength= Read16(pHeader + 32);

		SEntry entry;
		entry.nArchive			= nArchiveIndex;
		entry.usMethod			= usMethod;
		entry.nCompressedSize	= Read32(pHeader + 20);
		entry.nUncompressedSize	= Read32(pHeader + 24);
		entry.nLocalHeaderOffset= Read32(pHeader + 42);

		std::string strName((const char*)(pHeader + SIZE_CENTRAL_FILE_HEADER), usNameLength);

		// Zip64 extended information, when any of the three fields was saturated
		if (0xFFFFFFFFu == entry.nCompressedSize || 0xFFFFFFFFu == entry.nUncompressedSize
			|| 0xFFFFFFFFu == entry.nLocalHeaderOffset)
		{
			const BYTE *pExtra = pHeader + SIZE_CENTRAL_FILE_HEADER + usNameLength;
			const BYTE *pExtraEnd = pExtra + usExtraLength;
			while (pExtra + 4 <= pExtraEnd)
			{
				const unsigned short usTag	= Read16(pExtra);
				const unsigned short usSize	= Read16(pExtra + 2);
				if (0x0001 == usTag)
				{
					const BYTE *pField = pExtra + 4;
					if (0xFFFFFFFFu == entry.nUncompressedSize && pField + 8 <= pExtraEnd)
					{
						entry.nUncompressedSize = Read64(pField); pField += 8;
					}
					if (0xFFFFFFFFu == entry.nCompressedSize && pField + 8 <= pExtraEnd)
					{
						entry.nCompressedSize = Read64(pField); pField += 8;
					}
					if (0xFFFFFFFFu == entry.nLocalHeaderOffset && pField + 8 <= pExtraEnd)
					{
						entry.nLocalHeaderOffset = Read64(pField);
					}
					break;
				}
				pExtra += 4 + usSize;
			}
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

		CanonicaliseInPlace(strName);
		// first archive to declare a name keeps it
		m_mapEntry.insert(std::pair<std::string, SEntry>(strName, entry));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// reading

BOOL CResourcePack::ReadEntry(const SEntry &i_entry, std::vector<BYTE> &o_vectData) const
{
	const SArchive &archive = m_vectArchive[i_entry.nArchive];

	if (i_entry.nLocalHeaderOffset + SIZE_LOCAL_FILE_HEADER > archive.nSize
		|| SIGNATURE_LOCAL_FILE_HEADER != Read32(archive.pView + i_entry.nLocalHeaderOffset))
	{
		return FALSE;
	}

	// The local header repeats the name and extra field with its own lengths.
	const BYTE *pLocal = archive.pView + i_entry.nLocalHeaderOffset;
	const unsigned __int64 nDataOffset = i_entry.nLocalHeaderOffset + SIZE_LOCAL_FILE_HEADER
										 + Read16(pLocal + 26) + Read16(pLocal + 28);
	if (nDataOffset + i_entry.nCompressedSize > archive.nSize)
	{
		return FALSE;
	}

	const BYTE *pCompressed = archive.pView + nDataOffset;
	o_vectData.resize((size_t)i_entry.nUncompressedSize);
	if (0 == i_entry.nUncompressedSize)
	{
		return TRUE;
	}

	if (METHOD_STORED == i_entry.usMethod)
	{
		memcpy(&o_vectData[0], pCompressed, (size_t)i_entry.nUncompressedSize);
		return TRUE;
	}

	// Raw deflate: a private z_stream per call, so this is safe on any thread.
	z_stream stream;
	memset(&stream, 0x00, sizeof(stream));
	if (Z_OK != inflateInit2(&stream, -MAX_WBITS))
	{
		return FALSE;
	}

	BOOL bResult = TRUE;
	unsigned __int64 nRemainingIn	= i_entry.nCompressedSize;
	unsigned __int64 nRemainingOut	= i_entry.nUncompressedSize;
	const BYTE *pIn	= pCompressed;
	BYTE *pOut		= &o_vectData[0];

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
		if (Z_OK != nStatus && Z_STREAM_END != nStatus)
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

BOOL CResourcePack::Read(const char *i_szPath, std::vector<BYTE> &o_vectData) const
{
	if (FileExistsOnDisk(i_szPath))
	{
		return ReadWholeFile(i_szPath, o_vectData);
	}
	if (m_mapEntry.empty())
	{
		return FALSE;
	}

	std::string strNormalised;
	if (!NormalisePath(i_szPath, strNormalised))
	{
		return FALSE;
	}
	const std::string strKey = KeyForPath(strNormalised);
	if (strKey.empty())
	{
		return FALSE;
	}

	std::map<std::string, SEntry>::const_iterator itr = m_mapEntry.find(strKey);
	if (itr == m_mapEntry.end())
	{
		return FALSE;
	}
	return ReadEntry(itr->second, o_vectData);
}

BOOL CResourcePack::Exists(const char *i_szPath) const
{
	if (FileExistsOnDisk(i_szPath))
	{
		return TRUE;
	}
	if (m_mapEntry.empty())
	{
		return FALSE;
	}

	std::string strNormalised;
	if (!NormalisePath(i_szPath, strNormalised))
	{
		return FALSE;
	}
	const std::string strKey = KeyForPath(strNormalised);
	if (strKey.empty())
	{
		return FALSE;
	}
	return (m_mapEntry.find(strKey) != m_mapEntry.end()) ? TRUE : FALSE;
}

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
		std::string strNormalised;
		if (NormalisePath(i_szDirectory, strNormalised))
		{
			if ('/' != strNormalised[strNormalised.size() - 1])
			{
				strNormalised += '/';
			}
			std::string strPrefix = KeyForPath(strNormalised);
			// strPrefix is "" for the mount point itself, which is what we want

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
		szName = szName ? szName + 1 : archive.strPath.c_str();
		nWritten += _snprintf_s(o_szBuffer + nWritten, i_nBufferSize - nWritten, _TRUNCATE,
								" %s (%.1f MB%s)", szName,
								(double)archive.nSize / (1024.0 * 1024.0),
								archive.bHeapCopy ? ", buffered" : ", mapped");
	}
}

///////////////////////////////////////////////////////////////////////////////
// AtumMountMapArchives

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

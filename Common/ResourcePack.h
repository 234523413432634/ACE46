///////////////////////////////////////////////////////////////////////////////
//  ResourcePack.h : read game resources from .zip archives as well as from
//  disk
//
//  Both the servers and the client open a lot of small files.
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_RESOURCE_PACK_H_
#define _ATUM_RESOURCE_PACK_H_

#include <windows.h>
#include <map>
#include <string>
#include <vector>

// Uncompressed bytes per independently decodable chunk.  A writer uses it
// when it writes an archive; the reader takes whatever the archive says.
#define ATUM_RESPACK_CHUNK_SIZE			(256 * 1024)

// Zip extra field holding the chunk table.
#define ATUM_RESPACK_EXTRA_ID			0x4154		/* "AT" */
#define ATUM_RESPACK_EXTRA_VERSION		1

// "no such offset", from the calls that answer with one.
#define ATUM_RESPACK_NO_OFFSET			((unsigned __int64)~0ull)

class CResourcePack;

///////////////////////////////////////////////////////////////////////////////
// An open resource. Cheap to keep: it holds a file handle for a loose file and
// nothing at all for an archived one.
///////////////////////////////////////////////////////////////////////////////
class CResourceFile
{
public:
	CResourceFile();
	~CResourceFile();

	BOOL				IsOpen() const		{ return (NULL != m_pPack) ? TRUE : FALSE; }
	unsigned __int64	GetSize() const		{ return m_nSize; }
	void				Close();

	// Copies at most i_nBytes from i_nOffset into o_pBuffer and returns how
	// many bytes that was - short only at the end of the resource.
	size_t Read(unsigned __int64 i_nOffset, void *o_pBuffer, size_t i_nBytes);

	// The whole resource in one block when there is one to hand: a stored archive
	// entry, or anything Pin() has made resident.
	const BYTE *GetData();

	// TRUE when this is a loose file rather than an archive entry.
	BOOL IsLooseFile() const				{ return (INVALID_HANDLE_VALUE != m_hFile) ? TRUE : FALSE; }

private:
	CResourceFile(const CResourceFile &);
	CResourceFile &operator=(const CResourceFile &);

	friend class CResourcePack;

	const CResourcePack	*m_pPack;
	HANDLE				m_hFile;		// loose file, else INVALID_HANDLE_VALUE
	const void			*m_pEntry;		// CResourcePack::SEntry, else NULL
	unsigned __int64	m_nSize;
};

///////////////////////////////////////////////////////////////////////////////

class CResourcePack
{
public:
	static CResourcePack &Instance();

	// Indexes every *.zip directly inside i_szDirectory.
	BOOL MountDirectory(const char *i_szDirectory);
	void Unmount();

	// TRUE if the path names a loose file or an archive entry.
	BOOL Exists(const char *i_szPath) const;

	// Reads the whole resource.  A loose file wins over an archived one.
	BOOL Read(const char *i_szPath, std::vector<BYTE> &o_vectData) const;

	// Opens the resource for reading in pieces.  A loose file wins here too.
	BOOL Open(const char *i_szPath, CResourceFile &o_file) const;

	// Writes an archived resource out to the path it would have had, if it is not
	// already there.
	BOOL EnsureLooseCopy(const char *i_szPath) const;

	// The names - not paths - of the files directly inside i_szDirectory, from
	// disk and from the archives, loose files winning on a name clash.
	BOOL ListDirectory(const char *i_szDirectory, std::vector<std::string> &o_vectNames) const;

	///////////////////////////////////////////////////////////////////////////
	// residency

	// How much inflated archive data may be kept, on top of whatever Pin() has
	// asked for.
	void SetCacheBudget(unsigned __int64 i_nBytes);

	// Inflates the resource and keeps it, ignoring the budget.
	BOOL Pin(const char *i_szPath);

	// Pin() over a list, across every core.
	void PinAll(const char *const *i_arrPath, int i_nCount);

	int GetArchiveCount() const		{ return (int)m_vectArchive.size(); }
	int GetEntryCount() const		{ return (int)m_mapEntry.size(); }
	unsigned __int64 GetCachedBytes() const;

	// Human readable summary for the start-up log.
	void GetSummary(char *o_szBuffer, int i_nBufferSize) const;

private:
	CResourcePack();
	~CResourcePack();
	CResourcePack(const CResourcePack &);
	CResourcePack &operator=(const CResourcePack &);

	friend class CResourceFile;

	// pView is the whole archive when it could be mapped and NULL when it could
	// not, in which case every read goes through hFile.
	struct SArchive
	{
		std::string		strPath;
		HANDLE			hFile;
		HANDLE			hMapping;
		const BYTE		*pView;			// whole archive when mapped, else NULL
		unsigned __int64 nSize;
	};

	struct SEntry
	{
		int					nArchive;			// index into m_vectArchive
		unsigned __int64	nLocalHeaderOffset;
		unsigned __int64	nCompressedSize;
		unsigned __int64	nUncompressedSize;
		unsigned short		usMethod;			// 0 stored, 8 deflate
		// Chunked deflate, from the ATUM_RESPACK_EXTRA_ID extra field.
		unsigned int					nChunkSize;
		std::vector<unsigned __int64>	vectChunkOffset;
	};

	// One inflated run of an entry.  Allocated once and neither moved nor freed
	// before Unmount(), so a reader needs the lock only to find it.
	struct SBlock
	{
		std::vector<BYTE>	vectData;
		BOOL				bPinned;
	};

	struct SBlockKey
	{
		const SEntry	*pEntry;
		int				nChunk;			// -1 for "the whole entry"
		bool operator<(const SBlockKey &i_other) const
		{
			if (pEntry != i_other.pEntry)	{ return pEntry < i_other.pEntry; }
			return nChunk < i_other.nChunk;
		}
	};

	BOOL AddArchive(const char *i_szArchivePath);
	// TRUE when a loose file is one of the archives this pack mounted.
	BOOL IsMountedArchive(const char *i_szDirectory, const char *i_szFileName) const;
	const SEntry *FindEntry(const char *i_szPath) const;

	// Copies out of an archive whether or not it is mapped.
	BOOL ArchiveRead(const SArchive &i_archive, unsigned __int64 i_nAt,
					 void *o_pBuffer, size_t i_nBytes) const;

	// Where an entry's compressed bytes start inside its archive, or
	// ATUM_RESPACK_NO_OFFSET when the local header is bad.
	unsigned __int64 EntryDataOffset(const SEntry &i_entry) const;
	// The same place in memory, NULL when the archive is not mapped.
	const BYTE *EntryData(const SEntry &i_entry) const;
	// An entry's compressed bytes [i_nFrom, i_nTo) in one block, given the
	// offset EntryDataOffset() answered with: a pointer into the mapped archive
	// when there is one, otherwise read into io_vectTemp.  NULL on a bad range.
	const BYTE *EntryBytes(const SEntry &i_entry, unsigned __int64 i_nDataAt,
						   unsigned __int64 i_nFrom, unsigned __int64 i_nTo,
						   std::vector<BYTE> &io_vectTemp) const;

	BOOL ReadEntry(const SEntry &i_entry, std::vector<BYTE> &o_vectData) const;
	size_t ReadEntryRange(const SEntry &i_entry, unsigned __int64 i_nOffset,
						  void *o_pBuffer, size_t i_nBytes) const;
	// The cached block for a run of an entry, inflating it when it is not there
	// yet and the budget allows.  NULL means "not cached", not failure.
	const SBlock *GetBlock(const SEntry &i_entry, int i_nChunk, BOOL i_bPin) const;
	// The same without inflating anything: NULL means "not resident".
	const SBlock *LookupBlock(const SEntry &i_entry, int i_nChunk) const;
	BOOL PinEntry(const SEntry &i_entry) const;

	std::vector<SArchive>			m_vectArchive;
	std::map<std::string, SEntry>	m_mapEntry;			// key: full normalised path
	std::vector<std::string>		m_vectMountRoot;	// normalised, trailing '/'

	// The cache is the one mutable part, so it is the one part that locks.
	mutable CRITICAL_SECTION			m_csCache;
	mutable std::map<SBlockKey, SBlock*> m_mapBlock;
	mutable unsigned __int64			m_nCachedBytes;
	unsigned __int64					m_nCacheBudget;
};

///////////////////////////////////////////////////////////////////////////////
// Mounts <CONFIG_ROOT>..\map, where the servers keep their map resources, and
// writes a one line summary for the caller to log.  Safe to call more than
// once; only the servers that read map data need to call it at all.
BOOL AtumMountMapArchives(char *o_szSummary, int i_nSummarySize);

// Mounts the client's resource directories - the working directory itself and
// Res-Eff, Res-Map, Res-Obj, Res-Tex under it - so an archive may be either
// .\resources.zip holding "Res-Obj/00000123.obj" or .\Res-Obj\meshes.zip
// holding "00000123.obj".  Writes the same one line summary.
BOOL AtumMountClientArchives(char *o_szSummary, int i_nSummarySize);

#endif	// _ATUM_RESOURCE_PACK_H_

///////////////////////////////////////////////////////////////////////////////
//  ResourcePack.h : read map resources from .zip archives as well as from disk
//
//  The servers open a lot of small files at start-up: ~500 .dat/.sma map files
//  in .\map and ~4600 .obj meshes in .\map\Res-Obj, every one of which is read
//  whole (FieldServer hashes them, NPCServer parses them).
///////////////////////////////////////////////////////////////////////////////

#ifndef _ATUM_RESOURCE_PACK_H_
#define _ATUM_RESOURCE_PACK_H_

#include <windows.h>
#include <map>
#include <string>
#include <vector>

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

	// The names - not paths - of the files directly inside i_szDirectory, from
	// disk and from the archives, loose files winning on a name clash.
	BOOL ListDirectory(const char *i_szDirectory, std::vector<std::string> &o_vectNames) const;

	int GetArchiveCount() const		{ return (int)m_vectArchive.size(); }
	int GetEntryCount() const		{ return (int)m_mapEntry.size(); }

	// Human readable summary for the start-up log.
	void GetSummary(char *o_szBuffer, int i_nBufferSize) const;

private:
	CResourcePack();
	~CResourcePack();
	CResourcePack(const CResourcePack &);
	CResourcePack &operator=(const CResourcePack &);

	struct SArchive
	{
		std::string		strPath;
		HANDLE			hFile;
		HANDLE			hMapping;
		const BYTE		*pView;			// whole archive, mapped or heap copy
		unsigned __int64 nSize;
		BOOL			bHeapCopy;
	};

	struct SEntry
	{
		int					nArchive;			// index into m_vectArchive
		unsigned __int64	nLocalHeaderOffset;
		unsigned __int64	nCompressedSize;
		unsigned __int64	nUncompressedSize;
		unsigned short		usMethod;			// 0 stored, 8 deflate
	};

	BOOL AddArchive(const char *i_szArchivePath);
	// TRUE when a loose file is one of the archives this pack mounted.
	BOOL IsMountedArchive(const char *i_szDirectory, const char *i_szFileName) const;
	BOOL ReadEntry(const SEntry &i_entry, std::vector<BYTE> &o_vectData) const;

	// Normalised (absolute, upper case, '/' separated) form used for lookups.
	static BOOL NormalisePath(const char *i_szPath, std::string &o_strNormalised);
	// The archive relative key for a normalised path, "" when it is outside
	// every mount point.
	std::string KeyForPath(const std::string &i_strNormalised) const;

	std::vector<SArchive>			m_vectArchive;
	std::map<std::string, SEntry>	m_mapEntry;		// key: "RES-OBJ/00000123.OBJ"
	std::vector<std::string>		m_vectMountRoot;	// normalised, trailing '/'
};

///////////////////////////////////////////////////////////////////////////////
// Mounts <CONFIG_ROOT>..\map, where the servers keep their map resources, and
// writes a one line summary for the caller to log.  Safe to call more than
// once; only the servers that read map data need to call it at all.
BOOL AtumMountMapArchives(char *o_szSummary, int i_nSummarySize);

#endif	// _ATUM_RESOURCE_PACK_H_

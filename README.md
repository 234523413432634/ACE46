# AceOnline-ep46

Requirements
1.	Visual Studio 2026
2.	C++ MFC for x64/x86 (Latest MSVC)

Build once:
1.	AceOnline-ep46\Server\XmlRpc\XmlRpc.sln in "Release" configuration
2.	AceOnline-ep46\Server\XmlRpc\XmlRpc.sln in "Release|x64" configuration, for the 64-bit servers
3.	AceOnline-ep46\Server\ZipArchive\ZipArchive.sln in "Release STL MT" configuration
4.	AceOnline-ep46\Client\BaseClasses\baseclasses.sln in "Release" configuration
5.	AceOnline-ep46\Client\BaseClasses\baseclasses.sln in "Release|x64" configuration, for the 64-bit client

Build order:
1.	AceOnline-ep46\Server\GameServer\GameServer.sln in “R_Evo” configuration
2.	AceOnline-ep46\Server\GameServer\GameServer.sln in “R_Evo_ARENA” configuration for arena server
3.	AceOnline-ep46\Client\ProjectAtum.sln in “R_Evo” configuration

Every solution builds for Win32 and x64 from the same configurations.  The x64
servers land in Build\Bins\x64 and the x64 client in Build\Client\x64; the
launcher and the MFC admin tools are Win32 only.

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/Paths.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/INI_System/INI_Parser.hpp"

#include "../../include/PackageManagement/PackageConfig.hpp"

bool PackageConfig::GetPackage( std::string pkgname, Package & pkg )
{
	if( !HandlePkgDirs() )
		return false;

	StringToLower( pkgname );

	std::string file_name = PACKAGE_DIR + pkgname + PACKAGE_EXT;

	Electrux::INI_Parser parser;

	auto res = parser.LoadFromFile( file_name );

	if( res != Electrux::SUCCESS )
		return false;

	std::string deps;

	std::string prefix;
#ifdef __linux__
	prefix = "Linux";
#elif __APPLE__
	prefix = "Mac";
#endif
	parser.GetDataString( "Core", "Name", pkg.name );
	parser.GetDataString( "Core", "Description", pkg.description );
	parser.GetDataString( "Core", "Lang", pkg.lang );
	parser.GetDataString( "Core", "Type", pkg.type );
	parser.GetDataString( "Core", "Version", pkg.version );
	parser.GetDataString( "Core", "Deps", deps );
	pkg.deplist = DelimStringToVector( deps );

	parser.GetDataString( "Core", prefix + "IncludeDir", pkg.incdir );
	parser.GetDataString( "Core", prefix + "LibDir", pkg.libdir );
	parser.GetDataString( "Core", "LibraryFlags", pkg.libflags );

	parser.GetDataString( pkg.type, "URL", pkg.url );
	parser.GetDataString( pkg.type, prefix + "File", pkg.file );

	if( pkg.type == "Source" ) {
		parser.GetDataString( pkg.type, "BuildCommands", pkg.buildcmds );
	}

	return true;
}

bool PackageConfig::HandlePkgDirs()
{
	struct stat info;

	if( stat( PACKAGE_DIR.c_str(), & info ) != 0 ) {
		int res = mkdir( PACKAGE_DIR.c_str(), 0755 );

		if( res != 0 )
			return false;
	}

	struct stat info2;

	if( stat( PACKAGE_TMP.c_str(), & info2 ) != 0 ) {
		int res = mkdir( PACKAGE_TMP.c_str(), 0755 );

		if( res != 0 )
			return false;
	}

	return true;
}
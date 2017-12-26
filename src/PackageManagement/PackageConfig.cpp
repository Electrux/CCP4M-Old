#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/Paths.hpp"
#include "../../include/CommonFuncs.hpp"
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

	parser.GetDataString( "Core", "Name", pkg.name );
	parser.GetDataString( "Core", "Description", pkg.description );
	parser.GetDataString( "Core", "URL", pkg.url );
#ifdef __linux__
	parser.GetDataString( "Core", "LinuxFile", pkg.file );
	parser.GetDataString( "Core", "LinuxIncludeDir", pkg.incdir );
	parser.GetDataString( "Core", "LinuxLibDir", pkg.libdir );
#elif __APPLE__
	parser.GetDataString( "Core", "MacFile", pkg.file );
	parser.GetDataString( "Core", "MacIncludeDir", pkg.incdir );
	parser.GetDataString( "Core", "MacLibDir", pkg.libdir );
#endif
	parser.GetDataString( "Core", "Version", pkg.version );

	parser.GetDataString( "Core", "Deps", deps );
	pkg.deplist = DelimStringToVector( deps );

	parser.GetDataString( "Core", "LibraryFlags", pkg.libflags );

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
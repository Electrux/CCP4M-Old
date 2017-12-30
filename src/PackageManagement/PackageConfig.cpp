#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/Paths.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
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
	std::string existas;

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

	pkg.existfile = FetchExistFile( parser );

	parser.GetDataString( pkg.type, "URL", pkg.url );
	parser.GetDataString( pkg.type, prefix + "File", pkg.file );

	if( pkg.type == "Source" ) {
		parser.GetDataString( pkg.type, "BuildCommands", pkg.buildcmds );
	}

	return true;
}

std::string PackageConfig::FetchExistFile( Electrux::INI_Parser & parser )
{
	std::string existas;

	parser.GetDataString( "Core", "ExistsAs", existas );

	std::string file;
	if( !existas.empty() ) {
		auto vec = DelimStringToVector( existas, ':' );
		if( vec.size() <= 1 )
			return "";

		// Binary
		if( existas == "Binary" ) {
			file = vec[ 1 ];
		}
		// Library
		else if( existas == "Library" ) {
#ifdef __linux__
			file = vec[ 1 ];
#elif __APPLE__
			if( vec.size() <= 2 )
				return "";
			file = vec[ 2 ];
#endif
		}
		
		DispColoredData( "Exists as:", file, FIRST_COL, SECOND_COL, true );
	}

	if( file.empty() )
		return "";

	if( existas == "Binary" ) {
		auto path = GetEnvPath();

		for( auto p : path ) {
			if( LocExists( p + file ) ) {
				return p + file;
			}
		}
	}
	else if( existas == "Library" ) {
		if( LocExists( "/usr/lib/" + file ) ) {
			return "/usr/lib/" + file;
		}
		else if( LocExists( "/usr/local/lib/" + file ) ) {
			return "/usr/local/lib/" + file;
		}
	}

	return "";
 }

bool PackageConfig::HandlePkgDirs()
{
	if( !LocExists( PACKAGE_DIR ) && CreateDir( PACKAGE_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_TMP ) && CreateDir( PACKAGE_TMP, false ) != 0 )
		return false;

	return true;
}
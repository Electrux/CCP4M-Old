#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/CoreData.hpp"
#include "../../include/Paths.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/INI_System/INI_Parser.hpp"

#include "../../include/PackageManagement/PackageConfig.hpp"

bool PackageConfig::GetPackage( const std::string & packagename, Package & pkg )
{
	if( !HandlePkgDirs() )
		return false;
	
	std::string pkgname = packagename;

	StringToLower( pkgname );

	std::string file_name = PACKAGE_LIST_DIR + pkgname + PACKAGE_EXT;

	Electrux::INI_Parser parser;

	auto res = parser.LoadFromFile( file_name );

	if( res != Electrux::SUCCESS )
		return false;

	std::string deps;
	std::string existas;

	std::string prefix;

	SetVarForArchitecture( prefix, { "Linux", "Mac", "" } );

	parser.GetDataString( "Core", "Name", pkg.name );
	parser.GetDataString( "Core", "Description", pkg.description );
	parser.GetDataString( "Core", "Lang", pkg.lang );
	parser.GetDataString( "Core", "Type", pkg.type );
	parser.GetDataString( "Core", "Version", pkg.version );
	parser.GetDataString( "Core", "Deps", deps );
	pkg.deplist = DelimStringToVector( deps );

	parser.GetDataString( "Core", prefix + "IncludeDir", pkg.incdir );
	parser.GetDataString( "Core", prefix + "LibraryDir", pkg.libdir );

	if( pkg.incdir.empty() || pkg.libdir.empty() ) {
		parser.GetDataString( "Core", "IncludeDir", pkg.incdir );
		parser.GetDataString( "Core", "LibraryDir", pkg.libdir );
	}

	if( pkg.incdir.empty() || pkg.libdir.empty() ) {
		FetchDefaultIncLibDir( pkg );
	}

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
	std::vector< std::string > vec;
	if( !existas.empty() ) {
		vec = DelimStringToVector( existas, ':' );

		if( vec.size() <= 1 )
			return "";

		// Binary
		if( vec[ 0 ] == "Binary" ) {
			file = vec[ 1 ];
		}
		// Library
		else if( vec[ 0 ] == "Library" ) {
			SetVarForArchitecture( file, std::vector< std::string>( vec.begin() + 1, vec.end() ) );
		}
	}

	if( file.empty() )
		return "";

	if( vec[ 0 ] == "Binary" ) {

		std::string res;

		if( LocExistsInPath( file, res ) )
			return res;
	}
	else if( vec[ 0 ] == "Library" ) {
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
	if( !LocExists( PACKAGE_BASE_DIR ) && CreateDir( PACKAGE_BASE_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_LIST_DIR ) && CreateDir( PACKAGE_LIST_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_TMP ) && CreateDir( PACKAGE_TMP, false ) != 0 )
		return false;

	return true;
}

void PackageConfig::FetchDefaultIncLibDir( Package & pkg )
{
	SetVarForArchitecture( pkg.incdir, { "/usr/include", "/usr/local/include", "" } );
	SetVarForArchitecture( pkg.libdir, { "/usr/lib", "/usr/local/lib", "" } );
}
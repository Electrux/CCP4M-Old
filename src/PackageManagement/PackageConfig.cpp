#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/CoreData.hpp"
#include "../../include/Paths.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"
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

	SetVarForArchitecture( prefix, { "Linux", "Mac", "BSD", "" } );

	parser.GetDataString( "Core", "Name", pkg.name );
	parser.GetDataString( "Core", "Description", pkg.description );
	parser.GetDataString( "Core", "Lang", pkg.lang );
	parser.GetDataString( "Core", "Type", pkg.type );
	parser.GetDataString( "Core", "Version", pkg.version );
	parser.GetDataString( "Core", "Deps", deps );
	pkg.deplist = DelimStringToVector( deps );

	parser.GetDataString( "Core", "LibraryFlags", pkg.libflags );

	parser.GetDataString( pkg.type, "URL", pkg.url );

	if( pkg.type == "Source" ) {
		parser.GetDataString( pkg.type, "File", pkg.file );
		parser.GetDataString( pkg.type, "BuildMode", pkg.buildmode );
		parser.GetDataString( pkg.type, "CleanupDirectories", pkg.cleanupdirs );
	}
	else if( pkg.type == "Binary" ) {
		parser.GetDataString( pkg.type, prefix + "File", pkg.file );
	}

	return true;
}

bool PackageConfig::HandlePkgDirs()
{
	if( !LocExists( PACKAGE_BASE_DIR ) && CreateDir( PACKAGE_BASE_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_LIST_DIR ) && CreateDir( PACKAGE_LIST_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_TMP ) && CreateDir( PACKAGE_TMP, false ) != 0 )
		return false;
	
	if( !LocExists( PACKAGE_INSTALL_DIR ) && CreateDir( PACKAGE_INSTALL_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_INCLUDE_INSTALL_DIR ) && CreateDir( PACKAGE_INCLUDE_INSTALL_DIR, false ) != 0 )
		return false;

	if( !LocExists( PACKAGE_LIBRARY_INSTALL_DIR ) && CreateDir( PACKAGE_LIBRARY_INSTALL_DIR, false ) != 0 )
		return false;

	if( ARCH == MAC && !LocExists( PACKAGE_FRAMEWORKS_INSTALL_DIR ) && CreateDir( PACKAGE_FRAMEWORKS_INSTALL_DIR, false ) != 0 )
		return false;
	
	if( !LocExists( PACKAGE_INSTALL_DIR + "lib64" ) &&
		DispExecuteNoErr( "ln -s " + PACKAGE_LIBRARY_INSTALL_DIR + " " + PACKAGE_INSTALL_DIR + "lib64", false ) != 0 )
		return false;

	return true;
}
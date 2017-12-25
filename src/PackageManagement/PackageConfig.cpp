#include <algorithm>
#include <string>
#include <cctype>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/Paths.hpp"
#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/INI_System/INI_Parser.hpp"

#include "../../include/PackageManagement/PackageConfig.hpp"

bool PackageConfig::GetPackage( std::string pkgname, Package & pkg )
{
	struct stat info;

	if( stat( PACKAGE_DIR.c_str(), & info ) != 0 ) {
		int res = mkdir( PACKAGE_DIR.c_str(), 0755 );

		if( res != 0 )
			return false;
	}

	std::transform( pkgname.begin(), pkgname.end(), pkgname.begin(),
			[]( char c ) { return std::tolower( c ); } );

	std::string file_name = PACKAGE_DIR + pkgname + PACKAGE_EXT;

	Electrux::INI_Parser parser;

	auto res = parser.LoadFromFile( file_name );

	if( res != Electrux::SUCCESS )
		return false;

	parser.GetDataString( "Core", "Name", pkg.name );
	parser.GetDataString( "Core", "Description", pkg.description );
	parser.GetDataString( "Core", "URL", pkg.url );
	parser.GetDataString( "Core", "Version", pkg.version );

	return true;
}
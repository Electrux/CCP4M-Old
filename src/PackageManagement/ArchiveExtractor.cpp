#include <iostream>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"

#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/PackageManagement/ArchiveExtractor.hpp"

bool ExtractArchive( const Package & pkg )
{
	std::string archive = PACKAGE_TMP + pkg.file;

	std::string taroptions = GetTarOptions( pkg.file );
	if( taroptions.empty() ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unknown archive format! Exiting!", RED, true );
		return false;
	}

	std::string archivedir = GetPackageVersionDir( pkg );

	if( LocExists( archivedir ) ) {
		DispExecuteNoErr( "rm -rf " + archivedir, false );
	}

	if( !CreatePackageDir( pkg ) ) {
		DispColoredData( CROSS, RED, true );
		return false;
	}

	std::string cmd = "tar --strip 1 " + taroptions + " " + archive + " -C " + archivedir;

	if( DispExecuteNoErr( cmd, false ) != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to extract archive! Exiting!", RED, true );
		return false;
	}

	DispColoredData( TICK, GREEN, true );

	return true;
}

std::string GetTarOptions( const Package & pkg )
{
	if( pkg.file.find( ".tar.gz" ) != std::string::npos )
		return "-xzf";

	if( pkg.file.find( ".tar.bz2" ) != std::string::npos )
		return "-xjf";

	if( pkg.file.find( ".tar" ) != std::string::npos )
		return "-xf";

	std::string file = pkg.url + pkg.file;

	if( file.find( "tar.gz" ) != std::string::npos )
		return "-xzf";

	if( file.find( "tar.bz2" ) != std::string::npos )
		return "-xjf";

	if( file.find( "tar" ) != std::string::npos )
		return "-xf";

	return "";
}
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageUninstaller.hpp"

bool UninstallArchive( const Package & pkg )
{
	bool usecustomuninstaller = false;

	if( pkg.type == "Binary" || pkg.buildmode.empty() ) {
		DispColoredData( "Using auto generated uninstaller file ...", TICK, FIRST_COL, GREEN, true );
		usecustomuninstaller = true;
	}
	else {
		DispColoredData( "Using make uninstall command ...", TICK, FIRST_COL, GREEN, true );
	}

	if( usecustomuninstaller ) {
		DispColoredData( "Removing data ... " );
		std::vector< std::string > cpdata = GetCopiedData( pkg );

		if( !RemoveCopiedData( pkg, cpdata ) ) {
			DispColoredData( "Unable to remove all copied data!", CROSS, FIRST_COL, RED, true );
			return false;
		}

		DispColoredData( TICK, GREEN, true );
		return true;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		DispColoredData( "Unable to get working directory!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	if( !ChangeWorkingDir( GetPackageVersionDir( pkg ) ) ) {
		DispColoredData( "Unable to change working directory to the extracted package directory!",
				CROSS, FIRST_COL, RED, true );
		return false;
	}

	DispColoredData( "Uninstalling using make uninstall ... " );

	int res;
	std::string errors;

	res = DispExecute( "make uninstall", errors );

	if( res != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Unable to uninstall using make uninstall!", CROSS, FIRST_COL, RED, true );
		DispColoredData( "Attempting to uninstall using install_manifest.txt ... ", TICK, FIRST_COL, GREEN, false );
		if( !UninstallUsingInstallManifest( pkg ) )
			return false;
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		DispColoredData( TICK, GREEN, true );
	}

	ChangeWorkingDir( cwd );
	return true;
}

bool UninstallUsingInstallManifest( const Package & pkg )
{
	std::vector< std::string > data;
	std::string line;

	std::fstream file;
	file.open( "install_manifest.txt", std::ios::in );

	if( !file ) {
		DispColoredData( "Error: Unable to uninstall using install_manifest.txt! You are on your own!",
				CROSS, RED, RED, true );
		return false;
	}

	while( std::getline( file, line ) ) {
		data.push_back( line );
	}

	bool res = RemoveCopiedData( pkg, data );

	if( res ) {
		DispColoredData( "Removed installed files!", TICK, FIRST_COL, GREEN, true );
	}
	else {
		DispColoredData( "Error: Unable to remove installed files!", CROSS, RED, RED, true );
		return false;
	}

	file.close();

	if( !pkg.cleanupdirs.empty() ) {
		DispColoredData( "Attempting to clean directories up ... ", TICK, FIRST_COL, GREEN, true );
		auto cleanupdirs = DelimStringToVector( pkg.cleanupdirs );

		for( auto cleanupdir : cleanupdirs ) {
			DispColoredData( "=> Removing directory:", cleanupdir, FIRST_COL, SECOND_COL, true );
			if( DispExecuteNoErr( "rm -rf " + PACKAGE_INSTALL_DIR + cleanupdir, false ) != 0 ) {
				DispColoredData( "=> Unable to remove directory:",
						PACKAGE_INSTALL_DIR + cleanupdir, "!", RED, CYAN, RED, false );
				DispColoredData( CROSS, RED, true );
				return false;
			}
		}
	}

	DispColoredData( "Successfully cleaned all directories!", TICK, FIRST_COL, GREEN, true );
	return true;
}
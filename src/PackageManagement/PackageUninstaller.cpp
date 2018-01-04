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
		DispColoredData( "Attempting to uninstall using install_manifest.txt ...", TICK, FIRST_COL, GREEN, true );
		if( !UninstallUsingInstallManifest( pkg ) )
			return false;
		ChangeWorkingDir( cwd );
		return true;
	}
	else {
		DispColoredData( TICK, GREEN, true );
	}
	
	if( pkg.cleanupdirs.empty() )
		return true;

	DispColoredData( " =>", "Cleaning directories up ... ", SECOND_COL, FIRST_COL, false );
	auto cleanupdirs = DelimStringToVector( pkg.cleanupdirs );

	for( auto cleanupdir : cleanupdirs ) {
		if( DispExecuteNoErr( "rm -rf " + PACKAGE_INSTALL_DIR + cleanupdir ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( " =>", "Unable to remove directory:",
					PACKAGE_INSTALL_DIR + cleanupdir, RED, RED, CYAN, false );
			DispColoredData( " !", CROSS, RED, RED, true );
			ChangeWorkingDir( cwd );
			return false;
		}
	}

	ChangeWorkingDir( cwd );

	DispColoredData( TICK, GREEN, true );
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

	DispColoredData( " =>", "Removing installed files ... ", SECOND_COL, FIRST_COL, false );

	bool res = RemoveCopiedData( pkg, data );

	if( res ) {
		DispColoredData( TICK, GREEN, true );
	}
	else {
		DispColoredData( CROSS, RED, true );
		DispColoredData( " =>", "Error: Unable to remove installed files!", CROSS, RED, RED, RED, true );
		return false;
	}

	file.close();
	return true;
}
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <fstream>

#include "../../include/CoreData.hpp"
#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallDirectory( const Package & pkg )
{
	bool use_framework = false;

	std::vector< std::string > copiedfiles;

	auto copyfiles = GetCopyList( pkg, use_framework );

	FetchExtraDirs( pkg, copyfiles, copiedfiles );

	if( !CheckNecessaryPermissions( pkg, use_framework ) ) {
		DispColoredData( "Error: You do not have necessary permissions to modify package directories!",
				RED, true );
		return false;
	}

	for( auto dir : copiedfiles ) {
		if( CreateDir( dir, false ) != 0 ) {
			DispColoredData( "Error: Unable to create required directory for installation!", RED, true );
			return false;
		}
	}

	if( ARCH != MAC )
		use_framework = false;

	std::string archivedir = GetPackageVersionDir( pkg );
	std::string incdir = archivedir + "/include/";
	std::string libdir = archivedir + "/lib/";
	std::string fwdir = archivedir + "/Frameworks/";

	int prevsize = 0;

	std::string cpinput;
	std::string cpoutput;

	// INCLUDE FILES
	if( !copyfiles[ "inc" ].empty() )
		DispColoredData( "Copying include files ... " );

	for( auto file : copyfiles[ "inc" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = incdir + file.dir + file.file;
		cpoutput = pkg.incdir + "/" + file.dir;

		copiedfiles.push_back( cpoutput + file.file );

		MoveOutputCursorBack( prevsize );

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );

			DispColoredData( CROSS, RED, true );
			DispColoredData( "Error: Could not copy includes!", RED, true );
			DispColoredData( "Reverting installationg ... ", RED, true );

			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	MoveOutputCursorBack( prevsize );
	if( !copyfiles[ "inc" ].empty() )
		DispColoredData( TICK, GREEN, true );

	// LIBRARY FILES
	if( !copyfiles[ "lib" ].empty() )
		DispColoredData( "Copying library files ... " );

	for( auto file : copyfiles[ "lib" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = libdir + file.dir + file.file;
		cpoutput = pkg.libdir + "/" + file.dir;

		copiedfiles.push_back( cpoutput + file.file );

		MoveOutputCursorBack( prevsize );

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );

			DispColoredData( CROSS, RED, true );
			DispColoredData( "Error: Could not copy libraries!", RED, true );
			DispColoredData( "Reverting installationg ... ", RED, true );

			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	MoveOutputCursorBack( prevsize );
	if( !copyfiles[ "lib" ].empty() )
		DispColoredData( TICK, GREEN, true );

	// FRAMEWORK FILES
	if( !copyfiles[ "fw" ].empty() )
		DispColoredData( "Copying framework files ... " );

	for( auto file : copyfiles[ "fw" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = fwdir + file.dir + file.file;
		cpoutput = "/Library/Frameworks/" + file.dir;

		copiedfiles.push_back( cpoutput + file.file );

		MoveOutputCursorBack( prevsize );

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );

			DispColoredData( CROSS, RED, true );
			DispColoredData( "Error: Could not copy frameworks!", RED, true );
			DispColoredData( "Reverting installationg ... ", RED, true );

			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	MoveOutputCursorBack( prevsize );
	if( !copyfiles[ "fw" ].empty() )
		DispColoredData( TICK, GREEN, true );

	if( !SaveCopiedData( pkg, copiedfiles ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to save installation information!", RED, true );
		DispColoredData( "Reverting installation ... ", RED, true );
		RevertInstallation( pkg, copiedfiles );
	}

	return true;
}

std::map< std::string, std::vector< DirFile > > GetCopyList( const Package & pkg, bool & use_framework )
{
	std::string archivedir = GetPackageVersionDir( pkg );
	std::string incdir = archivedir + "/include";
	std::string libdir = archivedir + "/lib";
	std::string fwdir = archivedir + "/Frameworks";

	std::map< std::string, std::vector< DirFile > > list = {
		{ "inc", {} },
		{ "lib", {} },
		{ "fw", {} }
	};

	if( LocExists( incdir ) ) {
		GetWildCardFilesInDir( incdir, list[ "inc" ], "*" );

	}
	if( LocExists( libdir ) ) {
		GetWildCardFilesInDir( libdir, list[ "lib" ], "*.so*, *.l*, *.dyl*" );
	}

	if( ARCH == MAC ) {
		if( LocExists( fwdir ) && GetWildCardFilesInDir( fwdir, list[ "fw" ], "*" ) > 0 )
			use_framework = true;
	}

	return list;
}

void RevertInstallation( const Package & pkg, std::vector< std::string > & data )
{
	if( !RemoveCopiedData( pkg, data ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Reverting failed! Exiting!", FIRST_COL, true );
	}
	else {
		DispColoredData( TICK, GREEN, true );
		DispColoredData( "Reverting successful! Exiting!", FIRST_COL, true );
	}
}
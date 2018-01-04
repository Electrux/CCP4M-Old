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

	if( !CopyFiles( pkg, copyfiles, copiedfiles, "inc", "include" ) )
		return false;

	if( !CopyFiles( pkg, copyfiles, copiedfiles, "lib", "library" ) )
		return false;

	if( use_framework && !CopyFiles( pkg, copyfiles, copiedfiles, "fw", "framework" ) )
		return false;

	if( !SaveCopiedData( pkg, copiedfiles ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to save installation information!", RED, true );
		DispColoredData( "Reverting installation ... ", RED, true );
		RevertInstallation( pkg, copiedfiles );
		return false;
	}

	return true;
}

bool CopyFiles( const Package & pkg,
		std::map< std::string, std::vector< DirFile > > & copyfiles,
		std::vector< std::string > & copiedfiles,
		const std::string key, const std::string dispval )
{
	std::string archivedir = GetPackageVersionDir( pkg );

	std::string fromdir, todir;

	if( dispval == "include" ) {
		fromdir = archivedir + "/include/";
		todir = PACKAGE_INCLUDE_INSTALL_DIR;
	}
	else if( dispval == "library" ) {
		fromdir = archivedir + "/lib/";
		todir = PACKAGE_LIBRARY_INSTALL_DIR;
	}
	else if( dispval == "framework" ) {
		fromdir = archivedir + "/Frameworks/";
		todir = PACKAGE_FRAMEWORKS_INSTALL_DIR;
	}

	int prevsize = 0;

	if( !copyfiles[ key ].empty() ) {
		DispColoredData( "Copying " + dispval + " files ... " );
	
		std::string cpinput, cpoutput;

		for( auto file : copyfiles[ key ] ) {
			std::string op = "Copying file: " + file.dir + file.file;

			cpinput = fromdir + file.dir + file.file;
			cpoutput = todir + file.dir;

			copiedfiles.push_back( cpoutput + file.file );

			MoveOutputCursorBack( prevsize );

			prevsize = DisplayOneLinerString( op );
			if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
				MoveOutputCursorBack( prevsize );

				DispColoredData( CROSS, RED, true );
				DispColoredData( "Error: Could not copy " + dispval + " files!", RED, true );
				DispColoredData( "Reverting installation ... ", RED, true );

				RevertInstallation( pkg, copiedfiles );
				return false;
			}
		}
		MoveOutputCursorBack( prevsize );
		DispColoredData( TICK, GREEN, true );
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

void FetchExtraDirs( const Package & pkg,
		const std::map< std::string, std::vector< DirFile > > & copyfiles,
		std::vector< std::string > & fileanddir )
{
	for( auto type : copyfiles ) {
		std::string prefix;
		if( type.first == "inc" ) {
			prefix = PACKAGE_INCLUDE_INSTALL_DIR;
		}
		if( type.first == "lib" ) {
			prefix = PACKAGE_LIBRARY_INSTALL_DIR;
		}
		if( type.first == "fw" ) {
			prefix = PACKAGE_FRAMEWORKS_INSTALL_DIR;
		}
		for( auto data : type.second ) {
			std::string dir = prefix + data.dir;
			if( dir.empty() )
				continue;

			if( std::find( fileanddir.begin(), fileanddir.end(), dir ) != fileanddir.end() )
				continue;

			if( dir == PACKAGE_INCLUDE_INSTALL_DIR ||
				dir == PACKAGE_LIBRARY_INSTALL_DIR ||
				dir == PACKAGE_FRAMEWORKS_INSTALL_DIR )
				continue;

			fileanddir.push_back( dir );
		}
	}
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
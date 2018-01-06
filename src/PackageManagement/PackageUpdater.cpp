#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>

#include "../../include/CoreData.hpp"
#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManager.hpp"

#include "../../include/PackageManagement/PackageConfig.hpp"
#include "../../include/PackageManagement/PackageUpdater.hpp"

int UpdatePackageList( PackageManager & mgr )
{
	if( !PackageConfig::HandlePkgDirs() ) {
		DispColoredData( "Unable to create/manipulate the configuration directories! Check permissions!",
				CROSS, FIRST_COL, RED, true );
		return 1;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		DispColoredData( "Unable to get current working directory!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	if( !ChangeWorkingDir( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Unable to change working directory to the package list directory!",
				CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string res;
	if( !LocExistsInPath( GIT_CMD, res ) ) {
		DispColoredData( GIT_CMD, "is not installed! Cannot continue!", CROSS,
				SECOND_COL, FIRST_COL, RED, true );
		ChangeWorkingDir( cwd );
		return -1;
	}

	std::map< std::string, long long > prevupdatetimes = GetPackageUpdateTimes();
	std::map< std::string, long long > newupdatetimes;
	std::vector< std::string > newpkgs, removedpkgs;

	if( IsDirEmpty( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Cloning list repository to:", PACKAGE_LIST_DIR, "... ",
				FIRST_COL, SECOND_COL, THIRD_COL, false );

		if( DispExecuteNoErr( GIT_CLONE_CMD, true, false ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to clone repository! Cannot continue!", CROSS,
					FIRST_COL, RED, true );
			ChangeWorkingDir( cwd );
			return 1;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}
	else {
		DispColoredData( "Pulling list repository to:", PACKAGE_LIST_DIR, "... ",
				FIRST_COL, SECOND_COL, THIRD_COL, false );

		if( DispExecuteNoErr( GIT_PULL_CMD, true, false ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to pull repository! Cannot continue!", CROSS,
					FIRST_COL, RED, true );
			ChangeWorkingDir( cwd );
			return 1;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}
	ChangeWorkingDir( cwd );

	DispColoredData( "Updating package list successful!", TICK, FIRST_COL, GREEN, true );

	removedpkgs = GetRemovedPackages( prevupdatetimes );
	newpkgs = ReCreatePackageUpdateTimesFile( prevupdatetimes );
	newupdatetimes = GetPackageUpdateTimes();

	UpdatePackages( mgr, prevupdatetimes, newupdatetimes, newpkgs, removedpkgs );
	return 0;
}

std::map< std::string, long long > GetPackageUpdateTimes()
{
	std::map< std::string, long long > times;

	std::fstream file;

	file.open( PACKAGE_UPDATE_TIME_FILE, std::ios::in );

	if( !file )
		return times;
	
	std::string line;

	while( std::getline( file, line ) ) {
		TrimString( line );

		if( line == "" )
			continue;

		auto vec = DelimStringToVector( line );

		// vec[ 0 ] = package name, vec[ 1 ] = last modified time.
		times[ vec[ 0 ] ] = std::stoll( vec[ 1 ] );
	}

	file.close();

	return times;
}

std::vector< std::string > ReCreatePackageUpdateTimesFile( const std::map< std::string, long long > & prevupdatetimes )
{
	std::vector< std::string > newpkgs;

	if( !LocExists( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Error: Package list directory doesn't exist!", CROSS, RED, RED, true );
		return newpkgs;
	}

	std::vector< DirFile > pkgifiles;

	int res = GetWildCardFilesInDir( PACKAGE_LIST_DIR, pkgifiles, "*.pkgi", false );

	if( res <= 0 ) {
		DispColoredData( "Error: No package exists in the package list directory!", CROSS, RED, RED, true );
		return newpkgs;
	}

	std::fstream file;
	file.open( PACKAGE_UPDATE_TIME_FILE, std::ios::out );

	if( !file ) {
		DispColoredData( "Error: Unable to open package update time file for writing!", CROSS, RED, RED, true );
		return newpkgs;
	}

	for( auto pkgifile : pkgifiles ) {
		long long lastmodtime = GetLastModifiedTime( PACKAGE_LIST_DIR + pkgifile.file );
		std::string pkgname = PackageConfig::GetPackageNameFromFile( pkgifile.file );

		newpkgs.push_back( pkgname );
		file << pkgname << "," << lastmodtime << std::endl;
	}

	file.close();

	for( auto alreadyexists : prevupdatetimes ) {
		auto loc = std::find( newpkgs.begin(), newpkgs.end(), alreadyexists.first );

		if( loc == newpkgs.end() )
			continue;

		newpkgs.erase( loc );
	}

	return newpkgs;
}

std::vector< std::string > GetRemovedPackages( const std::map< std::string, long long > & prevupdatetimes )
{
	std::vector< std::string > removed;
	for( auto file : prevupdatetimes ) {
		Package pkg;

		if( !PackageConfig::GetPackage( file.first, pkg ) )
			removed.push_back( file.first );
	}

	return removed;
}

void UpdatePackages( PackageManager & mgr, const std::map< std::string, long long > & prevtimes,
			const std::map< std::string, long long > & newtimes,
			const std::vector< std::string > & newpackages,
			const std::vector< std::string > & removedpkgs )
{
	if( !newpackages.empty() ) {
		DispColoredData( "\n<========================New packages========================>\n",
				MAGENTA, true );
		DispColoredDataLaterally( newpackages, BOLD_GREEN );
	}

	std::vector< std::string > updated;

	if( !newtimes.empty() ) {
		for( auto newtime : newtimes ) {
			auto loc = prevtimes.find( newtime.first );

			// If a package dont exist in prevtimes, means it's new.
			if( loc == prevtimes.end() )
				continue;

			if( newtime.second > loc->second )
				updated.push_back( newtime.first );
		}

		if( !updated.empty() ) {
			DispColoredData( "\n<======================Updated packages======================>\n",
					MAGENTA, true );

			HighlightInstalledPackages( updated );
			DispColoredDataLaterally( updated, BOLD_CYAN );
		}
	}

	if( !removedpkgs.empty() ) {
		DispColoredData( "\n<=========================Removed packages=========================>\n",
				MAGENTA, true );
		DispColoredDataLaterally( removedpkgs, BOLD_CYAN );
	}

	if( !updated.empty() ) {
		DispColoredData( "\nRemoving old packages ...", FIRST_COL, true );

		int res = 0;

		for( auto upd : updated ) {
			if( mgr.IsInstalled( upd ) == 0 ) {
				res = mgr.UninstallPackage( upd );
				if( res != 0 )
					break;
			}
		}
		if( res != 0 ) {
			DispColoredData( "Unable to uninstall packages. Not upgrading!", CROSS, RED, RED, true );
		}

		DispColoredData( "\nInstalling new packages ...", FIRST_COL, true );

		mgr.InstallMultiplePackages( updated );
	}

	if( !removedpkgs.empty() ) {
		DispColoredData( "\nRemoving deprecated packages ...", FIRST_COL, true );
		int res = 0;
		for( auto rem : removedpkgs ) {
			if( mgr.IsInstalled( rem ) ) {
				res = mgr.UninstallPackage( rem );
				if( res != 0 )
					break;
			}
		}
		if( res != 0 ) {
			DispColoredData( "Unable to remove deprecated packages!", CROSS, RED, RED, true );
		}
	}
}

void HighlightInstalledPackages( std::vector< std::string > & allpkgs )
{
	if( !LocExists( INSTALLED_PKGS_FILE ) )
		return;

	std::fstream file;

	file.open( INSTALLED_PKGS_FILE );

	if( !file )
		return;

	std::string line;

	while( std::getline( file, line ) ) {
		TrimString( line );

		auto loc = std::find( allpkgs.begin(), allpkgs.end(), line );

		if( loc == allpkgs.end() )
			continue;
		
		* loc = BOLD_GREEN + * loc + " " + GREEN + TICK;
	}

	file.close();
}
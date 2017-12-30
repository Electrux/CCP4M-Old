#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/ColorDefs.hpp"
#include "../include/UTFChars.hpp"
#include "../include/Paths.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/DisplayExecute.hpp"

#include "../include/PackageManagement/PackageData.hpp"
#include "../include/PackageManagement/PackageConfig.hpp"
#include "../include/PackageManagement/ArchiveExtractor.hpp"
#include "../include/PackageManagement/PackageDownloader.hpp"
#include "../include/PackageManagement/PackageBuilder.hpp"
#include "../include/PackageManagement/PackageInstaller.hpp"
#include "../include/PackageManagement/PackageUninstaller.hpp"

#include "../include/PackageManager.hpp"

PackageManager::PackageManager( std::vector< std::string > & _args ) : args( _args ) {}

int PackageManager::HandleCommand()
{
	if( args.size() < 3 ) {
		DispColoredData( "Error: Use", args[ 0 ] + " pkg [install/search/update/remove]",
				FIRST_COL, SECOND_COL, true );
		return 1;
	}

	if( args[ 2 ] == "install" ) {
		if( args.size() < 4 ) {
			DispColoredData( "Error: Use", args[ 0 ] + " pkg install < Package Name >",
				FIRST_COL, SECOND_COL, true );
			return 1;
		}
		return InstallPackage( args[ 3 ] );
	}

	if( args[ 2 ] == "uninstall" ) {
		if( args.size() < 4 ) {
			DispColoredData( "Error: Use", args[ 0 ] + " pkg uninstall < Package Name >",
				FIRST_COL, SECOND_COL, true );
			return 1;
		}
		return UninstallPackage( args[ 3 ] );
	}

	return 1;
}

int PackageManager::InstallPackage( std::string package )
{
	Package pkg;

	DispColoredData( "Checking package exists ... " );
	if( !PackageExists( package, pkg ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Package does not exist!", FIRST_COL, true );
		DispColoredData( "Perhaps try to update package list using:", FIRST_COL, true );
		DispColoredData( "\n\t" + args[ 0 ] + " pkg update", FIRST_COL, true );
		return 1;
	}
	DispColoredData( TICK, GREEN, true );

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	DispColoredData( "Checking already installed ... " );
	int res = IsInstalled( package );
	if( res == 0 || res == -1 ) {
		DispColoredData( "Package already installed!", TICK, BOLD_YELLOW, BOLD_GREEN, true );
		return 0;
	}

	DispColoredData( "Fetching " + pkgtypelower + " package ... " );
	if( !FetchPackage( pkg ) ) {
		return 1;
	}

	DispColoredData( "Extracting downloaded archive ... " );
	if( !ExtractArchive( pkg ) ) {
		DispColoredData( "Unable to extract archive!", CROSS, FIRST_COL, RED, true );
		return 1;
	}

	if( pkg.type == "Source" ) {
		DispColoredData( "Building package from source ...", TICK, FIRST_COL, GREEN, true );
		if( !BuildDirectory( pkg ) ) {
			DispColoredData( "Build failed!", CROSS, FIRST_COL, RED, true );
			return 1;
		}
	}

	if( pkg.buildcmds.find( "install" ) == std::string::npos ) {
		DispColoredData( "Starting " + pkgtypelower + " package installation ...", TICK, FIRST_COL, GREEN, true );
		if( !InstallDirectory( pkg ) ) {
			DispColoredData( "Installation failed!", CROSS, FIRST_COL, RED, true );
			return 1;
		}
	}

	DispColoredData( "Finishing up ... " );

	RemoveTempFiles( pkg );

	DispColoredData( "Installation successful!", TICK, BOLD_YELLOW, BOLD_GREEN, true );

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::app );

	file << package << std::endl;

	file.close();

	return 0;
}

int PackageManager::UninstallPackage( std::string package )
{
	Package pkg;

	DispColoredData( "Checking package exists ... " );
	if( !PackageExists( package, pkg ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Package does not exist!", FIRST_COL, true );
		DispColoredData( "Perhaps try to update package list using:", FIRST_COL, true );
		DispColoredData( "\n\t" + args[ 0 ] + " pkg update", FIRST_COL, true );
		return 1;
	}
	DispColoredData( TICK, GREEN, true );

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	DispColoredData( "Checking if package is installed ... " );
	bool res = IsInstalled( package );
	if( res == 1 || res == -1 ) {
		DispColoredData( "Package not installed!\nNothing to remove!", CROSS, FIRST_COL, RED, true );
		return 1;
	}

	std::string pkgtolower = package;
	StringToLower( pkgtolower );

	DispColoredData( "Starting package " + pkgtolower + " uninstallation ...", TICK, FIRST_COL, GREEN, true );
	if( !UninstallArchive( pkg, args ) ) {
		DispColoredData( "Uninstallation failed!", CROSS, FIRST_COL, RED, true );
		return 1;
	}

	DispColoredData( "Removing all temporary files ... " );

	RemoveTempFiles( pkg, true );

	DispColoredData( "Removing installation entry ... " );
	return ( int )!RemoveInstalledEntry( pkg );
}

//bool PackageManager::GetInfo( std::string package );

//int Update();

//int GetDependencyInfo( std::string package );

//std::string GetDependencyFlags( std::string package );

bool PackageManager::RemoveTempFiles( const Package & pkg, bool allfiles )
{
	std::string rmcmd;

	std::string toremdir;

	// By default, this is true only for source packages since
	// buildcmds has some value in source packages only.
	if( pkg.buildcmds.find( "uninstall" ) == std::string::npos || allfiles ) {
		rmcmd = "rm -rf " + PACKAGE_TMP + pkg.file + " " + GetPackageVersionDir( pkg );
	}
	else {
		rmcmd = "rm -rf " + PACKAGE_TMP + pkg.file;
	}

	if( DispExecuteNoErr( rmcmd, false ) != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to remove temporary files... Continuing... ", RED, true );
		return false;
	}
	else {
		std::vector< DirFile > temp;
		if( GetWildCardFilesInDir( PACKAGE_TMP + pkg.name, temp, "*" ) <= 0 ) {

			DispColoredData( TICK, GREEN, true );
			DispColoredData( "Removing parent directory ... " );

			if( DispExecuteNoErr( "rm -rf " + GetPackageDir( pkg ), false ) != 0 ) {
				DispColoredData( CROSS, RED, true );
				DispColoredData( "Removing parent directory: " + GetPackageDir( pkg ) + " failed...",
						"Continuing... ", FIRST_COL, FIRST_COL, true );
			}
		}
		DispColoredData( TICK, GREEN, true );
	}

	return true;
}

bool PackageManager::RemoveInstalledEntry( const Package & pkg )
{
	if( !LocExists( INSTALLED_PKGS ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Installed package list does not exist!", RED, true );
		return false;
	}
	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::in );

	if( !file ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to open package list to read!", RED, true );
		return false;
	}

	std::string line;

	std::vector< std::string > output;

	while( std::getline( file, line ) ) {
		TrimString( line );
		if( line.find( pkg.name ) != std::string::npos || line.empty() || line == "\n" ) {
			continue;
		}
		output.push_back( line );
	}

	file.close();

	file.open( INSTALLED_PKGS, std::ios::out );

	if( !file ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to open package list to write!", RED, true );
		return false;
	}

	for( auto op : output ) {
		file << line << std::endl;
	}

	file.close();

	DispColoredData( TICK, GREEN, true );
	DispColoredData( "Uninstallation successful!", TICK, BOLD_YELLOW, BOLD_GREEN, true );
	return true;
}

bool PackageManager::PackageExists( std::string package, Package & pkg )
{
	return PackageConfig::GetPackage( package, pkg );
}

int PackageManager::IsInstalled( std::string package )
{
	Package pkg;

	if( !PackageExists( package, pkg ) ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Package", package, "does not exist!", RED, SECOND_COL, RED, true );
		return 1;
	}

	if( !LocExists( INSTALLED_PKGS ) ) {
		std::fstream file;
		file.open( INSTALLED_PKGS, std::ios::out );
		if( !file ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Error: Unable to create installed packages list!", RED, true );
			return 0;
		}
		file.close();

		return 1;
	}

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::in );

	if( !file ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Unable to open package list to read!", RED, true );
		return 0;
	}

	std::string line;

	bool found = false;

	while( std::getline( file, line ) ) {
		if( !line.empty() && line.find( package ) != std::string::npos ) {
			found = true;
			break;
		}
	}

	file.close();

	if( !found && !pkg.existfile.empty() ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Package", package, "is installed but unmanageable by ", FIRST_COL, SECOND_COL, THIRD_COL, false );
		DispColoredData( args[ 0 ], "because it was not installed by it.", SECOND_COL, THIRD_COL, true );
		return -1;
	}

	DispColoredData( TICK, GREEN, true );

	return !found;
}
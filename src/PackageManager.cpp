#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/CoreData.hpp"
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
#include "../include/PackageManagement/PackageListUpdater.hpp"

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
		if( args.size() < 4 && args[ 3 ] != "--force" ) {
			DispColoredData( "Error: Use", args[ 0 ] + " pkg install < Package Name >",
				FIRST_COL, SECOND_COL, true );
			return 1;
		}
		return InstallMultiplePackages();
	}

	if( args[ 2 ] == "uninstall" ) {
		if( args.size() < 4 ) {
			DispColoredData( "Error: Use", args[ 0 ] + " pkg uninstall < Package Name >",
				FIRST_COL, SECOND_COL, true );
			return 1;
		}
		return UninstallMultiplePackages();
	}

	if( args[ 2 ] == "update" ) {
		return Update();
	}

	if( args[ 2 ] == "info" ) {
		if( args.size() < 4 ) {
			DispColoredData( "Error: Use", args[ 0 ] + " pkg info < Package Name >",
				FIRST_COL, SECOND_COL, true );
			return 1;
		}
		return GetInfo( args[ 3 ] );
	}

	return 1;
}

int PackageManager::InstallMultiplePackages()
{
	int retval = 0;

	bool forceinstall = false;

	std::vector< std::string > packages;

	for( int i = 3; i < ( int )args.size(); ++i ) {
		if( args[ i ].find( "--force" ) != std::string::npos ) {
			forceinstall = true;
			continue;
		}

		packages.push_back( args[ i ] );
	}

	return InstallMultiplePackages( packages, forceinstall );
}

int PackageManager::InstallMultiplePackages( std::vector< std::string > & packages, bool forceinstall )
{
	int retval = 0;

	for( int i = 0; i < ( int )packages.size(); ++i ) {
		retval = InstallPackage( packages[ i ], forceinstall );

		if( i != packages.size() - 1 )
			DispColoredData( "", FIRST_COL, true );

		if( retval != 0 )
			break;
	}

	return retval;
}

int PackageManager::UninstallMultiplePackages()
{
	int retval = 0;

	for( int i = 3; i < args.size(); ++i ) {
		retval = UninstallPackage( args[ i ] );

		if( i != args.size() - 1 )
			DispColoredData( "", FIRST_COL, true );

		if( retval != 0 )
			break;
	}

	return retval;
}

int PackageManager::InstallPackage( std::string package, bool forceinstall )
{
	Package pkg;

	DispColoredData( "Starting package", package, "installation ...\n", BOLD_BLUE, BOLD_MAGENTA, BOLD_BLUE, true );

	DispColoredData( "Checking package exists ... " );
	if( !PackageExists( package, pkg ) ) {
		return 1;
	}
	DispColoredData( TICK, GREEN, true );

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	DispColoredData( "Checking already installed ... " );
	int res = IsInstalled( package );
	if( forceinstall ) {
		DispColoredData( "Forcing installation! This may produce unintended results!", TICK, RED, RED, true );
	}
	else if( res == 0 ) {
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
		DispColoredData( "Building and installing package from source ...", TICK, FIRST_COL, GREEN, true );
		if( !BuildDirectory( pkg ) ) {
			DispColoredData( "Build failed!", CROSS, FIRST_COL, RED, true );
			return 1;
		}
	}
	else if( pkg.type == "Binary" ) {
		DispColoredData( "Starting " + pkgtypelower + " package installation ...", TICK, FIRST_COL, GREEN, true );
		if( !InstallDirectory( pkg ) ) {
			DispColoredData( "Installation failed!", CROSS, FIRST_COL, RED, true );
			return 1;
		}
	}

	DispColoredData( "Finishing up ... " );

	RemoveTempFiles( pkg );

	if( !InstallEntryExists( pkg ) ) {
		std::fstream file;
		file.open( INSTALLED_PKGS, std::ios::app );

		file << package << std::endl;

		file.close();
	}

	DispColoredData( "Installation successful!", TICK, BOLD_YELLOW, BOLD_GREEN, true );
	return 0;
}

int PackageManager::UninstallPackage( std::string package )
{
	Package pkg;

	std::string pkgtolower = package;

	StringToLower( pkgtolower );

	DispColoredData( "Starting package", pkgtolower, "uninstallation ...\n",
			BOLD_BLUE, BOLD_MAGENTA, BOLD_BLUE, true );

	DispColoredData( "Checking package exists ... " );
	if( !PackageExists( package, pkg ) ) {
		return 1;
	}
	DispColoredData( TICK, GREEN, true );

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	DispColoredData( "Checking if package is installed ... " );
	int res = IsInstalled( package );
	if( res == 1 ) {
		DispColoredData( "Package not installed!\nNothing to remove!", CROSS, FIRST_COL, RED, true );
		return 1;
	}

	if( !UninstallArchive( pkg ) ) {
		DispColoredData( "Uninstallation failed!", CROSS, FIRST_COL, RED, true );
		return 1;
	}

	DispColoredData( "Removing all temporary files ... " );

	RemoveTempFiles( pkg, true );

	DispColoredData( "Removing installation entry ... " );
	return ( int )!RemoveInstallEntry( pkg );
}

int PackageManager::Update()
{
	DispColoredData( "Starting update of package lists ... ", TICK, FIRST_COL, SECOND_COL, true );

	int res = UpdatePackageList();

	if( res != 0 )
		DispColoredData( "Updating package list failed! ", CROSS, FIRST_COL, RED, true );
	else
		DispColoredData( "Updating package list successful!", TICK, FIRST_COL, GREEN, true );

	return res;
}

//int GetDependencyInfo( std::string package );

//std::string GetDependencyFlags( std::string package );

bool PackageManager::RemoveTempFiles( const Package & pkg, bool allfiles )
{
	std::string rmcmd;

	// Remove all files if package is binary since their installation information
	// has already been stored in the saved file.
	if( pkg.type == "Binary" || allfiles ) {
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

bool PackageManager::InstallEntryExists( const Package & pkg )
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

	while( std::getline( file, line ) ) {
		TrimString( line );
		if( line == pkg.name ) {
			file.close();
			return true;
		}
	}

	file.close();

	return false;
}

bool PackageManager::RemoveInstallEntry( const Package & pkg )
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
		if( line == pkg.name || line.empty() || line == "\n" ) {
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
		file << op << std::endl;
	}

	file.close();

	DispColoredData( TICK, GREEN, true );
	DispColoredData( "Uninstallation successful!", TICK, BOLD_YELLOW, BOLD_GREEN, true );
	return true;
}

bool PackageManager::PackageExists( std::string package, Package & pkg )
{
	bool exists = PackageConfig::GetPackage( package, pkg );

	if( !exists ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Package does not exist!", FIRST_COL, true );
		DispColoredData( "Perhaps try to update package list using:", FIRST_COL, true );
		DispColoredData( "\n\t" + args[ 0 ], "pkg update", SECOND_COL, EXTRA_COL, true );
	}

	return exists;
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

	DispColoredData( TICK, GREEN, true );

	return !found;
}

int PackageManager::GetInfo( std::string package )
{
	Package pkg;

	if( !PackageConfig::GetPackage( package, pkg ) ) {
		DispColoredData( "Error: No package by the name:", package, "exists!", FIRST_COL, SECOND_COL, THIRD_COL, true );
		return 1;
	}

	DispColoredData( "Package:", pkg.name, FIRST_COL, SECOND_COL, true );
	DispColoredData( "", FIRST_COL, true );

	DispColoredData( "\tDescription:", pkg.description, FIRST_COL, SECOND_COL, true );
	DispColoredData( "\tVersion:", pkg.version, FIRST_COL, SECOND_COL, true );
	DispColoredData( "\tLanguage:", ( pkg.lang == "c" ? "C" : "C++" ), FIRST_COL, SECOND_COL, true );

	DispColoredData( "", FIRST_COL, true );

	DispColoredData( "\tInstallation from:", pkg.type, FIRST_COL, SECOND_COL, true );

	DispColoredData( "", FIRST_COL, true );

	if( !pkg.deplist.empty() ) {
		DispColoredData( "\tDependencies: ", FIRST_COL, pkg.deplist.empty() );
		for( auto dep : pkg.deplist )
			DispColoredData( dep, "\b, ", SECOND_COL, FIRST_COL, false );

		DispColoredData( "\b\b ", FIRST_COL, true );

		DispColoredData( "", FIRST_COL, true );

	}

	DispColoredData( "\tInclude directory:", PACKAGE_INCLUDE_INSTALL_DIR, FIRST_COL, SECOND_COL, true );
	DispColoredData( "\tLibrary directory:", PACKAGE_LIBRARY_INSTALL_DIR, FIRST_COL, SECOND_COL, true );

	if( ARCH == MAC ) {
		DispColoredData( "\tFrameworks directory:", PACKAGE_LIBRARY_INSTALL_DIR, FIRST_COL, SECOND_COL, true );
	}

	DispColoredData( "", FIRST_COL, true );

	DispColoredData( "\tLibrary flags: ", FIRST_COL, pkg.libflags.empty() );

	auto libflags = DelimStringToVector( pkg.libflags );
	for( auto libf : libflags )
		DispColoredData( libf, "\b, ", SECOND_COL, FIRST_COL, false );

	if( !libflags.empty() )
		DispColoredData( "\b\b ", FIRST_COL, true );

	DispColoredData( "", FIRST_COL, true );

	DispColoredData( "\tDownload URL:", pkg.url + pkg.file, FIRST_COL, SECOND_COL, true );

	if( pkg.type == "Source" ) {
		DispColoredData( "", FIRST_COL, true );
		DispColoredData( "Build Mode:", pkg.buildmode, FIRST_COL, SECOND_COL, true );
	}

	return 0;
}
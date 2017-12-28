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
		std::cout << "Error: Use " << args[ 0 ] << " pkg [install/search/update/remove]"
			<< std::endl;
		return 1;
	}

	if( args[ 2 ] == "install" ) {
		if( args.size() < 4 ) {
			std::cout << "Error: Use " << args[ 0 ] << " pkg install < Package Name >"
				<< std::endl;
			return 1;
		}
		return InstallPackage( args[ 3 ] );
	}

	if( args[ 2 ] == "uninstall" ) {
		if( args.size() < 4 ) {
			std::cout << "Error: Use " << args[ 0 ] << " pkg uninstall < Package Name >"
				<< std::endl;
			return 1;
		}
		return UninstallPackage( args[ 3 ] );
	}

	return 1;
}

int PackageManager::InstallPackage( std::string package )
{
	Package pkg;

	std::cout << YELLOW << "Checking package exists ... " << RESET;
	if( !PackageExists( package, pkg ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << "Error: Package does not exist!" << std::endl
			<< "Perhaps try to update package list using:" << std::endl
			<< "\n\t" << args[ 0 ] << " pkg update\n" << std::endl;
		return 1;
	}
	std::cout << GREEN << TICK << RESET << std::endl;

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	std::cout << YELLOW << "Checking already installed ... " << RESET;
	bool res = IsInstalled( package );
	std::cout << GREEN << TICK << RESET << std::endl;
	if( res ) {
		std::cout << BOLD_YELLOW << "Package already installed! "
			<< BOLD_GREEN << TICK << RESET << std::endl;
		return 0;
	}

	std::cout << YELLOW << "Fetching " << pkgtypelower << " package ... " << RESET;
	std::cout.flush();
	if( !FetchPackage( pkg ) ) {
		return 1;
	}

	std::cout << YELLOW << "Extracting downloaded archive ... " << RESET;
	std::cout.flush();
	if( !ExtractArchive( pkg ) ) {
		std::cout << YELLOW << "Unable to extract archive! " << RED << CROSS << std::endl;
		return 1;
	}

	if( pkg.type == "Source" ) {
		std::cout << YELLOW << "Building from source"
			<< " package ... " << GREEN << TICK << RESET << std::endl;
		std::cout.flush();
		if( !BuildDirectory( pkg ) ) {
			std::cout << YELLOW << "Build failed! " << RED << CROSS << RESET << std::endl;
			return 1;
		}
	}

	if( pkg.buildcmds.find( "install" ) == std::string::npos ) {
		std::cout << YELLOW << "Starting " << pkgtypelower
			<< " package installation ... " << GREEN << TICK << RESET << std::endl;
		if( !InstallDirectory( pkg ) ) {
			std::cout << YELLOW << "Installation failed! " << RED << CROSS << RESET << std::endl;
			return 1;
		}
	}

	std::cout << YELLOW << "Finishing up ... " << RESET;
	std::cout.flush();

	RemoveTempFiles( pkg );

	std::cout << BOLD_YELLOW << "Installation Successful! " << BOLD_GREEN << TICK << RESET << std::endl;

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::app );

	file << package << std::endl;

	file.close();

	return 0;
}

int PackageManager::UninstallPackage( std::string package )
{
	Package pkg;

	std::cout << YELLOW << "Checking package exists ... " << RESET;
	if( !PackageExists( package, pkg ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << "Error: Package does not exist!" << std::endl
			<< "Perhaps try to update package list using:" << std::endl
			<< "\n\t" << args[ 0 ] << " pkg update\n" << std::endl;
		return 1;
	}
	std::cout << GREEN << TICK << RESET << std::endl;

	std::string pkgtypelower = pkg.type;
	StringToLower( pkgtypelower );

	std::cout << YELLOW << "Checking if package is installed ... " << RESET;
	bool res = IsInstalled( package );
	std::cout << GREEN << TICK << RESET << std::endl;
	if( !res ) {
		std::cout << YELLOW << "Package not installed!\nNothing to remove! "
			<< RED << CROSS << RESET << std::endl;
		return 1;
	}

	std::string pkgtolower = package;
	StringToLower( pkgtolower );

	std::cout << YELLOW << "Starting package " << pkgtolower
		<< " uninstallation ... " << GREEN << TICK << RESET << std::endl;
	if( !UninstallArchive( pkg, args ) ) {
		std::cout << YELLOW << "Uninstallation failed! " << RED << CROSS << RESET << std::endl;
		return 1;
	}

	std::cout << YELLOW << "Removing all temporary files ... " << RESET;
	std::cout.flush();
	RemoveTempFiles( pkg, true );

	std::cout << YELLOW << "Removing installed entry ... " << RESET;
	return ( int )!RemoveInstalledEntry( pkg );
}

//bool PackageManager::GetInfo( std::string package );

//int Update();

//int GetDependencyInfo( std::string package );

//std::string GetDependencyFlags( std::string package );

bool PackageManager::RemoveTempFiles( const Package & pkg, bool allfiles )
{
	std::string rmcmd;

	// By default, this is true only for source packages since
	// buildcmds has some value in source packages only.
	if( pkg.buildcmds.find( "uninstall" ) == std::string::npos || allfiles ) {
		rmcmd = "rm -rf " + PACKAGE_TMP + pkg.file + " " + GetArchiveDir( pkg );
	}
	else {
		rmcmd = "rm -rf " + PACKAGE_TMP + pkg.file;
	}

	if( DispExecuteNoErr( rmcmd, false ) != 0 ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << RED << "Error: Unable to remove temporary files... Continuing..."
			<< RESET << std::endl;
		return false;
	}
	else {
		std::cout << GREEN << TICK << std::endl;
	}

	return true;
}

bool PackageManager::RemoveInstalledEntry( const Package & pkg )
{
	if( !LocExists( INSTALLED_PKGS ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << RED << "Error: Installed package list does not exist!" << RESET << std::endl;
		return false;
	}
	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::in );

	if( !file ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << RED << "Error: Unable to open package list to read!" << RESET << std::endl;
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
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << RED << "Error: Unable to open package list to write!" << RESET << std::endl;
		return false;
	}

	for( auto op : output ) {
		file << line << std::endl;
	}

	file.close();

	std::cout << GREEN << TICK << RESET << std::endl;

	return true;
}

bool PackageManager::PackageExists( std::string package, Package & pkg )
{
	return PackageConfig::GetPackage( package, pkg );
}

bool PackageManager::IsInstalled( std::string package )
{
	Package pkg;

	if( !PackageExists( package, pkg ) ) {
		std::cout << RED << "Package " << CYAN << package << RED << " does not exist!" << RESET << std::endl;
		return false;
	}

	if( !LocExists( INSTALLED_PKGS ) ) {
		std::fstream file;
		file.open( INSTALLED_PKGS, std::ios::out );
		if( !file ) {
			std::cout << RED << "Error: Unable to create installed packages list!" << RESET << std::endl;
			return true;
		}
		file.close();

		return false;
	}

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::in );

	if( !file ) {
		std::cout << RED << "Error: Unable to open package list to read!" << RESET << std::endl;
		return true;
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

	return found;
}
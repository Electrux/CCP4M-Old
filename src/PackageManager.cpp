#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/ColorDefs.hpp"
#include "../include/UTFChars.hpp"
#include "../include/Paths.hpp"

#include "../include/PackageManagement/PackageData.hpp"
#include "../include/PackageManagement/PackageConfig.hpp"
#include "../include/PackageManagement/PackageDownloader.hpp"
#include "../include/PackageManagement/PackageInstaller.hpp"

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

	return 1;
}

int PackageManager::InstallPackage( std::string package )
{
	Package pkg;

	std::cout << YELLOW << "Checking package exists ... " << RESET;
	if( !PackageExists( package, pkg ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		return 1;
	}
	std::cout << GREEN << TICK << RESET << std::endl;

	std::cout << YELLOW << "Checking already installed ... " << RESET;
	bool res = IsInstalled( package );
	std::cout << GREEN << TICK << RESET << std::endl;
	if( res ) {
		std::cout << BOLD_YELLOW << "Package already installed! "
			<< BOLD_GREEN << TICK << RESET << std::endl;
		return 0;
	}

	std::cout << YELLOW << "Fetching package ... " << RESET;
	std::cout.flush();
	if( !FetchPackage( pkg ) ) {
		std::cout << " " << RED << CROSS << RESET << std::endl;
		return 1;
	}
	std::cout << " " << GREEN << TICK << RESET << std::endl;

	std::cout << YELLOW << "Starting installation ... " << GREEN << TICK << RESET << std::endl;
	if( !InstallArchive( pkg ) ) {
		std::cout << YELLOW << "Installation failed! " << RED << CROSS << RESET << std::endl;
		return 1;
	}
	std::cout << BOLD_YELLOW << "Installation Successful! " << BOLD_GREEN << TICK << RESET << std::endl;

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::app );

	file << package << std::endl;

	file.close();

	return 0;
}

//bool PackageManager::UninstallPackage( std::string package );

//bool PackageManager::GetInfo( std::string package );

//int Update();

//int GetDependencyInfo( std::string package );

//std::string GetDependencyFlags( std::string package );

bool PackageManager::PackageExists( std::string package, Package & pkg )
{
	if( !PackageConfig::GetPackage( package, pkg ) ) {
		std::cerr << "Error: Package does not exist!" << std::endl
			<< "Perhaps try to update package list using:" << std::endl
			<< "\n\t" << args[ 0 ] << " pkg update\n" << std::endl;
		return false;
	}

	return true;
}

bool PackageManager::IsInstalled( std::string package )
{
	Package pkg;

	if( !PackageExists( package, pkg ) )
		return false;

	struct stat info;

	if( stat( INSTALLED_PKGS.c_str(), & info ) != 0 ) {
		std::fstream file;
		file.open( INSTALLED_PKGS, std::ios::out );
		file.close();

		return false;
	}

	std::fstream file;
	file.open( INSTALLED_PKGS, std::ios::in );

	std::string line;

	bool found = false;

	while( std::getline( file, line ) ) {
		if( line.find( package ) != std::string::npos ) {
			found = true;
			break;
		}
	}

	file.close();

	return found;
}
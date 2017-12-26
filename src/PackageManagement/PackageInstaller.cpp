#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/Paths.hpp"
#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallArchive( const Package & pkg )
{
	if( !ExtractArchive( pkg ) )
		return false;

	std::string archivedir = PACKAGE_TMP + pkg.name;

	std::string copyinc, copylib, copyfw;

	GetCopyCommands( pkg, copyinc, copylib, copyfw );

	if( std::system( copyinc.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying includes!"
			<< "Check especially if you have necessary permissions!"
			<< RESET << std::endl;
		return false;
	}

	if( std::system( copylib.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying libraries!"
			<< "Check especially if you have necessary permissions!"
			<< RESET << std::endl;
		return false;
	}

#ifdef __APPLE__
	if( std::system( copyfw.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying includes!"
			<< "Check especially if you have necessary permissions!"
			<< RESET << std::endl;
		return false;
	}
#endif
	return true;
}

void GetCopyCommands( const Package & pkg, std::string & include, std::string & lib, std::string & framework )
{
	std::string archivedir = PACKAGE_TMP + pkg.name;
	std::string incdir = archivedir + "/include/";
	std::string libdir = archivedir + "/lib/";
	std::string fwdir = archivedir + "/framework/";

	include = "";
	lib = "";
	framework = "";

#ifdef __linux__
	if( DirExists( incdir ) )
		include = "cp -r " + archivedir + "/include/*" + " /usr/include/";
	if( DirExists( libdir ) )
		lib = "cp -r " + archivedir + "/lib/*" + " /usr/lib/";
#elif __APPLE__
	if( DirExists( incdir ) )
		include = "cp -r " + archivedir + "/include/*" + " /usr/local/include/";
	if( DirExists( libdir ) )
		lib = "cp -r " + archivedir + "/lib/*" + " /usr/local/lib/";
	if( DirExists( fwdir ) )
		framework = "cp -r " + archivedir + "/framework/*" + " /Library/Frameworks/";
#endif
}

bool DirExists( const std::string & dir )
{
	struct stat info;

	if( stat( dir.c_str(), & info ) == 0 )
		return true;

	return false;
}

bool ExtractArchive( const Package & pkg )
{
	std::cout << YELLOW << "Extracting downloaded archive..." << RESET << std::endl;

	std::string archive = PACKAGE_TMP + pkg.file;

	std::string taroptions = GetTarOptions( pkg.file );
	if( taroptions.empty() ) {
		std::cout << RED << "Error: Unknown archive format! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	if( !CreateArchiveDir( pkg ) )
		return false;

	std::string archivedir = PACKAGE_TMP + pkg.name;

	std::string cmd = "tar " + taroptions + " " + archive + " -C " + archivedir;

	if( std::system( cmd.c_str() ) != 0 ) {
		std::cout << RED << "Error: Unable to extract archive! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	std::cout << YELLOW << "Removing temporary archive..." << RESET << std::endl;

	cmd = "rm -rf " + archive;

	if( std::system( cmd.c_str() ) != 0 ) {
		std::cout << RED << "Error: Unable to remove temporary archive... Continuing..."
			<< RESET << std::endl;
	}

	return true;
}

std::string GetTarOptions( const std::string & filename )
{
	if( filename.find( ".tar.gz" ) != std::string::npos )
		return "-xzvf";

	if( filename.find( ".tar.bz2" ) != std::string::npos )
		return "-xjvf";

	if( filename.find( ".tar" ) != std::string::npos )
		return "-xvf";

	return "";
}

bool CreateArchiveDir( const Package & pkg )
{
	struct stat info;

	if( stat( ( PACKAGE_TMP + pkg.name ).c_str(), & info ) == 0 )
		return true;
	
	int ret = mkdir( ( PACKAGE_TMP + pkg.name ).c_str(), 0755 );

	if( ret != 0 ) {
		std::cout << RED << "Error: Unable to create temporary archive directory! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	return true;
}
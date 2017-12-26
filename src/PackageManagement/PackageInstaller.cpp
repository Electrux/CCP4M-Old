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
		std::cout << RED << "Error in copying frameworks!"
			<< "Check especially if you have necessary permissions!"
			<< RESET << std::endl;
		return false;
	}
#endif

	std::cout << BOLD_GREEN << "Successfully installed the library!" << RESET << std::endl;
	return true;
}

void GetCopyCommands( const Package & pkg, std::string & include, std::string & lib, std::string & framework )
{
	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include/*";
	std::string libdir = archivedir + "/lib/*";
	std::string fwdir = archivedir + "/Frameworks/*";

	include = "";
	lib = "";
	framework = "";

#ifdef __linux__
	if( DirExists( incdir ) )
		include = "cp -r " + incdir + " /usr/include/";
	if( DirExists( libdir ) )
		lib = "cp -r " + libdir + " /usr/lib/";
#elif __APPLE__
	if( DirExists( incdir ) )
		include = "cp -r " + incdir + " /usr/local/include/";
	if( DirExists( libdir ) )
		lib = "cp -r " + libdir + " /usr/local/lib/";
	if( DirExists( fwdir ) )
		framework = "cp -r " + fwdir + " /Library/Frameworks/";
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

	std::string file;
#ifdef __linux__
	file = pkg.filelinux;
#elif __APPLE__
	file = pkg.filemac;
#endif
	std::string archive = PACKAGE_TMP + file;

	std::string taroptions = GetTarOptions( file );
	if( taroptions.empty() ) {
		std::cout << RED << "Error: Unknown archive format! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	if( !CreateArchiveDir( pkg ) )
		return false;

	std::string archivedir = GetArchiveDir( pkg );

	std::string cmd = "tar --strip 1 " + taroptions + " " + archive + " -C " + archivedir;

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
		return "-xzf";

	if( filename.find( ".tar.bz2" ) != std::string::npos )
		return "-xjf";

	if( filename.find( ".tar" ) != std::string::npos )
		return "-xf";

	return "";
}

bool CreateArchiveDir( const Package & pkg )
{
	struct stat info;

	std::string archivedir = GetArchiveDir( pkg );

	if( stat( archivedir.c_str(), & info ) == 0 )
		return true;
	
	int ret = mkdir( archivedir.c_str(), 0755 );

	if( ret != 0 ) {
		std::cout << RED << "Error: Unable to create temporary archive directory! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	return true;
}

std::string GetArchiveDir( const Package & pkg )
{
	size_t loc;

	std::string file;

#ifdef __linux__
	file = pkg.filelinux;
#elif __APPLE__
	file = pkg.filemac;
#endif
	loc = file.find( ".tar" );

	std::string archivedir = PACKAGE_TMP;

	for( size_t i = 0; i < loc; ++i )
		archivedir += file[ i ];

	return archivedir;
}
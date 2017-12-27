#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallArchive( const Package & pkg )
{
	if( !ExtractArchive( pkg ) )
		return false;

	std::string copyinc, copylib, copyfw;

	GetCopyCommands( pkg, copyinc, copylib, copyfw );

	bool useframework = !copyfw.empty();

#ifndef __APPLE__
	useframework = false;
#endif

	if( !CheckNecessaryPermissions( pkg, useframework ) ) {
		std::cout << RED << "Error! Check if you have necessary permissions to modify package directories!"
			<< RESET << std::endl;
		return false;
	}

	if( std::system( copyinc.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying includes!"
			<< " Reverting installation!"
			<< RESET << std::endl;
		
		return false;
	}

	if( std::system( copylib.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying libraries!"
			<< RESET << std::endl;
		return false;
	}

#ifdef __APPLE__
	if( std::system( copyfw.c_str() ) != 0 ) {
		std::cout << RED << "Error in copying frameworks!"
			<< RESET << std::endl;
		return false;
	}
#endif
	return true;
}

void GetCopyCommands( const Package & pkg, std::string & include, std::string & lib, std::string & framework )
{
	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include";
	std::string libdir = archivedir + "/lib";
	std::string fwdir = archivedir + "/Frameworks";

	include = "";
	lib = "";
	framework = "";

	if( DirExists( incdir ) )
		include = "cp -r " + incdir + "/* " + pkg.incdir;
	if( DirExists( libdir ) )
		lib = "cp -r " + libdir + "/* " + pkg.libdir;
#ifdef __APPLE__
	if( DirExists( fwdir ) )
		framework = "cp -r " + fwdir + "/* /Library/Frameworks/";
#endif
}

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists )
{
	int ret = 0;

	if( std::system( ( "touch " + pkg.incdir + "/pkgtest" ).c_str() ) != 0 )
		return false;
	if( std::system( ( "touch " + pkg.libdir + "/pkgtest" ).c_str() ) != 0 )
		return false;
	if( framework_exists )
		ret = std::system( "touch /Library/Frameworks/pkgtest" );
	return !( bool )ret;
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
	std::cout << YELLOW << "Extracting downloaded archive... " << RESET;
	std::cout.flush();

	std::string archive = PACKAGE_TMP + pkg.file;

	std::string taroptions = GetTarOptions( pkg.file );
	if( taroptions.empty() ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error: Unknown archive format! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	if( !CreateArchiveDir( pkg ) ) {
		std::cout << RED << CROSS << std::endl;
		return false;
	}

	std::string archivedir = GetArchiveDir( pkg );

	std::string cmd = "tar --strip 1 " + taroptions + " " + archive + " -C " + archivedir;

	if( std::system( cmd.c_str() ) != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error: Unable to extract archive! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	std::cout << GREEN << TICK << std::endl;

	std::cout << YELLOW << "Removing temporary archive... " << RESET;

	cmd = "rm -rf " + archive;

	if( std::system( cmd.c_str() ) != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error: Unable to remove temporary archive... Continuing..."
			<< RESET << std::endl;
	}

	std::cout << GREEN << TICK << std::endl;

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

	loc = pkg.file.find( ".tar" );

	std::string archivedir = PACKAGE_TMP;

	for( size_t i = 0; i < loc; ++i )
		archivedir += pkg.file[ i ];

	return archivedir;
}
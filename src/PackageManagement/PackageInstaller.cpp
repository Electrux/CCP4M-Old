#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallDirectory( const Package & pkg )
{
	std::string copyinc, copyfw;
	std::vector< std::string > copylibs;
	std::vector< std::string > copiedfiles;

	GetCopyCommands( pkg, copyinc, copylibs, copyfw );

	bool useframework = !copyfw.empty();

#ifndef __APPLE__
	useframework = false;
#endif

	if( !CheckNecessaryPermissions( pkg, useframework ) ) {
		std::cout << RED << "Error! Check if you have necessary permissions to modify package directories!"
			<< RESET << std::endl;
		return false;
	}

	std::string dispexectmp;

	if( DispExecuteWithCopyFileLocations( copyinc, dispexectmp, copiedfiles ) != 0 ) {
		FetchExtraDirs( pkg, copiedfiles );
		std::cout << RED << "Error in copying includes!\nReverting installation ... " << RESET;
		std::cout.flush();
		RevertInstallation( copiedfiles );
		return false;
	}

	int failctr = 0;

	for( auto lib : copylibs ) {
		if( DispExecuteWithCopyFileLocations( lib, dispexectmp, copiedfiles ) != 0 ) {
			failctr++;
		}
		FetchExtraDirs( pkg, copiedfiles );
	}
	if( failctr >= copylibs.size() ) {
		std::cout << RED << "Error in copying libraries!\nReverting installation ... " << RESET;
		std::cout.flush();
		RevertInstallation( copiedfiles );
		return false;
	}

#ifdef __APPLE__
	if( DispExecuteWithCopyFileLocations( copyfw, dispexectmp, copiedfiles ) != 0 ) {
		FetchExtraDirs( pkg, copiedfiles );
		std::cout << RED << "Error in copying frameworks!\nReverting installation ... " << RESET;
		std::cout.flush();
		RevertInstallation( copiedfiles );
		return false;
	}
#endif
	std::fstream installedfiles;
	installedfiles.open( PACKAGE_DIR + "." + pkg.name, std::ios::out );
	if( !installedfiles ) {
		std::cout << RED << "Error in saving installation information!\nReverting installation ... " << RESET;
		std::cout.flush();
		RevertInstallation( copiedfiles );
		return false;
	}

	for( auto file : copiedfiles ) {
		installedfiles << file << std::endl;
	}

	installedfiles.close();

	return true;
}

void GetCopyCommands( const Package & pkg, std::string & include, std::vector< std::string > & libs, std::string & framework )
{
	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include";
	std::string libdir = archivedir + "/lib";
	std::string fwdir = archivedir + "/Frameworks";

	std::string lib;

	libs.clear();
	include = "";
	framework = "";

	if( LocExists( incdir ) ) {
		include = "cp -rv " + incdir + "/* " + pkg.incdir;
	}
	if( LocExists( libdir ) ) {
		lib = "cp -rv " + libdir + "/*.so* " + pkg.libdir;
		libs.push_back( lib );
		lib = "cp -rv " + libdir + "/*.l* " + pkg.libdir;
		libs.push_back( lib );
		lib = "cp -rv " + libdir + "/*.dyl* " + pkg.libdir;
		libs.push_back( lib );
	}
#ifdef __APPLE__
	if( LocExists( fwdir ) ) {
		framework = "cp -r " + fwdir + "/* /Library/Frameworks/";
	}
#endif
}

void RevertInstallation( std::vector< std::string > & copiedfiles )
{
	if( !RemoveAllCopiedFiles( copiedfiles ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << YELLOW << "Reverting failed! Exiting!" << RESET << std::endl;
	}
	else {
		std::cout << GREEN << TICK << RESET << std::endl;
		std::cout << YELLOW << "Reverting successful! Exiting!" << RESET << std::endl;
	}
}
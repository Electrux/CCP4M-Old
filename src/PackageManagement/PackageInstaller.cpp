#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
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

	if( DispExecute( copyinc, dispexectmp, false ) != 0 ) {
		std::cout << RED << "Error in copying includes!"
			<< " Reverting installation!"
			<< RESET << std::endl;
		
		return false;
	}

	int failctr = 0;

	for( auto lib : copylibs ) {
		if( DispExecute( lib, dispexectmp, false ) != 0 ) {
			failctr++;
		}
	}
	if( failctr >= copylibs.size() ) {
		std::cout << RED << "Error in copying libraries!"
			<< RESET << std::endl;
		return false;
	}

#ifdef __APPLE__
	if( DispExecute( copyfw, dispexectmp, false ) != 0 ) {
		std::cout << RED << "Error in copying frameworks!"
			<< RESET << std::endl;
		return false;
	}
#endif
	return true;
}

void GetCopyCommands( const Package & pkg, std::string & include, std::vector< std::string > & libs, std::string & framework )
{
	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include";
	std::string libdir = archivedir + "/lib";
	std::string fwdir = archivedir + "/Frameworks";

	include = "";
	std::string lib;
	framework = "";

	if( LocExists( incdir ) ) {
		include = "cp -r " + incdir + "/* " + pkg.incdir;
	}
	if( LocExists( libdir ) ) {
		lib = "cp -r " + libdir + "/*.so* " + pkg.libdir;
		libs.push_back( lib );
		lib = "cp -r " + libdir + "/*.l* " + pkg.libdir;
		libs.push_back( lib );
		lib = "cp -r " + libdir + "/*.dyl* " + pkg.libdir;
		libs.push_back( lib );
	}
#ifdef __APPLE__
	if( LocExists( fwdir ) ) {
		framework = "cp -r " + fwdir + "/* /Library/Frameworks/";
	}
#endif
}
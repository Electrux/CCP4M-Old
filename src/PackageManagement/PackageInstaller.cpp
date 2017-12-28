#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallDirectory( const Package & pkg )
{
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

	if( LocExists( incdir ) )
		include = "cp -r " + incdir + "/* " + pkg.incdir;
	if( LocExists( libdir ) )
		lib = "cp -r " + libdir + "/*.so* " + libdir + "/.l* " + libdir + "/.dyl* " + pkg.libdir;
#ifdef __APPLE__
	if( LocExists( fwdir ) )
		framework = "cp -r " + fwdir + "/* /Library/Frameworks/";
#endif
}
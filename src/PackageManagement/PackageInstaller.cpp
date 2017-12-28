#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <fstream>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageData.hpp"

#include "../../include/PackageManagement/PackageInstaller.hpp"

bool InstallDirectory( const Package & pkg )
{
	std::cout << YELLOW << "Copying files ... " << RESET;
	std::cout.flush();

	bool use_framework = false;

	std::vector< std::string > copiedfiles;

	auto copyfiles = GetCopyList( pkg, use_framework );

	FetchExtraDirs( pkg, copyfiles, copiedfiles );

	for( auto dir : copiedfiles ) {
		if( !CreateDir( dir, false ) ) {
			std::cout << RED << CROSS << std::endl;
			std::cout << RED << "Error: Unable to create required directory for installation!"
				<< RESET << std::endl;
			return false;
		}
	}

#ifndef __APPLE__
	use_framework = false;
#endif
	if( !CheckNecessaryPermissions( pkg, use_framework ) ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error! Check if you have necessary permissions to modify package directories!"
			<< RESET << std::endl;
		return false;
	}

	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include/";
	std::string libdir = archivedir + "/lib/";
	std::string fwdir = archivedir + "/Frameworks/";

	int prevsize = 0;

	std::string cpinput;
	std::string cpoutput;

	// INCLUDE FILES
	if( !copyfiles[ "inc" ].empty() ) {
		std::cout << YELLOW << "Copying include files ... " << RESET;
		std::cout.flush();
	}

	for( auto file : copyfiles[ "inc" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = incdir + file.dir + file.file;
		cpoutput = pkg.incdir + "/" + file.dir;

		copiedfiles.push_back( cpoutput );

		MoveOutputCursorBack( prevsize );

		std::cout << " cp -r " << cpinput << " " << cpoutput << std::endl;

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );
			std::cout << RED << CROSS << std::endl;
			std::cout << RED << "Error in copying includes!\nReverting installation ... " << RESET;
			std::cout.flush();
			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	std::cout << GREEN << TICK << std::endl;

	// LIBRARY FILES
	if( !copyfiles[ "lib" ].empty() ) {
		std::cout << YELLOW << "Copying library files ... " << RESET;
		std::cout.flush();
	}

	for( auto file : copyfiles[ "lib" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = libdir + file.dir + file.file;
		cpoutput = pkg.libdir + "/" + file.dir;

		copiedfiles.push_back( cpoutput );

		MoveOutputCursorBack( prevsize );

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );
			std::cout << RED << CROSS << std::endl;
			std::cout << RED << "Error in copying libraries!\nReverting installation ... " << RESET;
			std::cout.flush();
			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	std::cout << GREEN << TICK << std::endl;

	if( !copyfiles[ "fw" ].empty() ) {
		std::cout << YELLOW << "Copying framework files ... " << RESET;
		std::cout.flush();
	}

	for( auto file : copyfiles[ "fw" ] ) {
		std::string op = "Copying file: " + file.dir + file.file;

		cpinput = fwdir + file.dir + file.file;
		cpoutput = "/Library/Frameworks/" + file.dir;

		copiedfiles.push_back( cpoutput );

		MoveOutputCursorBack( prevsize );

		prevsize = DisplayOneLinerString( op );
		if( DispExecuteNoErr( "cp -r " + cpinput + " " + cpoutput, false ) != 0 ) {
			MoveOutputCursorBack( prevsize );
			std::cout << RED << CROSS << std::endl;
			std::cout << RED << "Error in copying frameworks!\nReverting installation ... " << RESET;
			std::cout.flush();
			RevertInstallation( pkg, copiedfiles );
			return false;
		}
	}
	std::cout << GREEN << TICK << std::endl;

	if( !SaveCopiedData( pkg, copiedfiles ) ) {
		std::cout << RED << CROSS << std::endl;
		RevertInstallation( pkg, copiedfiles );
	}

	std::cout << GREEN << TICK << std::endl;

	return true;
}

std::map< std::string, std::vector< DirFile > > GetCopyList( const Package & pkg, bool & use_framework )
{
	std::string archivedir = GetArchiveDir( pkg );
	std::string incdir = archivedir + "/include";
	std::string libdir = archivedir + "/lib";
	std::string fwdir = archivedir + "/Frameworks";

	std::map< std::string, std::vector< DirFile > > list = {
		{ "inc", {} },
		{ "lib", {} },
		{ "fw", {} }
	};

	if( LocExists( incdir ) ) {
		GetWildCardFilesInDir( incdir, list[ "inc" ], "*" );

	}
	if( LocExists( libdir ) ) {
		GetWildCardFilesInDir( libdir, list[ "lib" ], "*.so*, *.l*, *.dyl*" );
	}
#ifdef __APPLE__
	if( LocExists( fwdir ) && GetWildCardFilesInDir( fwdir, list[ "fw" ], "*" ) > 0 ) {
		std::cout << RED << "Using frameworks... " << RESET << std::endl;
		use_framework = true;
	}
#endif
	for( auto lst : list ) {
		for( auto item : lst.second ) {
			std::cout << BLUE << lst.first << " : " << CYAN << item.dir + item.file << RESET << std::endl;
		}
	}

	return list;
}

void RevertInstallation( const Package & pkg, std::vector< std::string > & data )
{
	if( !RemoveCopiedData( pkg, data ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << YELLOW << "Reverting failed! Exiting!" << RESET << std::endl;
	}
	else {
		std::cout << GREEN << TICK << RESET << std::endl;
		std::cout << YELLOW << "Reverting successful! Exiting!" << RESET << std::endl;
	}
}
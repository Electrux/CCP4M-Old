#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageBuilder.hpp"

bool BuildDirectory( const Package & pkg )
{
	std::vector< std::string > buildcmds =
		DelimStringToVector( pkg.buildcmds );

	if( buildcmds.empty() ) {
		std::cout << YELLOW << "Nothing to build! Exiting!" << RED << CROSS << std::endl;
		return false;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		std::cout << YELLOW << "Unable to get working directory! " << RED << CROSS << std::endl;
		return false;
	}

	if( !ChangeWorkingDir( GetArchiveDir( pkg ) ) ) {
		std::cout << YELLOW << "Unable to change working directory to "
			<< "extracted package directory! " << RED << CROSS << std::endl;
		return false;
	}

	std::string create, make, install;

	create = buildcmds[ 0 ];
	make = buildcmds[ 1 ];

	if( buildcmds.size() >= 3 ) {
		install = buildcmds[ 2 ];
	}

	if( create.find( "configure" ) != std::string::npos ) {
		std::cout << YELLOW << "Creating makefile using configure ... " << RESET;
	}
	else if( create.find( "cmake" ) != std::string::npos ) {
		std::cout << YELLOW << "Creating makefile using CMake ... " << RESET;
	}
	std::cout.flush();

	int res;
	std::string errors;

	res = DispExecute( create, errors );

	if( res != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << YELLOW << "Unable to create makefile! " << RED << CROSS << RESET << std::endl;
		if( !errors.empty() ) {
			std::cout << RED << "Errors:" << RESET << std::endl << errors << std::endl;
		}
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		std::cout << GREEN << TICK << std::endl;
	}

	std::cout << YELLOW << "Using make ... " << RESET;
	std::cout.flush();

	res = DispExecute( make, errors );

	if( res != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << YELLOW << "Unable to make the package! " << RED << CROSS << RESET << std::endl;
		if( !errors.empty() ) {
			std::cout << RED << "Errors:" << RESET << std::endl << errors << std::endl;
		}
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		std::cout << GREEN << TICK << std::endl;
	}

	if( !install.empty() ) {
		std::cout << YELLOW << "Installing using make install ... " << RESET;
		std::cout.flush();

		res = DispExecute( install, errors );

		if( res != 0 ) {
			std::cout << RED << CROSS << std::endl;
			std::cout << YELLOW << "Unable to install using make install! " << RED << CROSS << RESET << std::endl;
			if( !errors.empty() ) {
				std::cout << RED << "Errors:" << RESET << std::endl << errors << std::endl;
			}
			ChangeWorkingDir( cwd );
			return false;
		}
		else {
			std::cout << GREEN << TICK << std::endl;
		}
	}

	ChangeWorkingDir( cwd );

	return true;
}
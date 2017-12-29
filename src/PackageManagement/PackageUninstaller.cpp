#include <iostream>
#include <string>
#include <vector>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageData.hpp"

bool UninstallArchive( const Package & pkg, const std::vector< std::string > & args )
{
	std::vector< std::string > buildcmds = DelimStringToVector( pkg.buildcmds );

	bool usecustomuninstaller = false;

	if( pkg.type == "Binary" || buildcmds.size() < 4 ) {
		std::cout << YELLOW << "Using pre-generated " << args[ 0 ]
			<< " uninstaller file ... " << GREEN << TICK << RESET << std::endl;
		usecustomuninstaller = true;
	}
	else {
		std::cout << YELLOW << "Using uninstall command from library config ... "
			<< GREEN << TICK << RESET << std::endl;
	}

	std::cout << YELLOW << "Removing data ... " << RESET;
	std::cout.flush();

	if( usecustomuninstaller ) {
		std::vector< std::string > cpdata = GetCopiedData( pkg );

		if( !RemoveCopiedData( pkg, cpdata ) ) {
			std::cout << RED << CROSS << RESET << std::endl;
			std::cout << YELLOW << "Unable to remove all copied data! " << RED << CROSS << RESET << std::endl;
			return false;
		}

		std::cout << GREEN << TICK << std::endl;
		return true;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << YELLOW << "Unable to get working directory! " << RED << CROSS << RESET << std::endl;
		return false;
	}

	if( !ChangeWorkingDir( GetPackageVersionDir( pkg ) ) ) {
		std::cout << RED << CROSS << RESET << std::endl;
		std::cout << YELLOW << "Unable to change working directory to "
			<< "extracted package directory! " << RED << CROSS << RESET << std::endl;
		return false;
	}

	std::string makeuninstall = buildcmds[ 3 ];

	std::cout << YELLOW << "\nUninstalling using make uninstall ... " << RESET;
	std::cout.flush();

	int res;
	std::string errors;

	res = DispExecute( makeuninstall, errors );

	if( res != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << YELLOW << "Unable to uninstall using make uninstall! " << RED << CROSS << RESET << std::endl;
		if( !errors.empty() ) {
			std::cout << RED << "Errors:" << RESET << std::endl << errors << std::endl;
		}
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		std::cout << GREEN << TICK << std::endl;
	}

	ChangeWorkingDir( cwd );

	return true;
}
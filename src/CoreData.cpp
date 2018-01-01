#include <string>
#include <vector>
#include <climits>

#include "../include/CoreData.hpp"
#include "../include/Paths.hpp"

const std::string GIT_CLONE_CMD = GIT_CMD + " clone https://github.com/Electrux/CCP4M-PKGI.git " + PACKAGE_LIST_DIR;
const std::string GIT_PULL_CMD = GIT_CMD + " pull " + PACKAGE_LIST_DIR;

Architecture ARCH =
#ifdef __linux__
	LINUX
#elif __APPLE__
	MAC
#else
	OTHER
#endif
;

void SetVarForArchitecture( std::string & var, std::vector< std::string > options )
{
	if( options.size() <= ARCH ) {
		var = "";
		return;
	}

	var = options[ ARCH ];
}

void SetVarForArchitecture( int & var, std::vector< int > options )
{
	if( options.size() <= ARCH ) {
		var = INT_MIN;
		return;
	}

	var = options[ ARCH ];
}
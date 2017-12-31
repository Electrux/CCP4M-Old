#include <string>
#include <vector>
#include <climits>

#include "../include/CoreData.hpp"

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
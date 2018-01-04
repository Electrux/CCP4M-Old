#include <string>
#include <vector>
#include <climits>

#include "../include/CoreData.hpp"
#include "../include/Paths.hpp"

Architecture ARCH =
#ifdef __linux__
	LINUX
#elif __APPLE__
	MAC
#elif __FreeBSD__
	BSD
#else
	OTHER
#endif
;

void SetVarForArchitecture( std::string & var, std::vector< std::string > options )
{
	var = options.size() <= ARCH ? "" : options[ ARCH ];
}

void SetVarForArchitecture( int & var, std::vector< int > options )
{
	var = options.size() <= ARCH ? INT_MIN : options[ ARCH ];
}
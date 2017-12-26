#include <string>
#include <cstdlib>

#include "../include/Paths.hpp"

std::string PACKAGE_DIR = GetEnvVar( "HOME" ) + "/.ccp4mpkgs/";
std::string INSTALLED_PKGS = PACKAGE_DIR + "installed_pkgs.dat";
std::string PACKAGE_TMP = PACKAGE_DIR + "tmp/";

std::string GetEnvVar( std::string key )
{
	char * val;
	val = std::getenv( key.c_str() );
	std::string retval = "";
	if (val != NULL) {
		retval = val;
	}
	return retval;
}
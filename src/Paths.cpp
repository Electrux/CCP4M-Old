#include <string>
#include <cstdlib>
#include <unistd.h>

#include "../include/Paths.hpp"

std::string PACKAGE_DIR = GetHomeDir() + "/.ccp4mpkgs/";
std::string INSTALLED_PKGS = PACKAGE_DIR + "installed_pkgs.dat";
std::string PACKAGE_TMP = PACKAGE_DIR + "tmp/";

std::string GetHomeDir()
{
	std::string homedir;

	if( getuid() == 0) {
#ifdef __linux__
		homedir += "/home/";
#elif __APPLE__
		homedir += "/Users/";
#endif
		homedir += GetEnvVar( "SUDO_USER" );
	}
	else {
		homedir = GetEnvVar( "HOME" );
	}

	return homedir;
}

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

bool ChangeWorkingDir( std::string dir )
{
	return chdir( dir.c_str() ) == 0;
}

std::string GetWorkingDir()
{
	char cwd[ 1024 ];

	if( getcwd( cwd, sizeof( cwd ) ) != NULL ) {
		return cwd;
	}

	return "";
}

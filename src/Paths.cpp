#include <string>
#include <cstdlib>
#include <unistd.h>

#include "../include/CoreData.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/Paths.hpp"

std::string PACKAGE_BASE_DIR = GetHomeDir() + "/.ccp4mpkgs/";
std::string PACKAGE_LIST_DIR = PACKAGE_BASE_DIR + "pkgs/";
std::string INSTALLED_PKGS = PACKAGE_BASE_DIR + "installed_pkgs.dat";
std::string PACKAGE_TMP = PACKAGE_BASE_DIR + "tmp/";

std::string GetHomeDir()
{
	std::string homedir;

	if( getuid() == 0) {
		SetVarForArchitecture( homedir, { "/home/", "/Users/", "" } );
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

std::vector< std::string > GetEnvPath()
{
	std::string path = GetEnvVar( "PATH" );

	auto path_vec = DelimStringToVector( path, ':' );

	for( auto p = path_vec.begin(); p != path_vec.end(); ++p ) {
		* p = ( * p ) + "/";
	}

	return path_vec;
}
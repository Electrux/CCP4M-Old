#include <string>
#include <cstdlib>
#include <unistd.h>

#include "../include/CoreData.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/Paths.hpp"

const std::string PACKAGE_BASE_DIR = GetHomeDir() + "/.ccp4mpkgs/";
const std::string PACKAGE_LIST_DIR = PACKAGE_BASE_DIR + "pkgs/";
const std::string INSTALLED_PKGS_FILE = PACKAGE_BASE_DIR + "installed_pkgs.dat";
const std::string PACKAGE_TMP_DIR = PACKAGE_BASE_DIR + "tmp/";

const std::string PACKAGE_UPDATE_TIME_FILE = PACKAGE_BASE_DIR + "pkg_list.dat";

const std::string PACKAGE_INSTALL_DIR = PACKAGE_BASE_DIR + "installed/";
const std::string PACKAGE_INCLUDE_INSTALL_DIR = PACKAGE_INSTALL_DIR + "include/";
const std::string PACKAGE_LIBRARY_INSTALL_DIR = PACKAGE_INSTALL_DIR + "lib/";
const std::string PACKAGE_FRAMEWORKS_INSTALL_DIR = PACKAGE_INSTALL_DIR + "frameworks/";

const std::string GIT_CLONE_CMD = GIT_CMD + " clone https://github.com/Electrux/CCP4M-PKGI.git " + PACKAGE_LIST_DIR;
const std::string GIT_PULL_CMD = GIT_CMD + " pull";

std::string GetHomeDir()
{
	std::string homedir;

	if( getuid() == 0) {
		SetVarForArchitecture( homedir, { "/home/", "/Users/", "/home/", "" } );
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

	return val != NULL ? val : "";
}

bool ChangeWorkingDir( std::string dir )
{
	return chdir( dir.c_str() ) == 0;
}

std::string GetWorkingDir()
{
	char cwd[ 1024 ];

	if( getcwd( cwd, sizeof( cwd ) ) != NULL )
		return cwd;

	return "";
}

std::vector< std::string > GetEnvPath()
{
	std::string path = GetEnvVar( "PATH" );

	auto path_vec = DelimStringToVector( path, ':' );

	for( auto p = path_vec.begin(); p != path_vec.end(); ++p )
		* p = ( * p ) + "/";

	return path_vec;
}
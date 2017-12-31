#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>
#include <vector>

extern std::string PACKAGE_BASE_DIR;
extern std::string PACKAGE_LIST_DIR;
extern std::string INSTALLED_PKGS;
extern std::string PACKAGE_TMP;

const std::string PACKAGE_EXT = ".pkgi";
const std::string TMP_FILE = ".ccp4mtmp";

std::string GetHomeDir();

std::string GetEnvVar( std::string key );

bool ChangeWorkingDir( std::string dir );

std::string GetWorkingDir();

std::vector< std::string > GetEnvPath();

#endif // PATHS_HPP
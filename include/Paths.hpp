#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>

extern std::string PACKAGE_DIR;
const std::string PACKAGE_EXT = ".pkgi";

extern std::string INSTALLED_PKGS;
extern std::string PACKAGE_TMP;

std::string GetEnvVar( std::string key );

#endif // PATHS_HPP
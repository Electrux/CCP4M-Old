#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>
#include <vector>

// Directories used by package management.
// Base -> Base directory of package management. All others reside in it.
// List -> Directory in which the package configurations are stored ( fetched and updated from repository ).
// Installed_Pkgs -> File which contains a list of packages currently installed.
// Tmp -> Directory in which the downloaded and extracted package files are stored.
extern std::string PACKAGE_BASE_DIR;
extern std::string PACKAGE_LIST_DIR;
extern std::string INSTALLED_PKGS;
extern std::string PACKAGE_TMP;

// Extension of package configuration files.
const std::string PACKAGE_EXT = ".pkgi";
// File which is created for DispExecute() to fetch the
// stderr of a command which was executed.
const std::string TMP_FILE = ".ccp4mtmp";

// Git and its related commands for updating package lists.
const std::string GIT_CMD = "git";
extern const std::string GIT_CLONE_CMD;
extern const std::string GIT_PULL_CMD;

// Fetch home directory of user.
// Will fetch the home directory of
// SUDO_USER if program was executed by sudo.
std::string GetHomeDir();

// Fetch environment variable for key.
std::string GetEnvVar( std::string key );

// Change working directory to dir
// Returns true on success, false if not.
bool ChangeWorkingDir( std::string dir );

// Fetch current working directory.
std::string GetWorkingDir();

// Fetch PATH environmental variable as a vector of strings.
std::vector< std::string > GetEnvPath();

#endif // PATHS_HPP
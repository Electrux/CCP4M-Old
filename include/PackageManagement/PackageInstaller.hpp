#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>
#include <map>

#include "PackageData.hpp"

// This file contains functions which helps in installing packages ( libraries ).

// This function actually copies ( installs ) the library data
// from the extracted directory ( binary library ) or built directory ( source library )
// to their respective directories for includes/libraries/frameworks.
bool InstallDirectory( const Package & pkg );

// If the library installed is one which does not contain information of how to install/uninstall itself,
// GetCopyList will fetch the files to be copied as a map of "inc", "lib", and "fw" as the keys,
// and vectors of files which are copied.
std::map< std::string, std::vector< DirFile > > GetCopyList( const Package & pkg, bool & use_framework );

// This function is called when Installation fails and something has been altered ( copied ).
// It will remove all the copied files therefore, system is left as it was before.
void RevertInstallation( const Package & pkg, std::vector< std::string > & data );

#endif // PACKAGEINSTALLER_HPP
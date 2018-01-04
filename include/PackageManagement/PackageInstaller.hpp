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

// Copies the files in copyfiles of type key ( include, library, or frameworks )
// each of which are displayed on output as dispval, and inserts the copied files
// in copiedfiles vector.
// Returns true if copy goes correct,
// false if not, and reverts the copy as well.
bool CopyFiles( const Package & pkg,
		std::map< std::string, std::vector< DirFile > > & copyfiles,
		std::vector< std::string > & copiedfiles,
		const std::string key, const std::string dispval );

// If the library installed is one which does not contain information of how to install/uninstall itself,
// GetCopyList will fetch the files to be copied as a map of "inc", "lib", and "fw" as the keys,
// and vectors of files which are copied.
std::map< std::string, std::vector< DirFile > > GetCopyList( const Package & pkg, bool & use_framework );

// Fetches extra directories which are created when copying installation
// files. These directories are then used to remove the directories at the
// the time of uninstallation.
void FetchExtraDirs( const Package & pkg,
		const std::map< std::string, std::vector< DirFile > > & copyfiles,
		std::vector< std::string > & fileanddir );

// This function is called when Installation fails and something has been altered ( copied ).
// It will remove all the copied files therefore, system is left as it was before.
void RevertInstallation( const Package & pkg, std::vector< std::string > & data );

#endif // PACKAGEINSTALLER_HPP
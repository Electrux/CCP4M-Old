#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>

#include "PackageData.hpp"

bool InstallDirectory( const Package & pkg );

void GetCopyCommands( const Package & pkg, std::string & include, std::vector< std::string > & libs, std::string & framework );

void RevertInstallation( const Package & pkg, std::vector< std::string > & data );

#endif // PACKAGEINSTALLER_HPP
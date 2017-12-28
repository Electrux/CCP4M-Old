#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>

#include "PackageData.hpp"

bool InstallDirectory( const Package & pkg );

void GetCopyCommands( const Package & pkg, std::string & include, std::vector< std::string > & libs, std::string & framework );

bool RemoveAllCopiedFiles( std::vector< std::string > & files );

#endif // PACKAGEINSTALLER_HPP
#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>

#include "PackageData.hpp"

bool InstallDirectory( const Package & pkg );

void GetCopyCommands( const Package & pkg, std::string & include, std::string & lib, std::string & framework );

#endif // PACKAGEINSTALLER_HPP
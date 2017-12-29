#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>
#include <map>

#include "PackageData.hpp"

bool InstallDirectory( const Package & pkg );

std::map< std::string, std::vector< DirFile > > GetCopyList( const Package & pkg, bool & use_framework );

void RevertInstallation( const Package & pkg, std::vector< std::string > & data );

#endif // PACKAGEINSTALLER_HPP
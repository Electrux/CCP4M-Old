#ifndef PACKAGEUNINSTALLER_HPP
#define PACKAGEUNINSTALLER_HPP

#include "PackageData.hpp"

bool UninstallArchive( const Package & pkg, const std::vector< std::string > & args );

#endif // PACKAGEUNINSTALLER_HPP
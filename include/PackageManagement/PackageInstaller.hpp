#ifndef PACKAGEINSTALLER_HPP
#define PACKAGEINSTALLER_HPP

#include <string>

#include "PackageData.hpp"

bool InstallArchive( const Package & pkg );

void GetCopyCommands( const Package & pkg, std::string & include, std::string & lib, std::string & framework );

bool DirExists( const std::string & dir );

bool ExtractArchive( const Package & pkg );

std::string GetTarOptions( const std::string & filename );

bool CreateArchiveDir( const Package & pkg );

#endif // PACKAGEINSTALLER_HPP
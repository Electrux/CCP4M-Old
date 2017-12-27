#ifndef ARCHIVEEXTRACTOR_HPP
#define ARCHIVEEXTRACTOR_HPP

#include <string>

#include "PackageData.hpp"

bool ExtractArchive( const Package & pkg );

std::string GetTarOptions( const std::string & filename );

#endif // ARCHIVEEXTRACTOR_HPP
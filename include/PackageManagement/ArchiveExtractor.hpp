#ifndef ARCHIVEEXTRACTOR_HPP
#define ARCHIVEEXTRACTOR_HPP

#include <string>

#include "PackageData.hpp"

// Contains functions for Archive extraction purposes.
// Note: The archive must be in tar or tar.* format.

// Extracts downloaded archive, using GetTarOptions.
// Returns true on success, false on failure.
bool ExtractArchive( const Package & pkg );

// Gets the options to be used to extract the archive as string.
// For example, xzf, xf, etc., depending on archive format.
std::string GetTarOptions( const Package & pkg );

#endif // ARCHIVEEXTRACTOR_HPP
#ifndef PACKAGEBUILDER_HPP
#define PACKAGEBUILDER_HPP

#include "PackageData.hpp"

// This file contains function to build the extracted library/package.
// Returns true on success, false on failure.
bool BuildDirectory( const Package & pkg );

#endif // PACKAGEBUILDER_HPP
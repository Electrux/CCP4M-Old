#ifndef PACKAGECONFIG_HPP
#define PACKAGECONFIG_HPP

#include <vector>

#include "../INI_System/INI_Parser.hpp"

#include "PackageData.hpp"

// This is a static class which contains functions for fetching a Package's
// configuration settings from its file.

// All boolean returning functions return true on success, false on failuer.
// The string returning function returns the result string on success,
// empty string on failure.

class PackageConfig
{
public:
	// The main function which takes in the name of package,
	// and a reference to Package object - pkg.
	// If the function succeeds, return value is true, and pkg contains all
	// the information of that package.
	static bool GetPackage( const std::string & packagename, Package & pkg );

	// Fetches the name of package from the file of it.
	// Used primarily in PackageListUpdater.
	static std::string GetPackageNameFromFile( const std::string & file );

	// Creates relevant directories for the package manager, in $HOME.
	static bool HandlePkgDirs();
};

#endif // PACKAGECONFIG_HPP
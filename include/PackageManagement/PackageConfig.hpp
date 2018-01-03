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

	// Fetches the location of existsfile if it exists on the system.
	// Otherwise, it returns an empty string.
	static std::string FetchExistFile( Electrux::INI_Parser & parser );

	// Fetches the default include and library directories based
	// on the system architecture ( see CoreData.hpp ).
	// This function is called if there are no Include and Library installation directories
	// specified in the package configuration file.
	static void FetchDefaultIncLibDir( Package & pkg );

	// Creates relevant directories for the package manager, in $HOME.
	static bool HandlePkgDirs();
};

#endif // PACKAGECONFIG_HPP
#ifndef PACKAGECONFIG_HPP
#define PACKAGECONFIG_HPP

#include <vector>

#include "PackageData.hpp"

class PackageConfig
{
public:
	static bool GetPackage( std::string pkgname, Package & pkg );

	static bool HandlePkgDirs();
};

#endif // PACKAGECONFIG_HPP
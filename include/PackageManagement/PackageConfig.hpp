#ifndef PACKAGECONFIG_HPP
#define PACKAGECONFIG_HPP

#include <vector>
#include "../INI_System/INI_Parser.hpp"

#include "PackageData.hpp"

class PackageConfig
{
public:
	static bool GetPackage( std::string pkgname, Package & pkg );

	static std::string FetchExistFile( Electrux::INI_Parser & parser);

	static void FetchDefaultIncLibDir( Package & pkg );

	static bool HandlePkgDirs();
};

#endif // PACKAGECONFIG_HPP
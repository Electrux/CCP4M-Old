#ifndef PACKAGEDATA_HPP
#define PACKAGEDATA_HPP

#include <string>
#include <vector>

// This file contains structures related to data of/used by packages.

// Package structure contains all the information of any package. It is fetched
// using the PackageConfig class. The actual package config file is manually written
// by those wanting to put new libraries in the available libraries.
struct Package
{
	std::string name;
	std::string description;
	std::string lang;
	std::string type;
	std::string version;
	std::vector< std::string > deplist;

	std::string libflags;

	std::string url;
	std::string file;

	std::string buildmode;
	std::string cleanupdirs;
};

// Also, DirFile is used in FSFuncs to fetch the directory and file which resides
// in that directory.
struct DirFile
{
	std::string dir;
	std::string file;
};

#endif // PACKAGEDATA_HPP
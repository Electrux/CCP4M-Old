#ifndef PACKAGEDATA_HPP
#define PACKAGEDATA_HPP

#include <string>
#include <vector>

struct Package
{
	std::string name;
	std::string description;
	std::string lang;
	std::string type;
	std::string version;
	std::vector< std::string > deplist;

	std::string incdir;
	std::string libdir;
	std::string libflags;

	std::string url;
	std::string file;

	std::string buildcmds;
};

struct DirFile
{
	std::string dir;
	std::string file;
};

#endif // PACKAGEDATA_HPP
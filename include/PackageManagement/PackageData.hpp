#ifndef PACKAGEDATA_HPP
#define PACKAGEDATA_HPP

#include <string>
#include <vector>

struct Package
{
	std::string name;
	std::string url;
	std::string file;
	std::string version;
	std::string description;

	std::vector< std::string > deplist;

	std::string incdir;
	std::string libdir;
	std::string libflags;
};

#endif // PACKAGEDATA_HPP
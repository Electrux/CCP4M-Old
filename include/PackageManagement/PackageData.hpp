#ifndef PACKAGEDATA_HPP
#define PACKAGEDATA_HPP

#include <string>
#include <vector>

struct Package
{
	std::string name;
	std::string url;
	std::string filelinux;
	std::string filemac;
	std::string version;
	std::string description;

	std::vector< std::string > deplist;

	std::string incflag;
	std::string libflag;
};

#endif // PACKAGEDATA_HPP
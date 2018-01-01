#ifndef COREFUNCS_HPP
#define COREFUNCS_HPP

#include <string>
#include <vector>

#include "Paths.hpp"

enum Architecture
{
	LINUX,
	MAC,
	OTHER
};

const std::string GIT_CMD = "git";
const std::string GIT_CLONE_CMD = GIT_CMD + " clone https://github.com/Electrux/CCP4M-PKGI.git " + PACKAGE_LIST_DIR;
const std::string GIT_PULL_CMD = GIT_CMD + " pull " + PACKAGE_LIST_DIR;

extern Architecture ARCH;

void SetVarForArchitecture( std::string & var, std::vector< std::string > options );

void SetVarForArchitecture( int & var, std::vector< int > options );

#endif // COREFUNCS_HPP
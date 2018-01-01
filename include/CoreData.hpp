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
extern const std::string GIT_CLONE_CMD;
extern const std::string GIT_PULL_CMD;

extern Architecture ARCH;

void SetVarForArchitecture( std::string & var, std::vector< std::string > options );

void SetVarForArchitecture( int & var, std::vector< int > options );

#endif // COREFUNCS_HPP
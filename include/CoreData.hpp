#ifndef COREFUNCS_HPP
#define COREFUNCS_HPP

#include <string>
#include <vector>

#include "Paths.hpp"

// This enum defines various architectures available.
// Note: For now, BSD will simply point to MAC since both
// have same configurations pertaining to libraries.
enum Architecture
{
	LINUX,
	MAC,
	BSD,
	OTHER
};

// This variable contains the architecture of the machine
// this project is built on.
extern Architecture ARCH;

// This function is a helper function which sets the string/int var to
// a single string/int in options, based on architecture.
// Basically, var = options[ ARCH ]
// Although, if options doesn't contain sufficient possible values
// it won't do anything.
void SetVarForArchitecture( std::string & var, std::vector< std::string > options );
void SetVarForArchitecture( int & var, std::vector< int > options );

#endif // COREFUNCS_HPP
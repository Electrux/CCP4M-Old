#ifndef BUILDFILESGENERATOR_HPP
#define BUILDFILESGENERATOR_HPP

#include <vector>

#include "ProjectData.hpp"
#include "ConfigMgr.hpp"

// This file contains functions to build projects which are created using
//	ccp4m project new ...

// This is the main function which builds the project.
// Returns 0 on success, anything else on failure.
int GenerateBuildFiles();

// This function is a helper function which creates extra directories
// extracted from all other sources.
// Returns 0 on success, anything else on failure.
int CreateBuildDirectories( std::vector< std::string > & othersrc );

// This function fetches the directory part of a source file
// which can then be used to create respective build directory if it doesn't
// already exists.
// Returns directory string on success, empty string on failure.
std::string GetDirectoryFromSource( std::string src );

// This function fetches configuration data for the build process from the project
// configuration file ( like the flags, libraries, othersources, include directories, etc. ).
// Returns 0 on success, anything else on failure.
int GetBuildData( ConfigMgr & config, ProjectData & data, std::string & mainsrc,
		std::string & incdirs,
		std::string & flags, std::string & libs,
		std::vector< std::string > & othersrc );

// This function fetches source files based on the wildcard - '*'.
// It feeds the fetched source files in othersrc.
// Returns 0 on success, anything else on failure.
int GetWildCardSources( std::vector< std::string > & __othersrc,
			std::vector< std::string > & othersrc );

#endif // BUILDFILESGENERATOR_HPP

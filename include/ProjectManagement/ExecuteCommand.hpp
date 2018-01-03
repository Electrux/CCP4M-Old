#ifndef EXECUTECOMMAND_HPP
#define EXECUTECOMMAND_HPP

#include <vector>
#include <string>

#include "CompileCommandData.hpp"

// This file contains information of how to execute build commands
// including the multithreading support.

// This structure contains the post execution int result of a command
// and errors encountered by it ( string err ).
struct Result
{
	int res;
	std::string err;
};

// This function executes a command and returns Result on the callee.
Result ExecuteCommand( const std::string & command );

// This function displays the "building and linking" lines and shows errors
// if they are encountered.
void DisplayBuildResults( CCData & commands, const std::string & langstr, int res, const std::string & err, int percent );

// This function executes all the commands given to it, in a multithreaded fashion.
// Multithreading is such that the amount of cores used = 1/2 of total CPU cores.
// If a file fails to build, the process is completely stopped.
// Returns 0 on success, anything else on failure.
int ExecuteAllCommands( std::vector< CCData > & commands, const std::string & langstr, int count );

#endif // EXECUTECOMMAND_HPP

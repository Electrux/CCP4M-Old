#ifndef EXECUTECOMMAND_HPP
#define EXECUTECOMMAND_HPP

#include <vector>
#include <string>

#include "CompileCommandData.hpp"

struct Result
{
	int res;
	std::string err;
};

Result ExecuteCommand( const std::string & command );

void DisplayBuildResults( CCData & commands, const std::string & langstr, int res, const std::string & err, int percent );

int ExecuteAllCommands( std::vector< CCData > & commands, const std::string & langstr, int count );

#endif // EXECUTECOMMAND_HPP

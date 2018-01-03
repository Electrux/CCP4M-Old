#ifndef PROJECTMANAGER_HPP
#define PROJECTMANAGER_HPP

#include <vector>

// Shows the project subcommand's help.
void ShowProjectHelp( std::vector< std::string > & args );

// Called when subcommand to the main command is "project"
// and calls respective functions for new, help, build,
// and run.
// Returns the return values of these functions.
// 0 on success, anything else on failure.
int HandleProject( std::vector< std::string > & args );

#endif // PROJECTMANAGER_HPP

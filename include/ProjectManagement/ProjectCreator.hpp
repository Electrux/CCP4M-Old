#ifndef PROJECTCREATOR_HPP
#define PROJECTCREATOR_HPP

#include <string>
#include <vector>

// This function shows the usage syntax of the project create
// subcommand.
void ShowCreateHelp( std::vector< std::string > & args );

// This function initializes the project creation process.
// It first retrieves the flags and their data, and then
// calls ProjectFileGenerator() which generates the project
// files.
// Returns the value of ProjectFileGenerator() if all went good
// in this function, otherwise a non zero value is returned.
int CreateProject( std::vector< std::string > & args );

#endif // PROJECTCREATOR_HPP

#ifndef PROJECTBUILDER_HPP
#define PROJECTBUILDER_HPP

#include <vector>

// This function is called by the main() function and it
// initializes the build process of a project by calling
// BuildFilesGenerator().
// Returns the result of BuildFilesGenerator().
int BuildProject( std::vector< std::string > & args );

#endif // PROJECTBUILDER_HPP

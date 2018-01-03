#ifndef PROJECTEXECUTER_HPP
#define PROJECTEXECUTER_HPP

#include <vector>

// This function first calls BuildProject() and then
// executes it.
// Returns result of BuildProject() if it is non zero,
// otherwise runs the project, and returns 0 on success,
// anything else on failure.
int ExecuteProject( std::vector< std::string > & args );

#endif // PROJECTEXECUTER_HPP

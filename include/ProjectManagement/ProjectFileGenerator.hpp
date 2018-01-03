#ifndef PROJECTFILEGENERATOR_HPP
#define PROJECTFILEGENERATOR_HPP

#include "ProjectData.hpp"

// This function generates project files and directories
// based on the project data.
// The files and directories are:
// <Project Dir>/build
// <Project Dir>/include
// <Project Dir>/src
// <Project Dir>/src/main.c(pp)
// <Project Dir>/ccp4m.ini
// Returns 0 on success, anything else on failure.
int GenerateProjectFiles( ProjectData & data );

#endif // PROJECTFILEGENERATOR_HPP

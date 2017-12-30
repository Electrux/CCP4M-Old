#include <iostream>
#include <vector>
#include <string>

#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/ProjectManagement/ConfigMgr.hpp"
#include "../../include/ProjectManagement/BuildFilesGenerator.hpp"

#include "../../include/ProjectManagement/ProjectBuilder.hpp"

int BuildProject( std::vector< std::string > & args )
{
	return GenerateBuildFiles();
}
#include <iostream>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/FSFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/ProjectManagement/ConfigMgr.hpp"

#include "../../include/ProjectManagement/ProjectFileGenerator.hpp"

int GenerateProjectFiles( ProjectData & data )
{
	std::cout << YELLOW << "Project:\n\t" << CYAN << "Name: " << GREEN << data.name
		  << "\n\t" << CYAN << "Directory: " << GREEN << data.dir
		  << "\n\t" << CYAN << "Dependencies: ";

	if( !data.deps.empty() ) {
		for( auto dep : data.deps )
			std::cout << GREEN << dep << ", ";
		std::cout << "\b\b \n" << RESET << std::endl;
	}
	else {
		std::cout << GREEN << "NONE\n" << RESET << std::endl;
	}

	std::string proj_dir, proj_src, proj_inc, proj_build;

	SetFolderPaths( data.dir, data.name, proj_dir, proj_src, proj_inc, proj_build );

	CreateDir( proj_dir );
	CreateDir( proj_src );
	CreateDir( proj_inc );
	CreateDir( proj_build );

	std::cout << std::endl;

	if( data.lang == "c" )
		CreateFileWithContents( proj_src + "/main.c", DEFAULTMAINC );
	else
		CreateFileWithContents( proj_src + "/main.cpp", DEFAULTMAINCPP );

	ConfigMgr config;

	config.SetProject( data );

	return config.CreateDefaultConfig( proj_dir );
}

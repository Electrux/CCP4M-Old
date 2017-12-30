#include <iostream>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/ProjectManagement/ConfigMgr.hpp"

#include "../../include/ProjectManagement/ProjectFileGenerator.hpp"

int GenerateProjectFiles( ProjectData & data )
{
	DispColoredData( "Project information:", FIRST_COL, true );
	DispColoredData( "\tName: ", data.name, CYAN, GREEN, true );
	DispColoredData( "\tDirectory: ", data.dir, CYAN, GREEN, true );
	DispColoredData( "\tDependencies: ", CYAN, false );

	if( !data.deps.empty() ) {
		for( auto dep : data.deps )
			DispColoredData( dep + ", ", GREEN, false );
		DispColoredData( "\b\b \n", FIRST_COL, true );
	}
	else {
		DispColoredData( "NONE", GREEN, true );
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

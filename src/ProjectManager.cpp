#include <iostream>
#include <vector>

#include "../include/ColorDefs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/ProjectManagement/ProjectCreator.hpp"
#include "../include/ProjectManagement/ProjectBuilder.hpp"
#include "../include/ProjectManagement/ProjectExecuter.hpp"

#include "../include/ProjectManager.hpp"

int HandleProject( std::vector< std::string > & args )
{
	if( args.size() < 3 ) {
		DispColoredData( "Usage:", args[ 0 ] + " project [ help/new/run/build ] ...",
				FIRST_COL, SECOND_COL, true );
		return 1;
	}

	if( args[ 2 ] == "help" ) {
		ShowProjectHelp( args );
	}
	else if( args[ 2 ] == "new" ) {
		return CreateProject( args );
	}
	else if( args[ 2 ] == "build" ) {
		return BuildProject( args );
	}
	else if( args[ 2 ] == "run" ) {
		return ExecuteProject( args );
	}
	else {
		DispColoredData( "Invalid parameter. Possible Options are: help, new, run, build", FIRST_COL, true );
		return 1;
	}

	return 0;	
}

void ShowProjectHelp( std::vector< std::string > & args )
{
	DispColoredData( "Help:\n", FIRST_COL, true );

	DispColoredData( "Usage:", args[ 0 ] + " project [ help/new/run/build ]", FIRST_COL, SECOND_COL, true );
	DispColoredData( "\tParameters are:", FIRST_COL, true );
	DispColoredData( "\t\thelp", FIRST_COL, true );
	DispColoredData( "\t\t\tShow this help menu", SECOND_COL, true );

	DispColoredData( "\t\tnew", FIRST_COL, true );
	DispColoredData( "\t\t\tCreate a new C++ project", SECOND_COL, true );

	DispColoredData( "\t\tbuild", FIRST_COL, true );
	DispColoredData( "\t\t\tBuild C++ projects. You must be in the directory of a project", SECOND_COL, true );

	DispColoredData( "\t\trun", FIRST_COL, true );
	DispColoredData( "\t\t\tBuild and run C++ projects. You must be in the directory of a project", SECOND_COL, true );

	DispColoredData( "\nYou can get more information from the categorized help of ", FIRST_COL, false );
	DispColoredData( "each parameter. To do that, here is an example:", FIRST_COL, true );
	DispColoredData( "\t", args[ 0 ] + " project new help", FIRST_COL, SECOND_COL, true );

	DispColoredData( "", FIRST_COL, true );
}
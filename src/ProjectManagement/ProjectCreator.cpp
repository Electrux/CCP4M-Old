#include <iostream>
#include <vector>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/ProjectManagement/ProjectFileGenerator.hpp"

#include "../../include/ProjectManagement/ProjectCreator.hpp"

int CreateProject( std::vector< std::string > & args )
{
	if( args.size() < 3 ) {
		DispColoredData( "No parameter given to the command. Please use:", FIRST_COL, true );
		DispColoredData( "\t", args[ 0 ] + "project new help", FIRST_COL, SECOND_COL, true );
		DispColoredData( "for more information.", FIRST_COL, true );
		return 1;
	}

	if( args[ 3 ] == "help" ) {
		ShowCreateHelp( args );
		return 0;
	}

	if( args.size() < 4 ) {
		DispColoredData( "No name provided for the project. Please use:", FIRST_COL, true );
		DispColoredData( "\t", args[ 0 ] + "project new help", FIRST_COL, SECOND_COL, true );
		DispColoredData( "for more information.", FIRST_COL, true );
		return 1;
	}

	ProjectData data = GetProjectData( args );

	if( data.name.empty() ) {
		return 1;
	}

	return GenerateProjectFiles( data );
}

void ShowCreateHelp( std::vector< std::string > & args )
{
	DispColoredData( "Help:\n", FIRST_COL, true );
	DispColoredData( "Usage:", args[ 0 ] + "project new [ help ] / [ [ name ] --[ options ] ]",
			FIRST_COL, SECOND_COL, true );

	DispColoredData( "\tArgs are:", FIRST_COL, true );
	DispColoredData( "\t\thelp", FIRST_COL, true );
	DispColoredData( "\t\t\tShow this help menu", SECOND_COL, true );

	DispColoredData( "\t\tname", FIRST_COL, true );
	DispColoredData( "\t\t\tName of the C/C++ project. ( Mandatory )", SECOND_COL, true );

	DispColoredData( "\t\tOptions are:", FIRST_COL, true );

	DispColoredData( "\t\t\t--dir", SECOND_COL, true );
	DispColoredData( "\t\t\t\tSpecify the directory of project", EXTRA_COL, true );
	DispColoredData( "\t\t\t--deps", SECOND_COL, true );
	DispColoredData( "\t\t\t\tAdd a comma separated list of dependencies/libraries", EXTRA_COL, true );
	DispColoredData( "\t\t\t--lang", SECOND_COL, true );
	DispColoredData( "\t\t\t\tLanguage to use in the project ( default: C++ )", EXTRA_COL, true );

	DispColoredData( "", FIRST_COL, true );
}

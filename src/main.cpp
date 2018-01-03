#include <iostream>

#include "../include/ColorDefs.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/ProjectManager.hpp"
#include "../include/PackageManager.hpp"

void ShowMainHelp( std::vector< std::string > & args );

int main( int argc, char ** argv )
{
	auto args = ArgToVector( argc, argv );

	if( args.size() < 2 ) {
		DispColoredData( "Usage:", args[ 0 ] + " [ help/project/pkg ]", FIRST_COL, SECOND_COL, true );
		return 1;
	}

	int retval = 0;

	DispColoredData( "", FIRST_COL, true );

	if( args[ 1 ] == "project" ) {
		retval = HandleProject( args );
	}
	else if( args[ 1 ] == "pkg" ) {
		PackageManager pkgmgr( args );
		retval = pkgmgr.HandleCommand();
		DispColoredData( "", FIRST_COL, true );
	}
	else if( args[ 1 ] == "help" ) {
		ShowMainHelp( args );
		DispColoredData( "", FIRST_COL, true );
	}
	else {
		DispColoredData( "Invalid parameter. Possible options are: help, project, pkg", RED, true );
		retval = 1;
		DispColoredData( "", FIRST_COL, true );
	}

	return retval;
}

void ShowMainHelp( std::vector< std::string > & args )
{
	DispColoredData( "Help:\n", FIRST_COL, true );

	DispColoredData( "Usage: ", args[ 0 ] + " parameter [ parameter args ]", FIRST_COL, SECOND_COL, true );

	DispColoredData( "\tParameters are:", FIRST_COL, true );
	DispColoredData( "\t\thelp", FIRST_COL, true );
	DispColoredData( "\t\t\tShow this help menu", SECOND_COL, true );

	DispColoredData( "\t\tproject", FIRST_COL, true );
	DispColoredData( "\t\t\tCreate, manipulate and run C++ projects", SECOND_COL, true );

	DispColoredData( "\t\tpkg", FIRST_COL, true );
	DispColoredData( "\t\t\tManage C++ packages - libraries", SECOND_COL, true );

	DispColoredData( "\nYou can get more information from the categorized help of " );
	DispColoredData( "each parameter. To do that, here is an example:", FIRST_COL, true );
	DispColoredData( "\t", args[ 0 ] + " project help", FIRST_COL, SECOND_COL, true );

	DispColoredData( "", FIRST_COL, true );
}
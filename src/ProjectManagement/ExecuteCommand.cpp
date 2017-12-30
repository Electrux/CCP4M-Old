#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <future>
#include <vector>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/ProjectManagement/ExecuteCommand.hpp"
#include "../../include/ProjectManagement/CompileCommandData.hpp"

int ExecuteCommand( std::string command )
{
	int i = std::system( command.c_str() );

	return WEXITSTATUS( i );
}

void DisplayBuildResults( CCData & commands, const std::string & langstr, int res, int percent )
{
	DispColoredData( "[" + std::to_string( percent ) + "%]\t", RESET, false );
	DispColoredData( "Building and Linking " + langstr + " object:", "build/buildfiles/" + commands.othersource, " ... ",
			FIRST_COL, SECOND_COL, RESET, false );

	if( res == 0 )
		DispColoredData( TICK, GREEN, true );
	else
		DispColoredData( CROSS, RED, true );
}

int ExecuteAllCommands( std::vector< CCData > & commands, const std::string & langstr, int count )
{
	int retval = 0;

	float percent = 0.0, percentdelta = 100.0 / count;

	std::vector< std::future< int > > results;

	int ctr = 0;

	bool cancel = false;

	size_t cores = std::thread::hardware_concurrency() / 2;

	for( int i = 0; i < ( int )commands.size(); ++i ) {

		results.push_back( std::async( std::launch::async,
					       ExecuteCommand,
					       commands[ i ].command ) );

		if( ( i + 1 ) % cores == 0 || i == ( int )commands.size() - 1 ) {

			for( auto it = results.begin(); it != results.end(); ) {

				percent += percentdelta;

				retval |= it->get();
				it = results.erase( results.begin() );

				DisplayBuildResults( commands[ ctr ], langstr, retval, ( int )percent );

				ctr++;

				if( retval != 0 ) {
					cancel = true;
					break;
				}
			}
		}

		if( cancel )
			break;
	}

	for( auto & res : results )
		res.get();

	if( cancel )
		DispColoredData( "Error: Building failed. Cancelled process!", RED, true );

	return retval;
}

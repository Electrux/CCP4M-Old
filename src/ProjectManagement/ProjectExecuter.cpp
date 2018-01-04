#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "../../include/Paths.hpp"
#include "../../include/ColorDefs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/StringFuncs.hpp"

#include "../../include/ProjectManagement/ProjectBuilder.hpp"
#include "../../include/ProjectManagement/ConfigMgr.hpp"

#include "../../include/ProjectManagement/ProjectExecuter.hpp"

int ExecuteProject( std::vector< std::string > & args )
{
	if( BuildProject( args ) != 0 )
		return 1;

	ConfigMgr conf;

	if( conf.RetrieveConfig( "." ) != 0 )
		return 1;

	std::string projectname = conf.GetDataString( "Core", "Name" );

	std::string execenv = conf.GetDataString( "Core", "ExecEnv" );

	if( !execenv.empty() ) {
		// Replace , with space, and double space with single space.
		ReplaceInString( execenv, ',', ' ' );
		ReplaceInString( execenv, "  ", " " );
		ReplaceInString( execenv, LIB_DIR_REPLACEMENT, PACKAGE_LIBRARY_INSTALL_DIR );
		execenv += " ";
	}

	std::string command = execenv + "./build/" + projectname;

	if( args.size() > 3 ) {

		command += " ";

		for( int i = 3; i < ( int )args.size(); ++i ) {
			command += args[ i ];
			command += " ";
		}

		command.erase( command.end() - 1 );
	}

	DispColoredData( "\nExecuting Project...\n", BOLD_MAGENTA, true );
	return std::system( command.c_str() );
}

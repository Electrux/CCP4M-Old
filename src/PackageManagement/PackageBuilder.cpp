#include <iostream>
#include <string>
#include <vector>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageBuilder.hpp"

bool BuildDirectory( const Package & pkg )
{
	std::vector< std::string > buildcmds =
		DelimStringToVector( pkg.buildcmds );

	if( buildcmds.empty() ) {
		DispColoredData( "Nothing to build! Exiting!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		DispColoredData( "Unable to get working directory!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	if( !ChangeWorkingDir( GetPackageVersionDir( pkg ) ) ) {
		DispColoredData( "Unable to change working directory to the extracted package directory!",
				CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string create, make, install;

	create = buildcmds[ 0 ];
	make = buildcmds[ 1 ];

	if( buildcmds.size() >= 3 ) {
		install = buildcmds[ 2 ];
	}

	if( create.find( "configure" ) != std::string::npos ) {
		DispColoredData( "Creating makefile using configure ... " );
	}
	else if( create.find( "cmake" ) != std::string::npos ) {
		DispColoredData( "Creating makefile using CMake ... " );
	}

	int res;
	std::string errors;

	res = DispExecute( create, errors );

	if( res != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Unable to create makefile!", CROSS, FIRST_COL, RED, true );
		if( !errors.empty() ) {
			DispColoredData( "Errors:", RED, true );
			DispColoredData( errors, CYAN, true );
		}
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		DispColoredData( TICK, GREEN, true );
	}

	DispColoredData( "Using make ... " );

	res = DispExecute( make, errors );

	if( res != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Unable to make the package!", CROSS, FIRST_COL, RED, true );
		if( !errors.empty() ) {
			DispColoredData( "Errors:", RED, true );
			DispColoredData( errors, CYAN, true );
		}
		ChangeWorkingDir( cwd );
		return false;
	}
	else {
		DispColoredData( TICK, GREEN, true );
	}

	if( !install.empty() ) {
		DispColoredData( "Installing using make install ... " );

		res = DispExecute( install, errors );

		if( res != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to install using make install!", CROSS, FIRST_COL, RED, true );
			if( !errors.empty() ) {
				DispColoredData( "Errors:", RED, true );
				DispColoredData( errors, CYAN, true );
			}
			ChangeWorkingDir( cwd );
			return false;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}

	ChangeWorkingDir( cwd );

	return true;
}
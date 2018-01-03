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
	if( pkg.buildmode.empty() ) {
		DispColoredData( "Nothing to build! Exiting!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		DispColoredData( "Unable to get current working directory!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	if( !ChangeWorkingDir( GetPackageVersionDir( pkg ) ) ) {
		DispColoredData( "Unable to change working directory to the extracted package directory!",
				CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string create;

	if( pkg.buildmode.find( "configure" ) != std::string::npos ) {
		DispColoredData( "Creating makefile using configure ... " );
		create = "./configure --prefix=" + PACKAGE_INSTALL_DIR;
	}
	else if( pkg.buildmode.find( "cmake" ) != std::string::npos ) {
		DispColoredData( "Creating makefile using CMake ... " );
		create = "cmake -DCMAKE_INSTALL_PREFIX:PATH=" + PACKAGE_INSTALL_DIR + " .";
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

	DispColoredData( "Using make install ... " );

	res = DispExecute( "make install", errors );

	if( res != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Unable to make install the package!", CROSS, FIRST_COL, RED, true );
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

	ChangeWorkingDir( cwd );
	return true;
}
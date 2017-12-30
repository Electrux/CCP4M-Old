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

#include "../../include/PackageManagement/PackageData.hpp"

bool UninstallArchive( const Package & pkg, const std::vector< std::string > & args )
{
	std::vector< std::string > buildcmds = DelimStringToVector( pkg.buildcmds );

	bool usecustomuninstaller = false;

	if( pkg.type == "Binary" || buildcmds.size() < 4 ) {
		DispColoredData( "Using auto generated uninstaller file ...", TICK, FIRST_COL, GREEN, true );
		usecustomuninstaller = true;
	}
	else {
		DispColoredData( "Using uninstall command from library config ...", TICK, FIRST_COL, GREEN, true );
	}

	if( usecustomuninstaller ) {
		DispColoredData( "Removing data ... " );
		std::vector< std::string > cpdata = GetCopiedData( pkg );

		if( !RemoveCopiedData( pkg, cpdata ) ) {
			DispColoredData( "Unable to remove all copied data!", CROSS, FIRST_COL, RED, true );
			return false;
		}

		DispColoredData( TICK, GREEN, true );
		return true;
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

	std::string makeuninstall = buildcmds[ 3 ];

	DispColoredData( "Uninstalling using make uninstall ... " );

	int res;
	std::string errors;

	res = DispExecute( makeuninstall, errors );

	if( res != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Unable to uninstall using make uninstall!", CROSS, FIRST_COL, RED, true );
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
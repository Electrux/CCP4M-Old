#include <iostream>
#include <string>

#include "../../include/CoreData.hpp"
#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/PackageConfig.hpp"
#include "../../include/PackageManagement/PackageListUpdater.hpp"

int UpdatePackageList()
{
	if( !PackageConfig::HandlePkgDirs() ) {
		DispColoredData( "Unable to create/manipulate the configuration directories! Check permissions!",
				CROSS, FIRST_COL, RED, true );
		return 1;
	}

	std::string cwd = GetWorkingDir();

	if( cwd.empty() ) {
		DispColoredData( "Unable to get current working directory!", CROSS, FIRST_COL, RED, true );
		return false;
	}

	if( !ChangeWorkingDir( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Unable to change working directory to the package list directory!",
				CROSS, FIRST_COL, RED, true );
		return false;
	}

	std::string res;
	if( !LocExistsInPath( GIT_CMD, res ) ) {
		DispColoredData( GIT_CMD, "is not installed! Cannot continue!", CROSS,
				SECOND_COL, FIRST_COL, RED, true );
		ChangeWorkingDir( cwd );
		return -1;
	}

	if( IsDirEmpty( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Cloning list repository to:", PACKAGE_LIST_DIR, "... ",
				FIRST_COL, SECOND_COL, THIRD_COL, false );

		if( DispExecuteNoErr( GIT_CLONE_CMD, true ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to clone repository! Cannot continue!", CROSS,
					FIRST_COL, RED, true );
			ChangeWorkingDir( cwd );
			return 1;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}
	else {
		DispColoredData( "Pulling list repository to:", PACKAGE_LIST_DIR, "... ",
				FIRST_COL, SECOND_COL, THIRD_COL, false );

		if( DispExecuteNoErr( GIT_PULL_CMD, false ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to pull repository! Cannot continue!", CROSS,
					FIRST_COL, RED, true );
			ChangeWorkingDir( cwd );
			return 1;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}

	ChangeWorkingDir( cwd );

	return 0;
}
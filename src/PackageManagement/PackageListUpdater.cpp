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
		DispColoredData( "Unable to create/manipulate the configuration directories! Check permissions!", RED, true );
		return 1;
	}

	std::string res;
	if( !LocExistsInPath( GIT_CMD, res ) ) {
		DispColoredData( GIT_CMD, "is not installed! Cannot continue!", SECOND_COL, RED, true );
		return -1;
	}

	if( IsDirEmpty( PACKAGE_LIST_DIR ) ) {
		DispColoredData( "Cloning list repository to:", PACKAGE_LIST_DIR, "... ",
				FIRST_COL, SECOND_COL, THIRD_COL, false );

		if( DispExecuteNoErr( GIT_CLONE_CMD, false ) != 0 ) {
			DispColoredData( CROSS, RED, true );
			DispColoredData( "Unable to clone repository! Cannot continue!", RED, true );
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
			DispColoredData( "Unable to pull repository! Cannot continue!", RED, true );
			return 1;
		}
		else {
			DispColoredData( TICK, GREEN, true );
		}
	}

	return 0;
}
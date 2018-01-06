#ifndef PACKAGELISTUPDATER_HPP
#define PACKAGELISTUPDATER_HPP

#include <string>
#include <vector>
#include <map>

#include "../PackageManager.hpp"

// This function is used when
//	ccp4m pkg update
// command is used.

// Note: The local package repository should NOT be manually altered otherwise
// updating/creating it will probably fail.

// This function actually updates the local package repository.
// It will clone the original repository if there exists no repository locally,
// or it will pull the latest changes the the already existing local repository.
// Returns 0 on success, anything else on failure.
int UpdatePackageList( PackageManager & mgr );

// This function fetches last updated time of all packages,
// in terms of UNIX Epoch time.
std::map< std::string, long long > GetPackageUpdateTimes();

// Creates package update time file.
// Will overwrite previous one if one exists.
// prevupdatetimes defines packages already existing ( in this context ).
// So, those packages won't be included.
std::vector< std::string > ReCreatePackageUpdateTimesFile( const std::map< std::string, long long > & prevupdatetimes );

// Get packages which have been removed in last update.
std::vector< std::string > GetRemovedPackages( const std::map< std::string, long long > & prevupdatetimes );

// Display all the packages that are updated, new, or removed.
// and upgrade/remove them accordingly.
void UpdatePackages( PackageManager & mgr, const std::map< std::string, long long > & prevtimes,
			const std::map< std::string, long long > & newtimes,
			const std::vector< std::string > & newpackages,
			const std::vector< std::string > & removedpkgs );

// Mark the installed packages from allpkgs with a different color and tick.
void HighlightInstalledPackages( std::vector< std::string > & allpkgs );

// Checks if the version in first is greater than version in second.
// Returns true if version in first is greater.
bool IsVersionNewer( const std::string & first, const std::string & second );

#endif // PACKAGELISTUPDATER_HPP
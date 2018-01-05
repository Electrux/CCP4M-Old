#ifndef PACKAGELISTUPDATER_HPP
#define PACKAGELISTUPDATER_HPP

// This function is used when
//	ccp4m pkg update
// command is used.

// Note: The local package repository should NOT be manually altered otherwise
// updating/creating it will probably fail.

// This function actually updates the local package repository.
// It will clone the original repository if there exists no repository locally,
// or it will pull the latest changes the the already existing local repository.
// Returns 0 on success, anything else on failure.
int UpdatePackageList();

// This function fetches last updated time of all packages,
// in terms of UNIX Epoch time.
std::map< std::string, long long > GetPackageUpdateTimes();

// Creates package update time file.
// Will overwrite previous one if one exists.
// prevupdatetimes defines packages already existing ( in this context ).
// So, those packages won't be included.
std::vector< std::string > ReCreatePackageUpdateTimesFile( std::map< std::string, long long > & prevupdatetimes );

// Display all the packages that are updated or new.
void DisplayUpdatedPackages( const std::map< std::string, long long > & prevtimes,
			const std::map< std::string, long long > & newtimes,
			const std::vector< std::string > & newpackages );

// Mark the installed packages from allpkgs with a different color and tick.
void HighlightInstalledPackages( std::vector< std::string > & allpkgs );

#endif // PACKAGELISTUPDATER_HPP
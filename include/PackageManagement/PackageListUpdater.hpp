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

#endif // PACKAGELISTUPDATER_HPP
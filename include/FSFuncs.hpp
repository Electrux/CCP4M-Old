#ifndef FSFUNCS_HPP
#define FSFUNCS_HPP

#include <map>
#include <string>
#include <vector>

#include "PackageManagement/PackageData.hpp"

// This function sets the directory variables
// - projfolder, srcfolder, includefolder, buildfolder
// using "directory" of a project ( "projectname" ) when
// it is being created.
void SetFolderPaths( std::string & directory,
		     const std::string & projectname,
		     std::string & projfolder,
		     std::string & srcfolder,
		     std::string & includefolder,
		     std::string & buildfolder );

// This function is like a combination of direxists
// and fileexists.
// Returns true if it exists, false if not.
bool LocExists( const std::string & location );

// Checks if the location ( file ), exists in the PATH
// env variable's directories. If yes, returns the first
// directory as res in which it exists.
// Returns true if it exists, false if not.
bool LocExistsInPath( const std::string & location, std::string & res );

// Creates a directory ( or even nested directory ) ( equivalent to mkdir -p ).
// If verbose is true, shows what directory is being created ( in a single statement ).
// Returns 0 on success, non zero on failure.
int CreateDir( const std::string & dir, bool verbose = true );

// Creates a file ( filename ) with contents ( contents ).
// If contents is empty, an empty file is created.
// filename contains the entire path of file.
void CreateFileWithContents( const std::string & filename,
			     const std::string & contents = std::string(), bool verbose = true );

// Checks if the source file ( filename ) has no changes
// after the last time it was built.
// Returns true if it has no changes, false if it is not
// or no build file exists.
bool IsLatestBuild( std::string filename );

// Gets the last modified time of a file in UNIX Epoch time format.
// Returns -1 if the file doesn't exist.
long long GetLastModifiedTime( std::string file );

// Gets files in the src directories ( for compilation ), without
// actually appending the "src/" part in the path.
// Inserts all files in temp vector.
// Returns 0 on success, 1 if directory doesn't exist.
int GetFilesInDirNonSrc( std::string dir, std::vector< std::string > & temp, bool recursive = false );

// Fetches files in directory based on comma separated wildcards.
// Returns count of files encountered.
// If recursive is true, the files are found recursively,
// in the directory.
// The first overload takes in wildcards as comma separated
// and generates a vector of wildcards, which is the feeded
// to the second overload which uses the vector of wildcards.
int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp,
			std::string wildcards, bool recursive = true );

int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp,
			std::vector< std::string > wildcard_vec, bool recursive = true, std::string tempdir = std::string() );

// Checks if the directory has any files or not.
// Returns true if it is empty, false if not.
bool IsDirEmpty( std::string dir );

// Checks permissions to modify directories for package management.
// framework directory ( on mac ) is checked only if frameworks_exists = true.
// Returns true if permissions are fine, false if not.
bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists = false );

// Creates directory for downloaded package archive.
// The directory structure is:
//	PACKAGE_TMP/<package name>/<version number sequence>/
// Returns true if directory is created or already existing,
// false if not able to create.
bool CreatePackageDir( const Package & pkg, bool verbose = false );

// Fetches the package directory as string:
//	PACKAGE_TMP/<package name>
std::string GetPackageDir( const Package & pkg );
// Fetches package versioned directory as string:
//	PACKAGE_TMP/<package name>/<version string>
std::string GetPackageVersionDir( const Package & pkg );

// Removes the files and directories which are copied when package is installed.
// The file/directory in data is erased when it's removed.
// Returns true on success, false if not.
bool RemoveCopiedData( const Package & pkg, std::vector< std::string > & data );

// Saves the files and directories which have been copied, to a file in package
// management directory.
// Returns true if saved successfully, false if not.
bool SaveCopiedData( const Package & pkg, const std::vector< std::string > & data );

// Fetches the copied data from the file which was created by
// SaveCopiedData().
std::vector< std::string > GetCopiedData( const Package & pkg );

#endif // FSFUNCS_HPP

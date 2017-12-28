#ifndef FSFUNCS_HPP
#define FSFUNCS_HPP

#include <map>
#include <string>
#include <vector>

#include "PackageManagement/PackageData.hpp"

void SetFolderPaths( std::string & directory,
		     const std::string & projectname,
		     std::string & projfolder,
		     std::string & srcfolder,
		     std::string & includefolder,
		     std::string & buildfolder );

bool LocExists( const std::string & location );

int CreateDir( const std::string & dir, bool verbose = true );

void CreateFileWithContents( const std::string & filename,
			     const std::string & contents = std::string() );

bool IsLatestBuild( std::string filename );

long long GetLastModifiedTime( std::string file );

int GetFilesInDirNonSrc( std::string dir, std::vector< std::string > & temp, bool recursive = false );

int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp,
			std::string wildcards, bool recursive = true );

int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp,
			std::vector< DirFile > wildcard_vec, bool recursive = true, std::string tempdir = std::string() );

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists = false );

bool CreateArchiveDir( const Package & pkg, bool verbose = false );

std::string GetArchiveDir( const Package & pkg );

void FetchExtraDirs( const Package & pkg,
		const std::map< std::string, std::vector< DirFile > > & copyfiles,
		std::vector< std::string > & fileanddir );

bool RemoveCopiedData( const Package & pkg, std::vector< std::string > & data );

bool SaveCopiedData( const Package & pkg, const std::vector< std::string > & data );

std::vector< std::string > GetCopiedData( const Package & pkg );

#endif // FSFUNCS_HPP

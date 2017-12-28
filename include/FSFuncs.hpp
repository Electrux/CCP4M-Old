#ifndef FSFUNCS_HPP
#define FSFUNCS_HPP

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

int CreateDir( const std::string & dir );

void CreateFileWithContents( const std::string & filename,
			     const std::string & contents = std::string() );

bool IsLatestBuild( std::string filename );

long long GetLastModifiedTime( std::string file );

int GetFilesInDir( std::string dir, std::vector< std::string > & temp, bool recursive = false );

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists = false );

bool CreateArchiveDir( const Package & pkg );

std::string GetArchiveDir( const Package & pkg );

bool ChangeWorkingDir( std::string dir );

std::string GetWorkingDir();

void FetchExtraDirs( const Package & pkg, std::vector< std::string > & fileanddir );

bool RemoveCopiedData( const Package & pkg, std::vector< std::string > & data );

bool SaveCopiedData( const Package & pkg, const std::vector< std::string > & data );

std::vector< std::string > GetCopiedData( const Package & pkg );

#endif // FSFUNCS_HPP

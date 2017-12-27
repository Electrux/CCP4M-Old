#ifndef FSFUNCS_HPP
#define FSFUNCS_HPP

#include <string>

#include "PackageManagement/PackageData.hpp"

void SetFolderPaths( std::string & directory,
		     const std::string & projectname,
		     std::string & projfolder,
		     std::string & srcfolder,
		     std::string & includefolder,
		     std::string & buildfolder );

bool DirExists( const std::string & dir );

int CreateDir( const std::string & dir );

void CreateFileWithContents( const std::string & filename,
			     const std::string & contents = std::string() );

bool IsLatestBuild( std::string filename );

long long GetLastModifiedTime( std::string file );

int GetFilesInDir( std::string dir, std::vector< std::string > & temp, bool recursive = false );

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists = false );

bool CreateArchiveDir( const Package & pkg );

std::string GetArchiveDir( const Package & pkg );

#endif // FSFUNCS_HPP
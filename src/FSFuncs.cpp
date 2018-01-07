#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/ColorDefs.hpp"
#include "../include/UTFChars.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/Paths.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/DisplayExecute.hpp"

#include "../include/PackageManagement/PackageData.hpp"

#include "../include/FSFuncs.hpp"

void SetFolderPaths( std::string & directory,
		     const std::string & projectname,
		     std::string & projfolder,
		     std::string & srcfolder,
		     std::string & includefolder,
		     std::string & buildfolder )
{
	// Incase the dirname given by user does not contain '/' at the end...
	directory = ( * (directory.end() - 1) == '/' ) ? directory : directory + "/";

	projfolder      = directory + projectname;

	srcfolder       = projfolder + "/src";
	includefolder   = projfolder + "/include";
	buildfolder     = projfolder + "/build";
}

bool LocExists( const std::string & location )
{
	struct stat info;

	// Either hard file/directory, or even symlinks work.
	if( stat( location.c_str(), & info ) == 0 || lstat( location.c_str(), & info ) == 0 )
		return true;

	return false;
}

bool LocExistsInPath( const std::string & location, std::string & res )
{
	auto path = GetEnvPath();

	for( auto p : path ) {
		if( LocExists( p + location ) ) {
			res = p + location;
			return true;
		}
	}

	return false;
}

// Can create directory in directory B)
int CreateDir( const std::string & dir, bool verbose )
{
	if( dir.empty() || LocExists( dir ) )
		return 0;

	std::vector< std::string > dirs;

	std::string temp;
	for( auto ch : dir ) {

		if( ch == '/' && !temp.empty() ) {
			dirs.push_back( temp );
			temp.clear();
		}
		else {
			temp += ch;
		}
	}

	if( !temp.empty() )
		dirs.push_back( temp );

	if( verbose )
		DispColoredData( "Creating Directory:", dir, BLUE, MAGENTA, true );

	int retval = 0;
	std::string finaldir;

	for( int i = 0; i < ( int )dirs.size(); ++i ) {

		finaldir += dirs[ i ];

		if( !LocExists( finaldir ) )
			retval |= mkdir( finaldir.c_str(), 0755 );
		finaldir += "/";
	}

	return retval;
}

void CreateFileWithContents( const std::string & filename,
			     const std::string & contents, bool verbose )
{
	if( verbose )
		DispColoredData( "Creating file:", filename, BLUE, MAGENTA, true );

	std::fstream file;

	file.open( filename, std::ios::out );

	if( !contents.empty() )
		file << contents;

	file.close();
}

bool IsLatestBuild( std::string filename )
{
	std::vector< std::string > includes;

	std::fstream file;

	std::string srcfullpath = "src/" + GetStringTillLastSlash( filename );

	file.open( "src/" + filename, std::ios::in );

	if( !file )
		return false;

	std::string line;

	while( std::getline( file, line ) ) {
		if( line.find( "#include \"" ) != std::string::npos )
			includes.push_back( GetStringBetweenQuotes( line ) );
	}

	file.close();

	long long buildfilemodtime = GetLastModifiedTime(
		"build/buildfiles/" + filename + ".o" );

	if( buildfilemodtime < 0 )
		return false;

	for( auto inc : includes ) {

		long long modtime = GetLastModifiedTime( srcfullpath + inc );

		if( modtime < 0 || modtime > buildfilemodtime )
			return false;
	}

	long long srcfilemodtime = GetLastModifiedTime( "src/" + filename );

	if( srcfilemodtime < 0 || srcfilemodtime > buildfilemodtime )
		return false;

	return true;
}

long long GetLastModifiedTime( std::string file )
{
	struct stat info;

	if( stat( file.c_str(), & info ) != 0 )
		return -1;

	return info.st_mtime;
}

int GetFilesInDirNonSrc( std::string dir, std::vector< std::string > & temp, bool recursive )
{
	if( !LocExists( dir ) )
		return 1;

	DIR * dirp = opendir( dir.c_str() );
	struct dirent * p;

	std::string tempdir;

	if( dir == "src" )
		tempdir = "";
	else if( dir.find( "src/" ) != std::string::npos )
		tempdir = dir.substr( 4 ) + "/";

	while( ( p = readdir( dirp ) ) != NULL ) {

		if( strcmp( p->d_name, "." ) == 0 || strcmp( p->d_name, ".." ) == 0 )
			continue;

		if( p->d_type == DT_DIR && recursive )
			GetFilesInDirNonSrc( dir + "/" + p->d_name, temp );
		else if( p->d_type != DT_DIR )
			temp.push_back( tempdir + p->d_name );
	}

	return 0;
}

int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp, std::string wildcards, bool recursive )
{
	if( !LocExists( dir ) )
		return 0;
	
	if( * ( dir.end() - 1 ) == '/' )
		dir.erase( dir.end() - 1 );

	std::vector< std::string > wildcard_vec = DelimStringToVector( wildcards );

	for( auto wc = wildcard_vec.begin(); wc != wildcard_vec.end(); ++wc )
		TrimString( * wc );

	TrimWildCards( wildcard_vec );

	return GetWildCardFilesInDir( dir, temp, wildcard_vec, recursive );
}

int GetWildCardFilesInDir( std::string dir, std::vector< DirFile > & temp,
			std::vector< std::string > wildcard_vec, bool recursive, std::string tempdir )
{
	if( !LocExists( dir ) )
		return 0;

	DIR* dirp = opendir( dir.c_str() );
	struct dirent * p;

	if( tempdir != "" )
		tempdir += "/";

	int count = 0;

	while( ( p = readdir( dirp ) ) != NULL ) {
		if( strcmp( p->d_name, "." ) == 0 || strcmp( p->d_name, ".." ) == 0 )
			continue;

		if( p->d_type == DT_DIR && recursive ) {
			count += GetWildCardFilesInDir( dir + "/" + p->d_name, temp, wildcard_vec, recursive, tempdir + p->d_name );
		}
		else if( p->d_type != DT_DIR && IsWildCardsCompatible( p->d_name, wildcard_vec ) ) {
			temp.push_back( { tempdir, p->d_name } );
			count++;
		}
	}

	return count;
}

bool IsDirEmpty( std::string dir )
{
	std::vector< DirFile > temp;
	return !GetWildCardFilesInDir( dir, temp, "*" );
}

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists )
{
	int ret = 0;

	if( DispExecuteNoErr( "touch " + PACKAGE_INCLUDE_INSTALL_DIR + "pkgtest", false ) != 0 )
		return false;
	DispExecuteNoErr( "rm -rf " + PACKAGE_INCLUDE_INSTALL_DIR + "pkgtest", false );

	if( DispExecuteNoErr( "touch " + PACKAGE_LIBRARY_INSTALL_DIR + "pkgtest", false ) != 0 )
		return false;
	DispExecuteNoErr( "rm -rf " + PACKAGE_LIBRARY_INSTALL_DIR + "pkgtest", false );

	if( framework_exists ) {
		if( DispExecuteNoErr( "touch " + PACKAGE_FRAMEWORKS_INSTALL_DIR + "pkgtest", false ) != 0 )
			return false;
		DispExecuteNoErr( "rm -rf " + PACKAGE_FRAMEWORKS_INSTALL_DIR + "pkgtest", false );
	}
	return !( bool )ret;
}

bool CreatePackageDir( const Package & pkg, bool verbose )
{
	std::string archivedir = GetPackageVersionDir( pkg );

	if( LocExists( archivedir ) )
		return true;
	
	int ret = CreateDir( archivedir, verbose );

	if( ret != 0 ) {
		DispColoredData( "Error: Unable to create temporary archive directory! Exiting!", RED, true );
		return false;
	}

	return true;
}

std::string GetPackageDir( const Package & pkg )
{
	return PACKAGE_TMP_DIR + pkg.name;
}

std::string GetPackageVersionDir( const Package & pkg )
{
	return PACKAGE_TMP_DIR + pkg.name + "/" + pkg.version;
}

bool RemoveCopiedData( const Package & pkg, std::vector< std::string > & data )
{
	for( auto it = data.begin(); it != data.end(); ) {

		if( !LocExists( * it ) ) {
			it = data.erase( it );
			continue;
		}

		if( DispExecuteNoErr( "rm -rf " + ( * it ), true ) != 0 )
			return false;

		it = data.erase( it );
	}

	std::string cpdatafile = PACKAGE_BASE_DIR + "." + pkg.name;

	if( LocExists( cpdatafile ) && DispExecuteNoErr( "rm -rf " + cpdatafile, false ) != 0 )
		return false;

	return true;
}

bool SaveCopiedData( const Package & pkg, const std::vector< std::string > & copiedfiles )
{
	std::fstream installedfiles;
	installedfiles.open( PACKAGE_BASE_DIR + "." + pkg.name, std::ios::out );

	if( !installedfiles )
		return false;

	for( auto file : copiedfiles )
		installedfiles << file << std::endl;

	installedfiles.close();

	return true;
}

std::vector< std::string > GetCopiedData( const Package & pkg )
{
	std::vector< std::string > data;

	std::fstream file;
	file.open( PACKAGE_BASE_DIR + "." + pkg.name, std::ios::in );

	if( !file )
		return data;

	std::string line;

	while( std::getline( file, line ) ) {
		if( !line.empty() && line != "\n" ) {
			TrimString( line );
			data.push_back( line );
		}
	}

	file.close();

	return data;
}
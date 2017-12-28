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
	directory = ( *(directory.end() - 1) == '/' ) ? directory : directory + "/";


	projfolder      = directory + projectname;

	srcfolder       = projfolder + "/src";
	includefolder   = projfolder + "/include";
	buildfolder     = projfolder + "/build";
}

bool LocExists( const std::string & location )
{
	struct stat info;

	if( stat( location.c_str(), & info ) == 0 )
		return true;

	return false;
}

// Can create directory in directory B)
int CreateDir( const std::string & dir )
{
	if( dir.empty() || LocExists( dir ) )
		return 0;

	std::vector< std::string > dirs;

	std::string temp;
	for( auto ch : dir ) {

		if( ch == '/' ) {

			dirs.push_back( temp );
			temp.clear();
		}
		else {
			temp += ch;
		}
	}

	if( !temp.empty() )
		dirs.push_back( temp );

	std::cout << BLUE << "Creating Directory: " << MAGENTA << dir << RESET << "\n";

	int retval = 0;
	std::string finaldir;

	for( int i = 0; i < ( int )dirs.size(); ++i ) {

		finaldir += dirs[ i ];

		if( !LocExists( finaldir ) )
			retval |= CreateDir( finaldir );
		std::cout << "RetVal: " << retval << std::endl;

		finaldir += "/";
	}

	return retval;
}

void CreateFileWithContents( const std::string & filename,
			     const std::string & contents )
{
	std::cout << BLUE << "Creating file: " << MAGENTA << filename << RESET << std::endl;

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

		if( line.find( "#include \"" ) != std::string::npos ) {

			//std::cout << "includes: " << line << "\n";

			includes.push_back( GetStringBetweenQuotes( line ) );
		}
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

int GetFilesInDir( std::string dir, std::vector< std::string > & temp, bool recursive )
{
	struct stat info;
	if( stat( dir.c_str(), & info ) != 0 )
		return 1;

	DIR* dirp = opendir( dir.c_str() );
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
			GetFilesInDir( dir + "/" + p->d_name, temp );
		else if( p->d_type != DT_DIR )
			temp.push_back( tempdir + p->d_name );
	}

	return 0;
}

bool CheckNecessaryPermissions( const Package & pkg, bool framework_exists )
{
	int ret = 0;

	if( DispExecuteNoErr( "touch " + pkg.incdir + "/pkgtest", false ) != 0 )
		return false;
	DispExecuteNoErr( "rm -rf " + pkg.incdir + "/pkgtest", false );

	if( DispExecuteNoErr( "touch " + pkg.libdir + "/pkgtest", false ) != 0 )
		return false;
	DispExecuteNoErr( "rm -rf " + pkg.libdir + "/pkgtest", false );

	if( framework_exists ) {
		if( DispExecuteNoErr( "touch /Library/Frameworks/pkgtest", false ) != 0 )
			return false;
		DispExecuteNoErr( "rm -rf /Library/Frameworks/pkgtest", false );
	}
	return !( bool )ret;
}

bool CreateArchiveDir( const Package & pkg )
{
	std::string archivedir = GetArchiveDir( pkg );

	if( LocExists( archivedir ) )
		return true;
	
	int ret = CreateDir( archivedir );

	if( ret != 0 ) {
		std::cout << RED << "Error: Unable to create temporary archive directory! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	return true;
}

std::string GetArchiveDir( const Package & pkg )
{
	size_t loc;

	loc = pkg.file.find( ".tar" );

	std::string archivedir = PACKAGE_TMP;

	for( size_t i = 0; i < loc; ++i )
		archivedir += pkg.file[ i ];

	return archivedir;
}

bool ChangeWorkingDir( std::string dir )
{
	return chdir( dir.c_str() ) == 0;
}

std::string GetWorkingDir()
{
	char cwd[ 1024 ];

	if( getcwd( cwd, sizeof( cwd ) ) != NULL ) {
		return cwd;
	}

	return "";
}

void FetchExtraDirs( const Package & pkg, std::vector< std::string > & fileanddir )
{
	for( auto fnd : fileanddir ) {
		std::string dir = GetStringTillLastSlash( fnd );

		if( dir.empty() )
			continue;
		// Erase last trailing slash.
		dir.erase( dir.end() - 1 );

		if( std::find( fileanddir.begin(), fileanddir.end(), dir ) != fileanddir.end() )
			continue;

		if( dir == pkg.incdir || dir == pkg.libdir || dir == "/Library/Frameworks" ) {
			continue;
		}
		
		fileanddir.push_back( dir );
	}
}

bool RemoveCopiedData( const Package & pkg, std::vector< std::string > & data )
{
	for( auto it = data.begin(); it != data.end(); ) {

		if( !LocExists( * it ) ) {
			it = data.erase( it );
			continue;
		}
		if( DispExecuteNoErr( "rm -rf " + ( * it ), false ) != 0 ) {
			return false;
		}
		it = data.erase( it );
	}

	std::string cpdatafile = PACKAGE_DIR + "." + pkg.name;

	if( LocExists( cpdatafile ) ) {
		if( DispExecuteNoErr( cpdatafile, false ) != 0 ) {
			return false;
		}
	}

	return true;
}

bool SaveCopiedData( const Package & pkg, const std::vector< std::string > & copiedfiles )
{
	std::fstream installedfiles;
	installedfiles.open( PACKAGE_DIR + "." + pkg.name, std::ios::out );
	if( !installedfiles ) {
		std::cout << RED << "Error in saving installation information!\nReverting installation ... " << RESET;
		std::cout.flush();
		return false;
	}

	for( auto file : copiedfiles ) {
		installedfiles << file << std::endl;
	}

	installedfiles.close();

	return true;
}

std::vector< std::string > GetCopiedData( const Package & pkg )
{
	std::vector< std::string > data;

	std::fstream file;
	file.open( PACKAGE_DIR + "." + pkg.name, std::ios::in );

	if( !file ) {
		return data;
	}

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
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#include "../../include/CoreData.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/ProjectManagement/ConfigMgr.hpp"
#include "../../include/ProjectManagement/ExecuteCommand.hpp"
#include "../../include/ProjectManagement/CompileCommandData.hpp"

#include "../../include/ProjectManagement/BuildFilesGenerator.hpp"

int GenerateBuildFiles()
{
	std::string mainsrc, incdirs, flags, libs;
	std::vector< std::string > othersrc;

	ProjectData data;
	ConfigMgr config;

	if( GetBuildData( config, data, mainsrc, incdirs, flags, libs, othersrc ) != 0 )
		return 1;

	if( CreateBuildDirectories( othersrc ) != 0 )
		return 1;

	std::string compiler;

	SetVarForArchitecture( compiler, {
				data.lang == "c" ? "gcc" : "g++",
				data.lang == "c" ? "clang" : "clang++",
				data.lang == "c" ? "clang" : "clang++",
				""
			} );

	std::string langstr = data.lang == "c" ? "C" : "CXX";

	std::string standard = config.GetDataString( "Core", "Std" );

	int filecount = othersrc.size() + ( int )!mainsrc.empty();

	std::vector< CCData > commands;

	for( auto othersource : othersrc ) {

		std::string compilestr =
			compiler + " -c " + incdirs + flags + "-std=" + standard
			+ " -o build/buildfiles/" + othersource
			+ ".o src/" + othersource;

		bool res = IsLatestBuild( othersource );

		if( res )
			filecount--;
		else
			commands.push_back( { othersource, compilestr } );
	}

	if( ExecuteAllCommands( commands, langstr, filecount ) != 0 )
		return 1;

	if( !mainsrc.empty() ) {

		auto mainmodtime = GetLastModifiedTime( "src/" + mainsrc );
		auto buildfilemodtime = GetLastModifiedTime( "build/" + data.name );
		auto configmodtime = GetLastModifiedTime( "ccp4m.ini" );

		if( buildfilemodtime >= 0 && mainmodtime <= buildfilemodtime
			&& configmodtime >= 0 && configmodtime <= buildfilemodtime
			&& filecount == 1 ) {
			DispColoredData( "Project up to date!", BOLD_GREEN, true );
		}
		else {
			std::string compilestr =
				compiler + " -g " + incdirs + libs + flags + "-std=" + standard + " -o build/"
				+ data.name;

			for( auto othersource : othersrc )		
				compilestr += " build/buildfiles/" + othersource + ".o";

			compilestr += " src/" + mainsrc;

			if( !commands.empty() )
				DispColoredData( "", FIRST_COL, true );

			DispColoredData( "[100%]\t", RESET, false );
			DispColoredData( "Building and Linking " + langstr + " executable:", "build/" + data.name, "... ",
					BOLD_YELLOW, BOLD_GREEN, RESET, false );

			Result retval = ExecuteCommand( compilestr );

			if( retval.res == 0 ) {
				DispColoredData( TICK, GREEN, true );
			}
			else {
				DispColoredData( CROSS, RED, true );
				DispColoredData( "Errors:", RED, true );
				DispColoredData( "", FIRST_COL, true );
				DispColoredData( retval.err, RESET, true );
			}

			if( retval.res != 0 )
				return retval.res;
		}
	}

	return 0;
}

int CreateBuildDirectories( std::vector< std::string > & othersrc )
{
	std::string buildfilesdir = "build/buildfiles/";

	int retval = 0;
	int ctr = 0;

	if( !LocExists( buildfilesdir ) ) {
		retval |= CreateDir( buildfilesdir );
		ctr++;
	}

	for( auto src : othersrc ) {

		if( src.find( '/' ) == std::string::npos )
			continue;

		if( !LocExists( buildfilesdir + GetDirectoryFromSource( src ) ) ) {
			retval |= CreateDir( buildfilesdir + GetDirectoryFromSource( src ) );
			ctr++;
		}
	}

	if( ctr > 0 )
		DispColoredData( "", FIRST_COL, true );

	return retval;
}

std::string GetDirectoryFromSource( std::string src )
{
	std::string dir;

	bool foundslash = false;

	for( auto it = src.rbegin(); it != src.rend(); ++it ) {

		if( *it == '/' && foundslash == false ) {

			foundslash = true;
			continue;
		}

		if( foundslash )
			dir += *it;
	}

	std::reverse( dir.begin(), dir.end() );

	return dir;
}

int GetBuildData( ConfigMgr & config, ProjectData & data, std::string & mainsrc,
		std::string & incdirs,
		std::string & flags, std::string & libs,
		std::vector< std::string > & othersrc )
{
	if( config.RetrieveConfig( "." ) != 0 ) {
		DispColoredData( "Error: No project configuration file ( ccp4m.ini ) detected in current directory!",
				RED, true );
		return 1;
	}

	data.name = config.GetDataString( "Core", "Name" );
	data.deps = DelimStringToVector( config.GetDataString( "Core", "Libs" ) );
	data.dir = ".";

	data.lang = config.GetDataString( "Core", "Lang" );

	mainsrc = config.GetDataString( "Core", "MainSrc" );
	std::string _othersrc = config.GetDataString( "Core", "OtherSrc" );

	std::string _flags = config.GetDataString( "Core", "OtherFlags" );
	std::string _libs = config.GetDataString( "Core", "Libs" );

	std::vector< std::string > flagvec = DelimStringToVector( _flags );
	std::vector< std::string > libvec = DelimStringToVector( _libs );

	flags = "";
	for( auto fl : flagvec ) {
		flags += fl;
		flags += " ";
	}

	incdirs = "";
	for( auto lb : libvec ) {

		auto incflagvecstr = config.GetDataString( lb, "IncFlags" );

		std::replace( incflagvecstr.begin(), incflagvecstr.end(), ',', ' ' );
		ReplaceInString( incflagvecstr, INC_DIR_REPLACEMENT, PACKAGE_INCLUDE_INSTALL_DIR );

		if( !incflagvecstr.empty() ) {
			incdirs += incflagvecstr;
			incdirs += " ";
		}
	}

	libs = "";
	for( auto lb : libvec ) {

		auto libflagvecstr = config.GetDataString( lb, "LibFlags" );

		std::replace( libflagvecstr.begin(), libflagvecstr.end(), ',', ' ' );
		ReplaceInString( libflagvecstr, LIB_DIR_REPLACEMENT, PACKAGE_LIBRARY_INSTALL_DIR );

		if( !libflagvecstr.empty() ) {
			libs += libflagvecstr;
			libs += " ";
		}
	}

	std::vector< std::string > __othersrc = DelimStringToVector( _othersrc );

	if( __othersrc.size() == 1 && * __othersrc.begin() == "*" ) {

		if( GetFilesInDirNonSrc( "src", othersrc, true ) != 0 )
			return 1;
	}
	else {
		if( GetWildCardSources( __othersrc, othersrc ) != 0 )
			return 1;
	}

	if( mainsrc.empty() )
		return 0;

	for( auto it = othersrc.begin(); it != othersrc.end(); ++it ) {

		if( *it == mainsrc ) {
			othersrc.erase( it );
			break;
		}
	}

	return 0;
}

int GetWildCardSources( std::vector< std::string > & __othersrc,
			std::vector< std::string > & othersrc )
{
	for( auto it = __othersrc.begin(); it != __othersrc.end(); ++it ) {

		if( * ( it->end() - 1 ) == '*' ) {

			// Remove * and /
			it->erase( it->end() - 1 );
			it->erase( it->end() - 1 );

			std::string loc;

			loc += ( *it == "src" ) ? "src" : "src/" + *it;

			if( GetFilesInDirNonSrc( loc, othersrc ) != 0 )
				return 1;
		}

		if( * ( it->end() - 1 ) != '/' ) {
			othersrc.push_back( * it );
		}
	}

	return 0;
}
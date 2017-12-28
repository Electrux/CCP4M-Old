#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <array>
#include <memory>
#include <cstdio>

#include "../include/Paths.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/DisplayExecute.hpp"

int DispExecute( std::string cmd, std::string & err, bool show_output )
{
	std::array< char, 1024 > opline;

	std::string finalcmd = cmd + " 2>" + TMP_FILE;

	FILE * pipe = popen( finalcmd.c_str(), "r" );

	static int prevdisp = 0;

	if( !pipe )
		return false;

	while( !feof( pipe ) ) {
		if( fgets( opline.data(), 1024, pipe ) != NULL && show_output ) {
			MoveOutputCursorBack( prevdisp );
			std::string op = std::string( opline.data() );

			TrimString( op );

			std::string str = "[ " + op + " ]";
			std::cout << str;
			std::cout.flush();
			prevdisp = str.size();
		}
	}

	MoveOutputCursorBack( prevdisp );

	prevdisp = 0;

	err = "";

	std::fstream errfile;
	errfile.open( TMP_FILE, std::ios::in );
	if( errfile ) {
		std::string line;
		while( std::getline( errfile, line ) )
			err += line;
	}
	errfile.close();

	std::system( ( "rm -rf " + TMP_FILE ).c_str() );

	return pclose( pipe );
}

int DispExecuteWithCopyFileLocations( std::string cmd, std::string & err, std::vector< std::string > & files )
{
	std::array< char, 1024 > opline;

	std::string finalcmd = cmd + " 2>" + TMP_FILE + " | cut -d \" \" -f3 | cut -d \"'\" -f2";

	std::cout << "Executing: " << finalcmd << std::endl;

	FILE * pipe = popen( finalcmd.c_str(), "r" );

	if( !pipe )
		return false;

	while( !feof( pipe ) ) {
		if( fgets( opline.data(), 1024, pipe ) != NULL ) {
			std::string op = std::string( opline.data() );

			TrimString( op );

			if( std::find( files.begin(), files.end(), op ) != files.end() )
				continue;
			
			files.push_back( op );
		}
	}

	err = "";

	std::fstream errfile;
	errfile.open( TMP_FILE, std::ios::in );
	if( errfile ) {
		std::string line;
		while( std::getline( errfile, line ) )
			err += line;
	}
	errfile.close();

	std::system( ( "rm -rf " + TMP_FILE ).c_str() );

	return pclose( pipe );
}
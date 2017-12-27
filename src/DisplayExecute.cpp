#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <memory>
#include <cstdio>
#include <algorithm>

#include "../include/Paths.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/DisplayExecute.hpp"

int DispExecute( std::string cmd, std::string & err )
{
	std::array< char, 1024 > opline;

	std::string finalcmd = cmd + " 2>" + TMP_FILE;

	FILE * pipe = popen( cmd.c_str(), "r" );

	static int prevdisp = 0;

	if( !pipe )
		return false;

	while( !feof( pipe ) ) {
		if( fgets( opline.data(), 1024, pipe ) != NULL ) {
			MoveOutputCursorBack( prevdisp );
			std::string op = std::string( opline.data() );
			while( * ( op.end() - 1 ) == '\n' ) {
				op.erase( op.end() - 1 );
			}
			std::replace( op.begin(), op.end(), '\n', '-' );
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
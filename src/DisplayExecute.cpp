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
	char opline[ 10000 ];

	std::string finalcmd = cmd + " 2>" + TMP_FILE;

	FILE * pipe = popen( finalcmd.c_str(), "r" );

	int prevdisp = 0;

	if( !pipe )
		return false;

	while( !feof( pipe ) ) {
		if( fgets( opline, sizeof opline, pipe ) != NULL && show_output ) {
			std::cout << std::strlen( opline ) << ": " << opline << std::endl;
			MoveOutputCursorBack( prevdisp );
			std::string op = std::string( opline );

			TrimString( op );

			prevdisp = DisplayOneLinerString( op );
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

int DispExecuteNoErr( std::string cmd, bool show_output )
{
	std::string temperr;

	return DispExecute( cmd, temperr, show_output );
}
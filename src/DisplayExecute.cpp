#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <array>
#include <memory>
#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../include/Paths.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/DisplayExecute.hpp"

int DispExecute( std::string cmd, std::string & err, bool show_output )
{
	std::string finalcmd = cmd + " 2>" + TMP_FILE;

	FILE * pipe = popen( finalcmd.c_str(), "r" );

	int prevdisp = 0;

	if( !pipe )
		return false;

	winsize w;
	ioctl( STDOUT_FILENO, TIOCGWINSZ, & w );
	int term_width = w.ws_col;

	std::array< char, 10000 > opline_temp;

	while( !feof( pipe ) ) {
		if( fgets( opline_temp.data(), 10000, pipe ) != NULL && show_output ) {

			if( strlen( opline_temp.data() ) > term_width )
				continue;

			MoveOutputCursorBack( prevdisp );
			std::string opline = std::string( opline_temp.data() );

			TrimString( opline );

			prevdisp = DisplayOneLinerString( opline );
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
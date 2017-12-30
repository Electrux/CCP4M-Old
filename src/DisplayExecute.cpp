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

	int current_disp_len = GetLastDispLen();

	std::array< char, 10000 > opline_temp;

	while( !feof( pipe ) ) {
		if( fgets( opline_temp.data(), 10000, pipe ) != NULL && show_output ) {

			std::string opline = std::string( opline_temp.data() );

			TrimString( opline );

			if( current_disp_len + ( "[ " + opline + " ]" ).size() >= term_width ) {
				std::string teststr = "[  ]";
				opline = opline.substr( 0, term_width - current_disp_len - teststr.size() - 4 );
				opline += " ...";
			}

			MoveOutputCursorBack( prevdisp );

			prevdisp = DisplayOneLinerString( opline );
		}
	}

	MoveOutputCursorBack( prevdisp );

	err = "";

	std::fstream errfile;
	errfile.open( TMP_FILE, std::ios::in );
	if( errfile ) {
		std::string line;
		while( std::getline( errfile, line ) )
			err += line;
	}
	errfile.close();

	if( !err.empty() )
		std::system( ( "rm -rf " + TMP_FILE ).c_str() );

	return pclose( pipe );
}

int DispExecuteNoErr( std::string cmd, bool show_output )
{
	std::string temperr;

	return DispExecute( cmd, temperr, show_output );
}
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

	int current_disp_len = GetLastDispLen();

	std::array< char, 10000 > opline_temp;

	std::string brackets = "[  ]";
	std::string continuation = " ...";

	winsize w;
	int term_width;

	while( !feof( pipe ) ) {
		if( fgets( opline_temp.data(), 10000, pipe ) != NULL && show_output ) {

			std::string opline = std::string( opline_temp.data() );

			TrimString( opline );

			ioctl( STDOUT_FILENO, TIOCGWINSZ, & w );
			// To accomodate \n, max column usable is ws_col - 1
			term_width = w.ws_col - 1;

			if( current_disp_len + brackets.size() + opline.size() >= term_width ) {
				// the size to work with is:
				// term_width -> total size to work with
				// - current_disp_len -> already used by previous displayed line
				// - teststr.size() -> to account for the additional brackets put by DisplayOneLinerString()
				// - 4 -> to account for adding 3 dots and a space to describe left out output.
				if( term_width - current_disp_len - ( int )brackets.size() - ( int )continuation.size() < 0 )
					continue;

				opline = opline.substr( 0, term_width - current_disp_len - brackets.size() - continuation.size() );
				opline += continuation;
			}

			if( !opline.empty() && * opline.begin() == ' ' )
				opline.erase( opline.begin() );

			MoveOutputCursorBack( prevdisp );

			prevdisp = DisplayOneLinerString( opline );
		}
	}

	MoveOutputCursorBack( prevdisp );

	err = "";

	bool iserrfile = false;
	std::fstream errfile;
	errfile.open( TMP_FILE, std::ios::in );
	if( errfile ) {
		iserrfile = true;
		std::string line;
		while( std::getline( errfile, line ) )
			err += line;
	}
	errfile.close();

	if( iserrfile )
		std::system( ( "rm -rf " + TMP_FILE ).c_str() );

	return pclose( pipe );
}

int DispExecuteNoErr( std::string cmd, bool show_output )
{
	std::string temperr;

	return DispExecute( cmd, temperr, show_output );
}
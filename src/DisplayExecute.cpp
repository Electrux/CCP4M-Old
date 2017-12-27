#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <cstdio>

#include "../include/DisplayFuncs.hpp"

#include "../include/DisplayExecute.hpp"

int DispExecute( std::string cmd )
{
	std::array< char, 1024 > opline;

	FILE * pipe = popen( cmd.c_str(), "r" );

	static int prevdisp = 0;

	if( !pipe )
		return false;

	while( !feof( pipe ) ) {
		if( fgets( opline.data(), 1024, pipe ) != NULL ) {
			MoveOutputCursorBack( prevdisp );
			std::string op = std::string( opline.data() );
			if( * ( op.end() - 1 ) == '\n' ) {
				op.erase( op.end() - 1 );
			}
			std::string str = "[ " + op + " ]";
			std::cout << str;
			std::cout.flush();
			prevdisp = str.size();
		}
	}

	MoveOutputCursorBack( prevdisp );

	prevdisp = 0;

	return pclose( pipe );
}
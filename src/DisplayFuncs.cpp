#include <iostream>
#include <string>

#include "../include/ColorDefs.hpp"

#include "../include/DisplayFuncs.hpp"

void MoveOutputCursorBack( int len )
{
	for( int i = 0; i < len; ++i ) {
		std::cout << "\b \b";
	}
	std::cout.flush();
}

int DisplayOneLinerString( const std::string & str )
{
	std::string output = CYAN + "[ " + MAGENTA + str + CYAN + " ]" + RESET;

	std::cout << output;
	std::cout.flush();

	return ( "[ " + str + " ]" ).size();
}
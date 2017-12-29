#include <iostream>
#include <string>

#include "../include/ColorDefs.hpp"

#include "../include/DisplayFuncs.hpp"

void MoveOutputCursorBack( int & len )
{
	for( int i = 0; i < len; ++i ) {
		std::cout << "\b \b";
	}
	std::cout.flush();

	std::cout << "moved back " << len << std::endl;
}

int DisplayOneLinerString( const std::string & str )
{
	if( str.empty() )
		return 0;

	std::string output = BOLD_CYAN + "[ " + BOLD_MAGENTA + str + BOLD_CYAN + " ]" + RESET;

	std::cout << output;
	std::cout.flush();

	return ( "[ " + str + " ]" ).size();
}
#include <iostream>

#include "../include/DisplayFuncs.hpp"

void MoveOutputCursorBack( int len )
{
	for( int i = 0; i < len; ++i ) {
		std::cout << "\b \b";
	}
	std::cout.flush();
}
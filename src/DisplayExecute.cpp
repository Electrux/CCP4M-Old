#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <cstdio>

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
			MoveBack( prevdisp );
			std::string str = "[ " + std::string( opline.data() ) + " ]";
			std::cout << str;
			std::cout.flush();
			prevdisp = str.size();
		}
	}

	MoveBack( prevdisp );

	return pclose( pipe );
}

void MoveBack( int len )
{
	for( int i = 0; i < len; ++i ) {
		std::cout << "\b \b";
	}
}
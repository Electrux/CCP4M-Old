#include <iostream>
#include <string>

#include "../include/ColorDefs.hpp"

#include "../include/DisplayFuncs.hpp"

const std::string FIRST_COL = YELLOW;
const std::string SECOND_COL = CYAN;
const std::string THIRD_COL = YELLOW;
const std::string EXTRA_COL = MAGENTA;

static int last_disp_len = 0;

void MoveOutputCursorBack( int & len )
{
	for( int i = 0; i < len; ++i ) {
		std::cout << "\b \b";
	}
	std::cout.flush();

	len = 0;
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

int GetLastDispLen()
{
	return last_disp_len;
}

void DispColoredData( std::string data, const std::string col, bool endl )
{
	last_disp_len += data.size();

	std::cout << col << data << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}
	else {
		std::cout.flush();
	}
}

void DispColoredData( std::string data, std::string data2, const std::string col1, const std::string col2, bool endl )
{
	// + 1 because of space
	last_disp_len += data.size() + data2.size() + 1;

	std::cout << col1 << data << " " << col2 << data2 << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}
	else {
		std::cout.flush();
	}
}

void DispColoredData( std::string data, std::string data2, std::string data3,
		const std::string col1, const std::string col2, const std::string col3, bool endl )
{
	// + 2 because of spaces
	last_disp_len += data.size() + data2.size() + data3.size() + 2;

	std::cout << col1 << data << " " << col2 << data2 << " " << col3 << data3 << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}
	else {
		std::cout.flush();
	}
}
#include <iostream>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../include/ColorDefs.hpp"

#include "../include/DisplayFuncs.hpp"

const std::string FIRST_COL = YELLOW;
const std::string SECOND_COL = CYAN;
const std::string THIRD_COL = YELLOW;
const std::string EXTRA_COL = MAGENTA;

static int last_disp_len = 0;

int GetWordsPerLine( const std::vector< std::string > & wordlist )
{
	int totalletters = 0;
	for( auto word : wordlist )
		totalletters += word.size();

	// + 1 to be on safe side.
	int avgwordlen = wordlist.empty() ? APPROX_WORD_LEN : ( ( totalletters / wordlist.size() ) + 1 );

	return GetTermWidth() / ( avgwordlen + GAP_PER_WORD.size() );
}

int GetTermWidth()
{
	winsize w;

	ioctl( STDOUT_FILENO, TIOCGWINSZ, & w );
	// To accomodate \n, max column usable is ws_col - 1
	return w.ws_col - 1;
}

void MoveOutputCursorBack( int & len )
{
	for( int i = 0; i < len; ++i )
		std::cout << "\b \b";

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

void DispColoredData( const std::string data, const std::string col, bool endl )
{
	last_disp_len += data.size();

	std::cout << col << data << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}

	std::cout.flush();
}

void DispColoredData( const std::string data, const std::string data2, const std::string col1, const std::string col2, bool endl )
{
	// + 1 because of space
	last_disp_len += data.size() + data2.size() + 1;

	std::cout << col1 << data << " " << col2 << data2 << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}

	std::cout.flush();
}

void DispColoredData( const std::string data, const std::string data2, const std::string data3,
		const std::string col1, const std::string col2, const std::string col3, bool endl )
{
	// + 2 because of spaces
	last_disp_len += data.size() + data2.size() + data3.size() + 2;

	std::cout << col1 << data << " " << col2 << data2 << " " << col3 << data3 << RESET;

	if( endl ) {
		last_disp_len = 0;
		std::cout << std::endl;
	}

	std::cout.flush();
}

void DispColoredDataLaterally( const std::vector< std::string > & data, const std::string col )
{
	int wpl = GetWordsPerLine( data );

	for( auto it = data.begin(); it != data.end(); ) {
		for( int i = 0; i < wpl; ++i ) {

			if( i == wpl - 1 || it == data.end() - 1 )
				DispColoredData( * it, col, true );
			else
				DispColoredData( * it + GAP_PER_WORD, col, false );

			++it;
		}
	}
}
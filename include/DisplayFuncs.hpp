#ifndef DISPLAYFUNCS_HPP
#define DISPLAYFUNCS_HPP

#include <string>
#include <vector>

static const int APPROX_WORD_LEN = 10;
static const std::string GAP_PER_WORD = "   ";

// These constants are default colors to be used when something
// is displayed on screen using DispColoredData().
extern const std::string FIRST_COL;
extern const std::string SECOND_COL;
extern const std::string THIRD_COL;
extern const std::string EXTRA_COL;

// Fetches the maximum number of words on a line of terminal.
// Uses average word count from wordlist.
int GetWordsPerLine( const std::vector< std::string > & wordlist );

// Fetches the current terminal width - 1.
// -1 to accomodate newline character.
int GetTermWidth();

// This function moves back the cursor of output terminal by "len" amount.
// It will clear all the data till the amount as well.
void MoveOutputCursorBack( int & len );

// This function displays OneLiners.
// Basically, OneLiners are fleeting outputs which are to be erased after they
// are displayed. These are always displayed in between square brackets.
int DisplayOneLinerString( const std::string & str );

// This fetches the length of everything written uptil the point of call,
// on output, on the current single line ( before newline ).
// Once a newline is encountered, the counter is reset to 0.
int GetLastDispLen();

// These functions are wrappers over std::cout, to display colors and manage the
// displen variable. displen variable is incremented by the size of data.
// But if endl is true, displen is reset to 0.
// The overloads provide multiple strings and respective colors support.
void DispColoredData( const std::string data, const std::string col = FIRST_COL, bool endl = false );

void DispColoredData( const std::string data, const std::string data2,
		const std::string col1 = FIRST_COL, const std::string col2 = SECOND_COL,
		bool endl = false );

void DispColoredData( const std::string data, const std::string data2, const std::string data3,
		const std::string col1 = FIRST_COL, const std::string col2 = SECOND_COL, const std::string col3 = THIRD_COL,
		bool endl = false );

// Displays data in a horizontal manner depending on how big is terminal screen ( laterally ).
void DispColoredDataLaterally( const std::vector< std::string > & data, const std::string col = FIRST_COL );

#endif // DISPLAYFUNCS_HPP
#ifndef DISPLAYFUNCS_HPP
#define DISPLAYFUNCS_HPP

#include <string>

extern const std::string FIRST_COL;
extern const std::string SECOND_COL;
extern const std::string THIRD_COL;

void MoveOutputCursorBack( int & len );

int DisplayOneLinerString( const std::string & str );

int GetLastDispLen();

void DispColoredData( std::string data, const std::string col = FIRST_COL, bool endl = false );

void DispColoredData( std::string data, std::string data2,
		const std::string col1 = FIRST_COL, const std::string col2 = SECOND_COL,
		bool endl = false );

void DispColoredData( std::string data, std::string data2, std::string data3,
		const std::string col1 = FIRST_COL, const std::string col2 = SECOND_COL, const std::string col3 = THIRD_COL,
		bool endl = false );

#endif // DISPLAYFUNCS_HPP
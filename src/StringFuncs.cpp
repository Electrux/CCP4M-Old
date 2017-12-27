#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "../include/StringFuncs.hpp"

std::vector< std::string > ToVector( int argc, char ** argv )
{
	std::vector< std::string > temp;

	for( int i = 0; i < argc; ++i )
		temp.push_back( argv[ i ] );

	return temp;
}

std::vector< std::string > DelimStringToVector( std::string str, char delim )
{
	std::string temp;
	std::vector< std::string > val;

	for( auto ch : str ) {

		if( ch == delim ) {
			val.push_back( temp );
			temp.clear();
			continue;
		}

		temp += ch;
	}

	if( !temp.empty() )
		val.push_back( temp );

	return val;
}


std::string GetStringBetweenQuotes( std::string & str )
{
	bool inquote = false;

	std::string ret;

	for( auto ch : str ) {

		if( ch == '\'' || ch == '\"' )
			inquote = !inquote;

		else if( inquote )
			ret += ch;
	}

	return ret;
}

std::string GetStringTillLastSlash( std::string & str )
{
	std::string temp;

	bool gotslash = false;

	for( auto ch = str.rbegin(); ch != str.rend(); ++ch ) {

		if( * ch == '/' )
			gotslash = true;

		if( gotslash )
			temp += * ch;
	}

	if( !temp.empty() )
		std::reverse( temp.begin(), temp.end() );

	return temp;
}

void StringToLower( std::string & str )
{
	std::transform( str.begin(), str.end(), str.begin(),
			[]( char c ) { return std::tolower( c ); } );
}

void TrimString( std::string & str )
{
	while( * ( str.end() - 1 ) == ' ' || * ( str.end() - 1 ) == '\n' || * ( str.end() - 1 ) == '\t' )
		str.erase( str.end() - 1 );

	while( * str.begin() == ' ' || * str.begin() == '\n' || * str.begin() == '\t' )
		str.erase( str.begin() );

	int spc_count = 0;

	for( auto it = str.begin(); it != str.end(); ) {
		if( * it == '\t' )
			*it = ' ';

		if( * it == ' ' ) {
			if( spc_count > 1 ) {
				it = str.erase( it );
				spc_count--;
			}
			else {
				spc_count++;
				++it;
			}
		}
		else if( * it == '\r' ) {
			it = str.erase( it );
		}
		else {
			++it;
		}
	}
}
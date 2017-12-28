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

std::string GetStringTillLastSlash( const std::string & str )
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

std::string GetStringAfterLastSlash( const std::string & str )
{
	std::string temp;

	bool gotslash = false;

	for( auto ch = str.rbegin(); ch != str.rend(); ++ch ) {

		if( * ch == '/' )
			gotslash = true;

		if( !gotslash )
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

	for( auto it = str.begin(); it != str.end(); ) {
		if( * it == '\t' ) {
			*it = ' ';
			++it;
		}
		else if( * it == ' ' ) {
			++it;
			while( *it == ' ' ) {
				it = str.erase( it );
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

bool IsWildCardsCompatible( const std::string & str, std::vector< std::string > & wildcards )
{
	for( auto card : wildcards ) {
		if( IsWildCardCompatible( str, card ) )
			return true;
	}

	return false;
}

bool IsWildCardCompatible( const std::string & str, std::string & wildcard )
{
	if( wildcard == "*" )
		return true;

	auto strit = str.begin();
	auto wcardit = wildcard.begin();

	while( strit != str.end() ) {
		if( * wcardit == * strit ) {
			wcardit++;
		}
		else if( * wcardit == '*' ) {
			if( * ( wcardit + 1 ) == * ( strit + 1 ) && wcardit + 1 != wildcard.end() )
				wcardit++;
			else if( * ( wcardit + 1 ) == * strit && wcardit + 2 != wildcard.end() )
				wcardit += 2;
		}
		else {
			break;
		}

		strit++;
	}

	while( * wcardit == '*' )
		wcardit++;

	if( strit == str.end() && wcardit == wildcard.end() )
		return true;

	return false;
}

void TrimWildCards( std::vector< std::string > & wildcards )
{
	for( auto wildcard = wildcards.begin(); wildcard != wildcards.end(); ++wildcard ) {
		TrimWildCard( * wildcard );
	}
}

void TrimWildCard( std::string & wildcard )
{
	for( auto it = wildcard.begin(); it != wildcard.end(); ) {
		if( * it == '*' ) {
			++it;

			while( * it == '*' )
				it = wildcard.erase( it );
		}
		++it;
	}
}
#ifndef COMMONFUNCS_HPP
#define COMMONFUNCS_HPP

#include <string>
#include <vector>

std::vector< std::string > ToVector( int argc, char ** argv );

std::vector< std::string > DelimStringToVector( std::string str, char delim = ',' );

std::string GetStringBetweenQuotes( std::string & str );

std::string GetStringTillLastSlash( const std::string & str );

std::string GetStringAfterLastSlash( const std::string & str );

void StringToLower( std::string & str );

void TrimString( std::string & str );

bool IsWildCardsCompatible( const std::string & str, std::vector< std::string > & wildcards );

bool IsWildCardCompatible( const std::string & str, std::string & wildcard );

void TrimWildCards( std::vector< std::string > & wildcards );

void TrimWildCard( std::string & wildcard );

#endif //COMMONFUNCS_HPP
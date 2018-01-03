#ifndef COMMONFUNCS_HPP
#define COMMONFUNCS_HPP

#include <string>
#include <vector>

// Converts arguments, in main function, to vector of strings.
std::vector< std::string > ArgToVector( int argc, char ** argv );

// Delimits string by delim character.
// Returns vector of delimited strings.
// If delim character is not found, original string
// is returned as the element in vector.
std::vector< std::string > DelimStringToVector( std::string str, char delim = ',' );

// Fetches string between " " or ' '
std::string GetStringBetweenQuotes( std::string & str );

// Fetches the string uptil last forward slash, including the slash.
std::string GetStringTillLastSlash( const std::string & str );

// Fetches the string from after the last slash. Not including the slash.
std::string GetStringAfterLastSlash( const std::string & str );

// Replace all occurrences of "from" to "to" in string and return it.
void ReplaceInString( std::string & str, const char from, const char to );

// Replace all occurrences of "from" to "to" in string and return it,
// but from and to are strings themselves here.
void ReplaceInString( std::string & str, const std::string from, const std::string to );

// Convert string to lowercase.
void StringToLower( std::string & str );

// Trims the string, removing all tabs, and newlines,
// and multiple spaces in continuation.
void TrimString( std::string & str );

// Check if the string str is compatible with any single wildcard string
// in the string vector.
// Returns true if it is compatible, false if it is not compatible with any.
bool IsWildCardsCompatible( const std::string & str, std::vector< std::string > & wildcards );

// Check if the string str is compatible with the wildcard.
// Returns true if it is, false if not.
bool IsWildCardCompatible( const std::string & str, std::string & wildcard );

// Trim all wildcards in vector by calling TrimWildCard( string ) on each.
void TrimWildCards( std::vector< std::string > & wildcards );

// Trims the wildcard by removing extra asterisks for example: "**"
void TrimWildCard( std::string & wildcard );

#endif //COMMONFUNCS_HPP
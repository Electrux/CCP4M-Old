#ifndef ASYNCEXECUTE_HPP
#define ASYNCEXECUTE_HPP

#include <string>

int DispExecute( std::string cmd, std::string & err, bool show_output = true );

int DispExecuteWithCopyFileLocations( std::string cmd, std::string & err, std::vector< std::string > & files );

#endif // ASYNCEXECUTE_HPP
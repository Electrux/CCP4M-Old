#ifndef ASYNCEXECUTE_HPP
#define ASYNCEXECUTE_HPP

#include <string>
#include <vector>

int DispExecute( std::string cmd, std::string & err, bool show_output = true );

int SimpleExecuteGetOutput( std::string cmd, std::string & op );

int DispExecuteNoErr( std::string cmd, bool show_output = true );

#endif // ASYNCEXECUTE_HPP
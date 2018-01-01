#ifndef ASYNCEXECUTE_HPP
#define ASYNCEXECUTE_HPP

#include <string>
#include <vector>

int DispExecute( std::string cmd, std::string & err, bool show_output = true, bool create_temp_file = true );

int DispExecuteNoErr( std::string cmd, bool show_output = true, bool create_temp_file = false );

#endif // ASYNCEXECUTE_HPP
#ifndef ASYNCEXECUTE_HPP
#define ASYNCEXECUTE_HPP

#include <string>
#include <vector>

// This function executes a command using pipe(), therefore,
// fetching the stdout of that command as well.
// Since the stderr cannot be fetched, it is redirected to a temporary file,
// whose contents is stored in the err variable once the execution is finished.
// show_output, if true, will display a one liner fleeting output ( line is cleared
// once the next output is encountered, and the last line is cleared too ) of the current
// stdout of the running command.
// create_temp_file, if true will create the temporary file to fetch the stderr of
// the command. If it is false, the stderr of the command will be redirected to stdout.
// Also takes a tmp_file_thread_extension so that if multiple threads call this function,
// they can append distinct string ( such as a counter ) to the TMP_FILE to avoid its
// corruption.
int DispExecute( std::string cmd, std::string & err,
		bool show_output = true,
		bool create_temp_file = true,
		std::string tmp_file_thread_extension = "" );

// Same as above, except this one won't contain any stderr support ( since no err variable ).
// No temporary file will be created by default as well.
int DispExecuteNoErr( std::string cmd, bool show_output = true, bool create_temp_file = false );

#endif // ASYNCEXECUTE_HPP
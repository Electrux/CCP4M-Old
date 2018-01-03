#ifndef COMPILECOMMANDDATA_HPP
#define COMPILECOMMANDDATA_HPP

#include <string>

// This file has a structure which is used when compiling
// using ExecuteCommand, to send information of what is being
// compiled ( othersource ), and the command to compile ( command ).

struct CCData
{
	std::string othersource;
	std::string command;
};

#endif // COMPILECOMMANDDATA_HPP

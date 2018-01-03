#ifndef PROJECTDATA_HPP
#define PROJECTDATA_HPP

#include <vector>
#include <string>

// This structure contains project's information
// which is in ccp4m.ini file.
struct ProjectData
{
	std::string name;
	std::string lang;
	std::vector< std::string > deps;
	std::string dir;
};

// DEFAULTMAINCPP and DEFAULTMAINC are the default main
// files which are generated when a project is created.
// If it's a C++ project, DEFAULTMAINCPP is used.
// Otherwise ( C ), DEFAULTMAINC is used.
const std::string DEFAULTMAINCPP =
	"#include <iostream>\n"
	"\n"
	"int main()\n"
	"{\n"
	"\treturn 0;\n"
	"}\n";
const std::string DEFAULTMAINC =
	"#include <stdio.h>\n"
	"\n"
	"int main()\n"
	"{\n"
	"\treturn 0;\n"
	"}\n";

// This function fetches project data from the ccp4m.ini file.
ProjectData GetProjectData( std::vector< std::string > & args );

#endif // PROJECTDATA_HPP

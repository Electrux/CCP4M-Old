#ifndef CONFIGMGR_HPP
#define CONFIGMGR_HPP

#include <string>

#include "../INI_System/INI_Parser.hpp"

#include "ProjectData.hpp"

// This file contains the Configuration Manager class for fetching configuration
// files of projects, as well as creating default ones if needed.

// It uses INI system to create and fetch configuration files.

class ConfigMgr
{
	ProjectData data;
	Electrux::INI_Parser parser;

public:

	// SetProject and GetProject are used to provide and retrieve information of
	// the project whose configuration needs to be created/retrieved.
	void SetProject( ProjectData & proj );
	ProjectData GetProject();

	// CreateDefaultConfig is the function which creates a default configuration
	// file for the project based on provided parameters when creating new project.
	// It takes in the project_dir parameter which specifies where the configuration
	// file is to be generated.
	// Returns 0 on success, anything else on failure.
	int CreateDefaultConfig( std::string project_dir );

	// RetrieveConfig fetches an already existing ccp4m.ini configuration file
	// and loads it in the INI_Parser to be used later on. Once the instance of the
	// class goes out of scope, all the data loaded up is lost too ( but the actual
	// saved data in file remains intact ).
	// Returns 0 on success, anything else on failure.
	int RetrieveConfig( std::string project_dir );

	// GetDataString is a wrapper over INI_Parser's GetDataString, to fetch
	// a value using it's section and key.
	// RetrieveConfig must be called before that to actually have some data which
	// is read from the file.
	// Returns the value on success, empty string on failure.
	std::string GetDataString( std::string section, std::string key );

	// These functions are all helper functions to correctly format and fetch the data represented
	// by the function names themselves. They maybe considered a specialization of GetDataString
	// function for specific cases.
	// Returns the value on success, empty string on failure.
	std::string GetLibraryLang( std::string lib );
	std::string GetIncludeFlags( std::string lib );
	std::string GetLibraryFlags( std::string lib );
	std::string GetLibraryVersion( std::string lib );

	// This function checks if the project language is compatible with library language.
	// The only possible case where a project is not compatible with library is when
	// the project is written in C, but the library is written in C++.
	// Returns true if they are compatible, false if not.
	bool IsCompatible( std::string projlang, std::string liblang );
};

#endif // CONFIGMGR_HPP

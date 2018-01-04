#include <iostream>
#include <string>
#include <vector>

#include "../../include/ColorDefs.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/INI_System/INI_Parser.hpp"

#include "../../include/Paths.hpp"
#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/PackageManagement/PackageConfig.hpp"

#include "../../include/ProjectManagement/ConfigMgr.hpp"

void ConfigMgr::SetProject( ProjectData & proj ) { data = proj; }
ProjectData ConfigMgr::GetProject() { return data; }

int ConfigMgr::CreateDefaultConfig( std::string project_dir )
{
	if( data.name.empty() )
		return 1;

	std::string concatlibs;

	for( auto lib : data.deps ) {
		if( !IsCompatible( data.lang, GetLibraryLang( lib ) ) ) {
			DispColoredData( "Warning: Library:", lib, "is written in C++, but the project is in C",
					RED, SECOND_COL, RED, true );
			DispColoredData( "Skipping the library...", RED, true );
			continue;
		}
		concatlibs += lib;
		concatlibs += ", ";
	}

	if( !concatlibs.empty() ) {
		concatlibs.erase( concatlibs.end() - 1 );
		concatlibs.erase( concatlibs.end() - 1 );
	}

	std::string standard = data.lang == "cpp" ? "c++14" : "c11";

	parser.CreateSection( "Core" );
	parser.SetDataString( "Core", "Name", data.name );
	parser.SetDataString( "Core", "Lang", data.lang );
	parser.SetDataString( "Core", "Version", "0.1" );
	parser.SetDataString( "Core", "Libs", concatlibs );
	parser.SetDataString( "Core", "OtherFlags", "" );
	parser.SetDataString( "Core", "Std", standard );
	parser.SetDataString( "Core", "MainSrc", "main." + data.lang );
	parser.SetDataString( "Core", "OtherSrc", "" );

	parser.SetDataString( "Core", "ExecEnv", "LD_LIBRARY_PATH=" + LIB_DIR_REPLACEMENT );

	for( auto lib : data.deps ) {
		if( !IsCompatible( data.lang, GetLibraryLang( lib ) ) )
			continue;

		parser.CreateSection( lib );
		parser.SetDataString( lib, "IncFlags", "-I" + INC_DIR_REPLACEMENT );
		parser.SetDataString( lib, "LibFlags", GetLibraryFlags( lib ) );
		parser.SetDataString( lib, "Version", GetLibraryVersion( lib ) );
	}

	DispColoredData( "Creating file: ", project_dir + "/ccp4m.ini", BLUE, MAGENTA, true );
	parser.SaveToFile( project_dir + "/ccp4m.ini" );

	return 0;
}

int ConfigMgr::RetrieveConfig( std::string project_dir )
{
	auto status = parser.LoadFromFile( project_dir + "/ccp4m.ini" );

	if( status != Electrux::SUCCESS )
		return 1;

	return 0;
}

std::string ConfigMgr::GetDataString( std::string section, std::string key )
{
	std::string temp;

	parser.GetDataString( section, key, temp );

	return temp;
}

std::string ConfigMgr::GetLibraryLang( std::string lib )
{
	Package pkg;

	if( !PackageConfig::GetPackage( lib, pkg ) ) {
		DispColoredData( "Package:", lib, "does not exist!",
				FIRST_COL, SECOND_COL, THIRD_COL, true );
		DispColoredData( "You must check library compatibility yourselves.", FIRST_COL, true );
		return "";
	}

	return pkg.lang;
}

std::string ConfigMgr::GetLibraryFlags( std::string lib )
{
	Package pkg;

	if( !PackageConfig::GetPackage( lib, pkg ) ) {
		DispColoredData( "Package:", lib, "does not exist!",
				FIRST_COL, SECOND_COL, THIRD_COL, true );
		DispColoredData( "You must enter the flags for it if there are.", FIRST_COL, true );
		return "";
	}

	std::string flags;

	flags = "-L" + LIB_DIR_REPLACEMENT + ", " + pkg.libflags;

	return flags;
}

std::string ConfigMgr::GetLibraryVersion( std::string lib )
{
	Package pkg;

	if( !PackageConfig::GetPackage( lib, pkg ) ) {
		return "";
	}

	return pkg.version;
}

bool ConfigMgr::IsCompatible( std::string projlang, std::string liblang )
{
	if( projlang == "c" && liblang == "cpp" ) {
		return false;
	}

	return true;
}
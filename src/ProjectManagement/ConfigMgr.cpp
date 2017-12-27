#include <iostream>
#include <string>
#include <vector>

#include "../../include/ColorDefs.hpp"
#include "../../include/CommonFuncs.hpp"
#include "../../include/ProjectManagement/ProjectData.hpp"
#include "../../include/INI_System/INI_Parser.hpp"

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
			std::cout << RED << "Warning: Library: " << lib << " is written in C++, but the project is in C."
				<< std::endl << "Skipping the library..." << RESET << std::endl;
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

	for( auto lib : data.deps ) {
		if( !IsCompatible( data.lang, GetLibraryLang( lib ) ) )
			continue;

		parser.CreateSection( lib );
		parser.SetDataString( lib, "IncFlags", GetIncludeFlags( lib ) );
		parser.SetDataString( lib, "LibFlags", GetLibraryFlags( lib ) );
		parser.SetDataString( lib, "Version", GetLibraryVersion( lib ) );
	}

	std::cout << BLUE << "Creating file: " << MAGENTA << project_dir + "/ccp4m.ini" << RESET << std::endl;
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
		std::cout << "Package " << lib << " does not exist!" << std::endl
			<< "You must check library compatibility yourselves." << std::endl;
		return "";
	}

	return pkg.lang;
}

std::string ConfigMgr::GetIncludeFlags( std::string lib )
{
	Package pkg;

	if( !PackageConfig::GetPackage( lib, pkg ) ) {
		std::cout << "Package " << lib << " does not exist!" << std::endl
			<< "You must enter the flags for it if there are." << std::endl;
		return "";
	}

	std::vector< std::string > incdirs = DelimStringToVector( pkg.incdir );

	std::string incflag;
	for( auto incdir : incdirs ) {
		incflag += "-I" + incdir + ",";
	}

	if( !incflag.empty() )
		incflag.erase( incflag.end() - 1 );

	return incflag;
}

std::string ConfigMgr::GetLibraryFlags( std::string lib )
{
	Package pkg;

	if( !PackageConfig::GetPackage( lib, pkg ) ) {
		std::cout << "Package " << lib << " does not exist!" << std::endl
			<< "You must enter the flags for it if there are." << std::endl;
		return "";
	}

	std::string flags;

	std::vector< std::string > libdirs = DelimStringToVector( pkg.libdir );

	std::string libflag;
	for( auto incdir : libdirs ) {
		libflag += "-L" + incdir + ",";
	}

	flags = libflag + pkg.libflags;

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
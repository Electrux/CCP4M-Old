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
		concatlibs += lib;
		concatlibs += ", ";
	}

	if( !concatlibs.empty() ) {
		concatlibs.erase( concatlibs.end() - 1 );
		concatlibs.erase( concatlibs.end() - 1 );
	}

	parser.CreateSection( "Core" );
	parser.SetDataString( "Core", "Name", data.name );
	parser.SetDataString( "Core", "Version", "0.1" );
	parser.SetDataString( "Core", "Libs", concatlibs );
	parser.SetDataString( "Core", "OtherFlags", "" );
	parser.SetDataString( "Core", "Std", "c++14" );
	parser.SetDataString( "Core", "MainSrc", "main.cpp" );
	parser.SetDataString( "Core", "OtherSrc", "" );

	for( auto lib : data.deps ) {
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
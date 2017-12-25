#include <iostream>
#include <string>

#include "../include/ColorDefs.hpp"
#include "../include/PackageManagement/PackageData.hpp"
#include "../include/PackageManagement/PackageConfig.hpp"
#include "../include/PackageManager.hpp"

int PackageManager::InstallPackage( std::vector< std::string > & args, std::string package )
{
	Package pkg;

	if( !PackageConfig::GetPackage( package, pkg ) ) {
		std::cerr << "Error: Package does not exist!" << std::endl
			<< "Perhaps try to update package list using:" << std::endl
			<< "\n\t" << args[ 0 ] << " pkg update\n" << std::endl;
		return 1;
	}


}

//bool PackageManager::UninstallPackage( std::string package );

//bool PackageManager::Search( std::string package );
#include <iostream>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"

#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayExecute.hpp"

#include "../../include/PackageManagement/ArchiveExtractor.hpp"

bool ExtractArchive( const Package & pkg )
{
	std::string archive = PACKAGE_TMP + pkg.file;

	std::string taroptions = GetTarOptions( pkg.file );
	if( taroptions.empty() ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error: Unknown archive format! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	std::string archivedir = GetPackageVersionDir( pkg );

	if( LocExists( archivedir ) ) {
		DispExecuteNoErr( "rm -rf " + archivedir, false );
	}

	if( !CreatePackageDir( pkg ) ) {
		std::cout << RED << CROSS << std::endl;
		return false;
	}

	std::string cmd = "tar --strip 1 " + taroptions + " " + archive + " -C " + archivedir;

	if( DispExecuteNoErr( cmd, false ) != 0 ) {
		std::cout << RED << CROSS << std::endl;
		std::cout << RED << "Error: Unable to extract archive! Exiting!"
			<< RESET << std::endl;
		return false;
	}

	std::cout << GREEN << TICK << std::endl;

	return true;
}

std::string GetTarOptions( const std::string & filename )
{
	if( filename.find( ".tar.gz" ) != std::string::npos )
		return "-xzf";

	if( filename.find( ".tar.bz2" ) != std::string::npos )
		return "-xjf";

	if( filename.find( ".tar" ) != std::string::npos )
		return "-xf";

	return "";
}
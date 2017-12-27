#include <string>

#include "PackageManagement/PackageData.hpp"

class PackageManager
{
	std::vector< std::string > & args;
public:
	PackageManager( std::vector< std::string > & _args );

	int HandleCommand();

	int InstallPackage( std::string package );

	int UninstallPackage( std::string package );

	int GetInfo( std::string package );

	int Update();

	int GetDependencyInfo( std::string package );

	std::string GetDependencyFlags( std::string package );

	bool PackageExists( std::string package, Package & pkg );

	bool IsInstalled( std::string packge );
};
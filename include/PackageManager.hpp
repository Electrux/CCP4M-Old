#include <string>

#include "PackageManagement/PackageData.hpp"

class PackageManager
{
	std::vector< std::string > & args;
public:
	PackageManager( std::vector< std::string > & _args );

	int HandleCommand();

	int InstallMultiplePackages();

	int InstallMultiplePackages( std::vector< std::string > & packages, bool forceinstall = false );

	int InstallPackage( std::string package, bool forceinstall = false );

	int UninstallPackage( std::string package );

	int Update();

	int GetDependencyInfo( std::string package );

	std::string GetDependencyFlags( std::string package );

	bool RemoveTempFiles( const Package & pkg, bool allfiles = false );

	bool RemoveInstalledEntry( const Package & pkg );

	bool PackageExists( std::string package, Package & pkg );

	// 0 - Yes!
	// 1 - No!
	// -1 - Unmanageable ( other )
	int IsInstalled( std::string packge );

	int GetInfo( std::string package );
};
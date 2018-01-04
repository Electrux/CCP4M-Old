#include <string>

#include "PackageManagement/PackageData.hpp"

class PackageManager
{
	// The arguments given to the application.
	std::vector< std::string > & args;
public:
	// Constructor with setups the argument reference.
	PackageManager( std::vector< std::string > & _args );

	// This function is called if the subcommand of the main program is "pkg".
	// It then handles install, uninstall, info, update subcommmands of pkg.
	// Returns 0 on all good, anything else on something gone wrong.
	int HandleCommand();

	// Installing multiple packages is done by this function.
	// Basically, it calls InstallMultiplePackage( vector, bool ) after checking
	// if there exists a "--force" option.
	// Returns result of InstallMultiplePackages( vector, bool ).
	int InstallMultiplePackages();

	// Installs multiple packages ( packages ) by calling InstallPackage on each.
	// If any package fails to install, it will stop the process, neglecting the subsequent
	// packages.
	// Returns 0 if installed correctly, anything else if not.
	int InstallMultiplePackages( std::vector< std::string > & packages, bool forceinstall = false );

	// Uninstalls multiple packages by calling UninstallPackage on each.
	// If any package failes to uninstall, it will stop the process, neglecting the subsequent
	// packages.
	// Returns 0 if uninstalled correctly, anthing else if not.
	int UninstallMultiplePackages();

	// Installs a single package in the following manner:
	//	check if package exists -> check already installed ->
	//	fetch ( download ) package -> extract package ->
	//	build if source package ( and install if install is given in buildcommands )
	//	-> install package ( if install buildcommand is not given ) ->
	//	remove remaining files which are not required.
	// forceinstall, if true, will cause the package to be installed even if it already is.
	// Returns 0 on success, anything else if not.
	int InstallPackage( std::string package, bool forceinstall = false );

	// Uninstalls package in the following manner:
	//	check package exists -> check if it's installed ->
	//	perform uninstallation -> remove temporary files.
	// Returns 0 on success, anything else if not.
	int UninstallPackage( std::string package );

	// Updates the package list by calling UpdatePackageList().
	// Returns 0 on success, anything else if not.
	int Update();

	// Fetches dependency information of a package.
	// TODO
	int GetDependencyInfo( std::string package );

	// Fetches flags of dependency.
	// TODO
	std::string GetDependencyFlags( std::string package );

	// Removes temporary archive files of packages once they are installed
	// or uninstalled.
	// if allfiles is true, it will remove the extracted directory as well.
	// Returns true on success, false if not.
	bool RemoveTempFiles( const Package & pkg, bool allfiles = false );

	// Checks if there is an installed entry of a package in INSTALLED_PKGS file.
	bool InstallEntryExists( const Package & pkg );

	// Removes the installed entry of a package from INSTALLED_PKGS file.
	// Returns true on success, false if not.
	bool RemoveInstallEntry( const Package & pkg );

	// Checks if package exists in package list.
	// Returns true on success, along with package information
	// in pkg, otherwise returns false.
	bool PackageExists( std::string package, Package & pkg );

	// Checks if a package is installed on the system.
	// Return values are:
	//	0 - Yes!
	//	1 - No!
	//	-1 - Unmanageable -> package is installed from somewhere else.
	int IsInstalled( std::string packge );

	// Displays the information of a package.
	// Returns 0 on success, anything else if not.
	int GetInfo( std::string package );
};
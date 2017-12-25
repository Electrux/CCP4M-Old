#include <string>

class PackageManager
{
public:
	int InstallPackage( std::vector< std::string > & args, std::string package );

	int UninstallPackage( std::string package );

	int Search( std::string package );

	int Update();
};
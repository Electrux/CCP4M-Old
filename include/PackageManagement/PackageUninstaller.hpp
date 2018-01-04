#ifndef PACKAGEUNINSTALLER_HPP
#define PACKAGEUNINSTALLER_HPP

#include "PackageData.hpp"

// This function uninstalls an installed package.
// It will remove all copied files if there was no automatic installation/uninstallation
// method provided in package ( like make install/uninstall ).
// Otherwise, it will execute the uninstall commands and let them do the job.
// The input args is also provided as vector to display error messages.
// Returns true on success, false on failure.
bool UninstallArchive( const Package & pkg );

// Some packages provide install_manifests which contains files to remove,
// instead of providing actual "make uninstall" command.
// This function handles that should "make uninstall" fail.
bool UninstallUsingInstallManifest( const Package & pkg );

#endif // PACKAGEUNINSTALLER_HPP
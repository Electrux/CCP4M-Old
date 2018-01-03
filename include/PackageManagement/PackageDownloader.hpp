#ifndef PACKAGEDOWNLOADER_HPP
#define PACKAGEDOWNLOADER_HPP

#include <string>

#include "PackageData.hpp"

// This file contains functions for helping in downloading packages.
// Downloading is done by using LibCurl.

// FetchPackage is the function which actually downloads the package.
bool FetchPackage( const Package & pkg );

// curl_write_data and progress_func are helper functions for LibCurl
// for displaying information of the download progress.
static size_t curl_write_func( void *ptr, size_t size, size_t nmemb, void *stream );
int progress_func( void* ptr, double totdl, double cdl, double totup, double cup );

#endif // PACKAGEDOWNLOADER_HPP
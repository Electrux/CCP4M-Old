#ifndef PACKAGEDOWNLOADER_HPP
#define PACKAGEDOWNLOADER_HPP

#include <string>

#include "PackageData.hpp"

bool FetchPackage( const Package & pkg );

static size_t curl_write_func( void *ptr, size_t size, size_t nmemb, void *stream );

int progress_func( void* ptr, double totdl, double cdl, double totup, double cup );

#endif // PACKAGEDOWNLOADER_HPP
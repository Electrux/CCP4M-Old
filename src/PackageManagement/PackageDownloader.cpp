#include <iostream>
#include <string>
#include <curl/curl.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/Paths.hpp"
#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/PackageManagement/PackageConfig.hpp"

#include "../../include/PackageManagement/PackageDownloader.hpp"

bool FetchPackage( const Package & pkg )
{
	if( !PackageConfig::HandlePkgDirs() )
		return false;

	CURLcode ret;
	CURL * hnd;

	hnd = curl_easy_init();

	curl_easy_setopt( hnd, CURLOPT_URL, ( pkg.url + pkg.file ).c_str() );
	curl_easy_setopt( hnd, CURLOPT_FAILONERROR, 1L );
	curl_easy_setopt( hnd, CURLOPT_MAXREDIRS, 50L );
	curl_easy_setopt( hnd, CURLOPT_HTTP_VERSION, ( long )CURL_HTTP_VERSION_2TLS );
	curl_easy_setopt( hnd, CURLOPT_TCP_KEEPALIVE, 1L );
	curl_easy_setopt( hnd, CURLOPT_FILE, ( PACKAGE_TMP + pkg.file ).c_str() );
	curl_easy_setopt( hnd, CURLOPT_SERVER_RESPONSE_TIMEOUT, 10L );

	ret = curl_easy_perform( hnd );

	curl_easy_cleanup( hnd );

	hnd = NULL;

	if( ( int )ret != 0 ) {
		std::cout << RED << "Error: Failed to download package: " << YELLOW << pkg.name
			<< RESET << "\n" << MAGENTA << "LibCurl Error: "
			<< BLUE << curl_easy_strerror( ret ) << RESET << std::endl;
	}

	return !( bool )( int )ret;
}
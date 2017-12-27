#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cstdio>

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
	if( !hnd )
		return false;
	
	std::FILE * file;

	curl_easy_setopt( hnd, CURLOPT_URL, ( pkg.url + pkg.file ).c_str() );
	file = std::fopen( ( PACKAGE_TMP + pkg.file ).c_str(), "w" );

	curl_easy_setopt( hnd, CURLOPT_WRITEDATA, file );
	curl_easy_setopt( hnd, CURLOPT_WRITEFUNCTION, curl_write_func );
	curl_easy_setopt( hnd, CURLOPT_NOPROGRESS, 0 );
	curl_easy_setopt( hnd, CURLOPT_PROGRESSFUNCTION, progress_func );

	curl_easy_setopt( hnd, CURLOPT_FAILONERROR, 1L );
	curl_easy_setopt( hnd, CURLOPT_USERAGENT, "curl/7.54.0" );
	curl_easy_setopt( hnd, CURLOPT_MAXREDIRS, 50L );
	curl_easy_setopt( hnd, CURLOPT_HTTP_VERSION, ( long )CURL_HTTP_VERSION_2TLS );
	curl_easy_setopt( hnd, CURLOPT_TCP_KEEPALIVE, 1L );
	curl_easy_setopt( hnd, CURLOPT_SERVER_RESPONSE_TIMEOUT, 10L );

	ret = curl_easy_perform( hnd );

	std::fclose( file );

	curl_easy_cleanup( hnd );

	hnd = NULL;

	if( ( int )ret != 0 ) {
		std::cout << RED << "Error: Failed to download package: " << YELLOW << pkg.name
			<< RESET << "\n" << MAGENTA << "LibCurl Error: "
			<< BLUE << curl_easy_strerror( ret ) << RESET << std::endl;
	}

	return !( bool )( int )ret;
}

static size_t curl_write_func( void *ptr, size_t size, size_t nmemb, void *stream )
{
	size_t written = fwrite( ptr, size, nmemb, ( FILE * )stream );
	return written;
}

int progress_func( void* ptr, double totdl, double cdl, double totup, double cup )
{
	if( totdl <= 0.0 )
		return 0;

	double percentdown = ( cdl / totdl ) * 100;

	static int prevpercentsize = 0;

	std::string percent = "[ " + std::to_string( percentdown ) + "% ]";

	for( int i = 0; i < prevpercentsize; ++i ) {
		std::cout << "\b \b";
		std::cout.flush();
	}

	std::cout << CYAN << percent << RESET;
	std::cout.flush();

	prevpercentsize = ( int )percent.size();

	return 0;
}
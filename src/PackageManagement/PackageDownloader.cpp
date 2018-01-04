#include <iostream>
#include <string>
#include <curl/curl.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../../include/ColorDefs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/Paths.hpp"
#include "../../include/DisplayExecute.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"

#include "../../include/PackageManagement/PackageData.hpp"
#include "../../include/PackageManagement/PackageConfig.hpp"

#include "../../include/PackageManagement/PackageDownloader.hpp"

static int prevpercentsize = 0;

bool FetchPackage( const Package & pkg )
{
	if( !PackageConfig::HandlePkgDirs() )
		return false;

	CURLcode ret;
	CURL * hnd;

	std::FILE * file;

	if( pkg.file.empty() ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: No package file exists for the current architecture!", CROSS, RED, RED, true );
		return false;
	}

	std::string archive = PACKAGE_TMP + pkg.file;

	if( LocExists( archive ) && DispExecuteNoErr( "touch " + archive, false ) != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: You do not have correct permissions!", CROSS, RED, RED, true );
		return false;
	}

	DispExecuteNoErr( "rm " + archive, false );

	hnd = curl_easy_init();
	if( !hnd )
		return false;

	curl_easy_setopt( hnd, CURLOPT_URL, ( pkg.url + pkg.file ).c_str() );
	file = std::fopen( archive.c_str(), "w" );

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

	MoveOutputCursorBack( prevpercentsize );

	if( ( int )ret != 0 ) {
		DispColoredData( CROSS, RED, true );
		DispColoredData( "Error: Failed to download package:", pkg.name, RED, SECOND_COL, true );
		DispColoredData( "LibCurl Error:", curl_easy_strerror( ret ), RED, BLUE, true );
		return false;
	}

	DispColoredData( TICK, GREEN, true );

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

	winsize w;
	int term_width;

	ioctl( STDOUT_FILENO, TIOCGWINSZ, & w );
	// To accomodate \n, max column usable is ws_col - 1
	term_width = w.ws_col - 1;

	double percentdown = ( cdl / totdl ) * 100;

	std::string percent = std::to_string( percentdown ) + "%";

	int current_disp_len = GetLastDispLen();

	std::string brackets = "[  ]";
	std::string continuation = " ...";

	if( current_disp_len + brackets.size() + percent.size() >= term_width ) {
		// the size to work with is:
		// term_width -> total size to work with
		// - current_disp_len -> already used by previous displayed line
		// - brackets.size() -> to account for the additional brackets put by DisplayOneLinerString()
		// - continuation.size() -> to account for adding 3 dots and a space to describe left out output.
		if( term_width - current_disp_len - ( int )brackets.size() - ( int )continuation.size() < 0 )
			return 0;

		percent = percent.substr( 0, term_width - current_disp_len - brackets.size() - continuation.size() );
		percent += continuation;
	}

	if( !percent.empty() && * percent.begin() == ' ' )
		percent.erase( percent.begin() );

	MoveOutputCursorBack( prevpercentsize );
	prevpercentsize = DisplayOneLinerString( percent );

	return 0;
}
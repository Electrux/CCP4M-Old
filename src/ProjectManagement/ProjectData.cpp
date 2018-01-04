#include <iostream>
#include <vector>
#include <string>

#include "../../include/ColorDefs.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/ProjectManagement/ProjectData.hpp"

ProjectData GetProjectData( std::vector< std::string > & args )
{
	ProjectData data;

	bool invalidformat = false;
	bool founddir = false;
	bool founddeps = false;
	bool foundname = false;
	bool foundlang = false;

	for( auto arg = args.begin() + 3; arg != args.end(); ++arg ) {

		if( arg->find( "--" ) != std::string::npos ) {

			if( ( arg + 1 ) == args.end() ||
			    ( arg + 1 )->find( "--" ) != std::string::npos ) {

				DispColoredData( "Error: Invalid format!", RED, true );
				DispColoredData( "Please use:", FIRST_COL, true );
				DispColoredData( "\t", args[ 0 ] + " project new help", FIRST_COL, SECOND_COL, true );
				DispColoredData( "for more information.", FIRST_COL, true );
				invalidformat = true;
				break;
			}

			if( arg->find( "dir" ) != std::string::npos && !founddir ) {
				++arg;
				data.dir = * arg;
				founddir = true;
			}
			else if( arg->find( "deps" ) != std::string::npos && !founddeps ) {
				++arg;
				data.deps = DelimStringToVector( * arg, ',' );

				if( data.deps.empty() ) {
					DispColoredData( "Error: No dependencies specified, however the", "--deps", "flag was used!",
							RED, SECOND_COL, RED, true );
					invalidformat = true;
					break;
				}
			}
			else if( arg->find( "lang" ) != std::string::npos && !foundlang ) {
				++arg;
				data.lang = *arg;
				foundlang = true;
			}
			else {
				DispColoredData( "Error: Invalid flag or repetiton occurred.", RED, true );
				DispColoredData( "Please use:", FIRST_COL, true );
				DispColoredData( "\t", args[ 0 ] + " project new help", FIRST_COL, SECOND_COL, true );
				DispColoredData( "for more information.", FIRST_COL, true );
				invalidformat = true;
				break;
			}
		}
		else {
			if( foundname ) {
				DispColoredData( "Error: Invalid flag or repetiton occurred.", RED, true );
				DispColoredData( "Please use:", FIRST_COL, true );
				DispColoredData( "\t", args[ 0 ] + " project new help", FIRST_COL, SECOND_COL, true );
				DispColoredData( "for more information.", FIRST_COL, true );
				invalidformat = true;
				break;
			}

			data.name = * arg;
			foundname = true;
		}
	}

	if( !founddir )
		data.dir = ".";

	if( !foundlang )
		data.lang = "cpp";

	if( invalidformat ) {

		if( data.name.empty() )
			DispColoredData( "Error: Cannot have unnamed project!", RED, true );

		data.name.clear();
		data.deps.clear();
		data.dir.clear();
	}

	return data;
}

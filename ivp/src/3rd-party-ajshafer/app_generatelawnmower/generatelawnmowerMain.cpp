/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT Cambridge                                   */
/*    FILE: generatelawnmowerMain.cpp                       */
/*    DATE: 1 NOV 2007                                      */
/************************************************************/

#include <iostream>
#include <sstream>
#include <vector>
#include "MBUtils.h"
#include "XYPolygon.h"
#include "IO_GeomUtils.h"
#include "ArtifactUtils.h"

void help_message();

/// ArtfieldGenerator creates random artifact fields for testing purposes.

/// It reads its configuration information from the command line and
/// outputs the artifactfield to stdout.
/// \example config strig: label,A:-60,-40:50,10:80,-40:-40,-80
/// \example usage: artfieldgenerator label,A:-60,-40:50,10:80,-40:-40,-80 .25 25 > mines.art

int main(int argc, char *argv[])
{
	// argv[0] = program name
	// argv[1] = file to read polygons from
	// argv[2] = file to put poly/seglist into
	// Read in the polygons
	// generate lawnmower pattern
	// Output to files
	if (argc != 3){ help_message(); return 1; };
	
	
	std::vector<XYPolygon> vecPolys = readPolysFromFile(argv[1]);
	
	std::vector<XYPolygon>::iterator p;
	
	for(p = vecPolys.begin(); p != vecPolys.end(); p++) {
		std::cout << p->get_spec();
	};
}



/// help_message() prints the usage instructions for the program
void help_message()
{
	using namespace std;

	cout << endl;
	cout << "Usage:  generatelawnmower" << endl;
	cout << "generatelawnmower input_file output_file_header" << endl;
	cout << endl;
}


/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: artfieldgeneratorMain.cpp                       */
/*    DATE: 3 JUL 2007                                      */
/************************************************************/

#include <iostream>
#include <sstream>
#include "MBUtils.h"
#include "XYPolygon.h"
#include "ArtifactUtils.h"
#include <algorithm>

void help_message();

/// ArtfieldGenerator creates random artifact fields for testing purposes.

/// It reads its configuration information from the command line and
/// outputs the artifactfield to stdout.
/// \example config strig: label,A:-60,-40:50,10:80,-40:-40,-80
/// \example usage: artfieldgenerator label,A:-60,-40:50,10:80,-40:-40,-80 .25 25 > mines.art

int main(int argc, char *argv[])
{
	// argv[0] = program name
	// argv[1] = poly string
	// argv[2] = step
	// argv[3] = number of artifacts
	// Read in the polygon, generate random points
	// check if points are in bounds, direct output to stdout
	
	// Read in polygon:
	if (argc != 4){ help_message(); return(1); };
	
	std::string poly_init = argv[1];
	double step = atof(argv[2]);
	double num_artifacts = atof(argv[3]);
	
	XYPolygon poly;

	bool ok = poly.initialize(poly_init);
	
	if (!ok) {
		std::cerr << "ERROR:  Polygon string not parsed properly\n";
		return 1;
	}
	
	// Find the bounding square.  We'll generate random points within this
	double x, y;
	double min_x, max_x, min_y, max_y;
	
	min_x = poly.get_vx(0);
	max_x = poly.get_vx(0);
	min_y = poly.get_vy(0);
	max_y = poly.get_vy(0);
	
	for(int i = 1; i < poly.size(); i++){
		x = poly.get_vx(i);
		y = poly.get_vy(i);
		
		if (x < min_x) { min_x = x; }
		else if (x > max_x) { max_x = x; }
		
		if (y < min_y) { min_y = y; }
		else if (y > max_x) { max_y = y; }
	}

	std::vector<std::string> vArtifacts = generateArtifacts(min_x, max_x, min_y, max_y, step, num_artifacts, poly);
	
	// Write strings to stdout
	const std::string LINE_HEADER = "ARTIFACT = ";
	
	for(std::vector<std::string>::iterator p = vArtifacts.begin(); p != vArtifacts.end(); p++){
		std::cout << LINE_HEADER << *p << std::endl;
	};

}

/// help_message() prints the usage instructions for the program
void help_message()
{
	using namespace std;

	cout << endl;
	cout << "Usage:  artfieldgenerator" << endl;
	cout << "artfieldgenerator polygonstring step num_artifacts > ofile.art" << endl;
	cout << endl;
}


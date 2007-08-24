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
#include <algorithm>

void help_message();
std::vector<std::string> generateArtifacts(double, double, double, double, double, double, XYPolygon&);

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

	poly.initialize(poly_init);
	
	// Remove label if present
	if(poly_init.substr(0, 5) == "label"){
		poly_init = chompString(poly_init, ':').at(1);
	}
	
	// Put points into vector
	std::vector<std::string> vPoints = parseString(poly_init, ':');
	if(vPoints.size() < 3){
		std::cout << "Error: config string contains less than 3 points: " << poly_init << std::endl;
		return(2);
	};
	
	// Find min/max x,y
	std::vector<double> vx, vy;
	
	for(std::vector<std::string>::iterator p = vPoints.begin(); p != vPoints.end(); p++){
		std::vector<std::string> vPoint = parseString(*p, ',');
		vx.push_back(atof(vPoint.at(0).c_str()));
		vy.push_back(atof(vPoint.at(1).c_str()));
	}
	
	double min_x, min_y, max_x, max_y;
	min_x = *min_element(vx.begin(), vx.end());
	min_y = *min_element(vy.begin(), vy.end());
	max_x = *max_element(vx.begin(), vx.end());
	max_y = *max_element(vy.begin(), vy.end());

	std::vector<std::string> vArtifacts = generateArtifacts(min_x, max_x, min_y, max_y, step, num_artifacts, poly);
	
	// Write strings to stdout
	const std::string LINE_HEADER = "ARTIFACT = ";
	
	for(std::vector<std::string>::iterator p = vArtifacts.begin(); p != vArtifacts.end(); p++){
		std::cout << LINE_HEADER << *p << std::endl;
	};

}

/// generateArtifacts() produces a random vector of unique artifacts
/// \param step Discrete grid to snap artifacts to
/// \param num_artifacts Number of artifacts to generate
/// \param poly An XYPolygon to check to see if the points are within the bounds
std::vector<std::string> generateArtifacts(double min_x, double max_x, double min_y, double max_y, double step, double num_artifacts, XYPolygon& poly)
{
	std::vector<std::string> vArtifacts;
	
	srand(time(NULL));

	while(vArtifacts.size() < num_artifacts){
		double rand_x = snapToStep(min_x + rand() * (max_x - min_x) / RAND_MAX, step);
		double rand_y = snapToStep(min_y + rand() * (max_y - min_y) / RAND_MAX, step);
		std::ostringstream osArtifact;
		osArtifact << "X=" << rand_x << "," << "Y=" << rand_y;
		std::string sArtifact = osArtifact.str();
		
		if(poly.contains(rand_x, rand_y) && !vectorContains(vArtifacts, sArtifact)){ // New artifact to add
			vArtifacts.push_back(sArtifact);
		};
	};
	
	return vArtifacts;
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


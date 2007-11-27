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
#include "XYSegList.h"
#include "IO_GeomUtils.h"
#include "ArtifactUtils.h"

void help_message();

int main(int argc, char *argv[])
{
	// argv[0] = program name
	// argv[1] = file to read polygons from
	// argv[2] = file to put poly/seglist into
	// Read in the polygons
	// generate lawnmower pattern
	// Output to files
	if (argc != 2){ help_message(); return 1; };
	
	
	std::vector<XYPolygon> vecPolys = readPolysFromFile(argv[1]);
	
	std::vector<XYSegList> vecMowers;
	
	std::vector<XYPolygon>::iterator p;
	for(p = vecPolys.begin(); p != vecPolys.end(); p++) {
		std::cout << "Attempting to generate lawnmower pattern for: \n";
		std::cout << p->get_spec() << std::endl;
		
		//generateLawnmower(poly, x0, y0, angle, radius, clockwise?);
		vecMowers.push_back(generateLawnmower(*p, 0, 0, 45, 10, true));
		
		std::cout << "Got mower pattern: \n";
		std::cout << vecMowers.at(vecMowers.size()-1).get_spec() << std::endl;
		
	};
}



/// help_message() prints the usage instructions for the program
void help_message()
{
	using std::cout;
	using std::endl;

	cout << endl;
	cout << "Usage:  generatelawnmower" << endl;
	cout << "generatelawnmower input_file" << endl;
	cout << endl;
}


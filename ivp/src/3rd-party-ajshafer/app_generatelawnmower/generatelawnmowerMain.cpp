/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: MIT Cambridge                                   */
/*    FILE: generatelawnmowerMain.cpp                       */
/*    DATE: 1 NOV 2007                                      */
/************************************************************/

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include "MBUtils.h"
#include "XYPolygon.h"
#include "XYSegList.h"
#include "IO_GeomUtils.h"
#include "ArtifactUtils.h"
#include "AngleUtils.h"

void help_message();

int main(int argc, char *argv[])
{
	// argv[0] = program name
	// argv[1] = x
	// argv[2] = y
	// argv[3] = angle
	// argv[4] = radius
	// argv[5] = clockwise (true/false)
	// argv[6] = file to read polygons from
	// argv[7] = [snap value]
	// Read in the polygons
	// generate lawnmower pattern
	// Output to files
	if (argc != 8 && argc != 7 && argc != 2 && argc != 3){ help_message(); return 1; };
	
	double px0 = 0, py0 = 0, ang = 90, radius = 10, snap = 0.0;
	bool clockwise = true;
	std::string spx0 = "0", spy0 = "0", sang = "90", sradius = "10", sclockwise = "TRUE", ssnap = "0";
	std::string ofHeader;
	
    if (argc == 2) { ofHeader = argv[1]; }
    else if (argc == 3) { ofHeader = argv[1]; ssnap = argv[2]; }
    else if (argc == 7 || argc == 8) {
    	spx0 = argv[1];
    	spy0 = argv[2];
    	sang = argv[3];
    	sradius = argv[4];
    	sclockwise = argv[5];
    	ofHeader = argv[6];
    	if (argc == 7) {ssnap = argv[7];}
    }
	
	if (toupper(sclockwise) == "TRUE") {clockwise = true;}
	else if (toupper(sclockwise) == "FALSE") {clockwise = false;}
	
	bool ok = isNumber(spx0) && isNumber(spy0) && isNumber(sang) && isNumber(sradius) && isNumber(ssnap);
	if (ok) {
		px0 = atof(spx0.c_str());
		py0 = atof(spy0.c_str());
		ang = angle360(atof(sang.c_str()));
		radius = atof(sradius.c_str());
		snap = atof(ssnap.c_str());
	}
	else { help_message(); return 1; }
	
	std::ofstream ofPolys, ofSegLists;
	std::string ofPolyName = ofHeader;
			ofPolyName.append("_polys");
	std::string ofSegListName = ofHeader;
			ofSegListName.append("_seglists");
	
	ofPolys.open(ofPolyName.c_str());
	ofSegLists.open(ofSegListName.c_str());
	
	std::vector<XYPolygon> vecPolys = readPolysFromFile(ofHeader);
	std::vector<XYSegList> vecMowers;
	
	std::vector<XYPolygon>::iterator p;
	for(p = vecPolys.begin(); p != vecPolys.end(); p++) {
		std::cout << "Attempting to generate lawnmower pattern for: \n";
		std::cout << p->get_spec() << std::endl;
		
		//generateLawnmower(poly, x0, y0, angle, radius, clockwise?);
		//std::cout << "Calling with: " << px0 << py0 << ang << radius << clockwise << std::endl;;
		XYSegList mower = generateLawnmower(*p, px0, py0, ang, radius, clockwise);
		mower.set_label(p->get_label());
		mower.apply_snap(snap);
		vecMowers.push_back(mower);
		
		std::cout << "Got mower pattern: \n";
		std::cout << vecMowers.at(vecMowers.size()-1).get_spec() << std::endl;
		
		// Write out to files
		if (ofPolys.is_open() && ofSegLists.is_open()){
			ofPolys << p->get_spec() << std::endl;
			ofSegLists << vecMowers.at(vecMowers.size()-1).get_spec() << std::endl;
		}
		
	};
	
	ofPolys.close();
	ofSegLists.close();
}



/// help_message() prints the usage instructions for the program
void help_message()
{
	using std::cout;
	using std::endl;

	cout << endl;
	cout << "Usage:  generatelawnmower" << endl;
	cout << "generatelawnmower x y angle radius clockwise input_file [snap value]" << endl;
	cout << "generatelawnmower input_file [snap value]" << endl;
	cout << endl;
}


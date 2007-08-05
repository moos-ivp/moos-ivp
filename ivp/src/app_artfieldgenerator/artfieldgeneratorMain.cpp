/************************************************************/
/*    NAME: Andrew Shafer                                   */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: artfieldgeneratorMain.cpp                       */
/*    DATE: 3 JUL 2007                                      */
/************************************************************/

#include <iostream>
#include <fstream>
#include "MBUtils.h"

void help_message();
int generateArtifactFile(double x_lower, double x_upper, double y_lower, double y_upper, double step, int n, std::string ofile);

/// ArtfieldGenerator creates random artifact fields for testing purposes.

/// It reads its configuration information from the command line and
/// outputs the artifactfield as a text file.
int main(int argc, char *argv[])
{
	double x_lower, x_upper, y_lower, y_upper, step;
	int n;
	std::string ofile;

	if(argc != 8){
		help_message();
		return 0;
	}

	step = atof(argv[5]);
	x_lower = atof(argv[1]);
	x_upper = atof(argv[2]);
	y_lower = atof(argv[3]);
	y_upper = atof(argv[4]);
	n = atoi(argv[6]);
	ofile = argv[7];

/*
	int res = 0;
	// Look for a request for help
	res = getArg(argc, argv, 0, "-h", "--help");
	if(res) {
		help_message();
		return(0);
	}

	res = getArg(argc, argv, 1, "-x", "-x");
	if(res){
		x_lower = atof(argv[res]);
		x_upper = atof(argv[res+1]);
	};

	res = getArg(argc, argv, 1, "-y", "-y");
	if(res){
		y_lower = atof(argv[res]);
		y_upper = atof(argv[res+1]);
	};
	
	res = getArg(argc, argv, 1, "-n", "-n");
	if(res){
		n = atoi(argv[res]);
	};

	res = getArg(argc, argv, 1, "-o", "-o");
	if(res){
		ofile = argv[res];
	};
*/
	int fileError = 0;
	fileError = generateArtifactFile(x_lower, x_upper, y_lower, y_upper, step, n, ofile);

	return(fileError);
}

/// generateArtifactFile creates a random artifact field in \a ofile.

/// The \a n generated artifacts exist between the given bounds.
///
/// This function does not check for errors in writing to the file--MUST IMPLEMENT
/// \param x_lower Lower bound of x coordinates
/// \param x_upper Upper bound of x coordinates
/// \param y_lower Lower bound of y coordinates
/// \param y_upper Upper bound of y coordinates
/// \parm step Snap artifacts to points
/// \param n Number of artifacts to generate
/// \param ofile Output file to write to
int generateArtifactFile(double x_lower, double x_upper, double y_lower, double y_upper, double step, int n, std::string ofile)
{

	const std::string LINE_HEADER = "ARTIFACT: ";
	const std::string MAJOR_SEPERATOR = ",";

	srand(time(NULL));

	std::ofstream output_file(ofile.c_str());

	std::vector<double> x_values;
	std::vector<double> y_values;

	for( int i = 0; i < n; i++){
		double rand_x = x_lower + rand() * (x_upper - x_lower) / RAND_MAX;
		double rand_y = y_lower + rand() * (y_upper - y_lower) / RAND_MAX;

		x_values.push_back(snapToStep(rand_x, step));
		y_values.push_back(snapToStep(rand_y, step));
	};

	for( int i = 0; i < n; i++){
		output_file << LINE_HEADER << "X=" << x_values.at(i) << MAJOR_SEPERATOR << "Y=" << y_values.at(i) << std::endl;
	};

	return 0;
}


/// help_message() prints the usage instructions for the program
void help_message()
{
	using namespace std;

	cout << endl;
	cout << "Usage:  artfieldgenerator" << endl;
	cout << "artfieldgenerator x_lower x_upper y_lower y_upper step_size num_mines output_file" << endl;
	cout << endl;
}


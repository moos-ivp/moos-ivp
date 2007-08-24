/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: ArtifactField.cpp                                    */
/*    DATE: 3 JUL 2007                                           */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include "ArtifactField.h"
#include <sstream>
#include "MBUtils.h"
#include "GeomUtils.h"
// iostream only for debugging
// #include <iostream>

/// Puts an artifact into the field
void ArtifactField::addArtifact(std::string strInput)
{
	vecArtifacts.push_back(strInput);
	return;
}

/// An alternate way to add an X,Y artifact
void ArtifactField::addArtifact(double x, double y)
{
	std::string strAssembler;
	std::ostringstream sX, sY;
	
	sX << x;
	sY << y;
	
	strAssembler = "X=" + sX.str();
	strAssembler += ",";
	strAssembler += "Y=" + sY.str();
	vecArtifacts.push_back(strAssembler);
	return;
}

/// Returns the artifact at index \a i
/// No error catching is done, but at() will throw errors.  Could be implemented later.
std::string ArtifactField::getArtifact(int i)
{
	return vecArtifacts.at(i);
}

/// Returns the size of the artifact field
int ArtifactField::size()
{
	return vecArtifacts.size();
}

/// Returns a vector of all artifacts within the 2D, X,Y box specified by
/// \param x_lower Lower bound of x
/// \param x_upper Upper bound of x
/// \param y_lower Lower bound of y
/// \param y_upper Upper bound of y
std::vector<std::string> ArtifactField::getArtifactbox(double x_lower, double x_upper, double y_lower, double y_upper){
	// Parse all of the artifacts for X,Y values, adding to local copy
	std::string sx, sy;
	double x, y;
	
	std::vector<std::string>::iterator p;
	
	std::vector<std::string> vecResults;
	
	for (p = vecArtifacts.begin(); p != vecArtifacts.end(); p++){
		if(tokParse(*p, "X", ',', '=', x) && tokParse(*p, "Y", ',', '=', y)){
			x = atof(sx.c_str());
			y = atof(sy.c_str());
			if(x > x_lower && x < x_upper && y > y_lower && y < y_upper){
				vecResults.push_back(*p);
			};
		};
	};

	// Return remaining artifacts
	return vecResults;
};

/// Returns a vector of all artifacts within the 2D, X,Y circle specified by
/// \param x_val X value
/// \param y_val XY value
/// \param radius Radius to detect to
std::vector<std::string> ArtifactField::getArtifactcircle(double x_val, double y_val, double radius){
	// Parse all of the artifacts for X,Y values, adding to local copy
	std::string sx, sy;
	double x, y;
	
	std::vector<std::string>::iterator p;
	
	std::vector<std::string> vecResults;
	
	//std::cout << "Size of artifact field is " << vecArtifacts.size() << std::endl; //Debug line
	
	for (p = vecArtifacts.begin(); p != vecArtifacts.end(); p++){
		//std::cout << "Object " << p - vecArtifacts.begin() << " :" << *p << std::endl; //Debug line
		if(tokParse(*p, "X", ',', '=', sx) && tokParse(*p, "Y", ',', '=', sy)){
			x = atof(sx.c_str());
			y = atof(sy.c_str());
			//std::cout << "X, Y, radius in artfield query are " << x << "," << y << "," << radius << std::endl; //Debug line
			if(distPointToPoint(x_val, y_val, x, y) < radius){
				vecResults.push_back(*p);
			};
		};
	};

	// Return remaining artifacts
	return vecResults;
};

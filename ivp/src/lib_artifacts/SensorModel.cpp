/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: SensorModel.cpp                                      */
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

#include "SensorModel.h"
#include "MBUtils.h"
#include "ArtifactField.h"
// iostream only used for debugging
//#include <iostream>

/// Set the type of sensor the model should simulate
bool SensorModel::setSensorModel(std::string const sInputSensorType) {
	// Select the type of sensor
	
	if (tolower(sInputSensorType) == "fixedradius"){
		sSensorType = tolower(sInputSensorType);
		return true;
		}
	else if (tolower(sSensorType) == "blarg"){
		return true;
		}
	else return false;
};

/// Set the effective radius of the sensor (currently used for fixed radius sensor)
void SensorModel::setSensorRadius(double radius) {
	dSensorRadius = radius;
	if(dSensorRadius < 0) {dSensorRadius = 0;};
	return;
};

/// Query the sensor on a given artifact field
/// \param ArtField is a reference to an artifact field
std::vector<std::string> SensorModel::querySensor(std::string const sSensorQuery, ArtifactField const &ArtField) const {
	//std::cout << "Query string is: " << sSensorQuery << std::endl; //Debug line
	//std::cout << "for sensor type " << sSensorType << " of radius " << dSensorRadius << std::endl; //Debug line
	if(sSensorType.empty()) {std::vector<std::string> vecReturn; return vecReturn;};
	
	if(sSensorType == "fixedradius") return queryFRSensor(sSensorQuery, ArtField);
	else {std::vector<std::string> vecReturn; return vecReturn;};
};


std::vector<std::string> SensorModel::queryFRSensor(std::string const sSensorQuery, ArtifactField const &ArtField) const{
	std::string sx, sy;
	double x, y;
	std::vector<std::string> vecNearArtifacts;
	
	if(tokParse(sSensorQuery, "X", ',', '=', sx) && tokParse(sSensorQuery, "Y", ',', '=', sy)){
		x = atof(sx.c_str());
		y = atof(sy.c_str());
		//std::cout << "X and Y in the query are: " << x << "," << y << std::endl; //Debug line
		vecNearArtifacts = ArtField.getArtifactcircle(x, y, dSensorRadius);
		
		// Add probability of artifact to end of each string
		for(std::vector<std::string>::iterator p = vecNearArtifacts.begin(); p != vecNearArtifacts.end(); p++){
			*p += ",Prob=1";
		};
	};
	
	return vecNearArtifacts;
};

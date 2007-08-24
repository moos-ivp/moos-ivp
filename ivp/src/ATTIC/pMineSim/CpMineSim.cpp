// $Header: /home/cvsroot/project-marine-shell/src/pMineSim/CpMineSim.cpp,v 1.5 2007/06/18 18:14:31 ajshafer Exp $
// (c) 2007 Andrew Shafer <ajshafer@mit.edu>

// CpMineSim.cpp: implementation of the CpMineSim class.
////////////////////////////////////////////////////////

#include <vector>
#include <iostream>
#include <math.h>

#include <iterator>
#include <algorithm>
#include <sstream>
#include "CpMineSim.h"
#include "GeomUtils.h"
#include "MBUtils.h"

// Stuff for XYMineField class
//---------------------------------------------------------------
// Procedure: addMine
//   Purpose: Adds a new mine to the minefield
bool XYMineField::addMine(double x, double y)
{
	m_pos_x.push_back(x);
	m_pos_y.push_back(y);
	
	return true;
}

//---------------------------------------------------------------
// Procedure: countMines
//   Purpose: Returns the number of mines in the field
int XYMineField::countMines()
{
	return m_pos_x.size();
}

//---------------------------------------------------------------
// Procedure: distToMine
//   Purpose: Computes the distance to the nearest mine 
//            (or a given mine)
double XYMineField::distToMine(double x, double y)
{
	std::vector<double> vectDistances;
	
	for(int i = 0; i< countMines(); i++){
		vectDistances.push_back(distToMine(x, y, i));
	};

	return *min_element(vectDistances.begin(), vectDistances.end());
}	
		
  	
double XYMineField::distToMine(double x, double y, int index)
{
	return distPointToPoint(x, y, getPosX(index), getPosY(index));
}

//---------------------------------------------------------------
// Procedure: closestMine
//   Purpose: Returns the index of the nearest mine
int XYMineField::closestMine(double x, double y)
{
	std::vector<double> vectDistances;
	
	for(int i = 0; i< countMines(); i++){
		vectDistances.push_back(distToMine(x, y, i));
	};

	return min_element(vectDistances.begin(), vectDistances.end()) - vectDistances.begin();
}
  
//---------------------------------------------------------------
// Procedure: getPosX, getPosY
//   Purpose: Returns the x or y value of a given mine index
double XYMineField::getPosX(int index)
{
	return m_pos_x.at(index);
}

double XYMineField::getPosY(int index)
{
	return m_pos_y.at(index);
}

//---------------------------------------------------------------
// Procedure: getMines
//   Purpose: returns the indices of mines within a radius of x,y
std::vector<int> XYMineField::getMines(double x, double y, double radius)
{
	std::vector<int> mines;
	
	for (int i=0; i<countMines(); i++){
		if(distToMine(x,y,i) <= radius){
			mines.push_back(i);
		};
	};
	
	return mines;
}
	  
//---------------------------------------------------------------
// Procedure: getField
//   Purpose: returns a string with all of the mines as x,y: pairs
std::string XYMineField::getField()
{
	std::ostringstream stringrep;
	
	for(int i=0; i<countMines(); i++){
		stringrep << getPosX(i);
		stringrep << ",";
		stringrep << getPosY(i);
		if(!(i = countMines() - 1)){
			stringrep << ":";
		};
	};
	
	return stringrep.str();
}

//---------------------------------------------------------------
// Procedure: displayMine
//   Purpose: plot the mine in pMarineViewer
std::string XYMineField::mineBox(int index)
{
	// Create a box of width/height around mine location
	double dimension = 5;
	double halfdim = dimension/2;
	double x = getPosX(index);
	double y = getPosY(index);
	
	std::ostringstream mineBox;
	
	mineBox << "radial:";
	mineBox << x;
	mineBox << ",";
	mineBox << y;
	mineBox << ",";
	mineBox << 10;
	mineBox << ",";
	mineBox << 100;
	
	return mineBox.str();
}

// End XYMineField stuff


CpMineSim::CpMineSim()
{
	// constructor
}

CpMineSim::~CpMineSim()
{
	// destructor
}

bool CpMineSim::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
		
		if(msg.m_sKey == "NAV_X"){
			x = msg.m_dfVal;
		}
		if(msg.m_sKey == "NAV_Y"){
			y = msg.m_dfVal;
		}
	}
}

bool CpMineSim::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", is_float(int));
	
	m_Comms.Register("NAV_X", 0);
	m_Comms.Register("NAV_Y", 0);
	m_Comms.Register("MINE_REFRESH", 0);
	
	// Publish Mines for display
	for(int i = 0; i < MineField.countMines(); i++){
		m_Comms.Notify("VIEW_POLYGON", MineField.mineBox(i));
	};
	
	return true;
}

bool CpMineSim::Iterate()
{
	// happens AppTick times per second
	//  Here's where the real work is done:
	
	std::vector<int> LocalMines;
	LocalMines = MineField.getMines(x, y, DetectionRadius);
	
	// For the local mines, publish a list of the indices
	// and the locations
	std::ostringstream stringindices, stringlocs;
	
	for(int i=0; i<LocalMines.size(); i++){
		stringlocs << MineField.getPosX(LocalMines.at(i));
		stringlocs << ",";
		stringlocs << MineField.getPosY(LocalMines.at(i));
		stringindices << LocalMines.at(i);
		
		if(i != LocalMines.size() - 1){
			stringlocs << ":";
			stringindices << ":";
		};
	};
	
	m_Comms.Notify("MINE_LOCALS_LOC", stringlocs.str());
	m_Comms.Notify("MINE_LOCALS_INDEX", stringindices.str());
	
	// Find LocalMines not in DetectedMines, add them to end
	for(int i=0; i<LocalMines.size(); i++){
		if(find(DetectedMines.begin(), DetectedMines.end(), LocalMines.at(i)) == DetectedMines.end()){
			DetectedMines.push_back(LocalMines.at(i));
		};
	};
	
	std::ostringstream stringdetected;
	for(int i=0; i < DetectedMines.size(); i++){
		stringdetected << DetectedMines.at(i);
		if(i != DetectedMines.size() - 1){
			stringdetected << ":";
		};
	};
	
	m_Comms.Notify("MINE_FIELD", stringdetected.str());
		

	return true;
}

bool CpMineSim::OnStartUp()
{
	// happens before connection is open

	CMOOSApp::OnStartUp();
  	MOOSTrace("pMineSim starting....\n");

	// Add mines to MineField
  	std::list<std::string> sParams;
  	if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
  		std::list<std::string>::iterator p;
		for(p = sParams.begin(); p!=sParams.end(); p++) {
	    		std::string sLine  = stripBlankEnds(*p);
	    		std::string sKey   = stripBlankEnds(MOOSChomp(sLine, "="));
	    		sKey = tolower(sKey);
	    		if(MOOSStrCmp(sKey, "mine")) {
	    			// a mine line should look like
	    			// mine = 1,2 OR 1, 2 OR whatever (text returns 0.0 at atof)
	    			std::vector<std::string> svector = parseString(sLine, ',');
	    			if(svector.size() != 2){
	    				return false;
	    			};
	    			double sX = atof(stripBlankEnds(svector.at(0)).c_str());
	    			double sY = atof(stripBlankEnds(svector.at(1)).c_str());
	    			bool ok = MineField.addMine(sX, sY);
	    			if(!ok){
	    				return false;
	    			};
	    		};
	    	};
  	};
	
	MOOSTrace("There are %d mines in the minefield.\n", MineField.countMines());
	
	// Publish Mines for display
	for(int i = 0; i < MineField.countMines(); i++){
		m_Comms.Notify("VIEW_POLYGON", MineField.mineBox(i));
	};
	
	return true;
}


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PopulatorPolarPlot.cpp                               */
/*    DATE: July 16th, 2021                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "PopulatorPolarPlot.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

PopulatorPolarPlot::PopulatorPolarPlot()
{
}

//-------------------------------------------------------------
// Procedure: readFile()

bool PopulatorPolarPlot::readFile(string filename)
{
  cout << "Reading in File: " << filename << endl;

  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0) {
    cout << "  File not found - or empty file." << endl;
    return(false);
  }

  for(unsigned int i=0; i<lines.size(); i++) {
    string line  = stripBlankEnds(lines[i]);
    if(!strBegins(line, "//")) {
      string left  = tolower(biteStringX(lines[i], '='));
      string right = lines[i];

      bool line_ok = true;
      if(!isNumber(left) || !isNumber(right))
	line_ok = false;

      double dbl_hdg = atof(left.c_str());
      double dbl_spd = atof(right.c_str());
      if((dbl_hdg < -180) || (dbl_hdg > 360))
	line_ok = false;
      if((dbl_spd < 0) || (dbl_spd > 1))
	line_ok = false;
            
      if(!line_ok) {
	cout << "Problem with line: " << i << ": " << line << endl;
	return(false);
      }

      m_polar_plot.addSetPoint(dbl_hdg, dbl_spd);      
    }
  }
  cout << "Done reading in file: " << filename << endl;
  return(true);
}










/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeRepUtils.cpp                                     */
/*    DATE: Jun 26th 2011                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include "NodeRecordUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: string2NodeRecord
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=DRIVE,GROUP=A

NodeRecord string2NodeRecord(const string& node_rep_string, bool returnPartialResult)
{
  NodeRecord empty_record;
  NodeRecord new_record;

  vector<string> svector = parseStringZ(node_rep_string, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = toupper(biteStringX(svector[i], '='));
    string value = svector[i];

    if(param == "NAME")
      new_record.setName(value);
    else if(param == "TYPE")
      new_record.setType(value);
    else if(param == "COLOR")
      new_record.setColor(value);
    else if(param == "GROUP")
      new_record.setGroup(value);
    else if(param == "MODE")
      new_record.setMode(value);
    else if(param == "ALLSTOP")
      new_record.setAllStop(value);
    else if(param == "LOAD_WARNING")
      new_record.setLoadWarning(value);
    else if(param == "INDEX")
      new_record.setIndex(atof(value.c_str()));
    else if((param == "THRUST_MODE_REVERSE") && (tolower(value) == "true")) 
      new_record.setThrustModeReverse(true);
    else if(param == "TRAJECTORY")
      new_record.setTrajectory(stripBraces(value));
    else if(isNumber(value)) {
      if((param == "UTC_TIME") || (param == "TIME"))
	new_record.setTimeStamp(atof(value.c_str()));
      else if(param == "X")
	new_record.setX(atof(value.c_str()));
      else if(param == "Y")
	new_record.setY(atof(value.c_str()));
      else if(param == "LAT")
	new_record.setLat(atof(value.c_str()));
      else if(param == "LON")
	new_record.setLon(atof(value.c_str()));

      else if((param == "SPD") || (param == "SPEED"))
	new_record.setSpeed(atof(value.c_str()));
      else if(param == "SPD_OG")
	new_record.setSpeedOG(atof(value.c_str()));

      else if((param == "HDG") || (param == "HEADING"))
	new_record.setHeading(atof(value.c_str()));
      else if(param == "HDG_OG")
	new_record.setHeadingOG(atof(value.c_str()));

      

      else if(param == "YAW")
	new_record.setYaw(atof(value.c_str()));
      else if((param == "DEP") || (param == "DEPTH"))
	new_record.setDepth(atof(value.c_str()));
      else if((param == "ALT") || (param == "ALTITUDE"))
	new_record.setAltitude(atof(value.c_str()));
      else if((param == "LENGTH") || (param == "LEN"))
	new_record.setLength(atof(value.c_str()));
    }
  }
  
  //  if(!new_record.valid()&&(!returnPartialResult))
  //  return(empty_record);

  return(new_record);
}









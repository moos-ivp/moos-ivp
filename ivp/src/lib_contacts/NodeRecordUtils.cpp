/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "NodeRecordUtils.h"
#include "MBUtils.h"
#include "LinearExtrapolator.h"

using namespace std;

//---------------------------------------------------------
// Procedure: rangeBetweenRecords()

double rangeBetweenRecords(const NodeRecord& rec1,
			   const NodeRecord& rec2)
{
  double x1 = rec1.getX();
  double y1 = rec1.getY();
  double x2 = rec2.getX();
  double y2 = rec2.getY();
  
  return(hypot(x1-x2, y1-y2));
}

//---------------------------------------------------------
// Procedure: extrapolateRecord()

NodeRecord extrapolateRecord(const NodeRecord& record, double curr_time,
			     double max_delta)
{
  // Sanity Check 1
  if(curr_time <= 0)
    return(record);
  
  double record_utc = record.getTimeStamp();

  // Sanity Check 2
  if(curr_time <= (record_utc+0.05))
    return(record);

  LinearExtrapolator extrapolator;
  extrapolator.setDecay(max_delta, max_delta);
  extrapolator.setPosition(record.getX(), record.getY(),
			   record.getSpeed(), record.getHeading(),
			   record_utc);

  double new_x, new_y;
  bool ok = extrapolator.getPosition(new_x, new_y, curr_time);
  if(ok) {
    NodeRecord new_record = record;
    new_record.setX(new_x);
    new_record.setY(new_y);
    return(new_record);
  }

  return(record);
}

//---------------------------------------------------------
// Procedure: string2NodeRecord()
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
    else if(param == "MODE")
      new_record.setMode(value);
    else if(param == "ALLSTOP")
      new_record.setAllStop(value);
    else if(param == "INDEX")
      new_record.setIndex(atof(value.c_str()));
    else if(isNumber(value)) {
      if((param == "TIME") || (param == "UTC_TIME"))
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
      else if((param == "HDG") || (param == "HEADING"))
	new_record.setHeading(atof(value.c_str()));

      else if((param == "DEP") || (param == "DEPTH"))
	new_record.setDepth(atof(value.c_str()));
      else if((param == "LENGTH") || (param == "LEN"))
	new_record.setLength(atof(value.c_str()));
      else if(param == "YAW")
	new_record.setYaw(atof(value.c_str()));
      else if((param == "ALT") || (param == "ALTITUDE"))
	new_record.setAltitude(atof(value.c_str()));
      else if(param == "HDG_OG")
	new_record.setHeadingOG(atof(value.c_str()));
      else if(param == "SPD_OG")
	new_record.setSpeedOG(atof(value.c_str()));
      else if(param == "TRANSPARENCY")
	new_record.setTransparency(atof(value.c_str()));
    }
    else if(param == "COLOR")
      new_record.setColor(value);
    else if(param == "GROUP")
      new_record.setGroup(value);
    else if(param == "LOAD_WARNING")
      new_record.setLoadWarning(value);
    else if((param == "THRUST_MODE_REVERSE") && (tolower(value) == "true")) 
      new_record.setThrustModeReverse(true);
    else if(param == "TRAJECTORY")
      new_record.setTrajectory(stripBraces(value));

  }
  
  //  if(!new_record.valid()&&(!returnPartialResult))
  //  return(empty_record);

  return(new_record);
}






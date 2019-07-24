/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FldWrapDetect.cpp                                    */
/*    DATE:                                                      */
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

#include <iterator>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "FldWrapDetect.h"
#include "NodeRecordUtils.h"
#include "ACTable.h"


using namespace std;

//---------------------------------------------------------
// Constructor

FldWrapDetect::FldWrapDetect()
{
  m_iterations = 0;
  m_timewarp   = 1;
  m_deployed = false;
  m_max_num_segments = 1000;
  m_max_trail_distance = 100; // meters of memory
  m_num_wraps = 0;
}

//---------------------------------------------------------
// Destructor

FldWrapDetect::~FldWrapDetect()
{
}

//---------------------------------------------------------

// Procedure: OnNewMail

bool FldWrapDetect::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   

  string key    = msg.GetKey();

  if((key == "NODE_REPORT") && (m_deployed)){
    //if a node report is received, process the node report information.
    string sval  = msg.GetString(); 
    NodeRecord record = string2NodeRecord(sval);
    string node_name = record.getName();
    double x2 = record.getX();
    double y2 = record.getY();
    
    // find last (x,y) position
    NodeRecord prev_record = m_node_record[node_name];
    if(! (prev_record.valid()) ) {
      m_node_record[node_name] = record;
      m_odometer[node_name] = 0;
    }
    else{
      
      double x1 = prev_record.getX();
      double y1 = prev_record.getY();
      
      double node_dist_traveled = distPointToPoint(x1,y1,x2,y2);
      if(node_dist_traveled == 0){
	return false;
      }
      m_odometer[node_name] += node_dist_traveled;
      
      // now have (x,y) data for making new XYSegment 
      m_node_record[node_name] = record;
      XYSegment new_segment;
      new_segment.set(x1,y1,x2,y2);
      
      // add new_segment to front of list;
      m_xysegments[node_name].push_front(new_segment); 
      
      // trim segments down to allowed size
      while(m_xysegments[node_name].size() > (unsigned int) m_max_num_segments){
	m_xysegments[node_name].pop_back();
      } // end while
      
      
      if(CheckForWrap(node_name)){
	reportEvent("Wrap Around Detected: " + node_name);
	string out_string = "vname=" + node_name;
	Notify("WRAP_AROUND_DETECTED",out_string);
	m_num_wraps++;
	m_xysegments[node_name].clear();
      }
    } // end else for valide node record
  } // end key=node report
  else if(key == "DEPLOY_ALL"){
    m_deployed = true;
  }
  
  }// end for loop
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool FldWrapDetect::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool FldWrapDetect::Iterate()
{
  m_iterations++;
  AppCastingMOOSApp::Iterate();

  AppCastingMOOSApp::PostReport();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool FldWrapDetect::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string orig  = *p;
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      bool handled = false;
      if(param == "max_segments") {
	handled = setIntOnString(m_max_num_segments, value);
	if(m_max_num_segments < 100)
	  m_max_num_segments = 100;
      }
      else if(param == "max_trail_distance")
	handled = setPosDoubleOnString(m_max_trail_distance, value);

      if(!handled)
	reportUnhandledConfigWarning(orig);
    }
  }
  
  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void FldWrapDetect::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NODE_REPORT", 0);
  Register("DEPLOY_ALL",0);
  Register("WRAP_AROUND_DETECTED",0);
}


//---------------------------------------------------------
// Procedure: checkForWrap()

bool FldWrapDetect::CheckForWrap(string vname)
{
  // returns true if wrap around is detected
  
  list<XYSegment> segment_list = m_xysegments[vname];
  std::list<XYSegment>::iterator seg_it = segment_list.begin();
  
  double x1 = (*seg_it).get_x1();
  double y1 = (*seg_it).get_y1();
  double x2 = (*seg_it).get_x2();
  double y2 = (*seg_it).get_y2();
  double trip_odometer = distPointToPoint(x1,y1,x2,y2);

  if(seg_it != segment_list.end()){ 
    ++seg_it;
    double x1 = (*seg_it).get_x1();
    double y1 = (*seg_it).get_y1();
    double x2 = (*seg_it).get_x2();
    double y2 = (*seg_it).get_y2();
    trip_odometer += distPointToPoint(x1,y1,x2,y2);
    
    if(seg_it != segment_list.end()){ 
      ++seg_it;
    }
    else{
      return false;
      // need to go two nodes deep, otherwise have an intersection of (x2,y2) and (x3,y3) resulting in overlap
    }    
  }
  else{
    return false;
  }

  while(seg_it != segment_list.end()){
    double x3 = (*seg_it).get_x1();
    double y3 = (*seg_it).get_y1();
    double x4 = (*seg_it).get_x2();
    double y4 = (*seg_it).get_y2();
    trip_odometer += distPointToPoint(x3,y3,x4,y4);    

    if(trip_odometer > m_max_trail_distance){
      // only keep data for desired length of trail
      segment_list.pop_back();
      return false;
    }
    else if(segmentsCross(x1,y1,x2,y2,x3,y3,x4,y4)){
      // if a portion of the trail was crossed, report a wrap around
      return true;
    }
    ++seg_it;    
  }  
  return false;
}

//------------------------------------------------------------
// Procedure: buildReport()

bool FldWrapDetect::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "uFldWrapDetect                          \n";
  m_msgs << "============================================ \n";

  m_msgs << "   max number of segments in trail:   " << doubleToString(m_max_num_segments,0) << "\n\n" << endl;
  m_msgs << "      max length of trail checked :   " << doubleToString(m_max_trail_distance,0) << "\n\n" << endl;
  m_msgs << "          number of wraps detected:   " << doubleToString(m_num_wraps,0) << "\n\n" << endl;
  return(true);
}








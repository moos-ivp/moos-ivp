/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayAngle.cpp                             */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_ArrayAngle.h"
#include "MBUtils.h"
#include "AOF_ArrayAngle.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ArrayAngle::BHV_ArrayAngle(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_1BTrack");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");
 
  // addInfoVars("NAV_HEADING");
  addInfoVars("AEL_HEADING");
  addInfoVars("BEARING_STAT");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ArrayAngle::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

 if(param == "width") {
    width = (int) atof(val.c_str());
  }

  if(param == "desired_angle") {
     desired_angle = atof(val.c_str());
    return(true);
  }
  

  return true;
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_ArrayAngle::onRunState()  
{
  bool ok1,ok2;
  //get current course
  // Changed to array heading HS 103006
  // double osCourse = getBufferDoubleVal("NAV_HEADING", ok1);
  double osCourse = getBufferDoubleVal("AEL_HEADING", ok1);
  
  //get current tracking state
  string tState = getBufferStringVal("BEARING_STAT", ok2);

  if(!ok1 || !ok2){
    postEMessage("error,BHV_ArrayAngle: ownship data not available");
    return (0);
  }
    
  new_state = decode(tState);

  double relevance = getRelevance();

  if(relevance <= 0)
    return(0);

 
  AOF_ArrayAngle aof_track(m_domain);
  aof_track.setParam("width",width);
  aof_track.setParam("osCourse",osCourse);
  aof_track.setParam("t_bearing",true_bearing);
  aof_track.setParam("desired_angle",desired_angle);
  aof_track.initialize();

  OF_Reflector reflector(&aof_track,1);
 
  reflector.create(m_build_info);
 
  IvPFunction *of = reflector.extractOF();

  of->setPWT(m_priority_wt);
 
  return(of);
}

int BHV_ArrayAngle::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;


  // Parse the waypoint status string for "us"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("error,BHV_CloseRange: Invalid waypoint string");
      return(0);
    }

    string left  = stripBlankEnds(svector2[0]);
    string right = stripBlankEnds(svector2[1]);
   
    if(left == "bearing")   true_bearing  = atof(right.c_str());
    
  }
    
  return(1);
}

double BHV_ArrayAngle::getRelevance()
{
 
  //for now, this behavior is fully relevant when it is active
  return(1.0);
}


/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayAngle.cpp                             */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include "BHV_HArrayAngle.h"
#include "MBUtils.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HArrayAngle::BHV_HArrayAngle(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_ArrayAngleNew");
  this->setParam("unifbox", "course=3");
  this->setParam("gridbox", "course=9");

  m_domain = subDomain(m_domain, "course");
 
  addInfoVars("AEL_HEADING");
  addInfoVars("BEARING_STAT");
  
  over_shoot = 0.0;
  desired_angle = 90.0;

}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HArrayAngle::setParam(string param, string val) 
{

  // Dangerous: you're not returning false on an unrecognized
  // parameter - mikerb, 02/27/08
  // Inherited template! fixed HS 03/06/08

  //  IvPBehavior::setParamCommon(param, val);

  if(IvPBehavior::setParamCommon(param, val))
    return(true);

 if(param == "width") 
   {
     width = (int) atof(val.c_str());
     return(true);
   }

 if(param == "peak_width") 
   {
     pwidth = (int) atof(val.c_str());
     return(true);
   }

 if(param == "over_shoot") 
   {
     over_shoot = atof(val.c_str());
     return(true);
   }

 if(param == "desired_angle") 
   {
     desired_angle = atof(val.c_str());
     return(true);
   }
  
 return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HArrayAngle::onRunState()  
{
  bool ok1,ok2;
  
  //get the heading of the array
  double osCourse = getBufferDoubleVal("AEL_HEADING", ok1);
  
  //get current tracking state
  string tState = getBufferStringVal("BEARING_STAT", ok2);

  if(!ok1 || !ok2){
    postEMessage("error,BHV_HArrayAngle: ownship data not available");
    return (0);
  }
    
  int new_state = decode(tState);

  double current_angle = (osCourse-true_bearing);
  if (current_angle < 0.0)
    current_angle = fabs(current_angle);
  else if(current_angle >= 180.0)
    current_angle = 360- current_angle;

  postMessage("ARRAY_ANGLE",current_angle);

  double relevance = getRelevance();

  if(relevance <= 0)
    return(0);

  // overshoot desired angle
  double auv_desired_angle = desired_angle + over_shoot*(desired_angle-current_angle);
  postMessage("DESIRED_AUV_ANGLE",auv_desired_angle);
  //pick left side desired direction
  double leftabs = (true_bearing - auv_desired_angle);
  if (leftabs < 0.0)
    leftabs += 360.0;

  
  //pick right side desired direction
  double rightabs = (true_bearing + auv_desired_angle);
  if(rightabs > 360.0)
    rightabs -= 360.0;

  //postMessage("LEFTABS",leftabs);
  //postMessage("RIGHTABS",rightabs);
  
  ZAIC_PEAK crs_zaic(m_domain,"course");
  crs_zaic.setSummit(leftabs);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(width);
  crs_zaic.setBaseWidth(90.0 - width);
  crs_zaic.setSummitDelta(50.0);
  crs_zaic.setMinMaxUtil(0,100);

  int new_index = crs_zaic.addComponent();  
  crs_zaic.setSummit(rightabs, new_index);
  crs_zaic.setPeakWidth(width, new_index);
  crs_zaic.setBaseWidth(90.0 - width,new_index);
  crs_zaic.setSummitDelta(50.0, new_index);
  crs_zaic.setMinMaxUtil(0,100,new_index);

  IvPFunction *of = crs_zaic.extractOF();

  of->setPWT(m_priority_wt);
 
  return(of);
}

int BHV_HArrayAngle::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;


  // Parse the waypoint status string for "us"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("error,BHV_HArrayAngle: Invalid waypoint string");
      return(0);
    }

    string left  = stripBlankEnds(svector2[0]);
    string right = stripBlankEnds(svector2[1]);
   
    if(left == "bearing")   true_bearing  = atof(right.c_str());
    
  }
    
  return(1);
}

double BHV_HArrayAngle::getRelevance()
{
 
  //for now, this behavior is fully relevant when it is active
  return(1.0);
}


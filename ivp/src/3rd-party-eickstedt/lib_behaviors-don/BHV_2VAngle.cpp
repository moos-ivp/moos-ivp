/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_2VAngle.cpp                                */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_2VAngle.h"
#include "AOF_WPT2D.h"
#include "AOF_Shadow.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_2VAngle::BHV_2VAngle(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_2VAngle");
  this->setParam("build_info", "uniform_box=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed");

  range_min = 0;
  sign = 1;

  addInfoVars("NAV_X, NAV_Y, NAV_HEADING");
  addInfoVars("TRACK_STAT, V2_X, V2_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_2VAngle::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  if(param == "meas_min") {
     meas_min = (int) atof(val.c_str());
    return(true);
  }

  if(param == "range_min") {
     range_min = atof(val.c_str());
    return(true);
  }
  
  if(param == "range_max") {
     range_max = atof(val.c_str());
    return(true);
  }

  if(param == "sign") {
     sign= (int) atof(val.c_str());
    return(true);
  }
  
  return true;
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_2VAngle::onRunState() 
{
  bool ok1,ok2,ok3,ok4,ok5,ok6;
  //get current course
  double osCourse = getBufferDoubleVal("NAV_HEADING", ok1);
  //get current x
  double osX = getBufferDoubleVal("NAV_X", ok2);
  //get current heading
  double osY = getBufferDoubleVal("NAV_Y", ok3);
  //get current tracking state
  string tState = getBufferStringVal("TRACK_STAT", ok4);
  

  double v2_x = getBufferDoubleVal("V2_X",ok5);
  double v2_y = getBufferDoubleVal("V2_Y",ok6);
  
  
  if(!ok1 || !ok2 || !ok3 ||!ok4||!ok5||!ok6){
    postEMessage("error,BHV_2VAngle: buffer data not available");
    return (0);
  }
    
  new_state = decode(tState);

  //if not tracking do nothing
  //if ((new_state == NO_TRACK)||(new_state == LR_TRACK))
  //  return(0);
  
  double relevance = getRelevance(osX, osY, tx, ty);

  if(relevance <= 0)
    return(0);
  
  double angle1 = atan2(ty-osY,tx-osX);
  double angle2 = atan2(ty-v2_y,tx-v2_x);

  double sep_angle = angle1-angle2;

  if (sep_angle < -180.0)
    sep_angle += 360.0;

  if (sep_angle > 180.0)
    sep_angle = 360.0 - sep_angle;

  sep_angle = fabs(sep_angle)*180.0/M_PI;

  //cout << "sep angle = "<< sep_angle <<endl;
 
  double ang = 90.0 - (heading + (sign)* 135.0);
  
  double d_x = 50.0*cos(ang*M_PI/180.0) + tx;
  double d_y = 50.0*sin(ang*M_PI/180.0) + ty;

  double d_r = sqrt((d_x-osX)*(d_x-osX) + (d_y-osY)*(d_y-osY));

  //cout << "d_r = "<< d_r << "desired_x = " << d_x << "desired_y = " << d_y << endl;

  if(d_r < 5.0){
       
    AOF_Shadow aof(m_domain);
    aof.setParam("cn_crs", heading);
    aof.setParam("cn_spd", speed);
    aof.initialize();

    //AOF_2VAngle aof_angle(domain,heading,speed,osX,osY,tx,ty,v2_x,v2_y);
    
    OF_Reflector reflector(&aof,1);
    
    reflector.create(m_build_info);
    
    IvPFunction *of = reflector.extractOF();
    
    of->setPWT(relevance * m_priority_wt);
    return(of);
  }
  else{
    
    AOF_WPT2D aof(m_domain);
    aof.setParam("oslat", osY);
    aof.setParam("oslon", osX);
    aof.setParam("ptlat", d_y);
    aof.setParam("ptlon", d_x);
    aof.setParam("desired_speed", 3.0);
    aof.initialize();
    //AOF_2VAngle aof_angle(domain,heading,speed,osX,osY,tx,ty,v2_x,v2_y);
    
    OF_Reflector reflector(&aof,1);
    
    reflector.create(m_build_info);
    
    IvPFunction *of = reflector.extractOF();
    
    of->setPWT(relevance * m_priority_wt);
    return(of);
  }
  
  //return(of);
}

int BHV_2VAngle::decode(string status)
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
    if(left == "state")       state = atoi(right.c_str());
    if(left == "x")           tx     = atof(right.c_str());
    if(left == "y")           ty     = atof(right.c_str());
    if(left == "heading")     heading  = atof(right.c_str());
    if(left == "speed")       speed  = atof(right.c_str());
    if(left == "time")        time  = atof(right.c_str());
}
    
  return(state);
}

double BHV_2VAngle::getRelevance(double osX, double osY,
		 		  double cnX, double cnY)
{
  bool silent = true;

  //always fully relevant
  return(1.0);
}


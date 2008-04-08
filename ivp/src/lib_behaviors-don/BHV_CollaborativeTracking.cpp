/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CollaborativeTracking.cpp                  */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_CollaborativeTracking.h"
#include "AOF_CollaborativeTracking.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CollaborativeTracking::BHV_CollaborativeTracking(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)BHV_CollaborativeTracking");
  this->setParam("build_info", "uniform_piece=course:3");
  this->setParam("build_info", "uniform_grid=course:9");

  m_domain = subDomain(m_domain,"course");

  addInfoVars("NAV_X,NAV_Y,TRACK_STAT,V2_X,V2_Y");
  //info_vars.push_back("NAV_X");
  //info_vars.push_back("NAV_Y");
  //info_vars.push_back("TRACK_STAT");
  //info_vars.push_back("V2_X");
  //info_vars.push_back("V2_Y");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CollaborativeTracking::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  if(param == "position_uncertainty") {
     sigmapos = atof(val.c_str());
    return(true);
  }

  if(param == "measurement_uncertainty") {
     sigmatheta = atof(val.c_str());
    return(true);
  }
  
  return true;
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_CollaborativeTracking::onRunState() 
{
  bool ok2,ok3,ok4,ok5,ok6;
  //get current x
  double osX = getBufferDoubleVal("NAV_X", ok2);
  //get current heading
  double osY = getBufferDoubleVal("NAV_Y", ok3);
  //get current tracking state
  string tState = getBufferStringVal("TRACK_STAT", ok4);
  

  double v2_x = getBufferDoubleVal("V2_X",ok5);
  double v2_y = getBufferDoubleVal("V2_Y",ok6);
  
  
  
  if(!ok2 || !ok3 ||!ok4||!ok5||!ok6){
    postEMessage("error,BHV_CollaborativeTracking: buffer data not available");
    return (0);
  }
    
  int new_state = decode(tState);
  
  double relevance = getRelevance();

  if(relevance <= 0)
    return(0);
  
  double angle1 = atan2(ty-osY,tx-osX)*180.0/M_PI;
  double angle2 = atan2(ty-v2_y,tx-v2_x)*180.0/M_PI;

  double sep_angle = angle1-angle2;

  if (fabs(sep_angle) < 180.0)
    sep_angle = fabs(sep_angle);
  else
    sep_angle = -fabs(sep_angle) + 360.0;

  postMessage("BEHAVIOR_APERTURE",sep_angle);

 
  AOF_CollaborativeTracking aof_ctrack(m_domain);
  aof_ctrack.setParam("osY", osY);
  aof_ctrack.setParam("osX", osX);
  aof_ctrack.setParam("target_x",tx );
  aof_ctrack.setParam("target_y",ty );
  aof_ctrack.setParam("cs_x",v2_x );
  aof_ctrack.setParam("cs_y",v2_y );
  aof_ctrack.setParam("sigma_pos", sigmapos);
  aof_ctrack.setParam("sigma_theta", sigmatheta);

  OF_Reflector reflector(&aof_ctrack,1);
    
  reflector.create(m_build_info);
   
  IvPFunction *of = reflector.extractOF();
  
  of->setPWT(relevance*m_priority_wt);
    
return(of);
}
  

int BHV_CollaborativeTracking::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;


  // Parse the waypoint status string for "us"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("error,BHV_CollaborativeTracking: Invalid TRACK_STAT string");
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

double BHV_CollaborativeTracking::getRelevance()
{
  bool silent = true;

  //always fully relevant
  return(1.0);
}



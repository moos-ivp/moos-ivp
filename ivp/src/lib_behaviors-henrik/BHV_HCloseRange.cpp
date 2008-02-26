/***********************************************************/
/*    NAME: henrik Schmidt                                  */
/*    ORIGINAL: Don Eickstedt                                  */
/*    FILE: BHV_HCloseRange.cpp                             */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_HCloseRange.h"
#include "AOF_CutRangeCPA.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method: BHV_HCloseRange(IvPDomain gdomain) : IvPBehavior(gdomain)          *
 *                                                                            *
 *  Description: Constructor for the BHV_HCloseRange behavior class.           *
 *                                                                            * 
 ******************************************************************************/

BHV_HCloseRange::BHV_HCloseRange(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_HCloseRange");
  this->setParam("build_info", "uniform_piece=course:3,speed:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6");

  m_domain = subDomain(m_domain, "course,speed,tol");

  //set default values
  range_min = 0.0;
  range_max = 1.0;
  min_val   = 0.0;
  max_val   = 0.0;

  //subscribe to the necessary MOOS variables
  addInfoVars("TRACK_STAT");
  addInfoVars("NAV_X");
  addInfoVars("NAV_Y");  
 
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_HCloseRange::onIdleState()
{
  // Do your thing here
}


/******************************************************************************
 *                                                                            *
 *  Method:setParam(string,string)                                            *
 *                                                                            *
 *  Description: The setParam method reads the initial behaior parameters     *
 *  from the behavior file and approriately sets the internal variables.      *
 *                                                                            *
 ******************************************************************************/

bool BHV_HCloseRange::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  if(param == "range_min") {
     range_min = atof(val.c_str());
    return(true);
  }
  
  if(param == "range_max") {
     range_max = atof(val.c_str());
    return(true);
  }

  if(param == "max_val") {
     max_val = atof(val.c_str());
    return(true);
  }

  if(param == "min_val") {
     min_val = atof(val.c_str());
    return(true);
  }

  if(param == "use_relevance") {
     val = tolower(val);

    if((val!="true")&&(val!="false"))
      return(false);
    use_relevance = true;

    if(val == "false")
	use_relevance = false;
    return(true);
  }
  
  return true;
}

/******************************************************************************
 *                                                                            *
 *  Method: onRunState()                                                       *
 *                                                                            *
 *  Description: The onRunState method creates and returns the objective       *
 *  function for the BHV_HCloseRange behavior                                  *
 *                                                                            *
 ******************************************************************************/

IvPFunction *BHV_HCloseRange::onRunState() 
{
  bool ok2,ok3,ok4;
  
  string tState;
  //get current tracking state
  tState = getBufferStringVal("TRACK_STAT", ok4);

  //get current x
  double osX = getBufferDoubleVal("NAV_X", ok2);
  
  //get current y
  double osY = getBufferDoubleVal("NAV_Y", ok3);
    
  if(!ok2 || !ok3){
    postEMessage("error,BHV_HCloseRange: ownship data not available");
    return (0);
  }
  
  if(!ok4){
    postEMessage("error,BHV_HCloseRange: tracking data not available");
    return (0);
  }
  
  //decode the contact message
  new_state = decode(tState);
   
 // moving away or approaching?
  double b_ang = atan2(ty-osY,tx-osX);
  double x_b=cos(b_ang);
  double y_b=sin(b_ang);
  double h_ang=(90.0-heading)*M_PI/180.0;
  double x_h=cos(h_ang);
  double y_h=sin(h_ang);
  double v_prod=x_b*y_h-x_h*y_b;
  // scalar product
  if ( (x_b*x_h+y_b*y_h) >= 0 )
    // moving away, head for target
    //    c_course=90.0-b_ang*180.0/M_PI;
    c_course=radToHeading(b_ang);  
else {
  // intercepting
  // left or right? Vector product
    if (v_prod >= 0)
      c_course=angle360(heading+90.0);
    else
      c_course=angle360(heading-90.0);
  }

  // double c_ang = (90.0-c_course)*M_PI/180.0;  
  double c_ang = headingToRadians(c_course);
  double x_c = cos(c_ang);
  double y_c = sin(c_ang);

  double cos_ang=x_c*x_b + y_c*y_b;

  //determine the range to the intercept
  curr_range = cos_ang*sqrt((osX-tx)*(osX-tx)+(osY-ty)*(osY-ty));

  //post a flag if we are within the minimum range
  if (curr_range < range_min)
    {
          //go to loiter behavior
      double orbit_rad = 300;
      char buff[80];
      sprintf(buff,"polygon=radial:%f,%f,%f,6",osX,osY,orbit_rad);
      postMessage("PROSECUTE_LOITER",buff);
      postMessage("IN_RANGE","TRUE");
    }
  //determine the behavior priority (relevance)
  if(use_relevance)
     relevance = getRelevance(curr_range);
  else
     relevance = 1.0;
   
  if(relevance <= 0)
    return(0);
 
  ZAIC_PEAK crs_zaic(m_domain,"course");
  crs_zaic.setSummit(c_course);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(180.0);
  crs_zaic.setSummitDelta(100.0); // mikerb added

  IvPFunction *of = crs_zaic.extractOF();

  //form the objective function
  //  AOF_CutRangeCPA aof(m_domain);
  // aof.setParam("cnlat", ty);
  // aof.setParam("cnlon", tx);
  // aof.setParam("cncrs", heading);
  // aof.setParam("cnspd", speed);
  // aof.setParam("oslat", osY);
  // aof.setParam("oslon", osX);
  // aof.initialize();

  // OF_Reflector reflector(&aof,1);
 
  // reflector.createUniform(unif_box,grid_box);
 
  // IvPFunction *of = reflector.extractOF();

  of->setPWT(relevance * m_priority_wt);

  return(of);
}

/******************************************************************************
 *                                                                            *
 *  Method: decode(string status)                                             *
 *                                                                            *
 *  Description: The decode method parses the tracking state message          *
 *                                                                            *
 ******************************************************************************/

int BHV_HCloseRange::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;


  // Parse the waypoint status string for "us"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("error,BHV_HCloseRange: Invalid waypoint string");
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

/******************************************************************************
 *                                                                            *
 *  Method: getRelevance(target_range)                                        *
 *                                                                            *
 *  Description: The getRelevance method dynamically determines the priority  *
 *  of the behavior based on the range to the target.                         * 
 *                                                                            *
 ******************************************************************************/

double BHV_HCloseRange::getRelevance(double target_range)
{
  
  if(target_range < range_min)
    return(min_val);
  else if (target_range > range_max)
    return(max_val);

  double val = min_val + ((max_val-min_val)/(range_max-range_min))*(target_range-range_min);

  return(val);
}


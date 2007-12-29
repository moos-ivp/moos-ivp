/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CloseRange.cpp                             */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_CloseRange.h"
#include "AOF_CutRangeCPA.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method: BHV_CloseRange(IvPDomain gdomain) : IvPBehavior(gdomain)          *
 *                                                                            *
 *  Description: Constructor for the BHV_CloseRange behavior class.           *
 *                                                                            * 
 ******************************************************************************/

BHV_CloseRange::BHV_CloseRange(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_CloseRange");
  this->setParam("build_info", "uniform_box=course:3,speed:2,tol:2");
  this->setParam("build_info", "uniform_grid=course:9,speed:6,tol:6");

  m_domain = subDomain(m_domain, "course,speed,tol");

  //set default values
  range_min = 0.0;
  range_max = 1.0;
  min_val   = 0.0;
  max_val   = 0.0;

  //subscribe to the necessary MOOS variables
  addInfoVars("TRACK_STAT, NAV_X, NAV_Y");  
}

/******************************************************************************
 *                                                                            *
 *  Method:setParam(string,string)                                            *
 *                                                                            *
 *  Description: The setParam method reads the initial behaior parameters     *
 *  from the behavior file and approriately sets the internal variables.      *
 *                                                                            *
 ******************************************************************************/

bool BHV_CloseRange::setParam(string param, string val) 
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
 *  function for the BHV_CloseRange behavior                                  *
 *                                                                            *
 ******************************************************************************/

IvPFunction *BHV_CloseRange::onRunState() 
{
  bool ok2,ok3,ok4;
  
  string tState;
  //get current tracking state
  tState = m_info_buffer->sQuery("TRACK_STAT", ok4);

  //get current x
  double osX = m_info_buffer->dQuery("NAV_X", ok2);
  
  //get current y
  double osY = m_info_buffer->dQuery("NAV_Y", ok3);
    
  if(!ok2 || !ok3){
    postEMessage("error,BHV_CloseRange: ownship data not available");
    return (0);
  }
  
  if(!ok4){
    postEMessage("error,BHV_CloseRange: tracking data not available");
    return (0);
  }
  
  //decode the contact message
  new_state = decode(tState);
   
  //determine the range to the contact
  curr_range = sqrt((osX-tx)*(osX-tx)+(osY-ty)*(osY-ty));

  //post a flag if we are within the minimum range
  if (curr_range < range_min)
      postMessage("IN_RANGE","TRUE");

  //determine the behavior priority (relevance)
  if(use_relevance)
     relevance = getRelevance(curr_range);
  else
     relevance = 1.0;
   
  if(relevance <= 0)
    return(0);
     
  //form the objective function
  AOF_CutRangeCPA aof(m_domain);
  aof.setParam("cnlat", ty);
  aof.setParam("cnlon", tx);
  aof.setParam("cncrs", heading);
  aof.setParam("cnspd", speed);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.initialize();

  OF_Reflector reflector(&aof,1);
  reflector.create(m_build_info);
  IvPFunction *of = reflector.extractOF();

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

int BHV_CloseRange::decode(string status)
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

/******************************************************************************
 *                                                                            *
 *  Method: getRelevance(target_range)                                        *
 *                                                                            *
 *  Description: The getRelevance method dynamically determines the priority  *
 *  of the behavior based on the range to the target.                         * 
 *                                                                            *
 ******************************************************************************/

double BHV_CloseRange::getRelevance(double target_range)
{
  
  if(target_range < range_min)
    return(min_val);
  else if (target_range > range_max)
    return(max_val);

  double val = min_val + ((max_val-min_val)/(range_max-range_min))*(target_range-range_min);

  return(val);
}


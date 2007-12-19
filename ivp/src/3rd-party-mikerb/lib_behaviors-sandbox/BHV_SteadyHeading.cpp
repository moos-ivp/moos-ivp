/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SteadyHeading.cpp                                */
/*    DATE: Jul 21st 2005                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "BHV_SteadyHeading.h"
#include "ZAIC_PEAK.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_SteadyHeading::BHV_SteadyHeading(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_SteadyHeading");
  domain = subDomain(domain, "course");

  memory_time  = 30;  // Default 30 Seconds
  summit_delta = 75;  // Default
  peak_width   = 90;  // Default
  grace_time   = 2;   // Default 2 seconds
  curr_time    = 0;   // 
  variable_pwt = 0;   

  info_vars.push_back("NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_SteadyHeading::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "memory_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    memory_time = dval;
    return(true);
  }
  
  if(param == "grace_time") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    grace_time = dval;
    return(true);
  }
  
  if(param == "variable_pwt") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    variable_pwt = dval;
    return(true);
  }
  
  if(param == "peak_width") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    peak_width = dval;
    return(true);
  }
  
  if(param == "summit_delta") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    summit_delta = dval;
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: onIdleState()

void BHV_SteadyHeading::onIdleState()
{
  updateCurrHeadingTimeLog();
}


//-----------------------------------------------------------
// Procedure: updateCurrHeadingTimeLog

bool BHV_SteadyHeading::updateCurrHeadingTimeLog() 
{
  if(!info_buffer)
    return(false);

  bool   ok;

  curr_time    = info_buffer->getCurrTime();
  curr_heading = info_buffer->dQuery("NAV_HEADING", ok);
  curr_heading = angle360(curr_heading);

  addHeading(curr_heading, curr_time);

  return(ok);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_SteadyHeading::produceOF() 
{
  bool ok = updateCurrHeadingTimeLog();

  // Must get ownship heading from InfoBuffer
  if(!ok) {
    postEMessage("No ownship heading info in info_buffer.");
    return(0);
  }

  double heading_avg;
  ok = getHeadingAvg(heading_avg);

  if(!ok)
    return(0);
  
  double total_dev = 0;
  list<double>::iterator p;
  for(p=heading_val.begin(); p!=heading_val.end(); p++) {
    double hdg = fabs(*p); 
    total_dev = (hdg - heading_avg) * (hdg - heading_avg);
  }  

  total_dev   = sqrt(total_dev);

  double pct = (total_dev / 180);

  cout << "Priority_BAS: " << priority_wt  << endl;
  cout << "Priority_VAR: " << variable_pwt  << endl;
  cout << "PCT: "          << pct  << endl;
  double loc_priority_wt = priority_wt + (pct * variable_pwt);

  cout << "HeadingCur: " << curr_heading << endl;
  cout << "HeadingAvg: " << heading_avg  << endl;
  cout << "Total Dev: " << total_dev     << endl;
  cout << "Priority_AFT: " << loc_priority_wt  << endl;

  /*                        o                         */
  /*                       / \                        */
  /*                      /   \                       */ 
  /*                     /     \                      */ 
  /*                    /       \                     */ 
  /*                   o         o                    */ 
  /*                /   peakwidth   \                 */ 
  /*             /                     \              */ 
  /*          /                           \           */ 
  /*      /      base_width always 180      \         */ 
  

  postMessage("SDY_HDG_AVG", heading_avg);
  postMessage("SDY_HDG_CUR", curr_heading);
  postMessage("SDY_HDG_PWT", loc_priority_wt);

  ZAIC_PEAK crs_zaic(domain, "course");
  crs_zaic.setSummit(curr_heading);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(peak_width);
  crs_zaic.setBaseWidth(180-peak_width);
  crs_zaic.setSummitDelta(summit_delta);

  IvPFunction *ipf = crs_zaic.extractOF();

  if(ipf)
    ipf->setPWT(loc_priority_wt);

  return(ipf);
}


//-----------------------------------------------------------
// Procedure: addHeading(double)

void BHV_SteadyHeading::addHeading(double heading, 
				   double currtime)
{
  heading_val.push_back(heading);
  heading_time.push_back(currtime);
  
  int counter = 0;

  // Remove all stale elements from memory
  list<double>::iterator p;
  for(p = heading_time.begin(); p!=heading_time.end(); p++) {
    double itime = *p;
    if((currtime - itime) > memory_time) {
      counter++;
    }
  }

  for(int i=0; i<counter; i++) {
    heading_val.pop_front();
    heading_time.pop_front();
  }
}


//-----------------------------------------------------------
// Procedure: getHeadingAvg

bool BHV_SteadyHeading::getHeadingAvg(double& heading_avg)
{
  double ssum  = 0.0;
  double csum  = 0.0;
  int    count = 0;

  list<double>::iterator tp = heading_time.begin();
  list<double>::iterator vp;
  for(vp = heading_val.begin(); vp!=heading_val.end(); vp++) {
    double iheading_val  = *vp;
    double iheading_time = *tp;

    double delta_time = (curr_time - iheading_time); 

    if(delta_time > grace_time) {
      double s = sin((iheading_val * M_PI) / 180.0);
      double c = cos((iheading_val * M_PI) / 180.0);
      ssum += s;
      csum += c;    
    }
    tp++;
    count++;
  }

  if(count == 0)
    return(false);

  heading_avg = atan2(ssum,csum)*180.0/M_PI;

  if(heading_avg < 0.0)
    heading_avg += 360.0;

  return(true);
}

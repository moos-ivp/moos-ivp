/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Timutif.cpp                                          */
/*    DATE: Dec 25th, 2006                                       */
/*****************************************************************/
 
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include <stdlib.h>
#include "Timutif.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

Timutif::Timutif(string config_str)
{
  configured = false;
  util_type  = -1;

  if(config_str == "")
    return;

  vector<string> svector = parseString(config_str, ',');
  int vsize = svector.size();
  if(vsize == 0)
    return;

  for(int i=0; i<vsize; i++) 
    svector[i] = stripBlankEnds(svector[i]);

  if(svector[0] == "normal") {
    if(vsize != 5)
      return;
    for(int i=1; i<vsize; i++) 
      if(!isNumber(svector[i]))
	return;

    double tlow = atof(svector[1].c_str());
    double tmed = atof(svector[2].c_str());
    double thgh = atof(svector[3].c_str());
    double umed = atof(svector[4].c_str());
    
    if((tlow < 0) || (tlow > tmed) || (tmed > thgh))
      return;
    if((umed < 0) || (umed > 100))
      return;
    
    time_low   = tlow;
    time_med   = tmed;
    time_high  = thgh;
    util_med   = umed;
    configured = true;
    util_type  = 0;
  }
}

//----------------------------------------------------------------
// Procedure: evalUtility(time)
//                                                              
//   ^                                 o-------------------------
//  u|                             /    (time_high, 100)
//  t|                         /
//  i|                     /
//  l|                 /
//  i|             /
//  t|            o   (time_med, util_med)
//  y|           /
//   |          /
//   |         /
//   |        / 
//   |       /  (time_low, 0)
//   o------o-------------------------------------------------->
//                    total time in grid element
//                      
//   format:  "time_low,time_med,util_med,time_high"                     
//   checks: (1) 0 <= time_low <= time_med <= time_high
//           (2) 0 <= util_med <= 100                   
//           (3) for all value strings, isNumber(val_string) is true

double Timutif::evalUtility(double g_time) const
{
  //cout << time_low << "$" << time_med << "$" << time_high << "$" << util_med << endl;
  if(util_type == 0) {
    // Zone 1
    if(g_time <= time_low)
      return(0);
    // Zone 2
    if(g_time < time_med) {
      double pct  = (g_time - time_low) / (time_med - time_low);
      double util = (pct * util_med);
      return(util);
    }
    // Zone 3
    if(g_time < time_high) {
      double pct  = (g_time - time_med) / (time_high - time_med);
      double util = (pct * (100-util_med)) + util_med;
      return(util);
    }
    // Zone 4
    return(100);
  }

  return(0);
}

//----------------------------------------------------------------
// Procedure: toString

string Timutif::toString()
{
  if(!configured)
    return("");
  
  string str;
  if(util_type == 0) {
    str += "normal";
    str += "," + doubleToString(time_low);
    str += "," + doubleToString(time_med);
    str += "," + doubleToString(time_high);
    str += "," + doubleToString(util_med);
  }

  return(str);
}
  

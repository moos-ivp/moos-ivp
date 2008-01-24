/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: SearchPassValue.cpp                                  */
/*    DATE: 23 Jan 2008                                          */
/*****************************************************************/
 
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include "SearchPassValue.h"
#include "MBUtils.h"
#include <math.h>

using std::string;
using std::cout;
using std::vector;

//-----------------------------------------------------------
// Procedure: Constructor

SearchPassValue::SearchPassValue(string config_str)
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
// Procedure: evalValue(time)
// Value is the log of the number of "passes" - 1 (to set 0 time = 0 value

double SearchPassValue::evalValue(double d_time) const
{
	return log10( d_time );
}
  
  
//----------------------------------------------------------------
// Procedure: toString

string SearchPassValue::toString()
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

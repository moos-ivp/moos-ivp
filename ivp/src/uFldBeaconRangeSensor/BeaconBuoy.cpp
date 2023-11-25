/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BeaconBuoy.cpp                                       */
/*    DATE: Feb 1st, 2011                                        */
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

#include <cstdlib>
#include "BeaconBuoy.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

BeaconBuoy::BeaconBuoy()
{
  m_x = 0;
  m_y = 0;
  m_timestamp = 0;
  m_report_range = 100;

  m_pings_recvd = 0;
  m_pings_repld = 0;
  m_pings_unsol = 0;

  // resonable default values
  m_freq_low   = -1;  // or say 30
  m_freq_hgh   = -1;  // or say 60
  m_frequency  = -1;  // or say 45

  m_width      = 4;
  m_shape      = "circle";
  m_buoy_color = "orange";
}


//-----------------------------------------------------------
// Procedure: setFrequencyRange
//  Examples: "0:10", "5", "40:100"

bool BeaconBuoy::setFrequencyRange(string str)
{
  if(tolower(str) == "never") {
    m_freq_low = -1;
    m_freq_hgh = -1;
    return(true);
  }

  string low = biteStringX(str, ':');
  string hgh = str;

  double lval = atof(low.c_str());
  double hval = atof(hgh.c_str());

  if(hgh == "") {
    double lval = atof(low.c_str());
    if(!isNumber(low) || (lval < 0))
      return(false);
    else 
      hval = lval;
  }
  else {
    if(!isNumber(low) || !isNumber(hgh))
      return(false);
    if((lval < 0) || (hval < 0))
      return(false);
    if(low > hgh) 
      return(false);
  }
  
  m_freq_low = lval;
  m_freq_hgh = hval;
  resetFrequency();
  return(true);
}

//-----------------------------------------------------------
// Procedure: setTimeStamp

bool BeaconBuoy::setTimeStamp(double timestamp)
{
  if(timestamp < m_timestamp)
    return(false);
  
  m_timestamp = timestamp;
  resetFrequency();
  return(true);
}

//-----------------------------------------------------------
// Procedure: setShape

bool BeaconBuoy::setShape(string shape_str)
{
  shape_str = tolower(shape_str);
  if((shape_str == "circle") || (shape_str == "diamond") ||
     (shape_str == "square") || (shape_str == "triangle") ||
     (shape_str == "efield") || (shape_str == "gateway")) {
    m_shape = shape_str;
    return(true);
  }
  return(false);
}

//-----------------------------------------------------------
// Procedure: setPushDist
//      Note: A negative distance means push_dist = infinity.

bool BeaconBuoy::setPushDist(string dist_str)
{
  if((dist_str == "nolimit") || (dist_str == "unlimited")) {
    m_push_dist = -1; 
    return(true);
  }
  
  if(!isNumber(dist_str))
    return(false);
  
  m_push_dist = atof(dist_str.c_str());
  return(true);
}

//-----------------------------------------------------------
// Procedure: setPullDist
//      Note: A negative distance means pull_dist = infinity.

bool BeaconBuoy::setPullDist(string dist_str)
{
  if((dist_str == "nolimit") || (dist_str == "unlimited")) {
    m_pull_dist = -1; 
    return(true);
  }
  
  if(!isNumber(dist_str))
    return(false);
  
  m_pull_dist = atof(dist_str.c_str());
  return(true);
}


//-----------------------------------------------------------
// Procedure: setWidth

bool BeaconBuoy::setWidth(string width)
{
  if(!isNumber(width))
    return(false);

  double double_width = atof(width.c_str());
  if(double_width < 0)
    return(false);

  m_width = double_width;
  return(true);
}


//-----------------------------------------------------------
// Procedure: setBuoyColor()
// Procedure: setPulseLineColor()
// Procedure: setPulseFillColor()

bool BeaconBuoy::setBuoyColor(string color_str)
{
  if(!isColor(color_str))
    return(false);
  m_buoy_color = color_str;
  return(true);
}

bool BeaconBuoy::setPulseLineColor(string color_str)
{
  if(!isColor(color_str))
    return(false);
  m_pulse_line_color = color_str;
  return(true);
}

bool BeaconBuoy::setPulseFillColor(string color_str)
{
  if(!isColor(color_str))
    return(false);
  m_pulse_fill_color = color_str;
  return(true);
}

//-----------------------------------------------------------
// Procedure: getSpec()
//      Note: Returning the "full_spec" string includes features
//            of the beacon that do not have meaning for rendering.


string BeaconBuoy::getSpec(bool full_spec) const
{
  string str;
  str += "x=" + doubleToStringX(m_x);
  str += ",y=" + doubleToStringX(m_y);
  if(m_label != "")
    str += ",label=" + m_label;
  if(m_buoy_color != "")
    str += ",color=" + m_buoy_color;
  if(m_shape != "")
    str += ",type=" + m_shape;
  if(m_width > 0)
    str += ",width=" + doubleToStringX(m_width);
  if(full_spec) {
    str += ",freq=" + doubleToStringX(m_frequency);
    str += ",freq_low=" + doubleToStringX(m_freq_low);
    str += ",freq_hgh=" + doubleToStringX(m_freq_hgh);
    str += ",report_range=" + doubleToStringX(m_report_range);
  }
  
  return(str);
}

//-----------------------------------------------------------
// Procedure: resetFrequency
//      Note: A frequency of -1 means the beacon should never 
//            produce an uncolicited range report.

void BeaconBuoy::resetFrequency()
{
  if((m_freq_low < 0) || (m_freq_hgh < 0) || (m_freq_hgh < m_freq_low)) {
    m_frequency = -1;
    return;
  }

  double range = m_freq_hgh - m_freq_low;
  if(range <= 0)
    m_frequency = m_freq_low;
  
  int    rand_int = rand() % 1000;
  double rand_pct = (double)(rand_int) / 1000;
  
  m_frequency = m_freq_low + (rand_pct * range);
}


//-----------------------------------------------------------
// Procedure: getFreqSetting
//   Purpose: Return a string representation of the frequency config.
//            "30"
//            "[30:60]"

string BeaconBuoy::getFreqSetting() const
{
  if(m_frequency == -1)
    return("poll");

  if(m_freq_low == m_freq_hgh)
    return(doubleToString(m_frequency, 1));

  string str_range = "[" + doubleToStringX(m_freq_low,1) + ":";
  str_range += doubleToStringX(m_freq_hgh,1) + "]";

  return(str_range);
}












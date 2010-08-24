/************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ContactRecord.cpp                               */
/*    DATE: Feb 27th 2010                                   */
/************************************************************/

#include "ContactRecord.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ContactRecord::ContactRecord(const string& name)
{
  m_x   = 0;
  m_y   = 0;
  m_lat = 0;
  m_lon = 0;
  m_speed     = 0;
  m_heading   = 0;
  m_depth     = 0;
  m_timestamp = 0;

  m_x_set   = false;
  m_y_set   = false;
  m_lat_set = false;
  m_lon_set = false;
  m_speed_set     = false;
  m_heading_set   = false;
  m_depth_set     = false;
  m_type_set      = false;
  m_timestamp_set = false;

  m_name  = name;
  m_valid = false;
}

//---------------------------------------------------------------
// Procedure: valid
//      Note: Determines is all the required fields have been set

bool ContactRecord::valid()
{
  if(m_x_set     &&  m_y_set && 
     m_lat_set   &&  m_lon_set && 
     m_speed_set &&  m_heading_set && 
     m_type_set  && m_timestamp_set)
    return(true);
  else
    return(false);
}


//---------------------------------------------------------------
// Procedure: getX, getY, getLat, getLon
//      Note: Determines the vehicle position given it's last known
//            location and time stamp, and the current timestamp.
//      Note: If for some reason the duration between the given 
//            timestamps is less than zero, a duration of zero is
//            used.

double ContactRecord::getX(double timestamp)
{
  return(m_x);
}

double ContactRecord::getY(double timestamp)
{
  return(m_y);
}

double ContactRecord::getLat(double timestamp)
{
  return(m_lat);
}

double ContactRecord::getLon(double timestamp)
{
  return(m_lon);
}


//---------------------------------------------------------------
// Procedure: getValue

string ContactRecord::getValue(string key, double tstamp)
{
  key = tolower(key);
  if(key == "x")
    return(dstringCompact(doubleToString(getX(tstamp), 2)));
  else if(key == "y")
    return(dstringCompact(doubleToString(getY(tstamp), 2)));
  else if(key == "lat")
    return(dstringCompact(doubleToString(getLat(tstamp), 8)));
  else if(key == "lon")
    return(dstringCompact(doubleToString(getLon(tstamp), 8)));
  else if((key == "speed") || (key == "spd"))
    return(dstringCompact(doubleToString(m_speed, 2)));
  else if((key == "heading") || (key == "hdg"))
    return(dstringCompact(doubleToString(m_lon, 2)));
  else if((key == "depth") || (key == "dep"))
    return(dstringCompact(doubleToString(m_depth, 2)));
  else if((key == "timestamp") || (key == "time") || (key == "utime"))
    return(dstringCompact(doubleToString(m_timestamp, 2)));
  else
    return("");  
}


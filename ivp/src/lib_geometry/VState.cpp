/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VState.cpp                                           */
/*    DATE: Oct 25th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include "VState.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

VState::VState() 
{
  m_x      = 0;
  m_y      = 0;
  m_lat    = 0;
  m_lon    = 0;
  m_hdg    = 0;
  m_hog    = 0;
  m_spd    = 0;
  m_sog    = 0;
  m_dep    = 0;
  m_yaw    = 0;

  m_x_ts   = 0;
  m_y_ts   = 0;
  m_lat_ts = 0;
  m_lon_ts = 0;
  m_hdg_ts = 0;
  m_hog_ts = 0;
  m_spd_ts = 0;
  m_sog_ts = 0;
  m_dep_ts = 0;
  m_yaw_ts = 0;

  m_time_utc = 0;
}


//----------------------------------------------------------------
// Procedure: isSet()

bool VState::isSet(string param) const
{
  return(timeSet(param) > 0);
}


//----------------------------------------------------------------
// Procedure: timeSet()

double VState::timeSet(string param) const
{
  if(param == "x")
    return(m_x_ts);
  else if(param == "y")
    return(m_y_ts);
  else if(param == "lat")
    return(m_lat_ts);
  else if(param == "lon")
    return(m_lon_ts);
  else if((param == "hdg") || (param == "heading"))
    return(m_hdg_ts);
  else if((param == "spd") || (param == "speed"))
    return(m_spd_ts);
  else if((param == "dep") || (param == "dep"))
    return(m_dep_ts);
  else if((param == "hog") || (param == "heading_over_ground"))
    return(m_hog_ts);
  else if((param == "sog") || (param == "speed_over_ground"))
    return(m_sog_ts);
  else if(param == "yaw")
    return(m_yaw_ts);

  return(false);
}


//----------------------------------------------------------------
// Procedure: lastUpdate()

double VState::lastUpdate() const
{
  double last_update = 0;
  if(m_x_ts > last_update) 
    last_update = m_x_ts;
  if(m_y_ts > last_update) 
    last_update = m_y_ts;
  if(m_lat_ts > last_update) 
    last_update = m_lat_ts;
  if(m_lon_ts > last_update) 
    last_update = m_lon_ts;
  if(m_hdg_ts > last_update) 
    last_update = m_hdg_ts;
  if(m_spd_ts > last_update) 
    last_update = m_spd_ts;
  if(m_dep_ts > last_update) 
    last_update = m_dep_ts;
  if(m_hog_ts > last_update) 
    last_update = m_hog_ts;
  if(m_sog_ts > last_update) 
    last_update = m_sog_ts;
  if(m_yaw_ts > last_update) 
    last_update = m_yaw_ts;
  return(last_update);
}



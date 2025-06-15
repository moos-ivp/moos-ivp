/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LedgerSnap.cpp                                       */
/*    DATE: Dec 12th 2024 Thanksgiving in Waterloo               */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#include <iostream>
#include "LedgerSnap.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: setX(), setY(), setHdg(), setSpd(), setDep()

void LedgerSnap::setX(string vname, double dval)
{
  m_map_xpos[vname] = dval;
}
void LedgerSnap::setY(string vname, double dval)
{
  m_map_ypos[vname] = dval;
}
void LedgerSnap::setHdg(string vname, double dval)
{
  m_map_hdg[vname] = dval;
}
void LedgerSnap::setSpd(string vname, double dval)
{
  m_map_spd[vname] = dval;
}
void LedgerSnap::setDep(string vname, double dval)
{
  m_map_dep[vname] = dval;
}

//-----------------------------------------------------------
// Procedure: setLat(), setLon()

void LedgerSnap::setLat(string vname, double dval)
{
  m_map_lat[vname] = dval;
}
void LedgerSnap::setLon(string vname, double dval)
{
  m_map_lon[vname] = dval;
}

//-----------------------------------------------------------
// Procedure: setUTC(), setUTCAge()

void LedgerSnap::setUTC(string vname, double dval)
{
  m_map_utc[vname] = dval;
}
void LedgerSnap::setUTCAge(string vname, double dval)
{
  m_map_utc_age[vname] = dval;
}


//-----------------------------------------------------------
// Procedure: setUTCReceived(), setAgeReceived()

void LedgerSnap::setUTCReceived(string vname, double dval)
{
  m_map_utc_received[vname] = dval;
}
void LedgerSnap::setUTCAgeReceived(string vname, double dval)
{
  m_map_utc_age_received[vname] = dval;
}


//-----------------------------------------------------------
// Procedure: setGroup(), setType(), setVSource()

void LedgerSnap::setGroup(string vname, string sval)
{
  m_map_group[vname] = sval;
}
void LedgerSnap::setType(string vname, string sval)
{
  m_map_type[vname] = sval;
}
void LedgerSnap::setVSource(string vname, string sval)
{
  m_map_vsource[vname] = sval;
}



//-----------------------------------------------------------
// Procedure: getInfoDouble()

double LedgerSnap::getInfoDouble(std::string vname,
				 std::string fld,
				 bool& ok) const
{
  //vname = toupper(vname);

  ok = true;
  map<string, double>::const_iterator p;
  if(fld == "x") {
    p = m_map_xpos.find(vname);
    if(p != m_map_xpos.end())
      return(p->second);
  }
  else if(fld == "y") {
    p = m_map_ypos.find(vname);
    if(p != m_map_ypos.end())
      return(p->second);
  }
  else if(fld == "hdg") {
    p = m_map_hdg.find(vname);
    if(p != m_map_hdg.end())
      return(p->second);
  }
  else if(fld == "spd") {
    p = m_map_spd.find(vname);
    if(p != m_map_spd.end())
      return(p->second);
  }
  else if(fld == "dep") {
    p = m_map_dep.find(vname);
    if(p != m_map_dep.end())
      return(p->second);
  }
  else if(fld == "lat") {
    p = m_map_lat.find(vname);
    if(p != m_map_lat.end())
      return(p->second);
  }
  else if(fld == "lon") {
    p = m_map_lon.find(vname);
    if(p != m_map_lon.end())
      return(p->second);
  }
  else if(fld == "utc") {
    p = m_map_utc.find(vname);
    if(p != m_map_utc.end())
      return(p->second);
  }
  else if(fld == "utc_age") {
    p = m_map_utc_age.find(vname);
    if(p != m_map_utc_age.end())
      return(p->second);
  }
  else if(fld == "utc_received") {
    p = m_map_utc_received.find(vname);
    if(p != m_map_utc_received.end())
      return(p->second);
  }
  else if(fld == "utc_age_received") {
    p = m_map_utc_age_received.find(vname);
    if(p != m_map_utc_age_received.end())
      return(p->second);
  }

  ok = false;
  return(0);
}

//-----------------------------------------------------------
// Procedure: getInfoString()

string LedgerSnap::getInfoString(std::string vname,
				 std::string fld,
				 bool& ok) const
{
  //vname = toupper(vname);
  ok = true;
  map<string, string>::const_iterator p;
  if(fld == "group") {
    p = m_map_group.find(vname);
    if(p != m_map_group.end())
      return(p->second);
  }
  else if(fld == "type") {
    p = m_map_type.find(vname);
    if(p != m_map_type.end())
      return(p->second);
  }  
  else if(fld == "vsource") {
    p = m_map_vsource.find(vname);
    if(p != m_map_vsource.end())
      return(p->second);
  }
  
  ok = false;
  return("");
}

//-----------------------------------------------------------
// Procedure: hasVName()

bool LedgerSnap::hasVName(string vname) const
{
  if(m_map_xpos.count(vname) == 0)
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: clear()

void LedgerSnap::clear()
{
  m_map_xpos.clear();
  m_map_ypos.clear();
  m_map_hdg.clear();
  m_map_spd.clear();
  m_map_dep.clear();
  m_map_lat.clear();
  m_map_lon.clear();
  m_map_utc.clear();
  m_map_utc_age.clear();
  m_map_utc_received.clear();
  m_map_utc_age_received.clear();

  m_map_group.clear();
  m_map_type.clear();

  m_curr_time_utc = 0;
}

//-----------------------------------------------------------
// Procedure: size()

unsigned int LedgerSnap::size() const
{
  unsigned int max_size = 0;

  if(m_map_xpos.size() > max_size)
    max_size = m_map_xpos.size();
  if(m_map_ypos.size() > max_size)
    max_size = m_map_ypos.size();
  if(m_map_hdg.size() > max_size)
    max_size = m_map_hdg.size();
  if(m_map_spd.size() > max_size)
    max_size = m_map_spd.size();
  if(m_map_dep.size() > max_size)
    max_size = m_map_dep.size();

  if(m_map_lat.size() > max_size)
    max_size = m_map_lat.size();
  if(m_map_lon.size() > max_size)
    max_size = m_map_lon.size();

  if(m_map_utc.size() > max_size)
    max_size = m_map_utc.size();
  if(m_map_utc_age.size() > max_size)
    max_size = m_map_utc_age.size();
  if(m_map_utc_received.size() > max_size)
    max_size = m_map_utc_received.size();
  if(m_map_utc_age_received.size() > max_size)
    max_size = m_map_utc_age_received.size();
  
  if(m_map_group.size() > max_size)
    max_size = m_map_group.size();
  if(m_map_type.size() > max_size)
    max_size = m_map_type.size();
  if(m_map_vsource.size() > max_size)
    max_size = m_map_vsource.size();

  return(max_size);
}

//-----------------------------------------------------------
// Procedure: getSpec(vname)

string LedgerSnap::getSpec(string vname) const
{
  return("");
}




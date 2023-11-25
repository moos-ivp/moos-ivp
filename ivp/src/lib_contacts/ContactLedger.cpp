/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactLedger.cpp                                    */
/*    DATE: Aug 31st 2022                                        */
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

#include <iostream>
#include "ContactLedger.h"
#include "LinearExtrapolator.h"
#include "NodeRecordUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

ContactLedger::ContactLedger()
{
  m_curr_time_utc = 0;
}

//---------------------------------------------------------------
// Procedure: extrapolate()

void ContactLedger::extrapolate()
{
  map<string,NodeRecord>::iterator p;
  for(p=m_map_records_rep.begin(); p!=m_map_records_rep.end(); p++) {
    string vname = p->first;
    extrapolate(vname);
  }
}

//---------------------------------------------------------------
// Procedure: extrapolate()

void ContactLedger::extrapolate(string vname)
{
  if(!hasVName(vname))
    return;
  
  NodeRecord record_rep = m_map_records_rep[vname];
  double x = record_rep.getX();
  double y = record_rep.getY();
  double hdg = record_rep.getHeading();
  double spd = record_rep.getSpeed();
  double utc = record_rep.getSpeed();
  
  LinearExtrapolator extrapolator;
  
  extrapolator.setPosition(x, y, spd, hdg, utc);

  double new_x, new_y;
  bool ok = extrapolator.getPosition(new_x, new_y, m_curr_time_utc);

  if(!ok) {
    string failure_reason = extrapolator.getFailureReason();
    cout << "Extrap Fail: " << failure_reason << endl;
  }
  else {
    m_map_records_ext[vname].setX(new_x);
    m_map_records_ext[vname].setY(new_y);
  }
}

//---------------------------------------------------------------
// Procedure: processReport()

bool ContactLedger::processReport(string report)
{
  NodeRecord new_record = string2NodeRecord(report);

  if(!new_record.valid("name,x,y,time"))
    return(false);

  string raw_vname = new_record.getName();
  string vname = toupper(raw_vname);
  
  m_map_records_rep[vname] = new_record;
  m_map_records_ext[vname] = new_record;

  return(true);
}

//---------------------------------------------------------------
// Procedure: processRecord()

bool ContactLedger::processRecord(NodeRecord new_record)
{
  if(!new_record.valid("name,x,y,time"))
    return(false);
  
  string raw_vname = new_record.getName();
  string vname = toupper(raw_vname);
  
  m_map_records_rep[vname] = new_record;
  m_map_records_ext[vname] = new_record;

  return(true);
}


//---------------------------------------------------------------
// Procedure: hasVName()

bool ContactLedger::hasVName(string vname) const
{
  vname = toupper(vname);
  
  if(m_map_records_rep.count(vname) == 0)
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: getX()

double ContactLedger::getX(string vname, bool extrap) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getX());
}


//---------------------------------------------------------------
// Procedure: getY()

double ContactLedger::getY(string vname, bool extrap) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname);
  return(record.getY());
}


//---------------------------------------------------------------
// Procedure: getSpeed()

double ContactLedger::getSpeed(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getSpeed());
}

//---------------------------------------------------------------
// Procedure: getHeading()

double ContactLedger::getHeading(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getHeading());
}


//---------------------------------------------------------------
// Procedure: getDepth()

double ContactLedger::getDepth(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getDepth());
}


//---------------------------------------------------------------
// Procedure: getLat()

double ContactLedger::getLat(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLat());
}


//---------------------------------------------------------------
// Procedure: getLon()

double ContactLedger::getLon(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getLon());
}

//---------------------------------------------------------------
// Procedure: getGroup()

string ContactLedger::getGroup(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getGroup());
}

//---------------------------------------------------------------
// Procedure: getType()

string ContactLedger::getType(string vname) const
{
  if(!hasVName(vname))
    return(0);

  NodeRecord record = getRecord(vname, false);
  return(record.getType());
}


//---------------------------------------------------------------
// Procedure: getRecord()

NodeRecord ContactLedger::getRecord(string vname, bool extrap) const
{
  vname = toupper(vname);
  
  if(extrap) {
    map<string,NodeRecord>::const_iterator q=m_map_records_ext.find(vname);
    if(q!=m_map_records_ext.end())
      return(q->second);
  }
  else {
    map<string,NodeRecord>::const_iterator q=m_map_records_rep.find(vname);
    if(q!=m_map_records_rep.end())
      return(q->second);  
  }
  
  NodeRecord null_record;
  return(null_record);
}



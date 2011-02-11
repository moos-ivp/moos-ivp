/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Demuxer.cpp                                          */
/*    DATE: May 20th 2006                                        */
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

#include <iostream>
#include "Demuxer.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: getDemuxString()
//      Note: May be several strings in the m_demuxed_strings list
//            but we only return first one here. It is expected
//            that the user would make repeated calls until an 
//            empty string is returned.

string Demuxer::getDemuxString()
{
  if(!m_demuxed)
    demuxUnits();

  if(m_demuxed_strings.size() == 0)
    return("");
  
  string dstring = m_demuxed_strings.front();

  m_demuxed_strings.pop_front();
  m_demuxed_strings_timestamp.pop_front();

  return(dstring);
}

//-------------------------------------------------------------
// Procedure: getDemuxString(double&)
//      Note: Same as the getDemuxString() function but will
//            return the timestamp as well.

string Demuxer::getDemuxString(double& timestamp)
{
  if(!m_demuxed)
    demuxUnits();

  if(m_demuxed_strings.size() == 0)
    return("");
  
  string dstring = m_demuxed_strings.front();
  double tstamp  = m_demuxed_strings_timestamp.front();

  m_demuxed_strings.pop_front();
  m_demuxed_strings_timestamp.pop_front();

  timestamp = tstamp;
  return(dstring);
}


//-------------------------------------------------------------
// Procedure: addMuxPacket()
//      Note: When a packet is added, it is added to the existing
//            related DemuxUnit. If it doesn't exist, one is 
//            created. 
//      Note: If time_stamps are provided, a time is associated
//            with the creation of a DemuxUnit. This allows the
//            DemuxUnits to be checked for staleness, and avoid
//            potential runaway memory growth if packets are dropped.

bool Demuxer::addMuxPacket(const string& str, double time_stamp)
{
  // With the addition of any new packet, m_demuxed_strings structure
  // is labeled not up-to-date by setting demuxed=false. 
  m_demuxed = false;

  // Determine the "ID" of the string
  int cix = 2;
  string str_id = "";
  while(str[cix] != ',') {
    str_id += str[cix];
    cix++;
  }

  // Determine the number of total packets
  cix++;
  int str_total = 0;
  while(str[cix] != ',') {
    str_total *= 10;
    str_total += (int)(str[cix]-48);
    cix++;
  }

  // Determine the index of this packet
  cix++;
  int str_ix = 0;
  while(str[cix] != ',') {
    str_ix *= 10;
    str_ix += (int)(str[cix]-48);
    cix++;
  }
  str_ix--;
  cix++;
  
  string str_body = str.c_str() + cix;
 
  bool ok, prior_unit = false;
  list<DemuxUnit*>::iterator p;
  for(p = m_units.begin(); p!=m_units.end(); p++) {
    DemuxUnit *unit = *p;
    if(unit->getUnitID() == str_id) {
      prior_unit = true;
      ok = unit->addString(str_body, str_ix);
    }
  }

  if(prior_unit)
    return(ok);

  DemuxUnit *new_unit = new DemuxUnit(str_id, str_total, time_stamp);
  m_units.push_back(new_unit);

  ok = new_unit->addString(str_body, str_ix);

  return(ok);
}


//-------------------------------------------------------------
// Procedure: demuxUnits()
//   Purpose: Iterate through the DemuxUnits and check if the Unit
//            has received all its packets and is ready to give up
//            its combined, demuxed string. If so, we add it to 
//            the list of m_demuxed_strings and delete the DemuxUnit.
//            If its not ready, it is just left alone.

void Demuxer::demuxUnits()
{
  if(m_demuxed)
    return;
  list<DemuxUnit*>::iterator p1, pd;
  for(p1 = m_units.begin(); p1!=m_units.end();) {
    DemuxUnit *unit = *p1;
    pd = p1;
    p1++;
    if(unit->unitReady()) {
      string demux_string = unit->getDemuxString();
      double demux_tstamp = unit->getTimeStamp();
      m_demuxed_strings.push_back(demux_string);
      m_demuxed_strings_timestamp.push_back(demux_tstamp);
      delete(unit);
      m_units.erase(pd);
    }
  }
  m_demuxed = true;

#if 0
  if(m_demuxed)
    return;
  list<DemuxUnit*>::iterator p1;
  for(p1 = m_units.begin(); p1!=m_units.end(); p1++) {
    DemuxUnit *unit = *p1;
    if(unit->unitReady()) {
      string demux_string = unit->getDemuxString();
      double demux_tstamp = unit->getTimeStamp();
      m_demuxed_strings.push_back(demux_string);
      m_demuxed_strings_timestamp.push_back(demux_tstamp);
      //delete(unit);
      m_units.erase(p1);
    }
  }
  m_demuxed = true;
#endif
}

//-------------------------------------------------------------
// Procedure: removeStaleUnits()
//   Purpose: Check each of the DemuxUnits for staleness. 
//            A DemuxUnit is stale if it was first created further
//            in the past than allow_time seconds ago./
//      Note: For this to work, the user must provide a timestamp
//            when adding a packet. If a timestamp is not provided
//            the DemuxUnit will never be removed for staleness.

void Demuxer::removeStaleUnits(double curr_time, double allow_time)
{
  list<DemuxUnit*>::iterator p1;
  for(p1 = m_units.begin(); p1!=m_units.end(); p1++) {
    DemuxUnit *unit = *p1;
    double time_stamp = unit->getTimeStamp();
    if(time_stamp != 0) {
      if((curr_time - time_stamp) > allow_time) {
	delete(unit);
	m_units.erase(p1);
      }
    }
  }
}


//-------------------------------------------------------------
// Procedure: print()
// 

void Demuxer::print()
{
  demuxUnits();
  cout << "Begin Demuxer::print()" << endl;
  cout << "DemuxUnits:" << endl;
  list<DemuxUnit*>::iterator p1;
  for(p1 = m_units.begin(); p1!=m_units.end(); p1++) {
    DemuxUnit *unit = *p1;
    unit->print();
  }

  cout << "ReadyStrings: (" << m_demuxed_strings.size() << ")" << endl;
  list<string>::iterator p2;
  for(p2 = m_demuxed_strings.begin(); p2!=m_demuxed_strings.end(); p2++) {
    string str = *p2;
    cout << str << endl;
  }

  cout << "End Demuxer::print()" << endl;
}









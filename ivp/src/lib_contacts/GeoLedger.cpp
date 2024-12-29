/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GeoLedger.cpp                                        */
/*    DATE: Dec 28th 2024                                        */
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

#include <cmath>
#include <iostream>
#include "NodeRecordUtils.h"
#include "GeoLedger.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

GeoLedger::GeoLedger(ContactLedgerX *ledger)
{
  m_ledger = ledger;
}

//---------------------------------------------------------------
// Procedure: setGeodesy()

void GeoLedger::setGeodesy(CMOOSGeodesy geodesy)
{
  m_geodesy = geodesy;
  m_geodesy_init = true;  
  m_geodesy_updates++;

  updateLocalCoords();
}

//---------------------------------------------------------------
// Procedure: setGeodesy()

bool GeoLedger::setGeodesy(double dlat, double dlon)
{
  bool ok_init = m_geodesy.Initialise(dlat, dlon);
  if(!ok_init)
    return(false);
  
  m_geodesy_init = true;
  m_geodesy_updates++;

  updateLocalCoords();

  return(true);
}

//---------------------------------------------------------------
// Procedure: processNodeReport()
//   Returns: If successful, returns name of vehicle just added
//            Otherwise empty string

string GeoLedger::processNodeReport(string report,
				    string& whynot)
{
  NodeRecord record = string2NodeRecord(report);

  // Check 1: Must have a name
  string vname = record.getName();
  if(vname == "") {
    whynot = "Missing vname";    
    return("");
  }
  
  // Check 2: Must have a timestamp
  if(!record.isSetTimeStamp()) {
    whynot = "Missing timestamp";
    return("");
  }

  // Check 3: Must have either Lat/Lon or X/Y
  if(!record.isSetLatLon() && !record.isSetXY()) {
    whynot = "Missing both x/y and lat/lon";
    return("");
  }

  // Cross-fill coords if one or the other is missing
  if(record.isSetLatLon() && !record.isSetXY())
    updateLocalCoords(record);
  else if(!record.isSetLatLon() && record.isSetXY())
    updateGlobalCoords(record);

  m_ledger->setRecord(vname, record);
  return(vname);    
}


//---------------------------------------------------------------
// Procedure: updateLocalCoords()
//   Purpose: After a datum update, all X/Y values stored with any
//            contact are updated to be related to the new datum.

void GeoLedger::updateLocalCoords()
{
  vector<string> vnames = m_ledger->getVNames();

  for(unsigned int i=0; i<vnames.size(); i++) {
    string vname = vnames[i];
    NodeRecord record = m_ledger->getRecord(vname);
    
    updateLocalCoords(record);

    m_ledger->setRecord(vname, record);
  }
}

//---------------------------------------------------------------
// Procedure: updateLocalCoords()

void GeoLedger::updateLocalCoords(NodeRecord& record)
{
  double nav_lat = record.getLat();
  double nav_lon = record.getLon();
  double nav_x, nav_y;

#ifdef USE_UTM
  m_geodesy.LatLong2LocalUTM(nav_lat, nav_lon, nav_y, nav_x);
#else
  m_geodesy.LatLong2LocalGrid(nav_lat, nav_lon, nav_y, nav_x);
#endif      

  record.setX(nav_x);
  record.setY(nav_y);
}    

//---------------------------------------------------------------
// Procedure: updateGlobalCoords()
//      Note: For determining lat/lon based on x/y
//            WE REALLY DONT WANT TO BE DOING THIS IN GENERAL
//            (1) for backward compatibility where node reports
//            are being shared only with x/y, we support this.
//            Node reports with x/y will be deprecated eventually.
//            (2) For updating lat/lon after extrapolating in x/y

void GeoLedger::updateGlobalCoords(NodeRecord& record)
{
  double nav_x = record.getX();
  double nav_y = record.getY();
  double nav_lat, nav_lon;

#ifdef USE_UTM
  m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
  m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif      

  record.setLat(nav_lat);
  record.setLon(nav_lon);  
}    


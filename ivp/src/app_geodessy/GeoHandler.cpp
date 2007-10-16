/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeoHandler.cpp                                       */
/*    DATE: September 30th, 2007                                 */
/*****************************************************************/

#include <math.h>
#include <iostream>
#include "MBUtils.h"
#include "GeoHandler.h"

using namespace std;

GeoHandler::GeoHandler()
{
  m_lat1 = 0;    m_lat1_set = false;  
  m_lon1 = 0;    m_lon1_set = false;
  m_lat2 = 0;    m_lat2_set = false;
  m_lon2 = 0;    m_lon2_set = false;

  m_x1   = 0;      m_x1_set = false;
  m_y1   = 0;      m_y1_set = false;
  m_x2   = 0;      m_x2_set = false;
  m_y2   = 0;      m_y2_set = false;

  m_datum_set = false;
  m_datum_lat = 0;
  m_datum_lon = 0;
}

//-------------------------------------------------------------
// Procedure: setDatum

bool GeoHandler::setDatum(double datum_lat, double datum_lon)
{
  m_datum_lat = datum_lat;
  m_datum_lon = datum_lon;
  m_datum_set = m_geodesy.Initialise(datum_lat, datum_lon);
  return(m_datum_set);
}  


//-------------------------------------------------------------
// Procedure: setDatum

bool GeoHandler::setDatum(string datum_lat, string datum_lon)
{
  m_datum_lat = atof(datum_lat.c_str());
  m_datum_lon = atof(datum_lon.c_str());
  m_datum_set = m_geodesy.Initialise(m_datum_lat, m_datum_lon);
  return(m_datum_set);
}  


//-------------------------------------------------------------
// Procedure: setLat

void GeoHandler::setLat(int ix, double val)
{
  if(ix==1) {
    m_lat1 = val;
    m_lat1_set = true;
  }

  if(ix==2) {
    m_lat2 = val;
    m_lat2_set = true;
  }
}  

//-------------------------------------------------------------
// Procedure: setLon

void GeoHandler::setLon(int ix, double val)
{
  if(ix==1) {
    m_lon1 = val;
    m_lon1_set = true;
  }

  if(ix==2) {
    m_lon2 = val;
    m_lon2_set = true;
  }
}  


//-------------------------------------------------------------
// Procedure: setMetersX

void GeoHandler::setMetersX(int ix, double val)
{
  if(ix==1) {
    m_x1 = val;
    m_x1_set = true;
  }

  if(ix==2) {
    m_x2 = val;
    m_x2_set = true;
  }
}  

//-------------------------------------------------------------
// Procedure: setMetersY

void GeoHandler::setMetersY(int ix, double val)
{
  if(ix==1) {
    m_y1 = val;
    m_y1_set = true;
  }

  if(ix==2) {
    m_y2 = val;
    m_y2_set = true;
  }
}  

//-------------------------------------------------------------
// Procedure: query
//
// Types of Queries:
//   Given mx, my                  --> lat, lon
//   Given lat, lon                --> mx, my
//   Given lat1, lon1, lat2, lon2  --> distance in meters
//   Given x1, y1, x2, y2          --> distance in meters
//   Given lat, lon, mx, my        --> distance in meters

void GeoHandler::query()
{
  if((m_x1_set) && (m_y1_set) && 
     (!m_x2_set) && (!m_y2_set) &&
     (!m_lat1_set) && (!m_lon1_set) &&
     (!m_lat2_set) && (!m_lon2_set)) {
    queryPosLocal2LatLong();
    return;
  }
  
  if((!m_x1_set) && (!m_y1_set) && 
     (!m_x2_set) && (!m_y2_set) &&
     (m_lat1_set) && (m_lon1_set) &&
     (!m_lat2_set) && (!m_lon2_set)) {
    queryPosLatLong2Local();
    return;
  }
  
  if((!m_x1_set) && (!m_y1_set) && 
     (!m_x2_set) && (!m_y2_set) &&
     (m_lat1_set) && (m_lon1_set) &&
     (m_lat2_set) && (m_lon2_set)) {
    queryDistLatLongLatLong();
    return;
  }
  
}  

//-------------------------------------------------------------
// Procedure: queryPosLocal2LatLong

void GeoHandler::queryPosLocal2LatLong()
{
  bool ok_input = true;
  if(!m_datum_set || (!m_x1_set) || (!m_y1_set))
    ok_input = false;


  cout << endl << endl;
  cout << "Given Datum: " << endl;
  cout << "  LatOrigin: " << doubleToString(m_datum_lat,6) << endl;
  cout << "  LonOrigin: " << doubleToString(m_datum_lon,6) << endl;
  cout << "Given Query:       " << endl;
  cout << "  Meters-X:  " << m_x1 << endl;
  cout << "  Meters-Y:  " << m_y1 << endl;
  
  if(!ok_input) {
    cout << "Problems with input - Query aborted." << endl;
    return;
  }
  
  double new_lat, new_lon;
  bool ok_trans = m_geodesy.LocalGrid2LatLong(m_x1, m_y1, new_lat, new_lon);
  if(!ok_trans) {
    cout << "Problem with the MOOSGeodesy translation - exiting." << endl;
    exit(0);
  }
  else {
    cout << "Resulting New Lat/Lon Position: " << endl;
    cout << "  Latitude:   " << doubleToString(new_lat,6) << endl;
    cout << "  Longitude:  " << doubleToString(new_lon,6) << endl;
  }
}  

//-------------------------------------------------------------
// Procedure: queryPosLatLong2Local

void GeoHandler::queryPosLatLong2Local()
{
  bool ok_input = true;
  if(!m_datum_set || (!m_lat1_set) || (!m_lon1_set))
    ok_input = false;


  cout << endl << endl;
  cout << "Given Datum: " << endl;
  cout << "  LatOrigin: " << doubleToString(m_datum_lat,6) << endl;
  cout << "  LonOrigin: " << doubleToString(m_datum_lon,6) << endl;
  cout << "Given Query:       " << endl;
  cout << "  Latitude:  " << doubleToString(m_lat1,6) << endl;
  cout << "  Longitude: " << doubleToString(m_lon1,6) << endl;
  
  if(!ok_input) {
    cout << "Problems with input - Query aborted." << endl;
    return;
  }
  
  double new_x, new_y;
  bool ok_trans = m_geodesy.LatLong2LocalGrid(m_lat1, m_lon1, new_y, new_x);
  if(!ok_trans) {
    cout << "Problem with the MOOSGeodesy translation - exiting." << endl;
    exit(0);
  }
  else {
    cout << "Resulting New XY Local Grid Position: " << endl;
    cout << "  X:   " << doubleToString(new_x,5) << endl;
    cout << "  Y:   " << doubleToString(new_y,5) << endl;
  }
}  

//-------------------------------------------------------------
// Procedure: queryDistLatLongLatLong

void GeoHandler::queryDistLatLongLatLong()
{
  bool ok_input = true;
  if(!m_datum_set || (!m_lat1_set) || (!m_lon1_set) || 
     (!m_lat2_set) || (!m_lon2_set))
    ok_input = false;


  cout << endl << endl;
  cout << "Given Datum: " << endl;
  cout << "  LatOrigin: " << doubleToString(m_datum_lat,6) << endl;
  cout << "  LonOrigin: " << doubleToString(m_datum_lon,6) << endl;
  cout << "Given Query:    " << endl;
  cout << "  Latitude(1):  " << doubleToString(m_lat1,6) << endl;
  cout << "  Longitude(1): " << doubleToString(m_lon1,6) << endl;
  cout << "  Latitude(2):  " << doubleToString(m_lat2,6) << endl;
  cout << "  Longitude(2): " << doubleToString(m_lon2,6) << endl;
  
  if(!ok_input) {
    cout << "Problems with input - Query aborted." << endl;
    return;
  }
  
  double x1, y1, x2, y2;
  bool ok_trans1 = m_geodesy.LatLong2LocalGrid(m_lat1, m_lon1, x1, y1);
  bool ok_trans2 = m_geodesy.LatLong2LocalGrid(m_lat2, m_lon2, x2, y2);
  if(!ok_trans1 || !ok_trans2) {
    cout << "Problem with the MOOSGeodesy translation - exiting." << endl;
    exit(0);
  }
  else {
    double dist = hypot((x1-x2), (y1-y2));
    cout << "Resulting Distance Between Lat/Lon Positions: " << endl;
    cout << "  Distance (meters):   " << doubleToString(dist,6) << endl;
  }
}  


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp                                             */
/*    DATE:                                                      */
/*****************************************************************/

#include "MBUtils.h"
#include "GeoHandler.h"

#include <iostream>
using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  int res;

  // Look for a request for help
  res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    //help_message();
    return(0);
  }

  double datum_lat = 0;
  double datum_lon = 0;

  res = getArg(argc, argv, 0, "-d", "--dabob");
  if(res) {
    datum_lat = 47.7319;
    datum_lon = -122.85;
  }

  res = getArg(argc, argv, 1, "--dlat");
  if(res)
    datum_lat = atof(argv[res]);
  res = getArg(argc, argv, 1, "--dlon");
  if(res)
    datum_lon = atof(argv[res]);

  GeoHandler m_geo;
  bool ok_datum = m_geo.setDatum(datum_lat, datum_lon);
  if(!ok_datum || (datum_lat==0) || (datum_lon==0)) {
    cout << "Problem with the provided datum - exiting.";
    exit(0);
  }


  res = getArg(argc, argv, 0, "-x1", "--mx1");
  if(res && (argc > res+1))
    m_geo.setMetersX(1, atof(argv[res+1]));
  res = getArg(argc, argv, 0, "-x2", "--mx2");
  if(res && (argc > res+1))
    m_geo.setMetersX(2, atof(argv[res+1]));


  res = getArg(argc, argv, 0, "-y1", "--my1");
  if(res && (argc > res+1))
    m_geo.setMetersY(1, atof(argv[res+1]));
  res = getArg(argc, argv, 0, "-y2", "--my2");
  if(res && (argc > res+1))
    m_geo.setMetersY(2, atof(argv[res+1]));


  res = getArg(argc, argv, 0, "-lat1", "-lat");
  if(res && (argc > res+1))
    m_geo.setLat(1, atof(argv[res+1]));
  res = getArg(argc, argv, 0, "-lat2");
  if(res && (argc > res+1))
    m_geo.setLat(2, atof(argv[res+1]));

  res = getArg(argc, argv, 0, "-lon1", "-lon");
  if(res && (argc > res+1))
    m_geo.setLon(1, atof(argv[res+1]));
  res = getArg(argc, argv, 0, "-lon2");
  if(res && (argc > res+1))
    m_geo.setLon(2, atof(argv[res+1]));
  
  m_geo.query();

}

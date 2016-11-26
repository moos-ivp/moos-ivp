// Copyright 2009-2013 Toby Schneider (https://launchpad.net/~tes)
//                     Massachusetts Institute of Technology (2007-)
//                     Woods Hole Oceanographic Institution (2007-)
//                     Goby Developers Team (https://launchpad.net/~goby-dev)
// 
// Apr 14th, 2015 mikerb added DMS2DecDeg from original MOOSGeodesy 
//
// This file is part of the Goby Underwater Autonomy Project MOOS Interface Library
// ("The Goby MOOS Library").
//
// The Goby MOOS Library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The Goby MOOS Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Goby.  If not, see <http://www.gnu.org/licenses/>.

// This file uses the same basic API provided by MOOS pre-v10
// (C) Paul Newman, released GPL v3

#ifndef MOOSGeodesy20130916H
#define MOOSGeodesy20130916H

#include <proj_api.h>

class CMOOSGeodesy
{
 public:
  CMOOSGeodesy();
  virtual ~CMOOSGeodesy();
  
  double GetOriginLatitude();
  double GetOriginLongitude();
  double GetOriginNorthing();
  double GetOriginEasting();
  int GetUTMZone();
  
  // UTM transform pair (WGS-84 ellipsoid)
  bool LatLong2LocalUTM(double lat, double lon, double & MetersNorth, double & MetersEast);
  bool UTM2LatLong(double dfX, double dfY, double& dfLat, double& dfLong);    

  // Local Grid projection
  bool LocalGrid2LatLong(double dfEast, double dfNorth, double &dfLat, double &dfLon);
  bool LatLong2LocalGrid(double lat, double lon, double &MetersNorth, double &MetersEast);

  bool Initialise(double lat, double lon);
  
  double DMS2DecDeg(double dfVal);

 private:
  int m_sUTMZone;
  double m_dOriginEasting;
  double m_dOriginNorthing;
  double m_dOriginLongitude;
  double m_dOriginLatitude;
  projPJ pj_utm_, pj_latlong_;
  
  void SetOriginLatitude(double lat);
  void SetOriginLongitude(double lon);
  void SetOriginEasting(double East);
  void SetOriginNorthing(double North);
  void SetUTMZone(int zone);
};

#endif

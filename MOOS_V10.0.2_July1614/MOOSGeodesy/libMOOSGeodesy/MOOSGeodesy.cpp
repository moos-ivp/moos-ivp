// Copyright 2009-2013 Toby Schneider (https://launchpad.net/~tes)
//                     Massachusetts Institute of Technology (2007-)
//                     Woods Hole Oceanographic Institution (2007-)
//                     Goby Developers Team (https://launchpad.net/~goby-dev)
// 
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

#include <cmath>
#include <sstream>
#include <iostream>
#include <limits>

#include "include/MOOS/libMOOSGeodesy/MOOSGeodesy.h"

CMOOSGeodesy::CMOOSGeodesy() : m_sUTMZone(0),
                               m_dOriginEasting(0),
                               m_dOriginNorthing(0),
                               m_dOriginLongitude(0),
                               m_dOriginLatitude(0),
                               pj_utm_(0),
                               pj_latlong_(0)
{ }

CMOOSGeodesy::~CMOOSGeodesy()
{
    pj_free(pj_utm_);
    pj_free(pj_latlong_);
}

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
    //Set the Origin of the local Grid Coordinate System
    SetOriginLatitude(lat);
    SetOriginLongitude(lon);

    int zone = (static_cast<int>(std::floor((lon + 180)/6)) + 1) % 60;

    std::stringstream proj_utm;
    proj_utm << "+proj=utm +ellps=WGS84 +zone=" << zone;

    if (!(pj_utm_ = pj_init_plus(proj_utm.str().c_str())))
    {
        std::cerr << "Failed to initiate utm proj" << std::endl;
        return false;
    }
    if (!(pj_latlong_ = pj_init_plus("+proj=latlong +ellps=WGS84")) )
    {
        std::cerr << "Failed to initiate latlong proj" << std::endl;
        return false;
    }

    //Translate the Origin coordinates into Northings and Eastings 
    double tempNorth = lat*DEG_TO_RAD,
        tempEast = lon*DEG_TO_RAD;

    int err = pj_transform(pj_latlong_, pj_utm_, 1, 1, &tempEast, &tempNorth, NULL);
    if(err)
      {
        std::cerr << "Failed to transform datum, reason: " << pj_strerrno(err) << std::endl;
        return false;
    }

    //Then set the Origin for the Northing/Easting coordinate frame
    //Also make a note of the UTM Zone we are operating in
    SetOriginNorthing(tempNorth);
    SetOriginEasting(tempEast);
    SetUTMZone(zone);

    return true;
}

double CMOOSGeodesy::GetOriginLongitude()
{
    return m_dOriginLongitude;
}

double CMOOSGeodesy::GetOriginLatitude()
{
    return m_dOriginLatitude;    
}

void CMOOSGeodesy::SetOriginLongitude(double lon)
{
    m_dOriginLongitude = lon;
}

void CMOOSGeodesy::SetOriginLatitude(double lat)
{
    m_dOriginLatitude = lat;
}

void CMOOSGeodesy::SetOriginNorthing(double North)
{
    m_dOriginNorthing = North;
}

void CMOOSGeodesy::SetOriginEasting(double East)
{
    m_dOriginEasting = East;
}

void CMOOSGeodesy::SetUTMZone(int zone)
{
    m_sUTMZone = zone;
}

int CMOOSGeodesy::GetUTMZone()
{
    return m_sUTMZone;
}


bool CMOOSGeodesy::LatLong2LocalUTM(double lat,
                                    double lon, 
                                    double &MetersNorth,
                                    double &MetersEast)
{
  //double dN, dE;
    double tmpEast = lon * DEG_TO_RAD;
    double tmpNorth = lat * DEG_TO_RAD;
    MetersNorth = std::numeric_limits<double>::quiet_NaN();
    MetersEast =  std::numeric_limits<double>::quiet_NaN();

    if(!pj_latlong_ || !pj_utm_)
    {
        std::cerr << "Must call Initialise before calling LatLong2LocalUTM" << std::endl;
        return false;
    }

    int err = pj_transform(pj_latlong_, pj_utm_, 1, 1, &tmpEast, &tmpNorth, NULL );
    if(err)
    {
        std::cerr << "Failed to transform (lat,lon) = (" << lat << "," << lon << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }

    MetersNorth = tmpNorth - GetOriginNorthing();
    MetersEast = tmpEast - GetOriginEasting();
    return true;
}

double CMOOSGeodesy::GetOriginEasting()
{
    return m_dOriginEasting;
}

double CMOOSGeodesy::GetOriginNorthing()
{
    return m_dOriginNorthing;
}

bool CMOOSGeodesy::UTM2LatLong(double dfX, double dfY, double& dfLat, double& dfLong)
{
    double x = dfX + GetOriginEasting();
    double y = dfY + GetOriginNorthing();

    dfLat = std::numeric_limits<double>::quiet_NaN();
    dfLong = std::numeric_limits<double>::quiet_NaN();

    if(!pj_latlong_ || !pj_utm_)
    {
        std::cerr << "Must call Initialise before calling UTM2LatLong" << std::endl;
        return false;
    }
    
    int err = pj_transform(pj_utm_, pj_latlong_, 1, 1, &x, &y, NULL);
    if(err)
    {
        std::cerr << "Failed to transform (x,y) = (" << dfX << "," << dfY << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }

    dfLat = y * RAD_TO_DEG;
    dfLong = x * RAD_TO_DEG;
    return true;
}


bool CMOOSGeodesy::LatLong2LocalGrid(double lat, 
                                     double lon, 
                                     double &MetersNorth, 
                                     double &MetersEast)
{
    
    //(semimajor axis)
    double dfa  = 6378137;
    // (semiminor axis)
    double dfb = 6356752;

    double dftanlat2 = pow(tan(lat*DEG_TO_RAD),2);
    double dfRadius = dfb*sqrt(1+dftanlat2) / sqrt( ( pow(dfb,2)/pow(dfa,2) )+dftanlat2);



    //the decimal degrees conversion should take place elsewhere
    double dXArcDeg  = (lon - GetOriginLongitude()) * DEG_TO_RAD;
    double dX = dfRadius * sin(dXArcDeg)*cos(lat*DEG_TO_RAD);
    
    double dYArcDeg  = (lat - GetOriginLatitude()) * DEG_TO_RAD;
    double dY = dfRadius * sin(dYArcDeg);

    //This is the total distance traveled thus far, either North or East
    MetersNorth = dY;
    MetersEast  = dX;
    
    return true;
}

/**
 *Utility method for converting from a local grid fix to the 
 *global Lat, Lon pair.  This method will work for small grid
 *approximations - <300km sq
 *@param dfEast  The current local grid distance in meters traveled East (X dir) wrt to Origin
 *@param dfNorth The current local grid distance in meters traveled North (Y dir) wrt to Origin
 *@param dfLat the calculated latitude out
 *@param dfLon the calculated longitude out
 */

bool CMOOSGeodesy::LocalGrid2LatLong(double dfEast, double dfNorth, double &dfLat, double &dfLon) 
{
  if(std::isnan(dfEast) || std::isnan(dfNorth))  // changed to std::isnan, mikerb AUG/04/14
    return(false);

    //(semimajor axis)
    double dfa  = 6378137;
    // (semiminor axis)
    double dfb = 6356752;

    double dftanlat2 = pow( tan( GetOriginLatitude()*DEG_TO_RAD ), 2 );
    double dfRadius = dfb*sqrt( 1+dftanlat2 ) / sqrt( ( pow(dfb,2)/pow(dfa,2) )+dftanlat2 );

    //first calculate lat arc
    double dfYArcRad = asin( dfNorth/dfRadius ); //returns result in rad
    double dfYArcDeg = dfYArcRad * RAD_TO_DEG;
 
    double dfXArcRad = asin( dfEast/( dfRadius*cos( GetOriginLatitude()*DEG_TO_RAD ) ) );
    double dfXArcDeg = dfXArcRad * RAD_TO_DEG;
        
    //add the origin to these arc lengths
    dfLat = dfYArcDeg + GetOriginLatitude();
       dfLon = dfXArcDeg + GetOriginLongitude();

    return true;
}


double CMOOSGeodesy::DMS2DecDeg(double dfVal)
{
  int nDeg = (int)(dfVal/100.0);
  
  double dfTmpDeg = (100.0*(dfVal/100.0-nDeg))/60.0;
  
  return(dfTmpDeg+nDeg);
}

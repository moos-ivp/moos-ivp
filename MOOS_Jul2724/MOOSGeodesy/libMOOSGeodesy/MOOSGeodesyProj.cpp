///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a MOOS Core Component.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////

#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdlib.h>

#include <proj_api.h>

CMOOSGeodesy::~CMOOSGeodesy()
{
    pj_free(pj_utm_);
    pj_free(pj_latlong_);
}

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
    SetRefEllipsoid(23);
    SetOriginLatitude(lat);
    SetOriginLongitude(lon);

    pj_free(pj_utm_);
    pj_free(pj_latlong_);
    pj_utm_ = 0;
    pj_latlong_ = 0;

    char utmZone[4];
    double legacyNorth = 0.0, legacyEast = 0.0;
    LLtoUTM(m_iRefEllipsoid, m_dOriginLatitude,
            m_dOriginLongitude, legacyNorth,
            legacyEast, utmZone);

    std::stringstream projUTM;
    projUTM << "+proj=utm +ellps=WGS84 +zone=" << atoi(utmZone);
    if(lat < 0)
        projUTM << " +south";

    pj_utm_ = pj_init_plus(projUTM.str().c_str());
    if(!pj_utm_) {
        std::cerr << "Failed to initiate utm proj" << std::endl;
        return false;
    }

    pj_latlong_ = pj_init_plus("+proj=latlong +ellps=WGS84");
    if(!pj_latlong_) {
        std::cerr << "Failed to initiate latlong proj" << std::endl;
        return false;
    }

    double tempNorth = lat * deg2rad;
    double tempEast = lon * deg2rad;
    int err = pj_transform(pj_latlong_, pj_utm_, 1, 1,
                           &tempEast, &tempNorth, NULL);
    if(err) {
        std::cerr << "Failed to transform datum, reason: "
                  << pj_strerrno(err) << std::endl;
        return false;
    }

    SetOriginNorthing(tempNorth);
    SetOriginEasting(tempEast);
    SetUTMZone(utmZone);
    m_bSTEP_AFTER_INIT = true;

    return true;
}

bool CMOOSGeodesy::LatLong2LocalUTM(double lat,
                                    double lon,
                                    double &MetersNorth,
                                    double &MetersEast)
{
    double tmpNorth = lat * deg2rad;
    double tmpEast = lon * deg2rad;
    MetersNorth = std::numeric_limits<double>::quiet_NaN();
    MetersEast = std::numeric_limits<double>::quiet_NaN();

    if(!pj_latlong_ || !pj_utm_) {
        std::cerr << "Must call Initialise before calling LatLong2LocalUTM"
                  << std::endl;
        return false;
    }

    int err = pj_transform(pj_latlong_, pj_utm_, 1, 1,
                           &tmpEast, &tmpNorth, NULL);
    if(err) {
        std::cerr << "Failed to transform (lat,lon) = (" << lat << "," << lon
                  << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }

    MetersNorth = tmpNorth - GetOriginNorthing();
    MetersEast = tmpEast - GetOriginEasting();

    return true;
}

bool CMOOSGeodesy::UTM2LatLong(double dfX, double dfY,
                               double& dfLat, double& dfLong)
{
    double x = dfX + GetOriginEasting();
    double y = dfY + GetOriginNorthing();

    dfLat = std::numeric_limits<double>::quiet_NaN();
    dfLong = std::numeric_limits<double>::quiet_NaN();

    if(!pj_latlong_ || !pj_utm_) {
        std::cerr << "Must call Initialise before calling UTM2LatLong"
                  << std::endl;
        return false;
    }

    int err = pj_transform(pj_utm_, pj_latlong_, 1, 1, &x, &y, NULL);
    if(err) {
        std::cerr << "Failed to transform (x,y) = (" << dfX << "," << dfY
                  << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }

    dfLat = y * rad2deg;
    dfLong = x * rad2deg;
    return true;
}

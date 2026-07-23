///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2026 Massachusetts Institute of Technology.
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

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdlib.h>

#include <proj_api.h>

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
    if(!std::isfinite(lat) || !std::isfinite(lon) ||
       lat < -80.0 || lat > 84.0 || lon < -180.0 || lon > 180.0) {
        std::cerr << "Invalid datum (lat,lon) = (" << lat << "," << lon << ")"
                  << std::endl;
        return false;
    }

    char utmZone[4];
    double legacyNorth = 0.0, legacyEast = 0.0;
    LLtoUTM(23, lat, lon, legacyNorth,
            legacyEast, utmZone);

    std::stringstream projUTM;
    projUTM << "+proj=utm +ellps=WGS84 +zone=" << atoi(utmZone);
    if(lat < 0)
        projUTM << " +south";

    ProjectionPtr utmProjection(
        pj_init_plus(projUTM.str().c_str()), pj_free);
    if(!utmProjection) {
        std::cerr << "Failed to initiate utm proj" << std::endl;
        return false;
    }

    ProjectionPtr latlongProjection(
        pj_init_plus("+proj=latlong +ellps=WGS84"), pj_free);
    if(!latlongProjection) {
        std::cerr << "Failed to initiate latlong proj" << std::endl;
        return false;
    }

    double tempNorth = lat * deg2rad;
    double tempEast = lon * deg2rad;
    int err = pj_transform(latlongProjection.get(),
                           utmProjection.get(), 1, 1,
                           &tempEast, &tempNorth, NULL);
    if(err) {
        std::cerr << "Failed to transform datum, reason: "
                  << pj_strerrno(err) << std::endl;
        return false;
    }
    if(!std::isfinite(tempEast) || !std::isfinite(tempNorth)) {
        std::cerr << "Failed to transform datum: non-finite result" << std::endl;
        return false;
    }

    SetRefEllipsoid(23);
    SetOriginLatitude(lat);
    SetOriginLongitude(lon);
    SetOriginNorthing(tempNorth);
    SetOriginEasting(tempEast);
    SetUTMZone(utmZone);
    m_utm_projection = utmProjection;
    m_latlong_projection = latlongProjection;
    m_bSTEP_AFTER_INIT = true;

    return true;
}

bool CMOOSGeodesy::LatLong2LocalUTM(double lat,
                                    double lon,
                                    double &MetersNorth,
                                    double &MetersEast)
{
    MetersNorth = std::numeric_limits<double>::quiet_NaN();
    MetersEast = std::numeric_limits<double>::quiet_NaN();

    if(!m_latlong_projection || !m_utm_projection) {
        std::cerr << "Must call Initialise before calling LatLong2LocalUTM"
                  << std::endl;
        return false;
    }
    if(!std::isfinite(lat) || !std::isfinite(lon)) {
        std::cerr << "Invalid (lat,lon) = (" << lat << "," << lon << ")"
                  << std::endl;
        return false;
    }

    double tmpNorth = lat * deg2rad;
    double tmpEast = lon * deg2rad;

    int err = pj_transform(m_latlong_projection.get(),
                           m_utm_projection.get(), 1, 1,
                           &tmpEast, &tmpNorth, NULL);
    if(err) {
        std::cerr << "Failed to transform (lat,lon) = (" << lat << "," << lon
                  << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }
    if(!std::isfinite(tmpEast) || !std::isfinite(tmpNorth)) {
        std::cerr << "Failed to transform (lat,lon) = (" << lat << "," << lon
                  << "): non-finite result" << std::endl;
        return false;
    }

    SetMetersNorth(tmpNorth - GetOriginNorthing());
    SetMetersEast(tmpEast - GetOriginEasting());
    MetersNorth = GetMetersNorth();
    MetersEast = GetMetersEast();

    return true;
}

bool CMOOSGeodesy::UTM2LatLong(double dfX, double dfY,
                               double& dfLat, double& dfLong)
{
    dfLat = std::numeric_limits<double>::quiet_NaN();
    dfLong = std::numeric_limits<double>::quiet_NaN();

    if(!m_latlong_projection || !m_utm_projection) {
        std::cerr << "Must call Initialise before calling UTM2LatLong"
                  << std::endl;
        return false;
    }
    if(!std::isfinite(dfX) || !std::isfinite(dfY)) {
        std::cerr << "Invalid (x,y) = (" << dfX << "," << dfY << ")"
                  << std::endl;
        return false;
    }

    double x = dfX + GetOriginEasting();
    double y = dfY + GetOriginNorthing();
    if(!std::isfinite(x) || !std::isfinite(y)) {
        std::cerr << "Invalid (x,y) = (" << dfX << "," << dfY << ")"
                  << std::endl;
        return false;
    }

    int err = pj_transform(m_utm_projection.get(),
                           m_latlong_projection.get(), 1, 1, &x, &y, NULL);
    if(err) {
        std::cerr << "Failed to transform (x,y) = (" << dfX << "," << dfY
                  << "), reason: " << pj_strerrno(err) << std::endl;
        return false;
    }
    if(!std::isfinite(x) || !std::isfinite(y)) {
        std::cerr << "Failed to transform (x,y) = (" << dfX << "," << dfY
                  << "): non-finite result" << std::endl;
        return false;
    }

    dfLat = y * rad2deg;
    dfLong = x * rad2deg;
    return true;
}

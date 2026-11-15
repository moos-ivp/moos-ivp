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

#include <cmath>
#include <cstdlib>
#include <limits>

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
    if(!std::isfinite(lat) || !std::isfinite(lon) ||
       lat < -80.0 || lat > 84.0 || lon < -180.0 || lon > 180.0)
        return false;

    // We use the WGS-84 standard reference ellipsoid.
    SetRefEllipsoid(23);

    SetOriginLatitude(lat);
    SetOriginLongitude(lon);

    double tempNorth = 0.0, tempEast = 0.0;
    char utmZone[4];
    LLtoUTM(m_iRefEllipsoid, m_dOriginLatitude,
            m_dOriginLongitude, tempNorth,
            tempEast, utmZone);

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
    MetersNorth = std::numeric_limits<double>::quiet_NaN();
    MetersEast = std::numeric_limits<double>::quiet_NaN();
    if(m_sUTMZone[0] == '\0' || !std::isfinite(lat) || !std::isfinite(lon) ||
       lat < -80.0 || lat > 84.0)
        return false;

    double tmpNorth = 0.0, tmpEast = 0.0;
    char tmpUTM[4];
    LLtoUTM(m_iRefEllipsoid, lat, lon, tmpNorth, tmpEast, tmpUTM,
            std::atoi(m_sUTMZone), GetOriginLatitude() < 0.0);

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
    if(m_sUTMZone[0] == '\0' || !std::isfinite(dfX) || !std::isfinite(dfY))
        return false;

    // Written by Henrik Schmidt henrik@mit.edu. Invert the local projection
    // iteratively, with a limit so out-of-range coordinates cannot hang.
    double dfx = dfX;
    double dfy = dfY;
    const double eps = 0.001; // metres

    for(int iteration = 0; iteration < 100; ++iteration) {
        double dflat, dflon, dfnewX, dfnewY;

        if(!LocalGrid2LatLong(dfx, dfy, dflat, dflon))
            return false;

        // Intermediate estimates can lie beyond the UTM latitude limits.
        // Project them directly; validate the latitude once we converge.
        if(!std::isfinite(dflat) || !std::isfinite(dflon) ||
           dflat <= -90.0 || dflat >= 90.0)
            return false;
        char tmpUTM[4];
        LLtoUTM(m_iRefEllipsoid, dflat, dflon, dfnewY, dfnewX, tmpUTM,
                std::atoi(m_sUTMZone), GetOriginLatitude() < 0.0);
        dfnewX -= GetOriginEasting();
        dfnewY -= GetOriginNorthing();

        double diffX = dfnewX - dfX;
        double diffY = dfnewY - dfY;
        if(!std::isfinite(diffX) || !std::isfinite(diffY))
            return false;
        if(std::hypot(diffX, diffY) <= eps) {
            // Allow the solver's millimetre tolerance at the boundary, then
            // clamp that numerical overshoot to the valid UTM latitude.
            const double latitudeTolerance = eps / 100000.0;
            if(dflat < -80.0 - latitudeTolerance || dflat > 84.0 + latitudeTolerance)
                return false;
            dfLat = dflat < -80.0 ? -80.0 : (dflat > 84.0 ? 84.0 : dflat);
            dfLong = std::fmod(dflon + 180.0, 360.0);
            if(dfLong < 0.0)
                dfLong += 360.0;
            dfLong -= 180.0;
            return true;
        }
        dfx -= diffX;
        dfy -= diffY;
    }

    return false;
}

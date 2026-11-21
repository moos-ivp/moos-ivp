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

#include <math.h>

#ifdef _WIN32
#include <float.h>
#define isnan _isnan
#endif

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
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
    double tmpNorth = 0.0, tmpEast = 0.0;
    double dN = 0.0, dE = 0.0;
    char tmpUTM[4];

    LLtoUTM(m_iRefEllipsoid, lat, lon, tmpNorth, tmpEast, tmpUTM);

    if(m_bSTEP_AFTER_INIT) {
        dN = tmpNorth - GetOriginNorthing();
        dE = tmpEast - GetOriginEasting();
        SetMetersNorth(dN);
        SetMetersEast(dE);
        m_bSTEP_AFTER_INIT = !m_bSTEP_AFTER_INIT;
    } else {
        double totalNorth = tmpNorth - GetOriginNorthing();
        dN = totalNorth - GetMetersNorth();
        SetMetersNorth(dN + GetMetersNorth());

        double totalEast = tmpEast - GetOriginEasting();
        dE = totalEast - GetMetersEast();
        SetMetersEast(dE + GetMetersEast());
    }

    MetersNorth = GetMetersNorth();
    MetersEast = GetMetersEast();

    return true;
}

bool CMOOSGeodesy::UTM2LatLong(double dfX, double dfY,
                               double& dfLat, double& dfLong)
{
    // Written by Henrik Schmidt henrik@mit.edu
    double err = 1e20;
    double dfx = dfX;
    double dfy = dfY;
    double eps = 1.0;

    while(err > eps) {
        double dflat, dflon, dfnewX, dfnewY;

        if(!LocalGrid2LatLong(dfx, dfy, dflat, dflon))
            return false;

        if(!LatLong2LocalUTM(dflat, dflon, dfnewY, dfnewX))
            return false;

        if(isnan(dflat) || isnan(dflon)) {
            dflat = 91;
            dflon = 181;
            return false;
        }

        double diffX = dfnewX - dfX;
        double diffY = dfnewY - dfY;
        dfx -= diffX;
        dfy -= diffY;
        err = hypot(dfnewX - dfX, dfnewY - dfY);
    }

    if(!LocalGrid2LatLong(dfx, dfy, dfLat, dfLong))
        return false;

    return true;
}

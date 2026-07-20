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
#include "MOOSGeodesyBackend.h"

#include <math.h>

#ifdef _WIN32
#include <float.h>
#define isnan _isnan
#endif

class CMOOSGeodesyOriginal : public CMOOSGeodesyBackend
{
public:
    CMOOSGeodesyBackend* Clone() const
    {
        return new CMOOSGeodesyOriginal(*this);
    }

    bool Initialise(CMOOSGeodesy& geodesy, double lat, double lon)
    {
        geodesy.SetRefEllipsoid(23);
        geodesy.SetOriginLatitude(lat);
        geodesy.SetOriginLongitude(lon);

        double north = 0.0;
        double east = 0.0;
        char utmZone[4];
        geodesy.LLtoUTM(geodesy.m_iRefEllipsoid, lat, lon,
                        north, east, utmZone);

        geodesy.SetOriginNorthing(north);
        geodesy.SetOriginEasting(east);
        geodesy.SetUTMZone(utmZone);
        geodesy.m_bSTEP_AFTER_INIT = true;
        return true;
    }

    bool LatLong2LocalUTM(CMOOSGeodesy& geodesy,
                          double lat, double lon,
                          double& metersNorth, double& metersEast)
    {
        double north = 0.0;
        double east = 0.0;
        double deltaNorth = 0.0;
        double deltaEast = 0.0;
        char utmZone[4];

        geodesy.LLtoUTM(geodesy.m_iRefEllipsoid, lat, lon,
                        north, east, utmZone);

        if(geodesy.m_bSTEP_AFTER_INIT) {
            deltaNorth = north - geodesy.GetOriginNorthing();
            deltaEast = east - geodesy.GetOriginEasting();
            geodesy.SetMetersNorth(deltaNorth);
            geodesy.SetMetersEast(deltaEast);
            geodesy.m_bSTEP_AFTER_INIT = false;
        } else {
            double totalNorth = north - geodesy.GetOriginNorthing();
            deltaNorth = totalNorth - geodesy.GetMetersNorth();
            geodesy.SetMetersNorth(deltaNorth + geodesy.GetMetersNorth());

            double totalEast = east - geodesy.GetOriginEasting();
            deltaEast = totalEast - geodesy.GetMetersEast();
            geodesy.SetMetersEast(deltaEast + geodesy.GetMetersEast());
        }

        metersNorth = geodesy.GetMetersNorth();
        metersEast = geodesy.GetMetersEast();
        return true;
    }

    bool UTM2LatLong(CMOOSGeodesy& geodesy,
                     double x, double y, double& lat, double& lon)
    {
        double error = 1e20;
        double estimateX = x;
        double estimateY = y;
        const double epsilon = 1.0;

        while(error > epsilon) {
            double estimateLat;
            double estimateLon;
            double convertedX;
            double convertedY;

            if(!geodesy.LocalGrid2LatLong(estimateX, estimateY,
                                          estimateLat, estimateLon))
                return false;

            if(!LatLong2LocalUTM(geodesy, estimateLat, estimateLon,
                                 convertedY, convertedX))
                return false;

            if(isnan(estimateLat) || isnan(estimateLon)) {
                lat = 91;
                lon = 181;
                return false;
            }

            estimateX -= convertedX - x;
            estimateY -= convertedY - y;
            error = hypot(convertedX - x, convertedY - y);
        }

        return geodesy.LocalGrid2LatLong(estimateX, estimateY, lat, lon);
    }

    bool IsProj() const
    {
        return false;
    }
};

CMOOSGeodesyBackend* CreateMOOSGeodesyOriginal()
{
    return new CMOOSGeodesyOriginal;
}

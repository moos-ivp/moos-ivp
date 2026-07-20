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

#ifndef MOOS_GEODESY_BACKEND_H
#define MOOS_GEODESY_BACKEND_H

class CMOOSGeodesy;

class CMOOSGeodesyBackend
{
public:
    virtual ~CMOOSGeodesyBackend() {}

    virtual CMOOSGeodesyBackend* Clone() const = 0;
    virtual bool Initialise(CMOOSGeodesy& geodesy,
                            double lat, double lon) = 0;
    virtual bool LatLong2LocalUTM(CMOOSGeodesy& geodesy,
                                  double lat, double lon,
                                  double& metersNorth,
                                  double& metersEast) = 0;
    virtual bool UTM2LatLong(CMOOSGeodesy& geodesy,
                             double x, double y,
                             double& lat, double& lon) = 0;
    virtual bool IsProj() const = 0;
};

CMOOSGeodesyBackend* CreateMOOSGeodesyOriginal();
CMOOSGeodesyBackend* CreateMOOSGeodesyProj();

#endif

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
//   This file is part of a  MOOS Core Component.
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
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSGeodesy.h: interface for the CMOOSGeodesy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSGEODESY_H__95563F0D_91C8_43FF_B277_4247C8C25E4D__INCLUDED_)
#define AFX_MOOSGEODESY_H__95563F0D_91C8_43FF_B277_4247C8C25E4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef PI
    #define  PI 3.14159265
#endif

const double FOURTHPI = PI / 4;
const double deg2rad = PI / 180;
const double rad2deg = 180.0 / PI;

#define EARTH_RADIUS 6378137.0 //meters - WGS84 semi-major axis

//! Implements simple geodesy calculations
class CMOOSGeodesy
{
public:
    CMOOSGeodesy();
    virtual ~CMOOSGeodesy();

    double 	GetOriginNorthing();
    double 	GetOriginEasting();
    bool 	LatLong2LocalUTM(double lat, double lon, double & MetersNorth, double & MetersEast);
    bool 	LocalGrid2LatLong(double dfEast, double dfNorth, double &dfLat, double &dfLon);
    bool 	UTM2LatLong(double dfX, double dfY, double& dfLat, double& dfLong);
    
    
    char * 	GetUTMZone();
    int 	GetRefEllipsoid();
    double 	GetMetersEast();
    double 	GetMetersNorth();
    double 	GetOriginLatitude();
    double 	GetOriginLongitude();
    bool 	Initialise(double lat, double lon);

private:
    bool m_bSTEP_AFTER_INIT;
    char m_sUTMZone[4];
    int m_iRefEllipsoid;
    double m_dOriginEasting;
    double m_dOriginNorthing;
    double m_dEast;
    double m_dNorth;
    double m_dOriginLongitude;
    double m_dOriginLatitude;
    double m_dLocalGridX;
    double m_dLocalGridY;

    void SetUTMZone(char * utmZone);
    void SetRefEllipsoid(int refEllipsoid);
    void SetOriginEasting(double East);

    void SetOriginNorthing(double North);
    bool LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long, double &UTMNorthing, double &UTMEasting, char* UTMZone);
    void SetMetersEast(double East);
    void SetMetersNorth(double North);
    char UTMLetterDesignator(double Lat);

    void SetOriginLatitude(double lat);
    void SetOriginLongitude(double lon);
    void SetLocalGridY(double Y);
    void SetLocalGridX(double X);


public:
    double GetLocalGridY();
    double GetLocalGridX();

    bool LatLong2LocalGrid(double lat, double lon, double &MetersNorth, double &MetersEast);
    double DMS2DecDeg(double dfVal);
    class CEllipsoid
    {
    public:
        CEllipsoid(){};
        CEllipsoid(int Id, const char* name, double radius, double ecc)
        {
            id = Id;
            ellipsoidName = name;
            EquatorialRadius = radius;
            eccentricitySquared = ecc;
        };

        int id;
        const char* ellipsoidName;
        double EquatorialRadius;
        double eccentricitySquared;

    };

};

//! Ellposid information for CMOOSGeodesy
static CMOOSGeodesy::CEllipsoid ellipsoid[] =
{//  id, Ellipsoid name, Equatorial Radius, square of eccentricity
    CMOOSGeodesy::CEllipsoid( -1, "Placeholder", 0, 0),//placeholder only, To allow array indices to match id numbers
    CMOOSGeodesy::CEllipsoid( 1, "Airy", 6377563, 0.00667054),
    CMOOSGeodesy::CEllipsoid( 2, "Australian National", 6378160, 0.006694542),
    CMOOSGeodesy::CEllipsoid( 3, "Bessel 1841", 6377397, 0.006674372),
    CMOOSGeodesy::CEllipsoid( 4, "Bessel 1841 (Nambia) ", 6377484, 0.006674372),
    CMOOSGeodesy::CEllipsoid( 5, "Clarke 1866", 6378206, 0.006768658),
    CMOOSGeodesy::CEllipsoid( 6, "Clarke 1880", 6378249, 0.006803511),
    CMOOSGeodesy::CEllipsoid( 7, "Everest", 6377276, 0.006637847),
    CMOOSGeodesy::CEllipsoid( 8, "Fischer 1960 (Mercury) ", 6378166, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 9, "Fischer 1968", 6378150, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 10, "GRS 1967", 6378160, 0.006694605),
    CMOOSGeodesy::CEllipsoid( 11, "GRS 1980", 6378137, 0.00669438),
    CMOOSGeodesy::CEllipsoid( 12, "Helmert 1906", 6378200, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 13, "Hough", 6378270, 0.00672267),
    CMOOSGeodesy::CEllipsoid( 14, "International", 6378388, 0.00672267),
    CMOOSGeodesy::CEllipsoid( 15, "Krassovsky", 6378245, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 16, "Modified Airy", 6377340, 0.00667054),
    CMOOSGeodesy::CEllipsoid( 17, "Modified Everest", 6377304, 0.006637847),
    CMOOSGeodesy::CEllipsoid( 18, "Modified Fischer 1960", 6378155, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 19, "South American 1969", 6378160, 0.006694542),
    CMOOSGeodesy::CEllipsoid( 20, "WGS 60", 6378165, 0.006693422),
    CMOOSGeodesy::CEllipsoid( 21, "WGS 66", 6378145, 0.006694542),
    CMOOSGeodesy::CEllipsoid( 22, "WGS-72", 6378135, 0.006694318),
    CMOOSGeodesy::CEllipsoid( 23, "WGS-84", 6378137, 0.00669438)
};

#endif // !defined(AFX_MOOSGEODESY_H__95563F0D_91C8_43FF_B277_4247C8C25E4D__INCLUDED_)

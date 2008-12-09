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
// MOOSGeodesy.cpp: implementation of the CMOOSGeodesy class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSGeodesy.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*Reference ellipsoids derived from Peter H. Dana's website- 
http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
Department of Geography, University of Texas at Austin
Internet: pdana@mail.utexas.edu
3/22/95

Source
Defense Mapping Agency. 1987b. DMA Technical Report: Supplement to Department of Defense World Geodetic System
1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSGeodesy::CMOOSGeodesy()
{
    //create the variables by Zeroing
    SetMetersNorth(0.0);
    SetMetersEast(0.0);
    SetLocalGridX(0.0);
    SetLocalGridY(0.0);
    SetOriginNorthing(0.0);
    SetOriginEasting(0.0);
    SetOriginLongitude(0.0);
    SetOriginLatitude(0.0);
    
}

CMOOSGeodesy::~CMOOSGeodesy()
{

}

/**
*This method is called to set the Origins of the Coordinate system
*being used by the vehicle for a mission.  This class will store the
*vehicle's position in Northings and Eastings.  This allows for tracking the
*vehicle as if it were operating on a grid.
*
*@param lat the Latitude of where the vehicle is as it begins a mission
*@param lon the Longitude of where the vehicle is as it begins a mission
*
*@return only returns true at the moment, no reason as to why it should fail
*        perhaps some way of checking UTM zones vs a list of some sort? 
*/

bool CMOOSGeodesy::Initialise(double lat, double lon)
{
    //We will use the WGS-84 standard Reference Ellipsoid
    //This can be modified by a client if they desire with 
    //@see MOOSGeodesy.h
    SetRefEllipsoid(23);

    //Set the Origin of the local Grid Coordinate System
    SetOriginLatitude(lat);
    SetOriginLongitude(lon);

    //Translate the Origin coordinates into Northings and Eastings 
    double tempNorth = 0.0, tempEast = 0.0;
    char utmZone[4];
    LLtoUTM(m_iRefEllipsoid, m_dOriginLatitude, 
            m_dOriginLongitude, tempNorth, 
            tempEast, utmZone);

    //Then set the Origin for the Northing/Easting coordinate frame
    //Also make a note of the UTM Zone we are operating in
    SetOriginNorthing(tempNorth);
    SetOriginEasting(tempEast);
    SetUTMZone(utmZone);

    //We set this flag to indicate that the first calculation of distance
    //traveled is with respect to the origin coordinates.
    m_bSTEP_AFTER_INIT = true;

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


double CMOOSGeodesy::GetMetersNorth()
{
    return m_dNorth;
}

double CMOOSGeodesy::GetMetersEast()
{
    return m_dEast;
}


bool CMOOSGeodesy::LLtoUTM(int ReferenceEllipsoid, const double Lat, 
                            const double Long, double &UTMNorthing, 
                            double &UTMEasting, char *UTMZone)
{
    //converts lat/long to UTM coords.  Equations from USGS Bulletin 1532 
    //East Longitudes are positive, West longitudes are negative. 
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees
    //Written by Chuck Gantz- chuck.gantz@globalstar.com

    double a = ellipsoid[ReferenceEllipsoid].EquatorialRadius;
    double eccSquared = ellipsoid[ReferenceEllipsoid].eccentricitySquared;
    double k0 = 0.9996;

    double LongOrigin;
    double eccPrimeSquared;
    double N, T, C, A, M;
    
    //Make sure the longitude is between -180.00 .. 179.9
    double LongTemp = (Long+180)-int((Long+180)/360)*360-180; // -180.00 .. 179.9;

    double LatRad = Lat*deg2rad;
    double LongRad = LongTemp*deg2rad;
    double LongOriginRad;
    int    ZoneNumber;

    ZoneNumber = int((LongTemp + 180)/6) + 1;
  
    if( Lat >= 56.0 && Lat < 64.0 && LongTemp >= 3.0 && LongTemp < 12.0 )
        ZoneNumber = 32;

    // Special zones for Svalbard
    if( Lat >= 72.0 && Lat < 84.0 ) 
    {
      if(      LongTemp >= 0.0  && LongTemp <  9.0 ) ZoneNumber = 31;
      else if( LongTemp >= 9.0  && LongTemp < 21.0 ) ZoneNumber = 33;
      else if( LongTemp >= 21.0 && LongTemp < 33.0 ) ZoneNumber = 35;
      else if( LongTemp >= 33.0 && LongTemp < 42.0 ) ZoneNumber = 37;
     }
    LongOrigin = (ZoneNumber - 1)*6 - 180 + 3;  //+3 puts origin in middle of zone
    LongOriginRad = LongOrigin * deg2rad;

    //compute the UTM Zone from the latitude and longitude
    sprintf(UTMZone, "%d%c", ZoneNumber, UTMLetterDesignator(Lat));

    eccPrimeSquared = (eccSquared)/(1-eccSquared);

    N = a/sqrt(1-eccSquared*sin(LatRad)*sin(LatRad));
    T = tan(LatRad)*tan(LatRad);
    C = eccPrimeSquared*cos(LatRad)*cos(LatRad);
    A = cos(LatRad)*(LongRad-LongOriginRad);

    M = a*((1    - eccSquared/4        - 3*eccSquared*eccSquared/64    - 5*eccSquared*eccSquared*eccSquared/256)*LatRad 
                - (3*eccSquared/8    + 3*eccSquared*eccSquared/32    + 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatRad)
                                    + (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatRad) 
                                    - (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatRad));
    
    UTMEasting = (double)(k0*N*(A+(1-T+C)*A*A*A/6
                    + (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120)
                    + 500000.0);

    UTMNorthing = (double)(k0*(M+N*tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
                 + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));
    if(Lat < 0)
        UTMNorthing += 10000000.0; //10000000 meter offset for southern hemisphere

    return true;
}



char CMOOSGeodesy::UTMLetterDesignator(double Lat)
{
    //This routine determines the correct UTM letter designator for the given latitude
    //returns 'Z' if latitude is outside the UTM limits of 84N to 80S
    //Written by Chuck Gantz- chuck.gantz@globalstar.com
    char LetterDesignator;

    if((84 >= Lat) && (Lat >= 72)) LetterDesignator = 'X';
    else if((72 > Lat) && (Lat >= 64)) LetterDesignator = 'W';
    else if((64 > Lat) && (Lat >= 56)) LetterDesignator = 'V';
    else if((56 > Lat) && (Lat >= 48)) LetterDesignator = 'U';
    else if((48 > Lat) && (Lat >= 40)) LetterDesignator = 'T';
    else if((40 > Lat) && (Lat >= 32)) LetterDesignator = 'S';
    else if((32 > Lat) && (Lat >= 24)) LetterDesignator = 'R';
    else if((24 > Lat) && (Lat >= 16)) LetterDesignator = 'Q';
    else if((16 > Lat) && (Lat >= 8)) LetterDesignator = 'P';
    else if(( 8 > Lat) && (Lat >= 0)) LetterDesignator = 'N';
    else if(( 0 > Lat) && (Lat >= -8)) LetterDesignator = 'M';
    else if((-8> Lat) && (Lat >= -16)) LetterDesignator = 'L';
    else if((-16 > Lat) && (Lat >= -24)) LetterDesignator = 'K';
    else if((-24 > Lat) && (Lat >= -32)) LetterDesignator = 'J';
    else if((-32 > Lat) && (Lat >= -40)) LetterDesignator = 'H';
    else if((-40 > Lat) && (Lat >= -48)) LetterDesignator = 'G';
    else if((-48 > Lat) && (Lat >= -56)) LetterDesignator = 'F';
    else if((-56 > Lat) && (Lat >= -64)) LetterDesignator = 'E';
    else if((-64 > Lat) && (Lat >= -72)) LetterDesignator = 'D';
    else if((-72 > Lat) && (Lat >= -80)) LetterDesignator = 'C';
    else LetterDesignator = 'Z'; //This is here as an error flag to show that the Latitude is outside the UTM limits

    return LetterDesignator;
}

void CMOOSGeodesy::SetMetersNorth(double North)
{
    m_dNorth = North;
}

void CMOOSGeodesy::SetMetersEast(double East)
{
    m_dEast = East;
}

void CMOOSGeodesy::SetOriginNorthing(double North)
{
    m_dOriginNorthing = North;
}

void CMOOSGeodesy::SetOriginEasting(double East)
{
    m_dOriginEasting = East;
}

int CMOOSGeodesy::GetRefEllipsoid()
{
    return m_iRefEllipsoid;
}

void CMOOSGeodesy::SetRefEllipsoid(int refEllipsoid)
{
    m_iRefEllipsoid = refEllipsoid;
}

void CMOOSGeodesy::SetUTMZone(char * utmZone)
{
    sprintf(m_sUTMZone, utmZone);
}

char * CMOOSGeodesy::GetUTMZone()
{
    return m_sUTMZone;
}

/**
*This method is the interface to this class and allows the client to 
*query the amount of ground covered with respect to the origin where the
*origin is defined as a point in the UTM grid where we got an initial 
*GPS fix that we defined to be the origin.  What this method does not take
*into account is the curvature of the reference ellipsoid at a particular
*Lat/Lon value.  Curvature influences the deltaX and deltaY that this method
*calculates for determining the overall distance traveled wrt the origin.
*Therefore, at Lat/Lon values that are significantly far enough (~300km) away from
*the origin of the UTM grid (0,0), a shift in one dimension, i.e. just along
*Latitude, or just along Longitude, does not map to a corresponding one
*dimensional shift in our "local" grid where we should be seeing just a 
*deltaX or deltaY result in moving in only one direction.  Instead, we have
*observed that moving just .0001 degrees in Longitude (~1m in local) results in both
*a deltaX that is coupled to a deltaY.  
*
*@param lat The current Latitude the vehicle is at
*@param lon The current Longitude the vehicle is at
*@param MetersNorth The distance in meters traveled North wrt to Origin
*@param MetersEast The distance in meters traveled East wrt to Origin
*/

bool CMOOSGeodesy::LatLong2LocalUTM(double lat,
                                    double lon, 
                                    double &MetersNorth,
                                    double &MetersEast)
{
    //first turn the lat/lon into UTM
    double tmpNorth = 0.0, tmpEast = 0.0;
    double dN = 0.0, dE = 0.0; 
    char tmpUTM[4];

    

    LLtoUTM(m_iRefEllipsoid,lat,lon,tmpNorth,tmpEast,tmpUTM);

    //could check for the UTMZone differing, and if so, return false

    //If this is the first time through the loop,then
    //compare the returned Northing & Easting values with the origin.
    //This does not need to be split like into before and after first reading, but
    //makes the calculation clearer.  Plus, can add other features like logging
    //or publishing of value
    if(m_bSTEP_AFTER_INIT){
        dN = tmpNorth - GetOriginNorthing();
        dE = tmpEast - GetOriginEasting();
        SetMetersNorth(dN);
        SetMetersEast(dE);

        m_bSTEP_AFTER_INIT = !m_bSTEP_AFTER_INIT;
    }else{
        double totalNorth = tmpNorth - GetOriginNorthing();
        dN = totalNorth - GetMetersNorth();
        //add the increment to the current North value
        SetMetersNorth(dN + GetMetersNorth());

        double totalEast = tmpEast - GetOriginEasting();
        dE = totalEast - GetMetersEast();
        //add the increment to the current East value
        SetMetersEast(dE + GetMetersEast());
    }

    //This is the total distance traveled thus far, either North or East
    MetersNorth = GetMetersNorth();
    MetersEast = GetMetersEast();
    
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

double CMOOSGeodesy::DMS2DecDeg(double dfVal)
{
    int nDeg = (int)(dfVal/100.0);

    double dfTmpDeg = (100.0*(dfVal/100.0-nDeg))/60.0;
    
    return  dfTmpDeg+nDeg;


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

    double dftanlat2 = pow(tan(lat*deg2rad),2);
    double dfRadius = dfb*sqrt(1+dftanlat2) / sqrt( ( pow(dfb,2)/pow(dfa,2) )+dftanlat2);



    //the decimal degrees conversion should take place elsewhere
    double dXArcDeg  = (lon - GetOriginLongitude()) * deg2rad;
    double dX = dfRadius * sin(dXArcDeg)*cos(lat*deg2rad);
    
    double dYArcDeg  = (lat - GetOriginLatitude()) * deg2rad;
    double dY = dfRadius * sin(dYArcDeg);

    SetLocalGridX(dX);
    SetLocalGridY(dY);

    //This is the total distance traveled thus far, either North or East
    MetersNorth = GetLocalGridY();
    MetersEast  = GetLocalGridX();
    
    return true;
}

void CMOOSGeodesy::SetLocalGridX(double X)
{
    m_dLocalGridX = X;
}

void CMOOSGeodesy::SetLocalGridY(double Y)
{
    m_dLocalGridY = Y;
}

double CMOOSGeodesy::GetLocalGridX()
{
    return m_dLocalGridX;
}

double CMOOSGeodesy::GetLocalGridY()
{
    return m_dLocalGridY;
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
    //(semimajor axis)
    double dfa  = 6378137;
    // (semiminor axis)
    double dfb = 6356752;

    double dftanlat2 = pow( tan( GetOriginLatitude()*deg2rad ), 2 );
    double dfRadius = dfb*sqrt( 1+dftanlat2 ) / sqrt( ( pow(dfb,2)/pow(dfa,2) )+dftanlat2 );

    //first calculate lat arc
    double dfYArcRad = asin( dfNorth/dfRadius ); //returns result in rad
    double dfYArcDeg = dfYArcRad * rad2deg;
 
    double dfXArcRad = asin( dfEast/( dfRadius*cos( GetOriginLatitude()*deg2rad ) ) );
    double dfXArcDeg = dfXArcRad * rad2deg;
        
    //add the origin to these arc lengths
    dfLat = dfYArcDeg + GetOriginLatitude();
       dfLon = dfXArcDeg + GetOriginLongitude();

    return true;
}


bool CMOOSGeodesy::UTM2LatLong(double dfX, double dfY, double& dfLat, double& dfLong)
{
    //written by Henrik Schmidt henrik@mit.edu
    
    double err = 1e20;
    double dfx=dfX;
    double dfy=dfY;
    double eps = 1.0; // accuracy in m
    
    while (err > eps)
    {
        double dflat, dflon, dfnew_x, dfnew_y ;

        // first guess geodesic
        if (!LocalGrid2LatLong(dfx,dfy,dflat,dflon))
            return(false);
        
        // now convert latlong to UTM
        if (!LatLong2LocalUTM(dflat,dflon,dfnew_y,dfnew_x))
            return(false);
        
        // how different
        double dfdiff_x = dfnew_x -dfX;
        double dfdiff_y = dfnew_y -dfY;
        
        // subtract difference and reconvert        
        dfx -= dfdiff_x;
        dfy -= dfdiff_y;
        
        err = hypot(dfnew_x-dfX,dfnew_y-dfY);
        
        //MOOSTrace("UTM2LatLong: error = %f\n",err); 
    }
    
    if (!LocalGrid2LatLong(dfx, dfy, dfLat, dfLong))
        return(false);
    
    
 	return true;
}


/*****************************************************************/
/*    NAME: Gerry Poirier                                        */
/*    ORGN: NUWCDIVNPT Newport RI                                */
/*    FILE: AscmContact.cpp                                      */
/*    DATE: 03/27/08                                             */
/*                                                               */
/*    Class encapsulating basic contact information              */
/*                                                               */
/*****************************************************************/

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "AscmContact.h"

   AscmContact::AscmContact() // Constructor
   {
      id[0] = '\0';
      type[0] = '\0'; 
      MOOSDB_time = 0.0;
      x = 0.0;
      y = 0.0;
      latitude = 0.0;
      longitude = 0.0;
      speed = 0.0; // knots
      heading = 0.0;
      depth = 0.0;
      aouSemiMajor = 0.0; // yds
      aouSemiMinor = 0.0; // yds
      aouOrientation = 0.0;
   }

   AscmContact::AscmContact(const AscmContact &right) // Copy Constructor
   {
      strcpy( id, right.id );
      strcpy( type, right.type );
      MOOSDB_time = right.MOOSDB_time;
      x = right.x;
      y = right.y;
      latitude = right.latitude;
      longitude = right.longitude;
      speed = right.speed;
      heading = right.heading;
      depth = right.depth;
      aouSemiMajor = right.aouSemiMajor;
      aouSemiMinor = right.aouSemiMinor;
      aouOrientation = right.aouOrientation;
   }

   AscmContact::~AscmContact() // Destructor
   {
   }

   void AscmContact::setId( char* pId )
   {
      strcpy(id, pId);
   }

   void AscmContact::setType( char* pType )
   {
       strcpy(type, pType);
   }

   void AscmContact::setTime( double pTime )
   {
      MOOSDB_time= pTime;
   }

   void AscmContact::setXY( double pX, double pY )
   {
      x = pX;
      y = pY;
   }

   void AscmContact::setLatLon( double pLat, double pLon )
   {
      latitude = pLat;
      longitude = pLon;
   }

   void AscmContact::setSpeed( double pSpeed )
   {
      speed = pSpeed;
   }

   void AscmContact::setHeading( double pHeading )
   {
      heading = pHeading;
   }

   void AscmContact::setDepth( double pDepth )
   {
      depth = pDepth;
   }

   void AscmContact::setAou( double pMajor, double pMinor, double pOrientation )
   {
      aouSemiMajor = pMajor;
      aouSemiMinor = pMinor;
      aouOrientation = pOrientation;
   }


   char* AscmContact::getId( void )
   {
      return( &id[0] );
   }

   char* AscmContact::getType( void )
   {
      return( &type[0] );
   }

   double AscmContact::getTime( void )
   {
      return( MOOSDB_time );
   }

   double AscmContact::getX( void )
   {
      return( x );
   }

   double AscmContact::getY( void )
   {
      return( y );
   }

   double AscmContact::getLatitude( void )
   {
      return( latitude );
   }

   double AscmContact::getLongitude( void )
   {
      return( longitude );
   }

   double AscmContact::getSpeed( void )
   {
      return( speed );
   }

   double AscmContact::getHeading( void )
   {
      return( heading );
   }

   double AscmContact::getDepth( void )
   {
      return( depth );
   }

   double AscmContact::getAouSemiMajorAxis( void )
   {
      return( aouSemiMajor );
   }

   double AscmContact::getAouSemiMinorAxis( void )
   {
      return( aouSemiMinor );
   }

   double AscmContact::getAouOrientation( void )
   {
      return( aouOrientation );
   }



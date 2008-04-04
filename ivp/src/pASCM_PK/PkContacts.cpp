#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <iostream>
#include "PkContacts.h"

double PkContacts::longitudeFunc( double pOffsetInYds, double pLongitude, double pNewLatitude )
{
   double  lResult;
   double  lLonOffset;

   if ( ( pNewLatitude < 90.0 ) && ( pNewLatitude > -90 ) )
   {
      lLonOffset = pOffsetInYds * E0 * (180.0/MPI)/ cos(DEG2RAD * pNewLatitude);
      if (lLonOffset > 0.0)
      {
         while (lLonOffset > 360.0)
         {
            lLonOffset -= 360.0;
         }
         if (lLonOffset > 180.0)
         {
            lLonOffset -= 360.0;
         }
      }
      else
      {
         if (lLonOffset < 0.0)
         {
            while (lLonOffset < -360.0)
            {
               lLonOffset += 360.0;
            }
            if (lLonOffset < -180.0)
            {
               lLonOffset += 360.0;
            }
         }
      }
   }
   else
   {
      lLonOffset = 0.0;
   }
   lResult = pLongitude + lLonOffset;
   if (lResult > 180.0)
   {
      lResult -= 360.0;
   }
   else
   {
      if (lResult <= -180.0)
      {
         lResult += 360.0;
      }
   }

   return lResult ;
}
 
double PkContacts::latitudeFunc( double pOffsetInYds, double pLatitude )
{
   double  lResult;

   lResult = pLatitude + (pOffsetInYds * E0 * (180.0 / MPI) );

   if ((0.0 <= pLatitude) && (pLatitude <= 90.0))
   {
      if (pOffsetInYds >= 0.0)
      {
         if ( lResult > 90.0 )
         {
            cerr << "ERROR  latitude > 90.0" << endl;
         }
      }
      else
      {
         if (lResult < 0.0)
         {
            if (lResult < -90.0)
             {
                cerr << "ERROR  latitude > -90.0" << endl;
             }
         }
      }
   }
   else
   {
      if (pOffsetInYds >= 0.0)
      {
         if (lResult > 90.0)
         {
            cerr << "ERROR latitude > 90.0" << endl;
         }
      }
      else
      {
         if (lResult < -90.0)
         {
            cerr << "ERROR latitude > 270.0" << endl;
         }
      }
   }

   return lResult;
}

//void PkContacts::pkContact( long pDeltaTimeSec, 
//                               double pCourseOverGround, 
//                               double pSpeedOverGroundInKnots, 
//                               double pStartingLat, 
//                               double pStartingLon, 
//                               double *pPkLat, 
//                               double *pPkLon )

AscmContact* PkContacts::pkContact( AscmContact* pContactToPk, double pDeltaTimeSec )
{
   double lPkLat = pContactToPk->getLatitude();
   double lPkLon = pContactToPk->getLongitude();
   double lPkToTime = pContactToPk->getTime();
   
   AscmContact* lPkContact = new AscmContact( *pContactToPk );

//   cout << "pkContact(): pDeltaTimeSec        = " << pDeltaTimeSec << " sec" << endl;
//
// PK Lat/Lon
//
   double lDistanceTraveledInYds = pContactToPk->getSpeed() * KNOTS_2_YDSPERSEC * pDeltaTimeSec ;

//   cout << "distance traveled in yds = " << lDistanceTraveledInYds << endl;

   double lXDistance = sin( pContactToPk->getHeading() * DEG2RAD) * lDistanceTraveledInYds ;
   double lYDistance = cos( pContactToPk->getHeading() * DEG2RAD) * lDistanceTraveledInYds ;

   lPkLat = latitudeFunc( lYDistance, pContactToPk->getLatitude() );
   lPkLon = longitudeFunc( lXDistance, pContactToPk->getLongitude(), lPkLat );
//
// Need to recompute X/Y
//
// #include <MOOSGenLib/MOOSGenLib.h>
// double lPkX = 0.0;
// double lPkY = 0.0;
// if( !m_Geodesy.LatLong2LocalGrid( lPkLat, lPkLon, lPkY, lPkX ) )
// { 
//    cout << "ERROR: m_Geodesy.LatLong2LocalGrid "<<endl;
// }
// lPkContact->setX( lPkX, lPkY );
//

   lPkContact->setLatLon( lPkLat, lPkLon );
//
// Need to recompute AOU
//
//

   lPkContact->setTime( lPkToTime + (double)pDeltaTimeSec );
/*
   cout << "starting Lat   " << pContactToPk->getLatitude() << endl;
   cout << "final Lat      " << lPkContact->getLatitude() << endl;
   cout << "starting Lon   " << pContactToPk->getLongitude() << endl;
   cout << "final Lon      " << lPkContact->getLongitude() << endl;
   cout << "starting Time  " << pContactToPk->getTime() << endl;
   cout << "final Time     " << lPkContact->getTime() << endl;
*/
   return lPkContact ;      
}


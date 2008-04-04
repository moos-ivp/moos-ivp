#ifndef _PkContacts_h_
#define _PkContacts_h_

#include "AscmContact.h"

class PkContacts
{

   protected:

      static const double MPI = 3.141159265358979;
      static const double E0 = (1.0066979/6975197.3);
      static const double DEG2RAD = 0.01745329252;
      static const double KNOTS_2_YDSPERSEC = 0.5626;

      static double longitudeFunc( double pOffsetInYds, double pLongitude, double pNewLatitude );

      static double latitudeFunc( double pOffsetInYds, double pLatitude );

   public:

//    static void pkContact( long pDeltaTimeSec, double pCourseOverGround, 
//                           double pSpeedOverGroundInKnots, double pStartingLat, 
//                           double pStartingLon, double *pPkLat, double *pPkLon );

      static AscmContact* pkContact( AscmContact* pContactToPk, double pDeltaTimeSec ); // Calling routine needs to 
                                                                                      // delete returned AscmContact*

};

#endif


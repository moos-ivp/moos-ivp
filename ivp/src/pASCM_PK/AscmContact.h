/*****************************************************************/
/*    NAME: Gerry Poirier                                        */
/*    ORGN: NUWCDIVNPT Newport RI                                */
/*    FILE: AscmContact.h                                        */
/*    DATE: 03/27/08                                             */
/*                                                               */
/*    Class encapsulating basic contact information              */
/*                                                               */
/*****************************************************************/

#ifndef _AscmContact_h_
#define _AscmContact_h_

class AscmContact
{

   protected:

      char id[16]; // MMSI for AIS
      char type[64]; 
      double MOOSDB_time;
      double x;
      double y;
      double latitude;
      double longitude;
      double speed; // knots
      double heading;
      double depth;
      double aouSemiMajor; // yds
      double aouSemiMinor; // yds
      double aouOrientation;

   public:

      AscmContact(); // Constructor

      AscmContact(const AscmContact &right); // Copy Constructor

      virtual ~AscmContact(); // Destructor

      void setId( char* pId );
      void setType( char* pType );
      void setTime( double pTime );
      void setXY( double pX, double pY );
      void setLatLon( double pLat, double pLon );
      void setSpeed( double pSpeed );
      void setHeading( double pHeading );
      void setDepth( double pDepth );
      void setAou( double pMajor, double pMinor, double pOrientation );

      char* getId( void );
      char* getType( void );
      double getTime( void );
      double getX( void );
      double getY( void );
      double getLatitude( void );
      double getLongitude( void );
      double getSpeed( void );
      double getHeading( void );
      double getDepth( void );
      double getAouSemiMajorAxis( void );
      double getAouSemiMinorAxis( void );
      double getAouOrientation( void );
};

#endif


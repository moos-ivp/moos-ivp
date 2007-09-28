#ifndef XRAY_PACKETS_H
#define XRAY_PACKETS_H

#include <time.h>

#include "WhoiUtil.h"
#include "CCLMessageTypes.h"

// XRay packet identifiers
#define XRAY_BEHAVIOR_MODE  MDAT_XRAY_BEHAVIOR
#define XRAY_STATUS_MODE    MDAT_XRAY_STATUS      // from CCLMessageTypes.h

/* ------------------------------------------------------------------ */
/*                          BEHAVIOR PACKET                           */
/* ------------------------------------------------------------------ */

// Generic type definitions
typedef enum 
    {
        SEAGLIDER_DIVE = 0,
        ATTITUDE_HOLD,
        SPIRAL
        // etc... Up to 256 mods
    } XRAY_BEHAVIOR_TYPE; 


// Create boolean type
typedef char BOOLEAN;
#define TRUE 1
#define FALSE 0

// Packed (32-byte) behavior struct
typedef struct
{	
	unsigned char           mode;                   // XRAY_BEHAVIOR_MODE
	unsigned char		behaviorType;	        // Specify one of 256 behaviors
	TIME_DATE               time;			// Generic time (3 bytes)(flags:bit0 dependent)
	unsigned short	        depth1;	  		// Generic depth parameter
	unsigned short	        depth2;			// Generic depth parameter
	unsigned short	        depth3;			// Generic depth parameter
	LATLON_COMPRESSED	latitude1;		// Generic latitude parameter (3 bytes)
	LATLON_COMPRESSED	longitude1;		// Generic longitude parameter (3 bytes)
	LATLON_COMPRESSED	latitude2;		// Generic latitude parameter (3 bytes)
	LATLON_COMPRESSED	longitude2;		// Generic longitude parameter (3 bytes)
	unsigned char		heading;		// Magnetic heading
	unsigned char		headingRate;	        // Heading rate
	unsigned char		speed;			// Speed parameter (flags:bit1 dependent)
	unsigned char		flags;			// Generic flags (8 control bits)
	char           		password[2];	        // Password for behavior execution control (2 bytes)
	unsigned char		spare1;			// Unused: For future use
	unsigned char		spare2; 		// Unused: For future use
	unsigned char		spare3;			// Unused: For future use
} mdat_xray_behavior;  // was MDAT_XRAY_BEHAVIOR -MG

// Unpacked behavior struct
typedef struct
{	
    XRAY_BEHAVIOR_TYPE behaviorType;
    long               time;              // Unix sec-since-1970
    double             depth1;            // meters
    double             depth2;            // meters
    double             depth3;            // meters
    double             latitude1;         // degrees
    double             longitude1;        // degrees
    double             latitude2;         // degrees
    double             longitude2;        // degrees
    double             heading;           // magnetic, degrees
    double             headingRate;       // degrees/second
    double             speed;             // meters/second
    BOOLEAN            flag1;             // 0=False, Else True
    BOOLEAN            flag2;
    BOOLEAN            flag3;
    BOOLEAN            flag4;
    BOOLEAN            flag5;
    BOOLEAN            flag6;
    BOOLEAN            flag7;
    BOOLEAN            flag8;
    char               password[2];       // 2-char passwd

} xray_behavior; // was XRAY_BEHAVIOR; -MG

/* ------------------------------------------------------------------ */
/*                          STATUS PACKET                             */
/* ------------------------------------------------------------------ */

// Define XRay fault types
typedef enum
    {
        NO_FAULT,
        MVC_HOUSING_LEAK,
        PCS_LOST_COMMS,
        ERS_EVENT
        // etc... Up to 256 faults
    } XRAY_FAULT_TYPE;

// Define XRay response types
typedef enum
    {
        BEHAVIOR_VALID,
        BEHAVIOR_INVALID,
        BEHAVIOR_CLOSEST
        // etc... Up to 256 responses
    } XRAY_RESPONSE_TYPE;

// Packed status struct
typedef struct
{	
    unsigned char	  mode;            
    TIME_DATE		  time;				// Generic time (3 bytes)(flags:bit0)
    unsigned char	  energy;		    // Energy remaining (percent)
    unsigned short    depth;			// Estimated depth
    unsigned short    depthRate;		// Estimated rate of change of depth
    LATLON_COMPRESSED latitude;			// Estimated latitude (3 bytes)
    LATLON_COMPRESSED longitude;		// Estimated longitude (3 bytes)
    unsigned short	  altitude; 		// Measured distance to bottom
    unsigned short	  heading;			// Estimated heading
    unsigned short	  headingRate;		// Estimated heading rate
    unsigned short	  roll;				// Measured roll angle
    unsigned short	  pitch;			// Measured pitch angle
    unsigned short	  beVolume;         // Estimated water in BE tanks
    unsigned char	  behaviorResponse;	// Response to last requested behavior
    unsigned char	  fault;     		// Fault code
    unsigned char	  flags;			// Generic flags (8 control bits)
    unsigned char	  contacts;			// ASW contact status
} mdat_xray_status; // was MDAT_XRAY_STATUS; -MG

// Unpacked status struct
typedef struct
{	
	long               time;			 // Time, secs-since-1970
	double             energy;		     // Vehicle Energy remaining (percent)
	double             depth;			 // Estimated depth
	double             depthRate;		 // Estimated rate of change of depth
	double             latitude;		 // Estimated latitude
	double             longitude;		 // Estimated longitude
	double             altitude; 		 // Measured distance to bottom
	double        	   heading;			 // Estimated heading
	double       	   headingRate;		 // Estimated heading rate
	double             roll;			 // Measured roll angle
	double             pitch;			 // Measured pitch angle
	double             beVolume;         // Estimated water in BE tanks
	XRAY_RESPONSE_TYPE behaviorResponse; // Response to last requested behavior
	XRAY_FAULT_TYPE    fault;
	BOOLEAN            flag1;            // 0=False, Else True
	BOOLEAN            flag2;
	BOOLEAN            flag3;
	BOOLEAN            flag4;
	BOOLEAN            flag5;
	BOOLEAN            flag6;
	BOOLEAN            flag7;
	BOOLEAN            flag8;
	unsigned char	   contacts;         // ASW contact status (?)
} xray_status ; // was XRAY_STATUS; -MG


/* Method Protopyes */

// Convert unpacked behavior struct to packet
void encodeXRayBehavior( const xray_behavior* behavior,
                         mdat_xray_behavior* packet );

// Convert behavior packet to unpacked behavior struct
void decodeXRayBehavior( const mdat_xray_behavior* packet,
                         xray_behavior* behavior );

// Convert unpacked status struct to packet
void encodeXRayStatus( const xray_status* status,
                         mdat_xray_status* packet );

// Convert status packet to unpacked status struct
void decodeXRayStatus( const mdat_xray_status* packet,
                         xray_status* status );

// Convert a percentage (0-100) into compressed form
// (1 byte) for transmission.
unsigned char encodePercent( float percent );

// Decode a compressed percentage
float decodePercent( unsigned char compressed );

// Convert a heading rate (deg/sec) into compressed form
// (single byte) for transmission.
unsigned char encodeHeadingRate( float headingRate );

// Decode a compressed heading rate.
float decodeHeadingRate( unsigned char compressed );

// Convert a depth rate (met/sec) into compressed form
// (2 bytes) for transmission.
unsigned short encodeDepthRate( float depthRate );

// Decode a compressed depth rate.
float decodeDepthRate( unsigned short compressed );

// Convert a volume (cc) into compressed form
// (2 bytes) for transmission.
unsigned short encodeVolume( float volume );

// Decode a compressed volume.
float decodeVolume( unsigned short compressed );

// Convert an angle (deg) into compressed form
// (2 bytes) for transmission.
unsigned short encodeAngle( float angle );

// Decode a compressed angle
float decodeAngle( unsigned short compressed );

// Decode a compressed date/time directly into sec-since-1970
long decodeTimeDate( TIME_DATE input );

// Print an unpacked behavior struct
void printBehavior( const xray_behavior* behavior );

// Print an unpacked status struct
void printStatus( const xray_status* status );

#endif

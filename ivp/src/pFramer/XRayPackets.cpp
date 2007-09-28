#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "XRayPackets.h"
#include "CCLMessageTypes.h"

/**
 * Method:  encodeXRayBehavior()
 *
 * Purpose: Convert unpacked behavior struct to packet
 *
 * Note:  Resulting MDAT_XRAY_BEHAVIOR* must be allocated by caller
 */
void encodeXRayBehavior( const xray_behavior* behavior,
                         mdat_xray_behavior* packet ) 
{
    packet->mode = (unsigned char) XRAY_BEHAVIOR_MODE;
    packet->behaviorType = (unsigned char) behavior->behaviorType;
    packet->time = Encode_time_date( behavior->time );
    packet->depth1 = Encode_depth( behavior->depth1 );
    packet->depth2 = Encode_depth( behavior->depth2 );
    packet->depth3 = Encode_depth( behavior->depth3 );
    packet->latitude1 = Encode_latlon( behavior->latitude1 );
    // Force longitude to +/- 180
    float longitude =  behavior->longitude1;
    if( longitude > 180. ) longitude -= 360.;
    packet->longitude1 = Encode_latlon( longitude );
    packet->latitude2 = Encode_latlon( behavior->latitude2 );
    longitude = behavior->longitude2;
    if( longitude > 180. ) longitude -= 360.;
    packet->longitude2 = Encode_latlon( longitude );
    packet->heading = encodeAngle( behavior->heading );
    packet->headingRate = encodeHeadingRate( behavior->headingRate );
    packet->speed = Encode_est_velocity( behavior->speed );
    packet->flags = 0;
    if( behavior->flag1 != 0 ) packet->flags |= 0x01;
    if( behavior->flag2 != 0 ) packet->flags |= 0x02;
    if( behavior->flag3 != 0 ) packet->flags |= 0x04;
    if( behavior->flag4 != 0 ) packet->flags |= 0x08;
    if( behavior->flag5 != 0 ) packet->flags |= 0x10;
    if( behavior->flag6 != 0 ) packet->flags |= 0x20;
    if( behavior->flag7 != 0 ) packet->flags |= 0x40;
    if( behavior->flag8 != 0 ) packet->flags |= 0x80;
    strncpy( packet->password, behavior->password, 2 );
    packet->spare1 = 0;
    packet->spare2 = 0;
    packet->spare3 = 0;
}

/**
 * Method:  decodeXRayBehavior()
 *
 * Purpose: Convert behavior packet to unpacked behavior struct
 *
 * Note:  Resulting XRAY_BEHAVIOR* must be allocated by caller
 */
void decodeXRayBehavior( const mdat_xray_behavior* packet,
                         xray_behavior* behavior ) 
{
    behavior->behaviorType = (XRAY_BEHAVIOR_TYPE)packet->behaviorType;
    behavior->time = decodeTimeDate( packet->time );
    behavior->depth1 = Decode_depth( packet->depth1 );
    behavior->depth2 = Decode_depth( packet->depth2 );
    behavior->depth3 = Decode_depth( packet->depth3 );
    behavior->latitude1 = Decode_latlon( packet->latitude1 );
    behavior->longitude1 = Decode_latlon( packet->longitude1 );
    behavior->latitude2 = Decode_latlon( packet->latitude2 );
    behavior->longitude2 = Decode_latlon( packet->longitude2 );
    behavior->heading = decodeAngle( packet->heading );
    behavior->headingRate = decodeHeadingRate( packet->headingRate );
    behavior->speed = Decode_est_velocity( packet->speed );
    behavior->flag1 = (packet->flags & 0x01);
    behavior->flag2 = (packet->flags & 0x02) >> 1;
    behavior->flag3 = (packet->flags & 0x04) >> 2;
    behavior->flag4 = (packet->flags & 0x08) >> 3;
    behavior->flag5 = (packet->flags & 0x10) >> 4;
    behavior->flag6 = (packet->flags & 0x20) >> 5;
    behavior->flag7 = (packet->flags & 0x40) >> 6;
    behavior->flag8 = (packet->flags & 0x80) >> 7;
    strncpy( behavior->password, packet->password, 2 );
}

/**
 * Method:  encodeXRayStatus()
 *
 * Purpose: Convert unpacked status struct to packet
 *
 * Note:  Resulting MDAT_XRAY_STATUS* must be allocated by caller
 */
void encodeXRayStatus( const xray_status* status,
                       mdat_xray_status* packet ) 
{
    packet->mode = (unsigned char) XRAY_STATUS_MODE;
    packet->time = Encode_time_date( status->time );
    packet->energy = encodePercent(status->energy);
    packet->depth = Encode_depth( status->depth );
    packet->depthRate = encodeDepthRate( status->depthRate );
    packet->latitude = Encode_latlon( status->latitude );
    // Force longitude to +/- 180
    float longitude = status->longitude;
    if( longitude > 180. ) longitude -= 360.;
    packet->longitude = Encode_latlon( longitude );
    // Use depth encoder for altitude
    packet->altitude = Encode_depth( status->altitude );
    packet->heading = encodeAngle( status->heading );
    packet->headingRate = encodeHeadingRate( status->headingRate );
    packet->roll = encodeAngle( status->roll );
    packet->pitch = encodeAngle( status->pitch );
    packet->beVolume = encodeVolume( status->beVolume );
    packet->behaviorResponse = (unsigned char) status->behaviorResponse;
    packet->fault = (unsigned char) status->fault;
    packet->flags = 0;
    if( status->flag1 != 0 ) packet->flags |= 0x01;
    if( status->flag2 != 0 ) packet->flags |= 0x02;
    if( status->flag3 != 0 ) packet->flags |= 0x04;
    if( status->flag4 != 0 ) packet->flags |= 0x08;
    if( status->flag5 != 0 ) packet->flags |= 0x10;
    if( status->flag6 != 0 ) packet->flags |= 0x20;
    if( status->flag7 != 0 ) packet->flags |= 0x40;
    if( status->flag8 != 0 ) packet->flags |= 0x80;
    packet->contacts = status->contacts;    // ???
}

/**
 * Method:  decodeXRayStatus()
 *
 * Purpose: Convert status packet to unpacked status struct
 *
 * Note:  Resulting XRAY_STATUS* must be allocated by caller
 */
void decodeXRayStatus( const mdat_xray_status* packet,
                       xray_status* status ) 
{
    status->time = decodeTimeDate( packet->time );
    status->energy = decodePercent(packet->energy);
    status->depth = Decode_depth( packet->depth );
    status->depthRate = decodeDepthRate( packet->depthRate );
    status->latitude = Decode_latlon( packet->latitude );
    status->longitude = Decode_latlon( packet->longitude );
    status->altitude = Decode_depth( packet->altitude );
    status->heading = decodeAngle( packet->heading );
    status->headingRate = decodeHeadingRate( packet->headingRate );
    status->roll = decodeAngle( packet->roll );
    status->pitch = decodeAngle( packet->pitch );
    // Normalize roll & pitch
    if( status->roll > 180 ) status->roll -= 360;
    if( status->pitch > 180 ) status->pitch -= 360;
    status->beVolume = decodeVolume( packet->beVolume );
    status->behaviorResponse = (XRAY_RESPONSE_TYPE) packet->behaviorResponse;
    status->fault = (XRAY_FAULT_TYPE) packet->fault;
    status->flag1 = (packet->flags & 0x01);
    status->flag2 = (packet->flags & 0x02) >> 1;
    status->flag3 = (packet->flags & 0x04) >> 2;
    status->flag4 = (packet->flags & 0x08) >> 3;
    status->flag5 = (packet->flags & 0x10) >> 4;
    status->flag6 = (packet->flags & 0x20) >> 5;
    status->flag7 = (packet->flags & 0x40) >> 6;
    status->flag8 = (packet->flags & 0x80) >> 7;
    status->contacts = packet->contacts;
}

/**
 * Method:  encodePercent()
 *
 * Purpose: Convert a percentage (0-100) into compressed form
 *          (1 byte) for transmission.
 *
 * Limitations: Input must be between 0 and 100.
 *              Resolution is ~100/256 ~0.39%
 */
unsigned char encodePercent( float percent )
{
    // Convert to single char LSB units
    return (unsigned char)(percent*(255./100.) + 0.5);
}

/**
 * Method:  decodePercent()
 *
 * Purpose: Decode a compressed percentage
 */
float decodePercent( unsigned char compressed ) 
{
    return ((float)compressed * (100./255.));
}

/**
 * Method:  encodeHeadingRate()
 *
 * Purpose: Convert a heading rate (deg/sec) into compressed form
 *          (single byte) for transmission.
 *
 * Limitations: Input rate must be between -20 and +20 deg/sec.
 *              Resolution is ~40/256 ~0.16 deg/sec
 */
unsigned char encodeHeadingRate( float headingRate ) 
{
    // Make non-negative
    headingRate += 20.0;
    // Convert to single char LSB units
    return  (unsigned char)(headingRate*(255./40.) + 0.5);
}

/**
 * Method:  decodeHeadingRate()
 *
 * Purpose: Decode a compressed heading rate.
 */
float decodeHeadingRate( unsigned char compressed ) 
{
    // Decode to 0-40 range
    float headingRate = (float)compressed * (40./255.);
    // Enable sign
    return headingRate-20.;
}

/**
 * Method:  encodeDepthRate()
 *
 * Purpose: Convert a depth rate (met/sec) into compressed form
 *          (unsigned short) for transmission.
 *
 * Limitations: Input rate must be between -10 and +10 met/sec.
 *              Resolution is ~20/65536 ~3E-4 met/sec
 */
unsigned short encodeDepthRate( float depthRate ) 
{
    // Make non-negative
    depthRate += 10.0;
    // Convert to single char LSB units
    return (unsigned short)(depthRate*(65535./20.) + 0.5);
}

/**
 * Method:  decodeDepthRate()
 *
 * Purpose: Decode a compressed depth rate.
 */
float decodeDepthRate( unsigned short compressed ) 
{
    // Decode to 0-20 range
    float depthRate = (float)compressed * (20./65535.);
    // Enable sign
    return depthRate - 10.;
}

/**
 * Method:  encodeVolume()
 *
 * Purpose: Convert a volume (cc) into compressed form
 *          (2 bytes) for transmission.
 *
 * Limitations: Input volume must be between 0 and 40000 cc.
 *              Resolution is ~40000/65536 ~0.6cc
 */
unsigned short encodeVolume( float volume ) 
{
    // Convert to single char LSB units
    return (unsigned short)(volume*(65535./40000.) + 0.5);
}

/**
 * Method:  decodeVolume()
 *
 * Purpose: Decode a compressed volume
 */
float decodeVolume( unsigned short compressed ) 
{
    return ((float)compressed * (40000./65535.));
}

/**
 * Method:  encodeAngle()
 *
 * Purpose: Convert an angle (deg) into compressed form
 *          (2 bytes) for transmission.
 *
 * Limitations: Input angle must be between 0 and 360 deg.
 *              Resolution is ~360/65536 ~5E-3 deg
 */
unsigned short encodeAngle( float angle ) 
{
    // Force to [0-360]
    while( angle<0. ) angle += 360.;
    while( angle>360.) angle -= 360;
    return (unsigned short)(angle*(65535./360.) + 0.5);
}

/**
 * Method:  decodeAngle()
 *
 * Purpose: Decode a compressed angle
 */
float decodeAngle( unsigned short compressed ) 
{
    return ((float)compressed * (360./65535.));
}

/**
 * Method:  decodeTimeDate()
 *
 * Purpose: Decode a compressed date/time directly into sec-since-1970
 */
long decodeTimeDate( TIME_DATE input ) 
{
    // Use the WHOI routine first
    short mon, day, hour, min, sec;
    Decode_time_date( input, &mon, &day, &hour, &min, &sec );
    /*   printf("\n\n   - Month: %d, Day: %d, Hour: %d, Min: %d, Sec: %d", */
    /*          mon, day, hour, min, sec); */

    // Convert to seconds-since-1970 (GMT)
    struct tm theTime;
    theTime.tm_year = 2006 - 1900;    // FIXME
    theTime.tm_mon = mon - 1;  // tm month start from 0, not 1
    theTime.tm_mday = day;
    theTime.tm_hour = hour;
    theTime.tm_min = min;
    theTime.tm_sec = sec;
    time_t unixSeconds = timegm( &theTime );  // inverse of gmtime

    return (long)unixSeconds;
}

/**
 * Method:  printBehavior()
 *
 * Purpose: Print an unpacked behavior struct
 */
void printBehavior( const xray_behavior* behavior )
{
    printf("\n  - behaviorType = %d", behavior->behaviorType);
    printf("\n  - depth1 = %f", behavior->depth1);
    printf("\n  - depth2 = %f", behavior->depth2);
    printf("\n  - depth3 = %f", behavior->depth3);
    printf("\n  - latitude1 = %f", behavior->latitude1);
    printf("\n  - longitude1 = %f", behavior->longitude1);
    printf("\n  - latitude2 = %f", behavior->latitude2);
    printf("\n  - longitude2 = %f", behavior->longitude2);
    printf("\n  - heading = %f", behavior->heading );
    printf("\n  - headingRate = %f", behavior->headingRate);
    printf("\n  - speed = %f", behavior->speed);
    printf("\n  - time = %ld", behavior->time);
    printf("\n  - flag1 = %d", behavior->flag1);
    printf("\n  - flag2 = %d", behavior->flag2);
    printf("\n  - flag3 = %d", behavior->flag3);
    printf("\n  - flag4 = %d", behavior->flag4);
    printf("\n  - flag5 = %d", behavior->flag5);
    printf("\n  - flag6 = %d", behavior->flag6);
    printf("\n  - flag7 = %d", behavior->flag7);
    printf("\n  - flag8 = %d", behavior->flag8);
    printf("\n  - password = %s", behavior->password);
}

/**
 * Method:  printStatus()
 *
 * Purpose: Print an unpacked status struct
 */
void printStatus( const xray_status* status )
{
    printf("\n  - time = %ld", status->time);
    printf("\n  - energy = %f", status->energy);
    printf("\n  - depth = %f", status->depth);
    printf("\n  - depthRate = %f", status->depthRate);
    printf("\n  - latitude = %f", status->latitude);
    printf("\n  - longitude = %f", status->longitude);
    printf("\n  - altitude = %f", status->altitude);
    printf("\n  - heading = %f", status->heading );
    printf("\n  - headingRate = %f", status->headingRate);
    printf("\n  - roll = %f", status->roll);
    printf("\n  - pitch = %f", status->pitch);
    printf("\n  - beVolume = %f", status->beVolume);
    printf("\n  - behaviorResponse = %d", status->behaviorResponse);
    printf("\n  - fault = %d", status->fault);
    printf("\n  - flag1 = %d", status->flag1);
    printf("\n  - flag2 = %d", status->flag2);
    printf("\n  - flag3 = %d", status->flag3);
    printf("\n  - flag4 = %d", status->flag4);
    printf("\n  - flag5 = %d", status->flag5);
    printf("\n  - flag6 = %d", status->flag6);
    printf("\n  - flag7 = %d", status->flag7);
    printf("\n  - flag8 = %d", status->flag8);
    printf("\n  - contacts = %d", status->contacts);
}

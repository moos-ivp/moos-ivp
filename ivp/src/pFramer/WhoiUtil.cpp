#include <time.h>   // P.Brodsky
#include <stdio.h>

#include "WhoiUtil.h"

LATLON_COMPRESSED Encode_latlon(double latlon_in) {
  /* Latitude and longitude are compressed into 3 byte values each. */ 
  LONG_AND_COMP out;
  double encoded;
  encoded = latlon_in * ((double)(0x007FFFFFL)/180.0);
  encoded += (encoded > 0.0) ? 0.5 : -0.5;
  // deal with truncation 
  out.as_long =(long int) encoded;
  return(out.as_compressed);
} 


double Decode_latlon(LATLON_COMPRESSED latlon_in) { 
  LONG_AND_COMP in; 
  in.as_long = 0;   //Clear the MSB 
  in.as_compressed = latlon_in; 
  if (in.as_long & 0x00800000L) // sign extend 
    in.as_long |= 0xFF000000L; 
  // P.Brodsky - wrap first (double) below
  return((double)in.as_long * (180.0/(double)(0x007FFFFFL))); 
}

unsigned char Encode_heading(float heading) 
{
  return((unsigned char)(heading * 255.0/360.0 + 0.5));
} 

double Decode_heading(unsigned char heading) 
{
  return((double)heading * 360.0 / 255.0 );
} 

/* Encodes velocity in meters per second into a single byte with a 
 * resolution of 2.5 cm/sec. Input range is 0- ~6 meters/second 
 * (12 knots). 
 */ 
char Encode_est_velocity(float est_velocity) 
{
  return((char)(est_velocity * 25.0));
} 

float Decode_est_velocity(char est_velocity) 
{
  return(est_velocity/25.0);
} 

/* Code-decode salinity in range of 20-45 parts per thousand at a 
 * resolution of 0.1 part per thousand. */ 
unsigned char Encode_salinity(float salinity) 
{
  float output;
  if (salinity < 20.0) return(0);
  else 
    {
      output = (salinity - 20.0) * 10;
      if (output > 255) output = 255;
      return((unsigned char)output);
    }
}

float Decode_salinity(unsigned char sal) 
{
  if (sal == 0) return(sal);
  return(((float)sal/10.0) + 20.0);
}

unsigned short Encode_depth(float depth) 
     /* 0 -100 meters: 0-1000 (10 cm resolution) 
      * 100 -200 meters: 1001-1500 (20 cm resolution) 
      * 200 -1000 meters: 1501-3100 (50 cm resolution) 
      * 1000-6000 meters: 3101-8100 (1 meter resolution) */ 
{
  if (depth < 0) return(0);
  if (depth < 100) return((short unsigned int)((depth+.05) * 1.0/0.1));
  if (depth < 200) return((short unsigned int)(((depth-100) + 0.1 ) * 1.0/0.2 + 1000));
  if (depth < 1000) return((short unsigned int)(((depth-200) + 0.25) * 1.0/0.5 + 1500));
  if (depth < 6000) return((short unsigned int)(((depth-1000) + 0.5 ) * 1.0/1.0 + 3100));
  return(8100);
}

float Decode_depth(unsigned short depth) 
{
  /* 0 -100 meters: 0-1000 (10 cm resolution) 
   * 100 -200 meters: 1001-1500 (20 cm resolution) 
   * 200 -1000 meters: 1501-3100 (50 cm resolution) 
   * 1000-6000 meters: 3101-8100 (1 meter resolution) 
   */ 
  unsigned short DEPTH_MODE_MASK = 0x1FFF;  // P.Brodsky
  depth &= DEPTH_MODE_MASK;  // Only using bottom 13 bits. 
  if (depth <= 1000) return(depth * 0.1/1.0);
  else if (depth <= 1500) return(100 + (depth-1000) * 0.2/1.0);
  else if (depth <= 3100) return(200 + (depth-1500) * 0.5/1.0);
  else if (depth <= 8100) return(1000 + (depth-3100) * 1.0/1.0);
  else return(6000);
}

unsigned char Encode_temperature(float temperature) 
{
  if (temperature < -4) temperature = -4;
  temperature += 4;
  temperature = temperature * 256.0/40.0 + 0.5;
  if (temperature > 255) temperature = 255;
  return((unsigned char)temperature);
}

float Decode_temperature(unsigned char temperature) 
{ return(temperature * 40.0/256.0 - 4.0); }

unsigned char Encode_sound_speed(float sound_speed) 
{ return((unsigned char)((sound_speed - 1425.0) * 2)); }

float Decode_sound_speed(unsigned char sound_speed) 
{ return((float)sound_speed/2.0 + 1425.0); }

unsigned short Encode_hires_altitude(float alt) /* 10 cm resolution to 655 meters. */ 
{
  alt *= 100;
  if (alt > 65535.0)
    return(65535U);
  else if (alt < 0) return(0);
  else return((unsigned short)alt);
}

float Decode_hires_altitude(unsigned short alt) 
{ return((float)alt/100.0); }

unsigned short Encode_gfi_pitch_oil(float gfi, float pitch, float oil) 
{
  /* We are encoding 3 parameters that are somewhat specific to 
   * the REMUS 6000 into 2 bytes. * GFI= 5 bits: 0-100 (3.3 resolution) 
   * OIL= 5 bits: this gives us resolution of 3.3 percent 
   * Pitch=6 bits;
   180 into 64, resolution of 3 degrees. */ 
  unsigned short result;
  unsigned short temp;
  if (gfi < 0) // 5 bits of gfi 
    gfi = 0;
  else if (gfi > 100) gfi = 100;
  result = (unsigned short)(gfi * 31.0/100.0);
  if (oil < 0) // 5 bits of oil 
    oil = 0;
  else if (oil > 100) oil = 100;
  oil *= 31.0/100.0;
  result |= ((unsigned short)oil << 5);
  if (pitch > 90) // 6 bits of pitch 
    pitch = 90;
  else if (pitch < -90) pitch = -90;
  pitch *= 63.0/180.0;
  // Scale to 6 bits;
  if (pitch > 0.0) pitch += 0.5;
  else if (pitch < 0) pitch -= 0.5;
  temp = ((short)pitch << 10);
  result |= temp & 0xFC00;
  return(result);
}

void Decode_gfi_pitch_oil(unsigned short gfi_pitch_oil, 
                          float *gfi, float *pitch, float *oil) 
{
  unsigned short temp;
  short temp_pitch;
  
  temp = (unsigned int)((gfi_pitch_oil & 0x001F) * 100.0/31.0);
  *gfi = temp;
  temp = (gfi_pitch_oil & 0x03E0) >> 5;
  *oil = temp * 100.0/31.0;
  temp_pitch = ((short)(gfi_pitch_oil & 0xFC00)) >> 10;
  *pitch = temp_pitch * 180.0/63.0;
}

TIME_DATE Encode_time_date(long secs_since_1970) 
{
  /* The time is encoded as follows: 
   * Month 4 bits * Day 5 bits 
   * hour 5 bits 
   * Min 6 bits 
   * Secs 4 bits 
   // 4 secs per bit 
   * The year is not encoded. 
*/ 
  struct tm tm;
  TIME_DATE_LONG comp;
  /* Note: substitute localtime() for local timezone 
   * instead of GMT. */ 
  tm = *gmtime(&secs_since_1970);
  comp.as_long = (unsigned long)tm.tm_sec >> 2;
  comp.as_long += (unsigned long)tm.tm_min << 4;
  comp.as_long += (unsigned long)tm.tm_hour << (4 + 6);
  comp.as_long += (unsigned long)tm.tm_mday << (4 + 6 + 5);
  comp.as_long += (unsigned long)(tm.tm_mon+1) << (4 + 6 + 5 + 5);
  return(comp.as_time_date);
}

long Decode_time_date(TIME_DATE input, short *mon, short *day, short *hour, 
                      short *min, short *sec) 
{
  TIME_DATE_LONG comp;
  comp.as_long = 0;
  comp.as_time_date = input;
  
  *mon = (short)((comp.as_long >> (4 + 6 + 5 + 5)) & 0x000F);
  *day = (short)((comp.as_long >> (4 + 6 + 5)) & 0x001F);
  *hour = (short)((comp.as_long >> (4 + 6)) & 0x001F);
  *min = (short)((comp.as_long >> (4)) & 0x003F);
  *sec = (short)(((comp.as_long ) & 0x000F) * 4);

  /* It is possible to force this routine to return the 
   * seconds since 1970. Left as an exercise for the reader… 
   */ 
  return (0);
}

unsigned char Encode_watts(float volts, float amps) 
{
  /* Resolution is 4 watts, max is unsaturated is 1018 watts. */ 
  float watts = (volts * amps)/4;
  if (watts > 255) watts = 255;
  else if (watts < 0) watts = 0;
  // ok, not possible... 
  return((unsigned char)watts);
}

float Decode_watts(unsigned char watts_encoded) 
{
  return((float)watts_encoded * 4.0);
}

char Encode_speed(SPEED_MODE mode, float speed) 
{
  /* Max RPM: 2540 * Max Speed: 4.2 meters/sec (8.1 knots) */ 
  switch(mode) 
    {
    case SPEED_MODE_RPM: // Clamp to avoid errors. speed /= 20.0;
      if (speed > 127) speed = 127;
      else if (speed < -127) speed = -127;
      break;
    case SPEED_MODE_KNOTS: // Convert to m/sec speed *= 0.5144444;
      // Fall thru...
    case SPEED_MODE_MSEC: speed *= 30;
      if (speed > 127) speed = 127;
      else if (speed < -127) speed = -127;
      break;
    }
  return((char)speed);
}

float Decode_speed(SPEED_MODE mode, char speed) 
{
  switch(mode) 
    {
    case SPEED_MODE_RPM: 
      return(speed * 20.0);
    case SPEED_MODE_MSEC: 
      return((float)speed/30.0);
    case SPEED_MODE_KNOTS: // "Knots mode not supported by modem codec" 
      return(0);
    }
  return(0);
}


/* Latitude/Longitude 
 * Each are encoded into 3 bytes, giving a resolution of
 * about 2 meters. The compressed form is stored in a LATLON_COMPRESSED struct, 
 * which is 3 bytes. 
*/ 
typedef struct {
  char compressed[3]; 
} 
LATLON_COMPRESSED; 

typedef union {
  long as_long;
  LATLON_COMPRESSED as_compressed;
} LONG_AND_COMP;

typedef struct 
{
  char compressed[3];
}
TIME_DATE;

typedef union 
{
  long as_long;
  TIME_DATE as_time_date;
}
TIME_DATE_LONG;

typedef enum 
  {
    SPEED_MODE_RPM = 0, 
    SPEED_MODE_MSEC = 1, 
    SPEED_MODE_KNOTS = 2
  }
SPEED_MODE;

LATLON_COMPRESSED Encode_latlon(double latlon_in);
double Decode_latlon(LATLON_COMPRESSED latlon_in);
unsigned char Encode_heading(float heading);
double Decode_heading(unsigned char heading);
char Encode_est_velocity(float est_velocity);
float Decode_est_velocity(char est_velocity);
unsigned char Encode_salinity(float salinity);
float Decode_salinity(unsigned char sal);
unsigned short Encode_depth(float depth);
float Decode_depth(unsigned short depth);
unsigned char Encode_temperature(float temperature);
float Decode_temperature(unsigned char temperature);
unsigned char Encode_sound_speed(float sound_speed);
float Decode_sound_speed(unsigned char sound_speed);
unsigned short Encode_hires_altitude(float alt);
float Decode_hires_altitude(unsigned short alt);
unsigned short Encode_gfi_pitch_oil(float gfi, float pitch, float oil);
void Decode_gfi_pitch_oil(unsigned short gfi_pitch_oil, 
                          float *gfi, float *pitch, float *oil);
TIME_DATE Encode_time_date(long secs_since_1970);
long Decode_time_date(TIME_DATE input, short *mon, short *day, short *hour, 
                      short *min, short *sec);
unsigned char Encode_watts(float volts, float amps);
float Decode_watts(unsigned char watts_encoded);
char Encode_speed(SPEED_MODE mode, float speed);
float Decode_speed(SPEED_MODE mode, char speed);

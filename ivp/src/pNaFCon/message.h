#ifndef __whoi_message_h__
#define __whoi_message_h__

#define MDAT_NAV 19
 
typedef struct
{
	unsigned char src;         // sender address;
	unsigned char dest;        // receiver address;
	unsigned char mode;        // == CCLNAV
	unsigned char fix_type;    // GPS, etc. see doc.
	float heading;             // beacon heading (deg)
	float speed;               // beacon speed (knots)
	float depth;               // beacon depth (meters)
	double lat;                // beacon pos'n (lat) (deg)
	double lon;                // beacon pos'n (lon) (deg)
	float cep;                 // CEP - error (cm)
	float c_sound;             // speed of sound (m/s)
	float gps_std_lat;         // GPS Lat Std deviation (meters)CCL_nav_t
	float gps_std_lon;         // GPS Lon Std deviation (meters)
	unsigned long fix_time;    // time of beacon nav fix (unix time)
	unsigned long packet_time; // time of packet tx (unix time)
	int synch_min;             // min since clock synch
	int gps_nsat;              // number of satellites
	float gps_hdop;            // dilution of precision (meters)
}

CCL_nav_t;

#define UMODEM_MAXFRAMESIZE 32

typedef struct
{
	unsigned char caData[UMODEM_MAXFRAMESIZE];
}

sDataFrame;

#define CCLMAXINT (2147483647)

void CCL_MakeNavFrame(CCL_nav_t *sNav, sDataFrame &cclframe);
void CCL_RxNavFrame(sDataFrame *f, CCL_nav_t *s);

float decode_gps_hdop(unsigned short gps_hdop_nsat);
unsigned int decode_gps_nsat(unsigned short gps_hdop_nsat);
unsigned short encode_gps_hdop_nsat(float hdop, unsigned int nsat);
float decode_speed_of_sound(unsigned long csound_gpsstd);
float decode_gps_lat_stddev(unsigned long csound_gpsstd);
float decode_gps_lon_stddev(unsigned long csound_gpsstd);
unsigned long encode_csound_gpsstd(float csound, float gps_lat_stddev, float gps_lon_stddev);

#endif


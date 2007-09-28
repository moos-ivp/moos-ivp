#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "message.h"


static inline unsigned short encode_depth(float depth)
{
	if (depth < 0)
		return 0;

	if (depth < 100)
		return (unsigned short)((depth + 0.05) * 1.0 / 0.1);

	if (depth < 200)
		return (unsigned short)(((depth - 100) + 0.1 ) * 1.0 / 0.2 + 1000);

	if (depth < 1000)
		return (unsigned short)(((depth - 200) + 0.25) * 1.0 / 0.5 + 1500);

	if (depth < 6000)
		return (unsigned short)(((depth - 1000) + 0.5 ) * 1.0 / 1.0 + 3100);

	return 8100;
}

#define DEPTH_MODE_MASK (0x0fff | (1 << 12))

static inline float decode_depth(unsigned short depth)
{
	depth &= DEPTH_MODE_MASK;

	if (depth <= 1000)
		return depth * 0.1 / 1.0;

	if (depth <= 1500)
		return 100 + (depth - 1000) * 0.2 / 1.0;

	if (depth <= 3100)
		return 200 + (depth - 1500) * 0.5 / 1.0;

	if (depth <= 8100)
		return 1000 + (depth - 3100) * 1.0 / 1.0;

	return 6000;
}


///////////////////////////////////////////////////
// added code for bytes 8-11 and 30-31
// fixed places where bits where shifted by 18 instead of 16

void CCL_MakeNavFrame(CCL_nav_t *sNav, sDataFrame &cclframe)
{

	int idepth, icep, ilat, ilon, cgps, gps;

	// zeros everywhere
	memset(cclframe.caData, 0, UMODEM_MAXFRAMESIZE);

	// byte 0: the CCL "mode" - frame type
	cclframe.caData[0] = MDAT_NAV;

	// byte 1: fix type
	cclframe.caData[1] = sNav->fix_type;

#define MIN(A,B) ((A<B)?(A):(B))
	// byte 2: heading
	cclframe.caData[2] = (int)MIN(floor((sNav->heading) / 360.0 * 256.0), 255);

	// byte 3: speed
	cclframe.caData[3] = (int)MIN(floor((sNav->speed) / 6.4 * 256.0), 255);
#undef MIN

	// byte 4-5: depth
	idepth = encode_depth(sNav->depth);
	cclframe.caData[5] = ((idepth) & (0xff00)) >> 8;
	cclframe.caData[4] = (idepth) & (0x0ff);

	// byte 6-7: CEP
	icep = (int)(sNav->cep * 10);
	cclframe.caData[7] = ((icep) & (0xff00)) >> 8;
	cclframe.caData[6] = (icep) & (0x0ff);

	//byte 8-11: csound_gpsstd
	cgps = encode_csound_gpsstd(sNav->c_sound,
	                            sNav->gps_std_lat,
	                            sNav->gps_std_lon);
	cclframe.caData[11] = (((cgps) & (0xff000000)) >> 24);       // MSB
	cclframe.caData[10] = (((cgps) & (0x00ff0000)) >> 16);       // middle byte
	cclframe.caData[9] = (((cgps) & (0x0000ff00)) >> 8);        // middle byte
	cclframe.caData[8] = (cgps) & (0x000000ff);               // LSB

	//byte 12-15: latitude, degrees
	ilat = (int)(sNav->lat / 180 * CCLMAXINT);
	cclframe.caData[15] = (((ilat) & (0xff000000)) >> 24);       // MSB
	cclframe.caData[14] = (((ilat) & (0x00ff0000)) >> 16);       // middle byte
	cclframe.caData[13] = (((ilat) & (0x0000ff00)) >> 8);        // middle byte
	cclframe.caData[12] = (ilat) & (0x000000ff);               // LSB

	//byte 16-19: longitude, degrees
	ilon = (int)(sNav->lon / 180 * CCLMAXINT);
	cclframe.caData[19] = (((ilon) & (0xff000000)) >> 24);       // MSB
	cclframe.caData[18] = (((ilon) & (0x00ff0000)) >> 16);       // middle byte
	cclframe.caData[17] = (((ilon) & (0x0000ff00)) >> 8);        // middle byte
	cclframe.caData[16] = (ilon) & (0x000000ff);               // LSB

	//byte 20-23: unix time of packet
	cclframe.caData[23] = (unsigned char)(((sNav->packet_time) & (0xff000000)) >> 24); // MSB
	cclframe.caData[22] = (unsigned char)(((sNav->packet_time) & (0x00ff0000)) >> 16); // middle byte
	cclframe.caData[21] = (unsigned char)(((sNav->packet_time) & (0x0000ff00)) >> 8);  // middle byte
	cclframe.caData[20] = (unsigned char)(sNav->packet_time) & (0x000000ff);         // LSB

	//byte 24-27: unix time of fix
	cclframe.caData[27] = (unsigned char)(((sNav->fix_time) & (0xff000000)) >> 24); // MSB
	cclframe.caData[26] = (unsigned char)(((sNav->fix_time) & (0x00ff0000)) >> 16); // middle byte
	cclframe.caData[25] = (unsigned char)(((sNav->fix_time) & (0x0000ff00)) >> 8);  // middle byte
	cclframe.caData[24] = (unsigned char)(sNav->fix_time) & (0x000000ff);         // LSB

	// byte 28-29: minutes since synch:
	cclframe.caData[29] = ((sNav->synch_min) & (0xff00)) >> 8;  // MSB
	cclframe.caData[28] = (sNav->synch_min) & (0x0ff);        // LSB

	// bytes 30-31: gps hdop nsat
	gps = encode_gps_hdop_nsat(sNav->gps_hdop, sNav->gps_nsat);
	cclframe.caData[31] = ((gps) & (0xff00)) >> 8;  // MSB
	cclframe.caData[30] = (gps) & (0x0ff);        // LSB
}


void CCL_RxNavFrame(sDataFrame *f, CCL_nav_t *s)
{

	int iDepth, iCEP, iLat, iLon, cgps, gps;

	// fix type
	s->fix_type = f->caData[1];

	// heading
	s->heading = (f->caData[2] * 360.0f) / 256.0f;

	// speed
	s->speed = (f->caData[3] * 6.4f) / 256.0f;

	// depth
	iDepth = f->caData[5] * 256 + f->caData[4];
	s->depth = decode_depth(iDepth);

	// CEP
	iCEP = f->caData[7] * 256 + f->caData[6];
	s->cep = iCEP / 10.0f;

	// csound...gpsstd
	cgps = (f->caData[11] << 24) | (f->caData[10] << 16) | (f->caData[9] << 8) | (f->caData[8]);
	s->c_sound = decode_speed_of_sound(cgps);
	s->gps_std_lat = decode_gps_lat_stddev(cgps);
	s->gps_std_lon = decode_gps_lon_stddev(cgps);

	// lat
	iLat = (f->caData[15] << 24) | (f->caData[14] << 16) | (f->caData[13] << 8) | (f->caData[12]);
	s->lat = ((double)(iLat)) / CCLMAXINT * 180.0;

	// lon
	iLon = (f->caData[19] << 24) | (f->caData[18] << 16) | (f->caData[17] << 8) | (f->caData[16]);
	s->lon = ((double)(iLon)) / CCLMAXINT * 180.0;

	// packet_time
	s->packet_time = (f->caData[23] << 24) | (f->caData[22] << 16) | (f->caData[21] << 8) | (f->caData[20]);

	// fix_time
	s->fix_time = (f->caData[27] << 24) | (f->caData[26] << 16) | (f->caData[25] << 8) | (f->caData[24]);

	// synch_min
	s->synch_min = (f->caData[29] << 8) | (f->caData[28]);

	// hdop_nsat
	gps = (f->caData[31] << 8) | (f->caData[30]);
	s->gps_hdop = decode_gps_hdop(gps);
	s->gps_nsat = decode_gps_nsat(gps);

	return ;

}


/////////////////////////////////////////////////////

// functions for encoding/decoding:
// 1) gps stddev and c_sound
// 2) gps hdop and nsat


float decode_gps_hdop(unsigned short gps_hdop_nsat)
{
	int val = (gps_hdop_nsat & 0x0FFF);

	return ((float)val / 10.0);
}


unsigned int decode_gps_nsat(unsigned short gps_hdop_nsat)
{
	int val = (gps_hdop_nsat & 0xF000);

	return (val >> 12);
}

unsigned short encode_gps_hdop_nsat(float hdop, unsigned int nsat)
{
	if (hdop > 409.5)
		hdop = 409.5;

	if (hdop < 0.0)
		hdop = 0.0;

	if (nsat > 15)
		nsat = 15;

	if (nsat < 0)
		nsat = 0;

	int hdopint = (int) (hdop * 10.0);

	return ((nsat << 12) + hdopint);
}


float decode_speed_of_sound(unsigned long csound_gpsstd)
{
	int ss;

	ss = (csound_gpsstd & 0x0FFFL);
	return ((float)ss / 20.0) + 1425.0;
}

float decode_gps_lat_stddev(unsigned long csound_gpsstd)
{
	int std;

	std = ((csound_gpsstd & 0x003FF000L) >> 12);
	return ((float) std / 10.0);
}


float decode_gps_lon_stddev(unsigned long csound_gpsstd)
{
	int std;

	std = ((csound_gpsstd & 0xFFC00000L) >> 22);
	return ((float) std / 10.0);
}

unsigned long encode_csound_gpsstd(float csound,
                                   float gps_lat_stddev,
                                   float gps_lon_stddev)
{
	float maxsound = 1425.0 + ((float)0x0FFFL) / 20.0;
	float maxstd = 102.3; // 0x3FF / 10.0
	unsigned long ret = 0;

	if (csound > maxsound)
		csound = maxsound;

	if (csound < 0.0)
		csound = 0.0;

	csound = (csound - 1425) * 20.0;

	if (gps_lat_stddev > maxstd)
		gps_lat_stddev = maxstd;

	if (gps_lat_stddev < 0.0)
		gps_lat_stddev = 0.0;

	gps_lat_stddev *= 10.0;

	if (gps_lon_stddev > maxstd)
		gps_lon_stddev = maxstd;

	if (gps_lon_stddev < 0.0)
		gps_lon_stddev = 0.0;

	gps_lon_stddev *= 10.0;

	ret = ((unsigned long)csound & 0x0FFFL)
	      + (((unsigned long)gps_lat_stddev & 0x03FFL) << 12)
	      + (((unsigned long)gps_lon_stddev & 0x03FFL) << 22);

	return ret;
}


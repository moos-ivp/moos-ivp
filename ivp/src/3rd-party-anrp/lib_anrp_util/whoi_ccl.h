// start

#ifndef __whoi_ccl_h__
#define __whoi_ccl_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <time.h>

	typedef struct {
		char c[3];
	}

	LATLON_COMPRESSED;

	typedef struct {
		char c[3];
	}

	NAVXY_COMPRESSED;

	typedef struct {
		char c[3];
	}

	TIME_DATE;

	typedef union {
		long l;
		LATLON_COMPRESSED c;
	} LONG_AND_COMP;

	typedef union {
		long l;
		NAVXY_COMPRESSED c;
	} NAVXY_AND_COMP;

	typedef union {
		long l;
		TIME_DATE t;
	} TIME_DATE_LONG;

	typedef enum {
	    SPEED_MODE_RPM = 0,
	    SPEED_MODE_MSEC,
	    SPEED_MODE_KNOTS,
	} SPEED_MODE;

	typedef enum {
	    MDAT_EMPTY                 = 6,
	    MDAT_REDIRECT              = 7,
	    MDAT_USBL_POSITION         = 8,
	    MDAT_BATHY                 = 9,
	    MDAT_CTD                   = 10,
	    MDAT_COMMAND               = 11,
	    MDAT_USBL_DATA             = 12,
	    MDAT_CADCAC_DETECTION      = 13,
	    MDAT_STATE                 = 14,
	    MDAT_ERROR                 = 15,
	    MDAT_RANGER                = 16,
	    MDAT_MLO_REQUEST           = 17,
	    MDAT_STATE_MOD	       = 18,
	    MDAT_NAV		       = 19,
	    MDAT_LAST                  = 20,
	} MODEM_DATA;

#warning this code is not endian safe, it should be rewritten.. (probably with C++)

	typedef struct mdat_state_s {
		unsigned char mode;
		LATLON_COMPRESSED latitude;
		LATLON_COMPRESSED longitude;
		unsigned char fix_age;
		TIME_DATE time_date;
		unsigned char heading;
		unsigned short  mission_mode_depth;
		unsigned long faults;
		unsigned char faults_2;
		unsigned char mission_leg;
		char estimated_velocity;
		char objective_index;
		unsigned char watts_encoded;
		LATLON_COMPRESSED lat_goal;
		LATLON_COMPRESSED lon_goal;
		unsigned char battery_percent;
		unsigned short gfi_pitch_oil;
	}

	__attribute__ ((packed)) mdat_state_t;

	typedef struct mdat_state_mod_s {
		unsigned char mode;
		NAVXY_COMPRESSED navx;
		NAVXY_COMPRESSED navy;
		unsigned char fix_age;
		TIME_DATE time_date;
		unsigned char heading;
		unsigned short  mission_mode_depth;
		unsigned long faults;
		unsigned char faults_2;
		unsigned char mission_leg;
		char estimated_velocity;
		char objective_index;
		unsigned char watts_encoded;
		LATLON_COMPRESSED lat_goal;
		LATLON_COMPRESSED lon_goal;
		unsigned char battery_percent;
		unsigned short gfi_pitch_oil;
	}

	__attribute__ ((packed)) mdat_state_mod_t;

	typedef struct mdat_error_s {
		unsigned char mode;
		char message[31];
	}

	__attribute__ ((packed)) mdat_error_t;

#define STATE_MISSION_COMPLETED    0x0000
#define STATE_MANUAL_MODE          0x2000
#define STATE_TEST                 0x4000
#define STATE_FAULT                0x6000
#define STATE_REDIRECTED_MISSION   0xA000
#define STATE_NORMAL_OPERATION     0xC000

#define STATE_FAIL_VOLTAGE                                      0x00000001
#define STATE_FAIL_CURRENT                                      0x00000002
#define STATE_FAIL_GROUND_FAULT                                 0x00000004
#define STATE_FAIL_COMPASS                                      0x00000008
#define STATE_FAIL_ATTITUDE_SENSORS                             0x00000010
#define STATE_FAIL_PITCH_MOTOR                                  0x00000020
#define STATE_FAIL_RUDDER_MOTOR                                 0x00000040
#define STATE_FAIL_THRUSTER_MOTOR                               0x00000080
#define STATE_FAIL_ACOUSTIC_NAV                                 0x00000100
#define STATE_FAIL_DEPTH_SENSOR                                 0x00000200
#define STATE_FAIL_LEAK                                         0x00000400
#define STATE_FAIL_LEAK_CONTINUITY                              0x00000800
#define STATE_FAIL_ENERGY                                       0x00001000
#define STATE_FAIL_DISK_SPACE                                   0x00002000
#define STATE_FAIL_HARDWARE                                     0x00004000
#define STATE_FAIL_RANGER_PING                                  0x00008000

	static inline LATLON_COMPRESSED encode_latlon(double latlon_in) {
		LONG_AND_COMP out;
		double encoded;

		encoded = latlon_in * ((double)(0x007FFFFFL)/180.0);
		encoded += (encoded > 0.0) ? 0.5 : -0.5;

		out.l = (long)encoded;
		return out.c;
	}

	static inline long decode_navxy(NAVXY_COMPRESSED in);

	static inline NAVXY_COMPRESSED encode_navxy(double _xy_in) {
		NAVXY_AND_COMP out;
		int enc = 0;
		unsigned long xy_in;
		long tmp = _xy_in;
		memcpy(&xy_in, &tmp, 4);
		//fprintf(stderr, "enc %f %i ", _xy_in, xy_in);
		enc = xy_in & 0x007FFFFF;

		if(xy_in & (0x80000000))
			enc |= 0x00800000;

		out.l = enc;

		//fprintf(stderr, "out %i\n", enc);
		//fprintf(stderr, "dec -- ");
		long l = decode_navxy(out.c);

		//fprintf(stderr, "%i\n", l);
		return out.c;
	}

	static inline long decode_navxy(NAVXY_COMPRESSED in) {
		NAVXY_AND_COMP out;
		int dec = 0;

		out.c = in;
		dec = out.l & 0x007FFFFF;
		//fprintf(stderr, "stage1 %i\n", dec);

		if(out.l & 0x00800000) {
			dec |= 0xFF800000;
		}

		return dec;
	}

	static inline double decode_latlon(LATLON_COMPRESSED latlon_in) {
		LONG_AND_COMP in;

		in.l = 0;
		in.c = latlon_in;
		
		if(in.l & 0x00800000L)
			in.l |= 0xFF000000L;

		return ((double)(in.l)) * (180.0/((double)0x007FFFFFL));
	}

	static inline unsigned char encode_heading(float h) {
		return ((unsigned char)(h * 255.0/360.0 + 0.5));
	}

	static inline double decode_heading(unsigned char h) {
		return ((double)h * 360.0 / 255.0);
	}

	static inline char encode_est_velocity(float ev) {
		return ((char)(ev * 25.0));
	}

	static inline float decode_est_velocity(char ev) {
		return (ev / 25.0);
	}

	static inline unsigned char encode_salinity(float s) {
		float out;

		if(s < 20.0)
			return 0;
		else {
			out = (s - 20.0) * 10.0;

			if(out > 255.0)
				out = 255.0;

			return (unsigned char)out;
		}
	}

	static inline float decode_salinity(unsigned char s) {
		if(s == 0)
			return s;
		else
			return (((float)s/10.0) + 20.0);
	}

	static inline unsigned short encode_depth(float depth) {
		if(depth <    0)
			return 0;

		if(depth <  100)
			return (unsigned short)((depth + 0.05)          * 1.0/0.1);

		if(depth <  200)
			return (unsigned short)(((depth - 100)  + 0.1 ) * 1.0/0.2 + 1000);

		if(depth < 1000)
			return (unsigned short)(((depth - 200)  + 0.25) * 1.0/0.5 + 1500);

		if(depth < 6000)
			return (unsigned short)(((depth - 1000) + 0.5 ) * 1.0/1.0 + 3100);

		return 8100;
	}

#define DEPTH_MODE_MASK (0x0fff | (1 << 12))

	static inline float decode_depth(unsigned short depth) {
		depth &= DEPTH_MODE_MASK;

		if(depth <= 1000)
			return         depth         * 0.1/1.0;

		if(depth <= 1500)
			return 100  + (depth - 1000) * 0.2/1.0;

		if(depth <= 3100)
			return 200  + (depth - 1500) * 0.5/1.0;

		if(depth <= 8100)
			return 1000 + (depth - 3100) * 1.0/1.0;

		return 6000;
	}

	static inline unsigned char encode_temperature(float t) {
		if(t < -4)
			t = -4;

		t += 4;

		t = t * 250.0/40.0 + 0.5;

		if(t > 255)
			t = 255;

		return ((unsigned char)t);
	}

	static inline float decode_temperature(unsigned char t) {
		return (t * 40.0/256.0 - 4.0);
	}

	static inline unsigned char encode_sound_speed(float ss) {
		return (unsigned char)((ss - 1425.0) * 2);
	}

	static inline float decode_sound_speed(unsigned char ss) {
		return ((float)ss/2.0 + 1425.0);
	}

	static inline unsigned short encode_hires_altitude(float alt) {
		alt *= 100;

		if(alt > 65535.0)
			return 65535U;

		if(alt < 0)
			return 0;
		else
			return (unsigned short)alt;
	}

	static inline float decode_hires_altitude(unsigned short alt) {
		return (float)alt/100.0;
	}

	static inline unsigned short encode_gfi_pitch_oil(float gfi, float pitch, float oil) {
		unsigned short result;

		if(gfi <   0)
			gfi = 0;

		if(gfi > 100)
			gfi = 100;

		gfi *= 31.0/100.0;

		if(oil <   0)
			oil = 0;

		if(oil > 100)
			oil = 100;

		oil *= 31.0/100.0;

		if(pitch >  90)
			pitch = 90;

		if(pitch < -90)
			pitch = -90;

		pitch *= 63.0/180.0;

		if(pitch > 0.0)
			pitch += 0.5;

		if(pitch < 0.0)
			pitch -= 0.5;

		result = ((unsigned short)gfi) | (((unsigned short)oil) << 5) |
		         ((((short)pitch) << 10) & 0xFC00);

		return result;
	}

	static inline void decode_gfi_pitch_oil(unsigned short gfi_pitch_oil,
	                                        float *gfi, float *pitch, float *oil) {
		unsigned short temp;
		short temp_pitch;

		temp = (short)((gfi_pitch_oil & 0x001F) * 100.0 / 31.0);

		if(gfi)
			*gfi = temp;

		temp = (gfi_pitch_oil & 0x03E0) >> 5;

		if(oil)
			*oil = temp * 100.0/31.0;

		temp_pitch = ((short)(gfi_pitch_oil * 0xFC00)) >> 10;

		if(pitch)
			*pitch = temp_pitch * 180.0/63.0;
	}

	static inline TIME_DATE encode_time_date(long secs_since_1970) {

		struct tm tm;
		TIME_DATE_LONG comp;

		tm = *gmtime(&secs_since_1970);
		comp.l  = (unsigned long)tm.tm_sec     >>  2;
		comp.l += (unsigned long)tm.tm_min     <<  4;
		comp.l += (unsigned long)tm.tm_hour    << (4 + 6);
		comp.l += (unsigned long)tm.tm_mday    << (4 + 6 + 5);
		comp.l += (unsigned long)(tm.tm_mon+1) << (4 + 6 + 5 + 5);

		return comp.t;
	}

	static inline long decode_time_date(TIME_DATE input, short *monp, short *dayp,
	                                    short *hourp, short *minp, short *secp) {
		TIME_DATE_LONG comp;

		struct tm t;
		long tmp;
		short mon, day, hour, min, sec;

		comp.l = 0;
		comp.t = input;

		mon  = (short)((comp.l >> (4 + 6 + 5 + 5)) & 0x000F);
		day  = (short)((comp.l >> (4 + 6 + 5))     & 0x001F);
		hour = (short)((comp.l >> (4 + 6))         & 0x001F);
		min  = (short)((comp.l >> (4))             & 0x003F);
		sec  = (short)(((comp.l)                   & 0x000F) * 4);

		tmp = time(NULL);
		t = *gmtime(&tmp);

		t.tm_mon = mon - 1;
		t.tm_mday = day;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = sec;

		if(monp)
			*monp = mon;

		if(dayp)
			*dayp = day;

		if(hourp)
			*hourp = hour;

		if(minp)
			*minp = min;

		if(secp)
			*secp = sec;

		return timegm(&t);
	}

	static inline unsigned char encode_watts(float volts, float amps) {
		float watts = (volts * amps) / 4.0;

		if(watts > 255)
			watts = 255;

		if(watts < 0  )
			watts = 0;

		return (unsigned char)watts;
	}

	static inline float decode_watts(unsigned char watts_encoded) {
		return ((float)watts_encoded * 4.0);
	}

	static inline char encode_speed(SPEED_MODE mode, float speed) {
		switch(mode) {

		case SPEED_MODE_RPM:
			speed /= 20.0;

			if(speed >  127)
				speed = 127;

			if(speed < -127)
				speed = -127;

			break;

		case SPEED_MODE_KNOTS:
			speed *= 0.5144444;

		case SPEED_MODE_MSEC:
			speed *= 30;

			if(speed >  127)
				speed = 127;

			if(speed < -127)
				speed = -127;

			break;
		}

		return (char)speed;
	}

	static inline float decode_speed(SPEED_MODE mode, char speed) {
		switch(mode) {

		case SPEED_MODE_RPM:
			return ((float)speed) * 20.0;

		case SPEED_MODE_MSEC:
			return ((float)speed) / 30.0;

		case SPEED_MODE_KNOTS:
			return (((float)speed) / 30.0) / 0.5144444;
		}
	}


	// MDAT_NAV
	typedef struct {
		unsigned char mode;
		unsigned char type;
		unsigned char heading;
		unsigned char est_speed;
		unsigned short depth;
		unsigned short cep;
		unsigned long csound_gpsstd;
		float latitude;
		float longitude;
		unsigned long time_of_ping;
		unsigned long time_of_fix;
		unsigned short minutes_since_sync;
		unsigned short gps_hdop_nsat;
	} __attribute__ ((packed)) MODEM_MSG_DATA_NAV;

	enum fix_method {
		FIX_METHOD_GPS,
		FIX_METHOD_INS,
		FIX_METHOD_ACOUSTIC,
		FIX_METHOD_DEAD,
	};

	enum fix_mode {
		FIX_MODE_GENERIC,
		FIX_MODE_ACTIVE_LBL,
		FIX_MODE_DIFFERENTIAL,
		FIX_MODE_ACTIVE_USBL,
		FIX_MODE_WAAS,
		FIX_MODE_PASSIVE,
		FIX_MODE_UNKNOWN,
		FIX_MODE_UNKNOWN_2,
	};

	enum platform_motion {
		PLATFORM_FIXED,
		PLATFORM_MOORED,
		PLATFORM_MOBILE,
		PLATFORM_RESERVED,
	};

	static inline void generate_mdat_nav(MODEM_MSG_DATA_NAV *out,
					     int fix_method,
					     int fix_mode,
					     int platform_motion,
					     float heading,
					     float est_speed,
					     float depth,
					     float cep,
					     float speed_of_sound,
					     float gps_lat_sd,
					     float gps_lon_sd,
					     float lat,
					     float lon,
					     unsigned long time_of_ping,
					     unsigned long time_of_fix,
					     int minutes_since_sync,
					     float gps_hdop,
					     int nsat)
	{
		memset(out, 0, 32);
		out->mode = MDAT_NAV;
		switch(fix_method) {
		case FIX_METHOD_GPS:
			out->type = 0x00;
			break;
		case FIX_METHOD_INS:
			out->type = 0x01;
			break;
		case FIX_METHOD_ACOUSTIC:
			out->type = 0x02;
			break;
		case FIX_METHOD_DEAD:
			out->type = 0x03;
			break;
		}

		switch(fix_mode) {
		case FIX_MODE_GENERIC:
		case FIX_MODE_ACTIVE_LBL:
			out->type |= 0x00 << 2;
			break;
		case FIX_MODE_DIFFERENTIAL:
		case FIX_MODE_ACTIVE_USBL:
			out->type |= 0x01 << 2;
			break;
		case FIX_MODE_WAAS:
		case FIX_MODE_PASSIVE:
			out->type |= 0x02 << 2;
			break;
		case FIX_MODE_UNKNOWN:
		case FIX_MODE_UNKNOWN_2:
			out->type |= 0x03 << 2;
			break;
		}

		switch(platform_motion) {
		case PLATFORM_FIXED:
			out->type |= 0x00 << 4;
			break;
		case PLATFORM_MOORED:
			out->type |= 0x01 << 4;
			break;
		case PLATFORM_MOBILE:
			out->type |= 0x02 << 4;
			break;
		case PLATFORM_RESERVED:
			out->type |= 0x03 << 4;
			break;
		}
		
		out->heading = encode_heading(heading);
		out->est_speed = encode_est_velocity(est_speed);
		out->depth = encode_depth(depth);
		out->cep = (unsigned short)(cep * 10.0);
		
		printf("inp speed %f, %f, %f, %i\n", speed_of_sound, speed_of_sound - 1425.0,
		(speed_of_sound - 1425.0) / 0.05, (short)((speed_of_sound - 1425.0) / 0.05));
		out->csound_gpsstd = (0x00000FFF & (short)((speed_of_sound - 1425.0) / 0.05)) | 
					((unsigned short)(gps_lat_sd * 10) << 12) |
					((unsigned short)(gps_lon_sd * 10) << 22);
		
		out->latitude = lat;
		out->longitude = lon;

		out->time_of_ping = time_of_ping;
		out->time_of_fix = time_of_fix;
		out->minutes_since_sync = minutes_since_sync;

		out->gps_hdop_nsat = (unsigned short)(gps_hdop * 10) |
					((unsigned char)(nsat) << 12);
	}

	static inline void print_mdat_nav(MODEM_MSG_DATA_NAV *in)
	{
		if(in->mode != MDAT_NAV) {
			printf("Not an MDAT_NAV packet\n");
			return;
		} else {
			printf("MDAT_NAV packet:\n");
		}

		printf("\tType of position fix:       ");
		switch(in->type & 0x03) {
		case 0x00:
			printf("GPS fix");
			break;
		case 0x01:
			printf("INS fix");
			break;
		case 0x02:
			printf("Acoustic fix");
			break;
		case 0x03:
			printf("Dead reckoning fix");
			break;
		}
		printf("\n");
		printf("\tFix mode:                   ");
		switch((in->type >> 2) & 0x03) {
		case 0x00:
			printf("GPS generic/Acoustic LBL");
			break;
		case 0x01:
			printf("GPS differential/Acoustic USBL");
			break;
		case 0x02:
			printf("GPS WAAS/Acoustic passive");
			break;
		case 0x03:
			printf("Unknown");
			break;
		}
		printf("\n");
		printf("\tPlatform motion:            ");
		switch((in->type >> 4) & 0x03) {
		case 0x00:
			printf("Fixed");
			break;
		case 0x01:
			printf("Moored");
			break;
		case 0x02:
			printf("Mobile");
			break;
		case 0x03:
			printf("Reserved");
			break;
		}
		printf("\n");
		printf("\tHeading:                    %f\n", decode_heading(in->heading));
		printf("\tEstimated speed (m/s):      %f\n", decode_est_velocity(in->est_speed));
		printf("\tDepth:                      %f\n", decode_depth(in->depth));
		printf("\tCircular Error Probable:    %f\n", (float)(in->cep) * 0.1);
		printf("\tSpeed of sound:             %f\n",
			((float)(in->csound_gpsstd & 0x00000FFF) * 0.05) + 1425.0);
		
		printf("\tLatitude:                   %f\n", in->latitude);
		printf("\tLongitude:                  %f\n", in->longitude);
		printf("\tTime of ping:               %li\n", in->time_of_ping);
		printf("\tTime of fix:                %li\n", in->time_of_fix);
		printf("\tMinutes since GPS sync:     %i\n", in->minutes_since_sync);
		printf("\tGPS HDOP:                   %f\n",
			((float)(in->gps_hdop_nsat & 0x0FFF)) * 0.1);
		printf("\tGPS NSAT:                   %i\n",
			(int)((in->gps_hdop_nsat >> 12) & 0x0F));
	}
#ifdef __cplusplus
}

#endif

#endif


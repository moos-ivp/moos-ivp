#ifndef __sliva_parse_h__
#define __sliva_parse_h__

#include <vector>
#include <string>
#include <map>

typedef struct {
	double time;
	double data;
} timedata_t;

struct SlivaReader {
	SlivaReader(std::string filename, double gain = 0, bool is_old_array = false);

	void RemoveDCOffset();

	// first map param is 1-32
	std::map<int,std::vector<timedata_t> > channel_data;
	
	int major_version, minor_version;
	int cvers;

	int header_size;
	int data_format;

	int input_range, fs;
	float gain_hyd_preamp, gain_a2d_amp;

	int data_width;
	float acq_length;
	int octave;
	float array_depth;

	float array_current;
	float rail_voltage;
	float boa_temp;
	float temp_sensor_top, temp_sensor_bot;
	float tilt_sensor_top, tilt_sensor_mid, tilt_sensor_bot;

	float tilt_head_x, tilt_head_y, tilt_mid_x, tilt_mid_y, tilt_tail_x, tilt_tail_y;

	int pc_day, pc_month, pc_year, pc_hr, pc_min, pc_sec;

	int gps_month, gps_day, gps_year;

	int gps_hr, gps_min;
	
	double gps_sec;
	
	int lat_deg;
	double lat_min;
	double lat;

	int lon_deg;
	double lon_min;
	double lon;

	float north_vel, east_vel, up_vel;
	double altitude;
	int num_sats, minor_alarms, track_stat, fix_type, pps_output, gps_is_on;

	float sog, cog;
	int gps_qual;
	
};

#endif

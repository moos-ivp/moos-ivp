#include "sliva_parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bom.h"
#include "ssp.h"
#include <errno.h>
#include <stdexcept>
#include <math.h>
#include "byteorder.h"

using namespace std;

float RFloat(FILE *fp)
{
	float f;
	fread(&f, 4, 1, fp);

	ip_le2me32(&f);

	return f;
}

double RDouble(FILE *fp)
{
	// no 80 bit-ness here
	volatile double d;
	fread((void *)&d, 8, 1, fp);

	ip_le2me64(&d);

	return d;
}

int RInt(FILE *fp)
{
	int32_t i;
	fread(&i, 4, 1, fp);

	ip_le2me32(&i);

	return i;
}

static int octave_a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
static int octave_b_old[] = {10, 2, 12, 8, 14, 4, 16, 6, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 18, 26, 20, 28, 22, 30, 24, 32};
static int octave_c_old[] = {2, 3, 4, 6, 7, 8, 11, 15, 10, 12, 14, 16, 1, 5, 9, 13, 17, 21, 25, 29, 18, 20, 22, 24, 19, 24, 26, 27, 28, 30, 31, 32};
static int octave_b_new[] = {10, 2, 12, 8, 14, 4, 16, 6, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 28, 30, 31, 18, 26, 20, 27, 22, 29, 24, 32};
static int octave_c_new[] = {2, 3, 4, 6, 7, 8, 11, 15, 10, 12, 14, 16, 1, 5, 9, 13, 17, 21, 25, 30, 18, 20, 22, 24, 19, 23, 26, 28, 27, 29, 31, 32};

SlivaReader::SlivaReader(string filename, double gain, bool is_old_array)
{
	FILE *fp = fopen(filename.c_str(), "rb");
	if(!fp) throw runtime_error(ssp("Can't open %s: %s", filename.c_str(), strerror(errno)));

	bom head(24);
	fread(head.p(), 24, 1, fp);

	if(memcmp(head.p(), "MAZZI SLIVA ACQ", 15) != 0) {
		//throw runtime_error(ssp("File isn't a SLIVA data file"));
		fprintf(stderr, "I don't think this is a SLIVA file but trying anyway\n");
	}

	major_version = head[20] - '0';
	minor_version = (head[22] - '0') * 10;

	if(major_version < 0 || minor_version < 0) {
		throw runtime_error(ssp("File really isn't a SLIVA data file"));
	}

	fprintf(stderr, "File is version %i.%i\n", major_version, minor_version);

	cvers = major_version*100 + minor_version;

	if(cvers >= 220) {
		header_size = RInt(fp);
		data_format = RInt(fp);
	} else {
		data_format = 1; // def to offset binary
	}

	fs = RInt(fp);
	input_range = RInt(fp);

	if(cvers >= 220) {
		gain_hyd_preamp = RFloat(fp);
		gain_a2d_amp = RFloat(fp);
	}

	data_width = RInt(fp);
	acq_length = RFloat(fp);
	octave = RInt(fp);
	array_depth = RFloat(fp);

	if(cvers == 220) {
		array_current = RFloat(fp);
		boa_temp = RFloat(fp);
		temp_sensor_top = RFloat(fp);
		temp_sensor_bot = RFloat(fp);
		tilt_sensor_top = RFloat(fp);
		tilt_sensor_mid = RFloat(fp);
		tilt_sensor_bot = RFloat(fp);
	} else if(cvers >= 230) {
		array_current = RFloat(fp);
		if(cvers >= 240) {
			rail_voltage = RFloat(fp);
		}
		boa_temp = RFloat(fp);
		tilt_head_x = RFloat(fp);
		tilt_head_y = RFloat(fp);
		tilt_mid_x = RFloat(fp);
		tilt_mid_y = RFloat(fp);
		tilt_tail_x = RFloat(fp);
		tilt_tail_y = RFloat(fp);
	}
	
	pc_day = RInt(fp);
	pc_month = RInt(fp);
	pc_year = RInt(fp);
	pc_hr = RInt(fp);
	pc_min = RInt(fp);
	pc_sec = RInt(fp);

	if(cvers >= 210) {
		gps_month = RInt(fp);
		gps_day = RInt(fp);
		gps_year = RInt(fp);
	}

	gps_hr = RInt(fp);
	gps_min = RInt(fp);

	if(cvers == 210 || cvers == 200) {
		gps_sec = RFloat(fp);
	} else if (cvers >= 220) {
		gps_sec = RDouble(fp);
	}

	lat_deg = RInt(fp);

	if(cvers == 210 || cvers == 200) {
		lat_min = RFloat(fp);
	} else if (cvers >= 220) {
		lat_min = RDouble(fp);
	}

	lon_deg = RInt(fp);

	if(cvers == 210 || cvers == 200) {
		lon_min = RFloat(fp);
	} else {
		lon_min = RDouble(fp);
	}

	lat = ((double)lat_deg) + lat_min/60.0;
	lon = ((double)lon_deg) + lon_min/60.0;

	if(cvers >= 220) {
		north_vel = RFloat(fp);
		east_vel = RFloat(fp);
		up_vel = RFloat(fp);
		altitude = RDouble(fp);
		num_sats = RInt(fp);
		minor_alarms = RInt(fp);
		track_stat = RInt(fp);
		fix_type = RInt(fp);
		pps_output = RInt(fp);
		gps_is_on = RInt(fp);
	}

	if(cvers == 210) {
		sog = RFloat(fp);
		cog = RFloat(fp);
	}

	if(cvers == 200 || cvers == 210) {
		gps_qual = RInt(fp);
	}

	if(fs == 50000 || fs == 25000 || fs == 100000 || fs == 12500 || fs == 6250) {
		fprintf(stderr, "Correcting sample rate to 51/50 of reported\n");
		fs = fs * 51;
		fs /= 50;
	}

	fprintf(stderr, "Sampling rate %i\n", fs);
	
	uint32_t *arrayptr, *array;
	int numsamp;

	if(data_format == 0) {
		throw runtime_error("Don't know how to read file with data_format = 0");
	} else {
		int curpos = ftell(fp);
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fseek(fp, curpos, SEEK_SET);

		if((len - curpos) % 4 != 0) {
			printf("Junk at end of file?\n");
		}

		numsamp = (len - curpos) / 4;

		arrayptr = array = (uint32_t *)malloc(numsamp * 4);
		fread(array, 4, numsamp, fp);

		fprintf(stderr, "Number of possible samples: %i\n", numsamp);
	}

	double vmax;
	switch(input_range) {
	case 3:
		vmax = 10;
		break;
	case 2:
		vmax = 5;
		break;
	default:
		vmax = 2.5;
	}

	uint32_t max_range, bits;
	switch(data_width) {
	case 3:
		max_range = 0x01000000UL;
		bits = 24;
		break;
	case 2:
		max_range = 0x00100000UL;
		bits = 20;
		break;
	case 1:
		max_range = 0x00040000UL;
		bits = 18;
		break;
	default:
		max_range = 0x00010000UL;
		bits = 16;
		break;
	}

	uint32_t tagmask = 0x1F000000UL, datamask = 0x00FFFFFFUL;

#if 0
	for(int i=0; i<numsamp-1; i++) {
		uint32_t f, s;
		f = array[i] & tagmask;
		s = array[i+1] & tagmask;
		f >>= 24;
		s >>= 24;

		if(s == 0 && f == 31) continue;
		else if(s == f + 1) continue;
		else {
			fprintf(stderr, "Data dropped samp=%i f=%i s=%i\n", i, f, s);
		}
	}
#endif	
	int actualsamp;

	for(int i=0; i<numsamp; i++) { // find sample on channel zero
		if((array[i] & tagmask) >> 24 == 0) {
			arrayptr = &(array[i]);
			fprintf(stderr, "Found channel zero %i %i\n", numsamp, i);
			actualsamp = numsamp - i;
			break;
		}
	}

	int *octptr = octave_a;

	// disabled in originating code?
#if 0
	if(is_old_array) {
		switch(octave) {
		case 2:
			octptr = octave_b_old;
			break;
		case 3:
			octptr = octave_c_old;
			break;
		}
	} else {
		switch(octave) {
		case 2:
			octptr = octave_b_new;
			break;
		case 3:
			octptr = octave_c_new;
			break;
		}
	}
#endif
	
	fprintf(stderr, "Usable samples = %i\n", actualsamp);

	// prereserve space
	for(int i=0; i<32; i++) {
		channel_data[i+1] = vector<timedata_t>(actualsamp/30);
	}

	vector<int> actualsizes(32);

	double analog_gain = pow(10, (gain/20.0));

	for(int i=0; i<actualsamp; i++) {
		timedata_t dt;
		dt.data = datamask & arrayptr[i];
		dt.time = 1.0/((double)fs) * (i/32);

		//printf("%lf ", dt.data);

		dt.data = (dt.data / (((double)max_range)/2.0)) * vmax - vmax;
		dt.data /= analog_gain;

		uint32_t channel = arrayptr[i] & tagmask;
		channel >>= 24;

		int actualchannel = octptr[channel];

		channel_data[actualchannel][actualsizes[actualchannel-1]] = dt;
		//printf("%lf %u %lf %lf\n", dt.data, max_range, vmax, analog_gain);
		actualsizes[actualchannel-1]++;
	}

	for(int i=0; i<32; i++) {
		channel_data[i+1].resize(actualsizes[i]);
	}

	free(array);

	fclose(fp);
}

void SlivaReader::RemoveDCOffset()
{
	for(int i=1; i<=32; i++) {
		double sum = 0.0;
		for(int j=0; j<channel_data[i].size(); j++) {
			sum += channel_data[i][j].data;
		}
		sum /= ((double)channel_data[i].size());
		fprintf(stderr, "Found offset on channel %i of %lf V\n", i, sum);
		for(int j=0; j<channel_data[i].size(); j++) {
			channel_data[i][j].data -= sum;
		}
	}
}

#ifndef __CWinch_h__
#define __CWinch_h__

#include "CSerialPort.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

enum {
	TYPE_SPOOL = 1,
	TYPE_LINE,
};

class CWinch {
public:
	CWinch(string port, int baud);
	~CWinch();

	void SetReversed(bool rev);

	void Enable(void);
	void Disable(void);

	void ConfigureSpool(double enc, double ld, double swidth, double sdiam, double len) {
		type = TYPE_SPOOL;
		enc_per_drive_rev = enc;
		line_diameter = ld;
		spool_width = swidth;
		spool_diameter = sdiam;
		line_length = len;
	}

	void ConfigureLineFeeder(double enc, double lpt) {
		type = TYPE_LINE;
		enc_per_drive_rev = enc;
		line_per_turn = lpt;
	}

	long PositionToCounts(double line_pos);

	void Seek(long counts);

	long GetEncoderPosition(void);

	double GetPosition(long enc);

	long GetInputVoltage(int input);

	long GetCurrent(void);

	void GetZeroCurrentLevel(void);
	
private:
	pthread_mutex_t excl;

	CSerialPort *pt;
	
	void lock() { pthread_mutex_lock(&excl); }
	void unlock() { pthread_mutex_unlock(&excl); }

	int type;

	double enc_per_drive_rev;
	
	double line_diameter, spool_width, spool_diameter, line_length;

	double line_per_turn;
	
};

#endif

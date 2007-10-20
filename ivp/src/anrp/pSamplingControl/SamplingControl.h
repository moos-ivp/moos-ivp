#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MOOSGeodesy.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;
#define WINCH_UP 1
#define WINCH_DOWN 2

class SamplingControl : public CMOOSApp  
{
public:
	SamplingControl();
	virtual ~SamplingControl();

protected:
	
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();
        bool winch_down();
        bool winch_up();
        void winch_pos(double depth);

	CMOOSGeodesy m_Geodesy;
	double dfLatOrigin,dfLongOrigin,period,amplitude,last_time,Mission_Heading,Mission_Distance;
	double osSoundspd,Mission_Lat,Mission_Long,Mission_Interval,Mission_Duration,Mission_Depth,ctd_depth;
	int Mission_Mission,winch_state;
	bool on_station,mission_initialized;

	string loiter_str;
	bool ctd_deploy;
};


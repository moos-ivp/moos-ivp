#ifndef TRACKER_HEADER
#define TRACKER_HEADER

#include <string>
#include <vector>
#include <deque>
#include "2BTrack.h"

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2

#define MAX_NODES 10

using namespace std;

typedef enum{
    NO_CONTACT                  = 0,
    AMBIGUOUS_BEARING           = 1,
    BEARING_ONLY                = 2,
    RANGE_ONLY                  = 3,
    RANGE_AMBIGUOUS_BEARING     = 4,
    RANGE_BEARING               = 5,
    RANGE_BEARING_HEADING_SPEED = 6,
}CONTACT_TYPE;


class Measurement
{

 public:

  Measurement();
  ~Measurement();
  
  int node;

  double true_bearing;
  double Meas_Time;

  double auv_x;
  double auv_y; 

  double sigma;
};


class Contact
{
 public:
  Contact();
  Contact(int id);
  ~Contact();

  char         id;
  CONTACT_TYPE Ctype;

  double       xdot;
  double       ydot;
  double       range;
  double       bearing;
  double       heading;
  double       speed; 
  double       x_pos;
  double       y_pos;

  double       Last_Solution_Time;

  C2BTrack Topt;

};


class C2BTracker : public CMOOSApp  
{
public:
	C2BTracker();
	virtual ~C2BTracker();

protected:

	//a queue of local bearing measurements
	deque<Measurement> Local_Bearings;

	//the latest local measurement
	Measurement local_measurement;
        Measurement remote_measurement;

	//local measurement queue length in seconds
	int queue_length;

	//current contact info
	Contact Current_Contact;
	
	//decode the BEARING_STAT message
	bool decode(string,Measurement*);

	void unfiltered_position(Measurement*,Measurement*,double*,double*);
        void initialize_filter(double,double,double,double);

	void set_auv_x(double);
	void set_auv_y(double);
	double get_auv_x();
	double get_auv_y();
	
	//standard MOOS methods
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	

	//get new track solution
	bool Solve_Track(Measurement*);

	double get_aperture(Measurement*,Measurement*);

	//local node ID
	int local_node;
	
	//is the received bearing message from the local node
	bool IsLocalNode(int id);
	
	double auv_x,auv_y;
	bool solution,filter_initialized,silent,new_local_measurement,new_remote_measurement;
	double xdot_config,ydot_config,last_track_report_time,track_report_interval;

	void update_local_queue();
	void reset();
	
        bool Get_Local_Measurement(Measurement*,Measurement*);

};
#endif



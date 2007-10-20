#include<string>
#include<vector>

#include "1BTrack.h"

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2


using namespace std;


//typedef enum{
#define    NO_CONTACT                  0
#define    AMBIGUOUS_BEARING           1
#define    BEARING_ONLY                2
#define    RANGE_ONLY                  3
#define    RANGE_AMBIGUOUS_BEARING     4
#define    RANGE_BEARING               5
#define    RANGE_BEARING_HEADING_SPEED 6
      //}CONTACT_TYPE;

class Measurement
{

 public:

  Measurement();
  ~Measurement();

  int  state;
  int  beam;
  double true_bearing;
  double Meas_Time;

  double auv_x;
  double auv_y;
};

class CINFO
{
 public:
  CINFO();
  ~CINFO();

  int          num_meas;

  char         id;
  int          Ctype;
  double       range;
  double       bearing;
  double       heading;
  double       speed;
  char         hours;
  char         minutes;
  char         seconds;
  double       x_pos;
  double       y_pos;

  double       Last_Meas_Time;
  double       total_time;

};

class Contact
{
 public:
  Contact();
  Contact(int id);
  ~Contact();

  double       range_guess;

  int          num_meas;

  char         id;
  int          Ctype;
  double       range;
  double       bearing;
  double       heading;
  double       speed;
  char         hours;
  char         minutes;
  char         seconds;
  double       x_pos;
  double       y_pos;

  double       Last_Meas_Time;
  double       total_time;
  double       start_time; 

  C1BTrack Topt;

  void set_range_guess(double range);
  
};

class Contact_List
{
 public:

  Contact_List();
  ~Contact_List();

  int associate_bearing(Measurement*);
  int get_num_contacts();
  void set_num_contacts(int);
  void update_contact(int,Measurement*);
  void get_Cinfo(int,CINFO*);
  void reset();

  double start_time,prosecute_xdot,prosecute_ydot,prosecute_target_x,prosecute_target_y;
  double range_guess;
  int Z_length,skipped;

 protected:

  int num_contacts;

  vector<Contact> Contacts;

bool SolveTrack(int,double,double);
bool Add_Measurement(int,Measurement*);

};


    
class C1BTracker : public CMOOSApp  
{
public:
	C1BTracker();
	virtual ~C1BTracker();

	double  get_veh_x();
	double  get_veh_y();

protected:

	int max_meas,bstate,tstate,Mnum,beamno,init_meas,nmeas,meas_count;
	bool solution,tracking;
	double total_time,cBearing,start_time,trange,last_btime,auv_speed,dtheta;
	double prosecute_target_x,prosecute_target_y,prosecute_xdot,prosecute_ydot;
	double prosecute_target_range;

	int vehicleID;

	Contact_List CList;

	bool decode(string,Measurement*);


	//standard MOOS methods
	bool Iterate();
	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnConnectToServer();
	bool OnStartUp();	
	bool GetData();
	
        void reset();

	//get new track solution
	//bool SolveTrack();
	//bool Add_Measurement();

	//track optimization object
	//contains methods for computing the 1-bearing
	//track solution
	//C1BTrack Topt;
};



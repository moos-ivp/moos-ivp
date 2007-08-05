#ifndef CONTACT_HEADER
#define CONTACT_HEADER

#include "2BTrack.h"

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


class C2BTrack;
class CINFO
{
 public:
  CINFO();
  ~CINFO();

  int          num_meas;

  char         id;
  CONTACT_TYPE Ctype;
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
  CONTACT_TYPE Ctype;
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

  //C2BTrack Topt;

};
#endif

/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TStamp.h                                             */
/*    DATE: March 18th, 2020                                     */
/*****************************************************************/

#ifndef TSTAMP_HEADER
#define TSTAMP_HEADER

#include <string>

class TStamp {

public:
  TStamp();
  ~TStamp() {};

  bool  setTime(unsigned int hour, unsigned int min, double sec);
  bool  setTimeStr(std::string);

  std::string getTimeStr() const;
  bool  valid() const;

 protected:
  unsigned int   m_hour;    // Hour in 24-hour clock (valid is 0 to 23)
  unsigned int   m_min;     // Minute must be 0 to 59
  double         m_sec;     // Second must be > 0.0 and < 60.0
};
#endif

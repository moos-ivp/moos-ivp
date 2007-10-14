/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TransponderNMEA.cpp                                  */
/*    DATE: Oct 8th 2007                                         */
/*****************************************************************/

#ifndef TRANSPONDER_NMEA_HEADER
#define TRANSPONDER_NMEA_HEADER

#include <string>
#include "MOOSLib.h"
#include "MOOSGeodesy.h"

class TransponderNMEA : public CMOOSApp
{
public:
  TransponderNMEA();
  virtual ~TransponderNMEA() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  bool handleIncomingNMEA(const std::string&);

protected:
  CMOOSGeodesy m_Geodesy;
  double       m_start_time;

  double       m_nav_x;
  double       m_nav_y;
  double       m_nav_heading;

  bool         m_nav_x_set;
  bool         m_nav_y_set;
  bool         m_nav_h_set;

  double       m_nav_speed;
  double       m_nav_depth;
  std::string  m_vessel_name;
  std::string  m_vessel_type;
};

#endif








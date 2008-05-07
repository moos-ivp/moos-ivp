/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: BHV_Lawnmower.h                                      */
/*    DATE: MAY 6, 2008                                          */
/*****************************************************************/
 
#ifndef BHV_LAWNMOWER_HEADER
#define BHV_LAWNMOWER_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngine.h"

class BHV_Lawnmower : public IvPBehavior {
public:
  BHV_Lawnmower(IvPDomain);
  ~BHV_Lawnmower() {};
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);

protected:
  bool updateFromInfoBuffer();
  bool setNextWaypoint();

protected:
  WaypointEngine m_waypoint_engine;

protected: // configuration parameters
  double      m_cruise_speed;
  double      m_lead_distance;

protected: // intermediate or object global variables.
  double    m_osv;  // Ownship velocity
  double    m_osx;  // Ownship x position
  double    m_osy;  // Ownship y position
  double    m_ptx;
  double    m_pty;
  double    m_trackpt_x;
  double    m_trackpt_y;
};
#endif








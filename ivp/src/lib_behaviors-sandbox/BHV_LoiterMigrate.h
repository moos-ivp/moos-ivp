/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_LoiterMigrate.h                                  */
/*    DATE: May 9th 2007                                         */
/*****************************************************************/
 
#ifndef BHV_LOITER_MIGRATE_HEADER
#define BHV_LOITER_MIGRATE_HEADER

#include <string>
#include "IvPBehavior.h"
#include "XYPolygon.h"
#include "LoiterEngine.h"
#include "WaypointEngine.h"
#include "Migrator.h"

class IvPDomain;
class BHV_LoiterMigrate : public IvPBehavior {
public:
  BHV_LoiterMigrate(IvPDomain);
  ~BHV_LoiterMigrate() {};
  
  bool         setParam(std::string, std::string);
  void         onIdleState();
  IvPFunction* onRunState();

protected:
  bool         updateInfoIn();
  void         updateCenter();
  IvPFunction* buildIPF(const std::string&);
  void         updateInfoOut();

private:
  LoiterEngine   loiter_engine;
  WaypointEngine waypoint_engine;
  Migrator       migrator;  

  // Configuration Parameters
  bool         m_clockwise;
  double       m_desired_speed;
  double       m_acquire_dist;
  bool         m_center_activate;

  // State Variables
  bool         m_acquire_mode;   // true:acquiring, false:loitering
  double       m_dist_to_poly;   // Curr ownship dist to poly (mtrs)
  double       m_ptx;            // Curr waypt   x/lon (mtrs)
  double       m_pty;            // Curr waypt   y/lat (mtrs)
  double       m_osx;            // Curr owhship x/lon (mtrs)
  double       m_osy;            // Curr ownship y/lat (mtrs)
  double       m_osh;            // Curr ownship Heading (degs)
  int          m_iterations;     // Number of calls to produceOF  

  bool         m_poly_changed;
  bool         m_center_pending; 
  std::string  m_center_assign;  

  double       m_start_xpos;
  double       m_start_ypos;
  double       m_start_time;
};
#endif









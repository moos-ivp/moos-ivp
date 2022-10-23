/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_ZigZag.h                                         */
/*    DATE: Oct 16th 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef BHV_FULL_STOP_HEADER
#define BHV_FULL_STOP_HEADER

#include <string>
#include <list>
#include <map>
#include "IvPBehavior.h"
#include "Odometer.h"
#include "HintHolder.h"

class IvPDomain;
class BHV_FullStop : public IvPBehavior {
public:
  BHV_FullStop(IvPDomain);
  ~BHV_FullStop() {}

public: // Virtual functions overloaded
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onRunToIdleState();
  void         onIdleToRunState();
  std::string  expandMacros(std::string);
  
protected:
  IvPFunction *buildOF();

  bool checkForFullStop();
  void resetState();
  
  bool updateOSPos(std::string fail_action="err");
  bool updateOSHdg(std::string fail_action="err");
  bool updateOSSpd(std::string fail_action="err");
  bool updateOdometer();

  void postBeginPoint(bool active=true);

 protected: // State vars
  Odometer m_odometer;
  double   m_stem_hdg;
  double   m_stem_utc;
  XYPoint  m_begin_pt;
  double   m_max_dist_to_begin_pt;
  double   m_completed_utc;
  bool     m_completion_pending;
  
 private: // Config params
  double   m_mark_duration;
  double   m_stop_thresh;
  double   m_delay_complete;
  double   m_stale_nav_thresh;
  
  // Event flags unique to this behavior
  std::vector<VarDataPair> m_mark_flags;
  std::vector<VarDataPair> m_unmark_flags;
  
  // Visual hints affecting properties of seglists/points
  HintHolder  m_hints;
};

#endif

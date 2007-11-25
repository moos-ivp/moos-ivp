/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HeadOn14.h                                       */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef BHV_HEADON_HEADER
#define BHV_HEADON_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_HeadOn14 : public IvPBehavior {
public:
  BHV_HeadOn14(IvPDomain);
  ~BHV_HeadOn14() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  bool   getBufferInfo();
  double getRelevance();
  double getPriority();
  void   postInfo(double, double);
  void   postRange(bool ok=true);
  
private: // Configuration Parameters

  std::string m_contact; // Name for them in InfoBuffer
  std::string m_active_grade;

  double m_active_outer_dist;
  double m_active_inner_dist;

  double m_collision_dist;
  double m_all_clear_dist;

  double m_roc_max_heighten; // Rate of Closure w/ max relevance heightening
  double m_roc_max_dampen;   // Rate of Closure w/ max relevance dampening

private:  // State Variables

  double m_osx; // Current ownship x position (meters) 
  double m_osy; // Current ownship y position (meters) 
  double m_osh; // Current ownship heading (degrees 0-359)
  double m_osv; // Current ownship speed (meters) 

  double m_cnx; // Current contact x position (meters) 
  double m_cny; // Current contact y position (meters) 
  double m_cnh; // Current contact heading (degrees 0-359)
  double m_cnv; // Current contact speed (meters) 

  double m_curr_distance;
  double m_curr_closing_spd;

  bool   m_on_no_contact_ok;  // true if no trouble reported on no contact
};
#endif








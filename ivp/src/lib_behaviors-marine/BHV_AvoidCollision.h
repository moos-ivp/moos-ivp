/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollision.h                                 */
/*    DATE: Nov 18th 2006                                         */
/*****************************************************************/
 
#ifndef BHV_AVOID_COLLISION_HEADER
#define BHV_AVOID_COLLISION_HEADER

#include "IvPBehavior.h"

class IvPDomain;
class BHV_AvoidCollision : public IvPBehavior {
public:
  BHV_AvoidCollision(IvPDomain);
  ~BHV_AvoidCollision() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  bool   getBufferInfo();
  double getRelevance();
  double getPriority();
  
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
};
#endif









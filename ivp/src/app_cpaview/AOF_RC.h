/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_RC.h                                             */
/*    DATE: August 5th, 2007                                     */
/*****************************************************************/
 
#ifndef AOF_RC_RC_HEADER
#define AOF_RC_RC_HEADER

#include "AOF.h"

class IvPDomain;
class AOF_RC: public AOF {
public:
  AOF_RC(IvPDomain);
  ~AOF_RC() {};


public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double metric(double) const;

protected:
  double m_osx;   // Ownship X position
  double m_osy;   // Ownship Y position
  double m_cnx;   // Contact X position
  double m_cny;   // Contact Y position
  double m_cnh;   // Contact heading
  double m_cnv;   // Contact speed

  bool   m_osx_set;
  bool   m_osy_set;
  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;

  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif








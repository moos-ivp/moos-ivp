/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_R16.h                                            */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef AOF_R16_HEADER
#define AOF_R16_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class AOF_R16: public AOF {
public:
  AOF_R16(IvPDomain);
  ~AOF_R16() {if(m_cpa_engine) delete(m_cpa_engine);};

public: // virtual functions   
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize();
  
protected: // non-virtual functions
  double metric(double) const;

protected:
  double m_tol;   // Ownship Time on Leg
  double m_osx;   // Ownship X position
  double m_osy;   // Ownship Y position
  double m_cnx;   // Contact X position
  double m_cny;   // Contact Y position
  double m_cnh;   // Contact heading
  double m_cnv;   // Contact speed

  bool   m_tol_set;
  bool   m_osx_set;
  bool   m_osy_set;
  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;

  CPAEngine *m_cpa_engine;

  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif







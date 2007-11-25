/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_R17.h                                            */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef AOF_R17_HEADER
#define AOF_R17_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class IvPDomain;
class AOF_R17: public AOF {
public:
  AOF_R17(IvPDomain);
  ~AOF_R17() {if(cpa_engine) delete(cpa_engine);};

public: // virtual functions   
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize();
  
protected: // non-virtual functions
  double metric(double) const;
  double metric2(double, double) const;

protected:
  double os_lat;   // Ownship Lat position
  double os_lon;   // Ownship Lon position
  double cn_lat;   // Contact Lat position
  double cn_lon;   // Contact Lon position
  double cn_crs;   // Contact heading
  double cn_spd;   // Contact speed

  bool   os_lat_set;
  bool   os_lon_set;
  bool   cn_lat_set;
  bool   cn_lon_set;
  bool   cn_crs_set;
  bool   cn_spd_set;

  CPAEngine *cpa_engine;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
  int    tol_ix;  // Index of "tol" variable in IvPDomain
};

#endif







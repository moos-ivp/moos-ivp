/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_CutRangeFCPA.h                                   */
/*    DATE: Nov 4th 2006                                         */
/*****************************************************************/
 
#ifndef AOF_CUTRANGE_FCPA_HEADER
#define AOF_CUTRANGE_FCPA_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class AOF_CutRangeCPA: public AOF {
public:
  AOF_CutRangeCPA(IvPDomain);
  ~AOF_CutRangeCPA() {if(cpa_engine) delete(cpa_engine);};

public:    
  double evalBox(const IvPBox*) const;   // virtual defined
  bool   setParam(const std::string&, double);
  bool   initialize();
  
protected:
  double metric(double) const;

protected:
  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain

  double cn_lat;
  double cn_lon;
  double cn_crs;
  double cn_spd;
  double os_lat;
  double os_lon;
  double tol;

  bool   os_lat_set;
  bool   os_lon_set;
  bool   cn_lat_set;
  bool   cn_lon_set;
  bool   cn_crs_set;
  bool   cn_spd_set;
  bool   tol_set;

  CPAEngine *cpa_engine;
};

#endif








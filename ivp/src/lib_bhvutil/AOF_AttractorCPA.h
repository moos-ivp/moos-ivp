/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_AttractorFCPA.h                                  */
/*    DATE: Nov 4th 2006                                         */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef AOF_ATTRACTOR_FCPA_HEADER
#define AOF_ATTRACTOR_FCPA_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class AOF_AttractorCPA: public AOF {
public:
  AOF_AttractorCPA(IvPDomain);
  ~AOF_AttractorCPA() {if(cpa_engine) delete(cpa_engine);}

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
  double patience;
  
  double min_roc;
  double max_roc;
  double range_roc;
  double max_heading;

  double max_util_cpa_dist;
  double min_util_cpa_dist;

  bool   os_lat_set;
  bool   os_lon_set;
  bool   cn_lat_set;
  bool   cn_lon_set;
  bool   cn_crs_set;
  bool   cn_spd_set;
  bool   tol_set;
  bool   min_util_cpa_dist_set;
  bool   max_util_cpa_dist_set;

  CPAEngine *cpa_engine;
};

#endif




















/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_CutRangeCPA.h                                    */
/*    DATE: May 10th 2005                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef AOF_CUTRANGE_CPA_HEADER
#define AOF_CUTRANGE_CPA_HEADER

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
  int    tol_ix;  // Index of "tol" variable in IvPDomain

  double cn_lat;
  double cn_lon;
  double cn_crs;
  double cn_spd;
  double os_lat;
  double os_lon;

  bool   os_lat_set;
  bool   os_lon_set;
  bool   cn_lat_set;
  bool   cn_lon_set;
  bool   cn_crs_set;
  bool   cn_spd_set;

  CPAEngine *cpa_engine;
};

#endif








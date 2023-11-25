/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_Shadow.h                                         */
/*    DATE: May 10th 2005                                        */
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
 
#ifndef AOF_SHADOW_HEADER
#define AOF_SHADOW_HEADER

#include "AOF.h"

class IvPDomain;
class AOF_Shadow: public AOF {
 public:
  AOF_Shadow(IvPDomain);
  ~AOF_Shadow() {}
  
 public: // virtual functions   
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize();

 public: // non-virtual functions
  double metric(double, double) const;

 protected:
  double cn_spd;     // Contact Speed in kts.
  double cn_crs;     // Contact Course in degrees (0-359).
  bool   cn_spd_set;
  bool   cn_crs_set;
  
  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif




















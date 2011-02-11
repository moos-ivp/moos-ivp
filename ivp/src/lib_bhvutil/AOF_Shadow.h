/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Shadow.h                                         */
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
 
#ifndef AOF_SHADOW_HEADER
#define AOF_SHADOW_HEADER

#include "AOF.h"

class IvPDomain;
class AOF_Shadow: public AOF {
 public:
  AOF_Shadow(IvPDomain);
  ~AOF_Shadow() {};
  
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










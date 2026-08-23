/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: AOF_OpRegion.h                                       */
/*    DATE: Aug 1oth, 2026                                       */
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
 
#ifndef AOF_OPREGION_HEADER
#define AOF_OPREGION_HEADER

#include "AOF.h"
#include "XYGenPolygon.h"
#include "PlatModel.h"

class IvPDomain;
class AOF_OpRegion: public AOF {
public:
  AOF_OpRegion(IvPDomain);
  ~AOF_OpRegion() {}

 public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
 public: // More virtuals defined Declare a known min/max eval range
  bool   minMaxKnown() const {return(true);}
  double getKnownMin() const {return(0);}
  double getKnownMax() const {return(100);}

  void   setGenPoly(XYGenPolygon gpoly) {m_gpoly=gpoly;}
  void   setPlatModel(PlatModel pm) {m_plat_model=pm;}
  
  bool   initialize();

protected:
  double evalUtilETA(double, double) const;
  double evalUtilCPA(double, double) const;
  
 private: // Config variables
  XYGenPolygon m_gpoly;
  PlatModel    m_plat_model;

  double m_min_util_eta;
  double m_max_util_eta;
  double m_min_util_cpa;
  double m_max_util_cpa;
  double m_cpa_window;

  bool   m_eta_factored;
  bool   m_cpa_factored;
  
 private: // State variables
  int  m_crs_ix;  // Index of "course" variable in IvPDomain
  int  m_spd_ix;  // Index of "speed"  variable in IvPDomain
  
  bool m_min_util_eta_is_set;
  bool m_max_util_eta_is_set;
  bool m_min_util_cpa_is_set;
  bool m_max_util_cpa_is_set;
  bool m_cpa_window_is_set;
};

#endif

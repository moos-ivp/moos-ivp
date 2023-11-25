/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_CutRangeFCPA.h                                   */
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
 
#ifndef AOF_CUTRANGE_FCPA_HEADER
#define AOF_CUTRANGE_FCPA_HEADER

#include "AOF.h"
#include "CPAEngine.h"

class AOF_CutRangeCPA: public AOF {
public:
  AOF_CutRangeCPA(IvPDomain);
  ~AOF_CutRangeCPA() {if(m_cpa_engine) delete(m_cpa_engine);}

public:    
  double evalBox(const IvPBox*) const;   // virtual defined
  bool   setParam(const std::string&, double);
  bool   initialize();
  
  void   discourageLowSpeeds(double thresh=0, double value=0);
  void   okLowSpeeds();

protected:
  double metric(double) const;

protected:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain

  // Parameters set by the user
  double m_cnx;
  double m_cny;
  double m_cnh;
  double m_cnv;
  double m_osx;
  double m_osy;
  double m_tol;
  double m_patience;
  
  // Intermediate variables set during initialization
  CPAEngine *m_cpa_engine;

  double m_min_roc;
  double m_max_roc;
  double m_range_roc;
  double m_distance_os_cn;
  
  // Instance variables to indicated whether critical parameters
  // are set by the user - each is initialized to false
  bool   m_osx_set;
  bool   m_osy_set;
  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;
  bool   m_tol_set;

  bool   m_discourage_low_speeds;
  double m_discourage_low_speeds_thresh;
  double m_discourage_low_speeds_value;
};

#endif




















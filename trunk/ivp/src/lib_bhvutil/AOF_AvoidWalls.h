/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidWalls.h                                     */
/*    DATE: Oct 29th, 2018                                       */
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
 
#ifndef AOF_AVOID_WALLS_HEADER
#define AOF_AVOID_WALLS_HEADER

#include "AOF.h"
#include "WallEngine.h"
#include "IvPBox.h"
#include "IvPDomain.h"

class IvPDomain;
class AOF_AvoidWalls : public AOF {
 public:
  AOF_AvoidWalls(IvPDomain);
  ~AOF_AvoidWalls();

 public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

 public: // More virtuals defined Declare a known min/max eval range
  bool   minMaxKnown() const {return(true);}
  double getKnownMin() const {return(0);}
  double getKnownMax() const {return(m_max_util);}
  
  bool   setWalls(std::vector<XYSegList> walls);

  std::vector<double> getXpts()   {return(m_hit_cache_x);}
  std::vector<double> getYpts()   {return(m_hit_cache_y);}
  std::vector<bool>   getBools()  {return(m_hit_cache_b);}

  void setVName(std::string str) {m_vname=str;}
  
 protected:
  double metric(double) const;
  
 protected:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain

  double m_tol;
  double m_osx;
  double m_osy;
  double m_osh;

  double m_turn_radius;

  double m_nogo_ttcpa;
  double m_safe_ttcpa;
  double m_ttc_base;
  double m_ttc_rate;
  
  double m_collision_distance;
  double m_all_clear_distance;

  bool   m_tol_set;
  bool   m_osy_set;
  bool   m_osx_set;
  bool   m_osh_set;
  bool   m_turn_radius_set;
  bool   m_nogo_ttcpa_set;
  bool   m_safe_ttcpa_set;
  bool   m_ttc_base_set;
  bool   m_ttc_rate_set;
  bool   m_collision_distance_set;
  bool   m_all_clear_distance_set;

  WallEngine m_wall_engine;
  bool       m_wall_engine_initialized;

  std::vector<double> m_hit_cache_x;
  std::vector<double> m_hit_cache_y;
  std::vector<bool>   m_hit_cache_b;

  double m_max_util;

  unsigned int m_helm_iter;
  std::string m_vname;
  
  FILE *fptr;
  
  std::vector<XYSegList> m_walls;
};

#endif


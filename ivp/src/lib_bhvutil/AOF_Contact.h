/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_ContactX.h                                       */
/*    DATE: May 17th, 2013 (Generalizing over existing classes)  */
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
 
#ifndef AOF_CONTACT_HEADER
#define AOF_CONTACT_HEADER

#include <vector>
#include "AOF.h"
#include "CPAEngine.h"

class AOF_Contact: public AOF {
public:
  AOF_Contact(IvPDomain);
  ~AOF_Contact() {}

public: // virtual functions   
  virtual double evalPoint(const std::vector<double>&) const {return(0);}
  virtual bool   setParam(const std::string&, double);
  virtual bool   setParam(const std::string&, const std::string&) {return(false);}
  virtual bool   initialize();

  void setCPAEngine(const CPAEngine& engine);
  
  void setOwnshipParams(double osx, double osy);
  void setContactParams(double cnx, double cny, double cnh, double cnv);

  double getCNSpeedInOSPos() const;
  double getRangeGamma() const;
  bool   aftOfContact() const;
  bool   portOfContact() const;
  
protected:
  double m_tol;    // Ownship Time on Leg
  double m_osx;    // Ownship X position (meters)
  double m_osy;    // Ownship Y position (meters)
  double m_cnx;    // Contact X position (meters)
  double m_cny;    // Contact Y position (meters)
  double m_cnh;    // Contact heading
  double m_cnv;    // Contact speed

  double m_collision_distance;
  double m_all_clear_distance;

  bool   m_tol_set;
  bool   m_osy_set;
  bool   m_osx_set;
  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;
  bool   m_collision_distance_set;
  bool   m_all_clear_distance_set;

  double m_stat_bng_os_cn;
  
  CPAEngine m_cpa_engine;
  bool      m_cpa_engine_initialized;
};

#endif


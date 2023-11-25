/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RefineryCPA.h                                        */
/*    DATE: November 14th, 2017                                  */
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
 
#ifndef REFINERY_CPA_HEADER
#define REFINERY_CPA_HEADER

#include <vector>
#include "IvPBox.h"
#include "IvPDomain.h"
#include "CPAEngine.h"

class RefineryCPA {
 public:
  RefineryCPA();
  ~RefineryCPA() {}

  bool init(double osx, double osy, double cnx, double cny,
	    double cnh, double cnv, double ostol,
	    double min_ucd, double max_ucd, IvPDomain domain,
	    CPAEngine *cpa_engine);

  std::vector<IvPBox> getRefineRegions();

  unsigned int getTotalQueriesCPA() const {return(m_cpa_queries);}
  void setVerbose(bool v=true) {m_verbose=v;}

  std::string getLogicCase() {return(m_logic_case);}

  void setQuit1() {m_quit1=true;}
  void setQuit2() {m_quit2=true;}
  void setQuit3() {m_quit3=true;}
  
 protected:

  std::vector<IvPBox> getRefineRegionsAft();

 protected: // Fore utilities
  std::vector<IvPBox> getRefineRegionsFore();         
  std::vector<IvPBox> getRefineRegionsForeInRGam();   // Q2 Q3
  std::vector<IvPBox> getRefineRegionsForeInRGamX();   
  std::vector<IvPBox> getRefineRegionsForeOutRGam();  // Q1
  double getFleeSpeed() const;

  double getMinSpdAtHdg(double hdg);  // unused
  double getMaxSpdAtHdg(double hdg);

  double getMinFirstSatHdgAtSpd(double spd, double hmin);
  double getMaxFirstSatHdgAtSpd(double spd, double hmax);
  
  double getMinLastSatHdgAtSpd(double spd, double hmax);
  double getMaxLastSatHdgAtSpd(double spd, double hmin);

  double getMinSpdClockwise(double hmin, double hmax, double spd);
  double getMinSpdCounterClock(double hmin, double hmax, double spd);

  double getPassHdgClockwise(double hdg, double spd, double endspd);
  double getPassHdgCounterClock(double hdg, double spd, double endspd);

  double getFleeSpdClockwise(double hdg, double spd, double endspd);
  double getFleeSpdCounterClock(double hdg, double spd, double endspd);

  bool   validHdg(double hval) const;
  bool   validSpd(double sval) const;

  unsigned int getHdgDiscPts(double hmin, double hmax, int snap) const;

  double evalCPA(double hdg, double spd, double ostol){
    m_cpa_queries++;
    return(m_cpa_engine->evalCPA(hdg, spd, ostol));
  }

protected: // general utilities
  double getSpdSnappedHgh(double) const;
  double getSpdSnappedLow(double) const;
  double getHdgSnappedHgh(double) const;
  double getHdgSnappedLow(double) const;
  
  
 protected:

  unsigned int m_crs_ix;
  unsigned int m_spd_ix;
  unsigned int m_crs_pts;
  unsigned int m_spd_pts;
  
  bool      m_initialized;

  CPAEngine *m_cpa_engine;

  double    m_contact_range;
  double    m_range_gamma;
  double    m_range_epsilon;
  double    m_os_cn_abs_bng;
  bool      m_os_port_of_cn;
  bool      m_os_star_of_cn;
  
  double    m_theta_delta;

  unsigned int m_cpa_queries;
  
 protected:  // Configuration variables
  IvPDomain m_domain;
  bool   m_verbose;
  
  double m_min_util_cpa_dist;
  double m_max_util_cpa_dist;

  double m_osx;
  double m_osy;
  double m_cnx;
  double m_cny;
  double m_cnh;
  double m_cnv;

  double m_ostol;

  bool m_quit1;
  bool m_quit2;
  bool m_quit3;

  std::string m_logic_case;
};

#endif



















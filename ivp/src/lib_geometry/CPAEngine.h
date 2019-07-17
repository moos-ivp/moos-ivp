/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAEngine.h                                          */
/*    DATE: May 12th 2005                                        */
/*    DATE: January 2016 Major revision                          */
/*    DATE: July 2017 Major revision - to "FAST-CPA"             */
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
 
#ifndef CPA_ENGINE_17_HEADER
#define CPA_ENGINE_17_HEADER

#include <vector>
#include "CPAEngineRoot.h"

class CPAEngine : public CPAEngineRoot {
public:
  CPAEngine();
  CPAEngine(double cny, double cnx, double cnh,
	    double cnv, double osy, double osx);

  void reset(double cny, double cnx, double cnh,
	     double cnv, double osy, double osx);
  
  ~CPAEngine() {}

public:
  void   initNonCache();
  
  double evalCPA(double osh, double osv, double ostol) const;
  double evalTimeCPA(double osh, double osv, double ostol) const;
  double evalROC(double osh, double osv) const;

  double evalRangeRateOverRange(double osh, double osv, double time) const;

  // ----------------------------------------------------------
  // Checks for Crossing Stern and/or Bow
  // ----------------------------------------------------------
  bool   crossesStern(double osh, double osv) const;
  double crossesSternDist(double osh, double osv) const;
  bool   crossesSternDist(double osh, double osv, double& xdist) const;
  bool   crossesBow(double osh, double osv) const;
  double crossesBowDist(double osh, double osv) const;
  bool   crossesBowDist(double osh, double osv, double& xdist) const;
  bool   crossesBowOrStern(double osh, double osv) const;

  // ----------------------------------------------------------
  // Checks for Passing Port and/or Starboard
  // ----------------------------------------------------------
  bool   passesPort(double osh, double osv) const;
  double passesPortDist(double osh, double osv) const;
  bool   passesPortDist(double osh, double osv, double& xdist) const;
  bool   passesStar(double osh, double osv) const;
  double passesStarDist(double osh, double osv) const;
  bool   passesStarDist(double osh, double osv, double& xdist) const;
  bool   passesPortOrStar(double osh, double osv) const;
  
  bool   turnsRight(double old_osh, double new_osh) const;
  bool   turnsLeft(double old_osh, double new_osh) const;

  bool   foreOfContact() const {return(m_stat_os_fore_of_cn);}
  bool   aftOfContact() const {return(m_stat_os_aft_of_cn);}
  bool   portOfContact() const {return(m_stat_os_port_of_cn);}
  bool   starboardOfContact() const {return(m_stat_os_star_of_cn);}

  double minMaxROC(double, double, double&, double&) const;

  double bearingRate(double osh, double osv) const;
  double bearingRateOld(double osh, double osv);

  double ownshipContactRelBearing(double osh) const;
  double contactOwnshipRelBearing() const {return(m_stat_rel_bng_cn_os);}

  double ownshipContactAbsBearing() const {return(m_stat_abs_bng_os_cn);}


 public: // Getters for intermediate values used in other calculations
  double cnSpdToOS() const {return(m_stat_cn_to_os_spd);}

  double getOSSpeedInCNHeading(double osh, double osv) const;
  double getOSSpeedGamma(double osh, double osv) const;
  double getOSSpeedEpsilon(double osh, double osv) const;
  double getOSTimeGamma(double osh, double osv) const;
  double getOSTimeEpsilon(double osh, double osv) const;
  double getCNSpeedInOSPos() const {return(m_stat_cn_to_os_spd);}
  double getRangeGamma() const   {return(m_stat_range_gam);}
  double getRangeEpsilon() const {return(m_stat_range_eps);}
  double getThetaGamma() const   {return(m_stat_theta_os_gam);}
  double getThetaEpsilon() const {return(m_stat_theta_os_eps);}

  double getRange() const {return(m_stat_range);}

  double getARange(double osh, double osv, double time) const;
  double getARangeRate(double osh, double osv, double time) const;

 protected:
  void   setStatic();
  double smallAngle(double, double) const;

  void   initTrigCache();
  void   initRateCache();
  void   initK1Cache();
  void   initK2Cache();
  
  void   initOSCNRelBngCache();
  void   initOSCNTangentCache();
  void   initOSGammaCache();
  void   initOSCNHCosCache();

  void   setOSForeOfContact();
  void   setOSAftOfContact();
  void   setOSPortOfContact();
  void   setOSStarboardOfContact();
  
 protected: // Cached values
  double m_cos_cnh;  // Cosine of  cnCRS.
  double m_sin_cnh;  // Sine  of   cnCRS.

  double m_stat_k2;  // Components of k2, k1, k0 that are 
  double m_stat_k1;  // static (independent of the values of
  double m_stat_k0;  // osCRS, osSPD, osTOL).
  double m_stat_range;
   
  double m_stat_cosCNH_x_cnSPD;
  double m_stat_sinCNH_x_cnSPD;
  
  bool   m_stat_os_on_contact;      // true if ownship on contact position
  bool   m_stat_os_on_bowline;      // true if ownship on contact bowline
  bool   m_stat_os_on_sternline;    // true if ownship on contact sternline
  bool   m_stat_os_on_bowsternline; // true if any of above three are true
  bool   m_stat_os_on_beam; 

  double m_stat_theta_os_eps;
  double m_stat_theta_os_gam;
  double m_stat_theta_tn;
  double m_stat_tn_constant;

  double m_stat_spd_cn_at_tangent;
  
  double m_stat_cn_to_os_spd;
  bool   m_stat_cn_to_os_closing;
    
  double m_stat_abs_bng_os_cn;
  double m_stat_rel_bng_cn_os;

  bool   m_stat_os_fore_of_cn;
  bool   m_stat_os_aft_of_cn;
  bool   m_stat_os_port_of_cn;
  bool   m_stat_os_star_of_cn;

  double m_stat_range_gam;
  double m_stat_range_eps;
  
  std::vector<double> m_cos_cache_3600;
  std::vector<double> m_sin_cache_3600;

  std::vector<double> m_os_vthresh_cache_360;
  
  std::vector<double> m_k1_cache;
  std::vector<double> m_k2_cache;

  std::vector<double> m_cos_cache;
  std::vector<double> m_sin_cache;

  std::vector<double> m_os_cn_relbng_cache;
  std::vector<double> m_os_cn_relbng_cos_cache;

  std::vector<double> m_os_tn_cos_cache;
  std::vector<double> m_os_gam_cos_cache;
  std::vector<double> m_os_cnh_cos_cache;
};

#endif

/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEngine.h                                          */
/*    DATE: May 12th 2005                                        */
/*    DATE: June 24th 2024  Mods to support platmodel version    */
/*    DATE: Jan 4th 2026  Further Mods to support platmodel      */
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
 
#ifndef CPX_ENGINE_OG_HEADER
#define CPX_ENGINE_OG_HEADER

#include <vector>
#include "CPXEngine.h"

class CPXEngineOG : public CPXEngine {
public:
  CPXEngineOG(double cny=0, double cnx=0, double cnh=0, double cnv=0,
	    double osy=0, double osx=0, double osh=0, double osv=0);

  void reset(double cny, double cnx, double cnh, double cnv,
	     double osy, double osx, double osh=0, double osv=0);
  
  ~CPXEngineOG() {}

public: // Group 1

public: // Group 2
  double evalROC(double osh, double osv) const;

public: // Group 3
  double bearingRate(double osh, double osv) const; 

public: // Group 4
  
public: // Group 5

public: // Group A - CPA
  double evalCPA(double osh, double osv, double ostol) const;
  double evalTimeCPA(double osh, double osv, double ostol) const;
  void   evalCPA(double osh, double osv, double ostol,
		 double& xdist, double& xtime) const {};

public:  // Group B - Checks for Passing Port and/or Starboard
  bool   passesPort(double osh, double osv) const;
  double passesPortDist(double osh, double osv) const;
  double passesPortTime(double osh, double osv) const;
  bool   passesPortDist(double osh, double osv,
			double& xdist, double& xtime) const;

  bool   passesStar(double osh, double osv) const;
  double passesStarDist(double osh, double osv) const;
  double passesStarTime(double osh, double osv) const;
  bool   passesStarDist(double osh, double osv,
			double& xdist, double& xtime) const;
  bool   passesPortOrStar(double osh, double osv) const;

  
public: // Group C - Checks for Crossing Stern and/or Bow  
  bool   crossesStern(double osh, double osv) const;
  double crossesSternDist(double osh, double osv) const;
  double crossesSternTime(double osh, double osv) const;
  bool   crossesSternDist(double osh, double osv,
			  double& xdist, double& xtime) const;

  bool   crossesBow(double osh, double osv) const;
  double crossesBowDist(double osh, double osv) const;
  double crossesBowTime(double osh, double osv) const;
  bool   crossesBowDist(double osh, double osv,
			double& xdist, double& xtime) const;
  bool   crossesBowOrStern(double osh, double osv) const;

public: // Group W - Specific to this CPAEngine holonomic  
  double osSpdInCNHdgX(double osh, double osv) const; 
  double osToCNRelBngX(double osh) const;
  double minMaxROC(double, double, double&, double&) const;

 public: // Group X - Specific to this CPAEngine holonomic  
  double getOSSpeedGamma(double osh, double osv) const;
  double getOSSpeedEpsilon(double osh, double osv) const;
  double getOSTimeGamma(double osh, double osv) const;
  double getOSTimeEpsilon(double osh, double osv) const;

 public: // Group Y - Specific to this CPAEngine holonomic  
  double getThetaGamma() const   {return(m_stat_theta_os_gam);}
  double getThetaEpsilon() const {return(m_stat_theta_os_eps);}

protected: // Group Z - Specific to this CPAEngine holonomic  
  void   setStatic();

  void   initRateCache();
  void   initK1Cache();
  void   initK2Cache();
  
  void   initOSCNRelBngCache();
  void   initOSCNTangentCache();
  void   initOSGammaCache();
  void   initOSCNHCosCache();

 protected: // Cached values
  double m_stat_k2;  // Components of k2, k1, k0 that are 
  double m_stat_k1;  // static (independent of the values of
  double m_stat_k0;  // osCRS, osSPD, osTOL).
   
  double m_stat_cosCNH_x_cnSPD;
  double m_stat_sinCNH_x_cnSPD;
  
  double m_stat_theta_os_eps;
  double m_stat_theta_os_gam;
  double m_stat_theta_tn;
  double m_stat_tn_constant;

  double m_stat_spd_cn_at_tangent;
  
  double m_stat_range_gam;
  double m_stat_range_eps;
  
  std::vector<double> m_os_vthresh_cache_360;
  
  std::vector<double> m_k1_cache;
  std::vector<double> m_k2_cache;

  std::vector<double> m_os_cn_relbng_cache;
  std::vector<double> m_os_cn_relbng_cos_cache;

  std::vector<double> m_os_tn_cos_cache;
  std::vector<double> m_os_gam_cos_cache;
  std::vector<double> m_os_cnh_cos_cache;
};

#endif


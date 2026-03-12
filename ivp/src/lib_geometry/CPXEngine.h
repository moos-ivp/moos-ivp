/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEngineRoot.h                                      */
/*    DATE: June 12th 2017                                       */
/*    DATE: June 24th 2025  Mods to support platmodel version    */
/*    DATE: Dec 28th 2025  More Mods to support platmodel        */
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
 
#ifndef CPX_ENGINE_SUPER_HEADER
#define CPX_ENGINE_SUPER_HEADER

#include <vector>
#include "PlatModel.h"

class CPXEngine {
public:
  CPXEngine(double cny=0, double cnx=0, double cnh=0, double cnv=0,
	    double osy=0, double osx=0, double osh=0, double osv=0);

  virtual void reset(double cny, double cnx, double cnh, double cnv,
		     double osy, double osx, double osh=0, double osv=0);

  virtual ~CPXEngine() {}

  virtual void setPlatModel(PlatModel) {}

  std::string getSpec() const;
  
  double osx() const {return(m_osx);}
  double osy() const {return(m_osy);}
  double osh() const {return(m_osh);}
  double osv() const {return(m_osv);}

  double cnx() const {return(m_cnx);}
  double cny() const {return(m_cny);}
  double cnh() const {return(m_cnh);}
  double cnv() const {return(m_cnv);}
  
 public: // Group 1
  virtual bool   foreOfContact() const {return(m_stat_os_fore_of_cn);}
  virtual bool   aftOfContact() const  {return(m_stat_os_aft_of_cn);}
  virtual bool   portOfContact() const {return(m_stat_os_port_of_cn);}
  virtual bool   starOfContact() const {return(m_stat_os_star_of_cn);}
  virtual bool   osOnContact() const   {return(m_stat_os_on_contact);}

 public: // Group 2
  virtual double getRange() const {return(m_stat_range);} 
  virtual double evalROC(double osh, double osv) const = 0;

 public: // Group 3
  virtual double bearingRate(double osh, double osv) const = 0;
  virtual double getRangeGamma() const   {return(m_stat_range_gam);}
  virtual double getRangeEpsilon() const {return(m_stat_range_eps);}
  
 public: // Group 4  
  virtual double osToCNRelBng() const {return(m_stat_rel_bng_os_cn);}
  virtual double osToCNAbsBng() const {return(m_stat_abs_bng_os_cn);}
  virtual double cnToOSRelBng() const {return(m_stat_rel_bng_cn_os);}
  virtual double cnToOSAbsBng() const {return(m_stat_abs_bng_cn_os);}

 public: // Group 5
  virtual double osSpdInCNPos() const;
  virtual double osSpdInCNHdg() const;
  virtual double cnSpdInOSPos() const {return(m_stat_cn_to_os_spd);} 
  virtual double cnSpdInOSHdg() const;

public: // Group A
  virtual double evalCPA(double osh, double osv, double ost) const = 0;
  virtual double evalTimeCPA(double osh, double osv, double ost) const = 0;
  virtual void   evalCPA(double osh, double osv, double ost,
			 double& xdist, double& xtime) const = 0;

public: // Group B
  virtual bool   passesPort(double osh, double osv) const = 0;
  virtual double passesPortDist(double osh, double osv) const = 0;
  virtual double passesPortTime(double osh, double osv) const = 0;
  virtual bool   passesPortDist(double osh, double osv,
				double& xdist, double& xtime) const = 0;

  virtual bool   passesStar(double osh, double osv) const = 0;
  virtual double passesStarDist(double osh, double osv) const = 0;
  virtual double passesStarTime(double osh, double osv) const = 0;
  virtual bool   passesStarDist(double osh, double osv,
				double& xdist, double& xtime) const = 0;

  virtual bool   passesPortOrStar(double osh, double osv) const = 0;

public: // Group C
  virtual bool   crossesStern(double osh, double osv) const = 0;
  virtual double crossesSternDist(double osh, double osv) const = 0;
  virtual double crossesSternTime(double osh, double osv) const = 0;
  virtual bool   crossesSternDist(double osh, double osv,
				  double& xdist, double& xtime) const = 0;

  virtual bool   crossesBow(double osh, double osv) const = 0;
  virtual double crossesBowDist(double osh, double osv) const = 0;
  virtual double crossesBowTime(double osh, double osv) const = 0;
  virtual bool   crossesBowDist(double osh, double osv,
				double& xdist, double& xtime) const = 0;

  virtual bool   crossesBowOrStern(double osh, double osv) const = 0;
  
  
 protected: //* Setting initial static values
  void   setStaticRoot();
  void   initTrigCache();

 protected: // Config parameters
  double m_osx; 
  double m_osy; 
  double m_osh; 
  double m_osv; 

  double m_cnx; 
  double m_cny; 
  double m_cnh; 
  double m_cnv; 

 protected: // Static vars given current ownship heading
  double m_stat_range;
  
  std::vector<double> m_cos_cache;
  std::vector<double> m_sin_cache;

  double m_stat_abs_bng_cn_os;
  double m_stat_rel_bng_cn_os;
  double m_stat_abs_bng_os_cn;
  double m_stat_rel_bng_os_cn;

  bool   m_stat_os_fore_of_cn;
  bool   m_stat_os_aft_of_cn;
  bool   m_stat_os_port_of_cn;
  bool   m_stat_os_star_of_cn;
  
  double m_stat_cn_to_os_spd;
  bool   m_stat_cn_to_os_closing;
    
  bool   m_stat_os_on_contact;      // true if ownship on contact position
  bool   m_stat_os_on_bowline;      // true if ownship on contact bowline
  bool   m_stat_os_on_sternline;    // true if ownship on contact sternline
  bool   m_stat_os_on_bowsternline; // true if any of above three are true
  bool   m_stat_os_on_beam; 

  double m_stat_range_gam;
  double m_stat_range_eps;
};

#endif



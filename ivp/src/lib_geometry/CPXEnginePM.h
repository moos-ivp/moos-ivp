/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPXEnginePM.h                                        */
/*    DATE: Dec 26th 2025                                        */
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
 
#ifndef CPX_ENGINE_PM_HEADER
#define CPX_ENGINE_PM_HEADER

#include "CPXEngine.h"
#include "PlatModel.h"

class CPXEnginePM : public CPXEngine {
public:
  CPXEnginePM(double cny=0, double cnx=0, double cnh=0, double cnv=0,
	      double osy=0, double osx=0, double osh=0, double osv=0);

  void reset(double cny, double cnx, double cnh, double cnv,
	     double osy, double osx, double osh=0, double osv=0);
  
  ~CPXEnginePM() {}

  void setPlatModel(PlatModel pm) {m_plat_model=pm;}
  
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
		 double& xdist, double& xtime) const;

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

protected:
  bool   seglrIndex(const XYSeglr&, unsigned int,
		    double& osx, double& osy, double& osh, double osv,
		    double& cnx, double& cny, double& segt) const;

protected:
  PlatModel m_plat_model;
};

#endif

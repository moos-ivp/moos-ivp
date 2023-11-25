/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_SPD.h                                           */
/*    DATE: May 31st 2015                                        */
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

#ifndef OF_ZAIC_SPD_HEADER
#define OF_ZAIC_SPD_HEADER

#include <string>
#include <vector>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_SPD {
public:
  ZAIC_SPD(IvPDomain domain, std::string varname="");
  virtual ~ZAIC_SPD() {};

  bool setParams(double med_spd, double low_spd, double hgh_spd, 
		 double low_spd_util, double hgh_spd_util,
		 double min_spd_util=0, double max_spd_util=0,
		 double max_util=100);

  bool   setMedSpeed(double);
  bool   setLowSpeed(double);
  bool   setHghSpeed(double);

  bool   setLowSpeedUtil(double);
  bool   setHghSpeedUtil(double);
  bool   setMinSpdUtil(double);
  bool   setMaxSpdUtil(double);
  bool   setMaxUtil(double);
  bool   setMinMaxUtil(double, double, double);
  
  double getParam(std::string);

  void   disableLowSpeed();
  void   disableHighSpeed();
  
  bool   adjustParams();
  
  bool         stateOK()     {return(m_ivp_domain.size() == 0);};
  std::string  getWarnings() {return(m_warning);};
  IvPFunction* extractOF();
  IvPDomain    getIvPDomain() {return(m_ivp_domain);}
  IvPFunction* extractIvPFunction() {return(extractOF());};

  std::vector<std::string> getSummary() const;
  
protected:
  void   setPointLocations();
  PDMap* setPDMap();
  
protected:
  double m_med_spd;
  double m_low_spd;
  double m_hgh_spd;
  double m_low_spd_util;
  double m_hgh_spd_util;

  double m_min_spd_util;
  double m_max_spd_util;
  double m_max_util;

  bool   m_low_spd_enabled;
  bool   m_hgh_spd_enabled;
  
 protected:
  std::vector<unsigned int> m_dom;
  std::vector<double>       m_rng;

private:
  double m_domain_high;
  double m_domain_low;

  std::string  m_warning;
  IvPDomain    m_ivp_domain;
};

#endif


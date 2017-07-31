/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_PHTP.h                                          */
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
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_SPD {
public:
  ZAIC_SPD(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_SPD() {};

  bool setParams(double medspd, double lowspd, double hghspd, 
		 double lowspd_util, double hghspd_util,
		 double lminutil=0, double hminutil=0, double maxutil=100);

  bool   setMedSpeed(double);
  bool   setLowSpeed(double);
  bool   setHghSpeed(double);
  bool   setLowSpeedUtil(double);
  bool   setHghSpeedUtil(double);
  bool   setMinMaxUtil(double, double, double);
  
  double getParam(std::string);

  bool         stateOK()     {return(m_ivp_domain.size() == 0);};
  std::string  getWarnings() {return(m_warning);};
  IvPFunction* extractOF();
  IvPDomain    getIvPDomain() {return(m_ivp_domain);}
  IvPFunction* extractIvPFunction() {return(extractOF());};
  
protected:
  void   setPointLocations();
  PDMap* setPDMap();
  
protected:
  double m_medspd;
  double m_lowspd;
  double m_hghspd;
  double m_lowspd_util;
  double m_hghspd_util;

  double m_lminutil;
  double m_hminutil;
  double m_maxutil;

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





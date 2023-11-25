/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_APEAK.h                                         */
/*    DATE: May 11th 2016                                        */
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

#ifndef OF_ZAIC_HDG_HEADER
#define OF_ZAIC_HDG_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_HDG {
public:
  ZAIC_HDG(IvPDomain, std::string varname);
  virtual ~ZAIC_HDG() {}

  bool   setParams(double summit, double ldelta,  double hdelta, 
		   double ldelta_util, double hdelta_util,  
		   double lminutil, double hminutil, double maxutil);
  
  bool   setSummit(double);
  bool   setLowDelta(double);
  bool   setHighDelta(double);
  bool   setLowDeltaUtil(double);
  bool   setHighDeltaUtil(double);
  bool   setMinMaxUtil(double, double, double);

  double getParam(std::string);

  IvPFunction* extractOF();
  IvPFunction* extractIvPFunction() {return(extractOF());}

  IvPDomain getIvPDomain() const {return(m_ivp_domain);}
  
protected:
  double evalPoint(unsigned int pt_ix);

  PDMap* setPDMap(double tolerance = 0.001);
  
protected:
  double m_summit;
  double m_ldelta;
  double m_hdelta;

  double m_ldelta_util;
  double m_hdelta_util;
  
  double m_lminutil;
  double m_hminutil;
  double m_maxutil;

private:
  unsigned int m_domain_pts;

  double m_domain_high;
  double m_domain_low;
  double m_domain_delta;

  std::vector<double> m_ptvals;
  IvPDomain           m_ivp_domain;
};
#endif






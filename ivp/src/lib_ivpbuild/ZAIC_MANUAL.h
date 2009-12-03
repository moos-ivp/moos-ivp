/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_MANUAL.h                                        */
/*    DATE: Aug 5th 2008                                         */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef OF_ZAIC_MANUAL_HEADER
#define OF_ZAIC_MANUAL_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_MANUAL {
public:
  ZAIC_MANUAL(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_MANUAL() {};

  int addComponent(double low,  bool lowtype,  double lowutil, 
		   double high, bool hightype, double highutil);

  bool addComponent(std::string bnds, double lowpt, double lowutil, 
		    double highpt, double highutil);


  double       getParam(std::string);
  std::string  getParam(std::string, int index);

  int          size()                {return(m_low_val.size());};
  void         setValueWrap(bool v)  {m_value_wrap=v;};
  bool         getValueWrap()        {return(m_value_wrap);};
  bool         stateOK()             {return(m_state_ok);};
  std::string  getWarning()          {return(m_warning);};
  IvPFunction* extractOF();

protected:
  double evalPoint(unsigned int pt_ix);

  PDMap* setPDMap(double tolerance = 0.001);
  
protected:
  bool   m_value_wrap;

private:
  int    m_domain_pts;
  int    m_domain_ix;
  double m_domain_high;
  double m_domain_low;
  double m_domain_delta;

  std::vector<double> m_low_val;
  std::vector<bool>   m_low_val_inc;
  std::vector<double> m_low_val_util;

  std::vector<double> m_high_val;
  std::vector<bool>   m_high_val_inc;
  std::vector<double> m_high_val_util;

  bool   m_state_ok;

  std::string         m_warning;
  std::vector<double> m_ptvals;
  IvPDomain           m_ivp_domain;
};
#endif













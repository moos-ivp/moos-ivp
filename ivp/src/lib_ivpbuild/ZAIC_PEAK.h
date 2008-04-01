/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_PEAK.h                                          */
/*    DATE: Aug 17th 2006                                        */
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
#ifndef OF_ZAIC_PEAK_HEADER
#define OF_ZAIC_PEAK_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_PEAK {
public:
  ZAIC_PEAK(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_PEAK() {};

  int  addSummit();

#if 1  // New interface Start -------------
  bool setParams(double summit, double pwidth,  double bwidth, 
		 double delta,  double minutil, double maxutil, 
		 int index=0);
  bool setParam(std::string param, std::string value);
  bool setParam(std::string param, double value, int index=0);
  bool setParam(std::string param1, double value1, 
		std::string param2, double value2, int index=0);
#endif  // New interface  End  -------------

#if 1
    //bool   addSummit(double, double, double, double, double, double);

  bool   setSummit(double, int index=0);
  bool   setBaseWidth(double, int index=0);
  bool   setPeakWidth(double, int index=0);
  bool   setSummitDelta(double, int index=0);
  bool   setMinMaxUtil(double, double, int index=0);
  void   setSummitInsist(bool v)      {m_summit_insist=v;};
  void   setValueWrap(bool v)         {m_value_wrap=v;};
#endif


  double getParam(std::string, int index=0);
  bool   getValueWrap()     {return(m_value_wrap);};
  bool   getSummitInsist()  {return(m_summit_insist);};
  int    getSummitCount()   {return(v_summit.size());};

  IvPFunction* extractOF(bool maxval=true);

protected:
  double evalPoint(int pt_ix, bool maxval=true);
  double evalPoint(int summit_ix, int pt_ix);

  void   insistSummit(int summit_ix);
  PDMap* setPDMap(double tolerance = 0.001);
  
protected:
  std::vector<double> v_summit;
  std::vector<double> v_basewidth;
  std::vector<double> v_peakwidth;
  std::vector<double> v_summitdelta;
  std::vector<double> v_minutil;
  std::vector<double> v_maxutil;

  bool   m_summit_insist;
  bool   m_value_wrap;

private:
  int    m_domain_ix;
  double m_domain_high;
  double m_domain_low;
  double m_domain_delta;
  int    m_domain_pts;

  bool   m_state_ok;

  std::vector<double> m_ptvals;
  IvPDomain           m_ivp_domain;
};
#endif













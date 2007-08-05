/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_TPEAK.h                                         */
/*    DATE: Jun 15th 2006                                        */
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
#ifndef OF_ZAIC_TPEAK_HEADER
#define OF_ZAIC_TPEAK_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_TPEAK {
public:
  ZAIC_TPEAK(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_TPEAK() {};

  bool  setSummit(double);
  bool  setBaseWidth(double);
  bool  setPeakWidth(double);
  bool  setSummitDelta(double);
  bool  setParam(const std::string&, const std::string&);

  IvPFunction* extractOF();

protected:
  bool   setDPointLocations();
  bool   setIPointLocations();
  PDMap* setPDMap();
  
protected:
  double summit;
  double base_width;
  double peak_width;
  double summit_delta;

  int    domain_ix;
  double domain_high;
  double domain_low;
  double domain_delta;
  int    domain_pts;
  
  double dpt_low;
  double dpt_baselow;
  double dpt_platlow;
  double dpt_cent;
  double dpt_plathigh;
  double dpt_basehigh;
  double dpt_high;

  int    ipt_low;
  int    ipt_baselow;
  int    ipt_platlow;
  int    ipt_cent;
  int    ipt_plathigh;
  int    ipt_basehigh;
  int    ipt_high;

  int    i_basewidth;
  int    i_peakwidth;

  IvPDomain    ivp_domain;
  std::string  varname;
};
#endif











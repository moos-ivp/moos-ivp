/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_WPEAK.h                                         */
/*    DATE: Jun 19th 2006                                        */
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
#ifndef OF_ZAIC_WPEAK_HEADER
#define OF_ZAIC_WPEAK_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_WPEAK {
public:
  ZAIC_WPEAK(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_WPEAK() {};

  bool  setSummit(double);
  bool  setBaseWidth(double);
  bool  setPeakWidth(double);
  bool  setSummitDelta(double);
  bool  setParam(const std::string&, const std::string&);

  IvPFunction* extractOF();

protected:
  double evalPoint(int);

  PDMap* setPDMap(double tolerance = 0.001);
  PDMap* setPDMap2();
  
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

  std::vector<double> ptvals;
  IvPDomain           ivp_domain;
  std::string         varname;
};
#endif











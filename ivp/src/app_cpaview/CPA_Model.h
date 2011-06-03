/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPA_Model.h                                          */
/*    DATE: Feb 18, 2007                                         */
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

#ifndef CPA_MODEL_HEADER
#define CPA_MODEL_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class CPA_Model
{
 public:
  CPA_Model();
  ~CPA_Model() {};

public:
  bool   initFromFile(std::string filename);
  void   push_new_contact();

  void   set_osx(double v)        {m_os_x = v;};
  void   set_osy(double v)        {m_os_y = v;};
  void   set_os_maxspd(double v)  {m_os_maxspd = v;};
  void   set_os_maxtol(double v)  {m_os_maxtol = v;};
  void   shift_osx(double v)      {m_os_x += v;};
  void   shift_osy(double v)      {m_os_y += v;};
  void   shift_os_maxspd(double v);
  void   shift_os_maxtol(double v);

  void   set_cnx(double v, unsigned int index=0);
  void   set_cny(double v, unsigned int index=0);
  void   set_cnspd(double v, unsigned int index=0);
  void   set_cncrs(double v, unsigned int index=0);
  void   set_cnrng(double v, unsigned int index=0);
  void   set_cnbng(double v, unsigned int index=0);

  void   shift_cnx(double v, unsigned int index=0);
  void   shift_cny(double v, unsigned int index=0);
  void   shift_cnspd(double v, unsigned int index=0);
  void   shift_cncrs(double v, unsigned int index=0);
  void   shift_range(double v, unsigned int index=0);

  void   set_cn_range_bearing(std::string, unsigned int index=0);

  double get_param(const std::string&, unsigned int index=0) const;
  unsigned int size() {return(m_cnx.size());};

  IvPFunction *calc_cpa_ipf(unsigned int index, unsigned int unif_units);
  IvPFunction *calc_wpt_ipf(unsigned int index, unsigned int unif_units);
  IvPFunction *calc_spt_ipf(unsigned int index, unsigned int unif_units);
  IvPFunction *calc_avd_ipf(unsigned int index, unsigned int unif_units);
  IvPFunction *calc_roc_ipf(unsigned int index, unsigned int unif_units);
  IvPFunction *calc_r16_ipf(unsigned int index, unsigned int unif_units);

  std::vector<double> extrapolate(double spd, double time, unsigned int ix);

 private:
  double  m_os_x;
  double  m_os_y;
  double  m_os_maxtol;
  double  m_os_maxspd;
  double  m_os_wantcpa;

  std::vector<double>  m_cnx;
  std::vector<double>  m_cny;
  std::vector<double>  m_cnspd;
  std::vector<double>  m_cncrs;

  IvPDomain m_ivp_domain;
};

#endif 


/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPA_Model.h                                          */
/*    DATE: Feb 18, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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

  void   set_osx(double v)    {m_os_x = v;};
  void   set_osy(double v)    {m_os_y = v;};
  void   shift_osx(double v)  {m_os_x += v;};
  void   shift_osy(double v)  {m_os_y += v;};
  void   shift_os_maxspd(double v);
  void   shift_os_maxtol(double v);

  void   set_cnx(double v, int index=0);
  void   set_cny(double v, int index=0);
  void   shift_cnx(double v, int index=0);
  void   shift_cny(double v, int index=0);
  void   shift_cnspd(double v, int index=0);
  void   shift_cncrs(double v, int index=0);

  double get_param(const std::string&, int index=0) const;
  int    size() {return(m_cnx.size());};

  IvPFunction *calc_cpa_ipf(int index, int unif_units);
  IvPFunction *calc_wpt_ipf(int index, int unif_units);
  IvPFunction *calc_avd_ipf(int index, int unif_units);

  std::vector<double> extrapolate(double spd, double time, int ix);

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

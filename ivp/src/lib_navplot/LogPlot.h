/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogPlot.h                                            */
/*    DATE: May 31st, 2005                                       */
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

#ifndef LOG_PLOT_HEADER
#define LOG_PLOT_HEADER

#include <string>
#include <vector>
#include <list>

class LogPlot
{
public:
  LogPlot();
  ~LogPlot() {};

  void   set_varname(std::string s)   {m_varname = s;};
  void   set_vehi_name(std::string s) {m_vehi_name = s;};
  bool   set_value(double gtime, double gvalue);
  double get_time_by_index(unsigned int index) const;
  double get_value_by_index(unsigned int index) const;
  double get_value_by_time(double gtime, bool interp=false) const;
  double get_median();
  double get_min_time() const;
  double get_max_time() const;
  double get_min_val() const        {return(m_min_val);};
  double get_max_val() const        {return(m_max_val);};
  std::string get_varname() const   {return(m_varname);};
  std::string get_vehi_name() const {return(m_vehi_name);};
  unsigned int  size() const        {return(m_time.size());};
  void   print() const;
  
  unsigned int get_index_by_time(double) const;

protected:
  std::string m_vehi_name;       // Name of the platform
  std::string m_varname;         // Variable name to be plotted
  std::vector<double> m_time;
  std::vector<double> m_value;

  double m_utc_start_time;
  double m_min_val;
  double m_max_val;

  bool   m_median_set;
  double m_median;
};
#endif 









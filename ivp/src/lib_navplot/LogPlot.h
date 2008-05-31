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

class LogPlot
{
public:
  LogPlot() {min_val=0; max_val=0; utc_start_time=0;};
  ~LogPlot() {};

  void   set_varname(std::string s) {varname = s;};
  void   set_vehicle(std::string s) {vehicle = s;};
  bool   set_value(double gtime, double gvalue);
  double get_time_by_index(unsigned int index) const;
  double get_value_by_index(unsigned int index) const;
  double get_value_by_time(double gtime) const;
  double get_min_time() const;
  double get_max_time() const;
  double get_min_val() const      {return(min_val);};
  double get_max_val() const      {return(max_val);};
  std::string get_varname() const {return(varname);};
  std::string get_vehicle() const {return(vehicle);};
  unsigned int  size() const      {return(time.size());};
  void   print() const;
  
protected:
  int get_index_by_time(double) const;

protected:
  std::string vehicle;         // Name of the platform
  std::string varname;         // Variable name to be plotted
  std::vector<double> time;
  std::vector<double> value;

  double utc_start_time;
  double min_val;
  double max_val;
};
#endif 









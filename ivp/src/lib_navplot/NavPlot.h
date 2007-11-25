/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NavPlot.h                                            */
/*    DATE: June 1st, 2005                                       */
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

#ifndef NAV_PLOT_HEADER
#define NAV_PLOT_HEADER

#include <string>
#include <vector>

class NavPlot
{
public:
  NavPlot() {};
  ~NavPlot() {};

  bool   set_vehicle(std::string s) {vehicle = s;};
  bool   set_value(double, double, double, double hdg=999);
  double get_time_by_index(int) const;

  double get_xval_by_index(int) const;
  double get_yval_by_index(int) const;
  double get_cval_by_index(int) const;

  double get_xval_by_time(double) const;
  double get_yval_by_time(double) const;
  double get_cval_by_time(double) const;

  double get_min_time() const;
  double get_max_time() const;
  int    get_size() const       {return(time.size());};
  void   print() const;

  std::string get_vehicle() const {return(vehicle);};

protected:
  int get_index_by_time(double) const;

protected:
  std::string vehicle;       // Typically platform name
  std::vector<double> time;
  std::vector<double> xval;
  std::vector<double> yval;
  std::vector<double> cval;  // course/heading btween pts
};
#endif 









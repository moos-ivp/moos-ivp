/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IPF_Plot.h                                           */
/*    DATE: Feb 24th, 2007                                       */
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

#ifndef IPF_PLOT_HEADER
#define IPF_PLOT_HEADER

#include <string>
#include <vector>

class IPF_Plot
{
public:
  IPF_Plot() {};
  ~IPF_Plot() {};

  void   set_ipf_source(std::string s) {m_ipf_source = s;};
  bool   add_entry(double, const std::string& str);
  double get_time_by_index(unsigned int) const;

  std::string get_ipf_by_index(unsigned int) const;
  std::string get_ipf_by_time(double) const;
  std::string get_ipf_source() const {return(m_ipf_source);};

  double get_min_time() const;
  double get_max_time() const;
  int    size() const          {return(m_time_stamps.size());};
  void   print() const;

  std::string  nullHeadingSpeedIPF() const;

protected:
  int get_index_by_time(double) const;

protected:
  std::string              m_ipf_source;  // Vehicle_Behavior
  std::vector<double>      m_time_stamps;
  std::vector<std::string> m_ipf_strings; 
};
#endif 









/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmPlot.h                                           */
/*    DATE: July 15th, 2009                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef HELM_PLOT_HEADER
#define HELM_PLOT_HEADER

#include <string>
#include <vector>

class HelmPlot
{
public:
  HelmPlot();
  ~HelmPlot() {};

  void   set_vehi_name(std::string s) {m_vehi_name = s;};
  void   set_vehi_length(double len)  {m_vehi_length = len;};
  void   set_vehi_type(std::string);
  
  bool   add_entry(double time, std::string helm_summary);
  
  double get_time_by_index(unsigned int index) const;
  double get_time_by_iter_add(double curr_time, 
			      unsigned int iter_offset) const;
  double get_time_by_iter_sub(double curr_time, 
			      unsigned int iter_offset) const;
  unsigned int get_iter_by_time(double) const;

  std::string  get_value_by_index(std::string value_type, 
				  unsigned int index) const;
  std::string  get_value_by_time(std::string value_type,
				 double gtime) const;
  
  std::string  get_vehi_name()    const {return(m_vehi_name);};
  std::string  get_vehi_type()    const {return(m_vehi_type);};
  double       get_vehi_length()  const {return(m_vehi_length);};
  unsigned int size()         const {return(m_time.size());};
  double       get_min_time() const;
  double       get_max_time() const;
  void         print()        const;

protected:
  int get_index_by_time(double) const;

protected:
  std::string m_vehi_name;
  std::string m_vehi_type;
  double      m_vehi_length;

  std::vector<double>       m_time;
  std::vector<unsigned int> m_helm_iter_v;
  std::vector<std::string>  m_helm_iter_s;
  std::vector<std::string>  m_helm_mode;
  std::vector<std::string>  m_helm_mode_short;
  std::vector<std::string>  m_helm_utc;
  std::vector<std::string>  m_helm_idle_bhvs;
  std::vector<std::string>  m_helm_running_bhvs;
  std::vector<std::string>  m_helm_active_bhvs;
  std::vector<std::string>  m_helm_completed_bhvs;
  std::vector<std::string>  m_helm_decision;

  double m_utc_start_time;
};
#endif 






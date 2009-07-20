/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmPlot.h                                           */
/*    DATE: July 15th, 2009                                      */
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

#ifndef HELM_PLOT_HEADER
#define HELM_PLOT_HEADER

#include <string>
#include <vector>

class HelmPlot
{
public:
  HelmPlot() {m_utc_start_time=0;};
  ~HelmPlot() {};

  void   set_vehi_name(std::string s) {m_vehicle_name = s;};
  void   set_vehi_type(std::string s) {m_vehicle_type = s;};
  void   set_vehi_length(double len)  {m_vehicle_length = len;};
  
  bool   add_entry(double time, std::string helm_summary);
  
  double get_time_by_index(unsigned int index) const;
  
  std::string  get_value_by_index(std::string value_type, 
				  unsigned int index) const;
  std::string  get_value_by_time(std::string value_type,
				 double gtime) const;

  double get_min_time() const;
  double get_max_time() const;
  unsigned int  size() const  {return(m_time.size());};
  void   print() const;
  
protected:
  int get_index_by_time(double) const;

protected:
  std::string m_vehicle_name;
  std::string m_vehicle_type;
  double      m_vehicle_length;

  std::vector<double>      m_time;
  std::vector<std::string> m_helm_iter;
  std::vector<std::string> m_helm_mode;
  std::vector<std::string> m_helm_utc;
  std::vector<std::string> m_helm_idle_bhvs;
  std::vector<std::string> m_helm_running_bhvs;
  std::vector<std::string> m_helm_active_bhvs;
  std::vector<std::string> m_helm_completed_bhvs;

  double m_utc_start_time;
};
#endif 

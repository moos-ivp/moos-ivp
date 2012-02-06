/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTPlot.h                                           */
/*    DATE: May3105, Sep2811                                     */
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
#include "TimePlot.h"

class LogTPlot : public TimePlot {
 public:
  LogTPlot();
  ~LogTPlot() {};
  
 public: // Settting
  void   setVarName(std::string s)   {m_varname = s;};
  bool   setValue(double gtime, double gvalue, bool recalc_median=true);
  void   resetMedian();

 public: // Getting
  double getValueByIndex(unsigned int index) const;
  double getMedian() const;

  double getValueByTime(double gtime, bool interp=false) const;
  double getValueByTimeUTC(double gtime, bool interp=false) const;

  double getMinVal() const        {return(m_min_val);};
  double getMaxVal() const        {return(m_max_val);};
  void   print() const;

  std::string getVarName() const  {return(m_varname);};
  
protected:
  std::string m_varname;         // Variable name to be plotted
  std::vector<double> m_value;

  double m_min_val;
  double m_max_val;

  bool   m_median_set;
  double m_median;
};
#endif 










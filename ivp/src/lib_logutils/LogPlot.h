/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogPlot.h                                            */
/*    DATE: May 31st, 2005                                       */
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

#ifndef LOG_PLOT_HEADER
#define LOG_PLOT_HEADER

#include <string>
#include <vector>

class LogPlot
{
public:
  LogPlot();
  ~LogPlot() {}
  
 public: // Setting
  void   setVName(std::string s)    {m_vname = s;}
  void   setVarName(std::string s)  {m_varname = s;}
  bool   setValue(double gtime, double gvalue);
  bool   setValueByIndex(unsigned int ix, double gvalue);

 public: // Modification
  void   applySkew(double skew);
  void   modValues(double modval);
  void   filterOutHighest(double pct);


 public: // Querying
  double getTimeByIndex(unsigned int index) const;
  double getValueByIndex(unsigned int index) const;
  double getValueByTime(double gtime, bool interp=false) const;
  double getAvgTimeGap();
  
  unsigned int getLPIndexByTime(double gtime) const;
  
  double getMedian();
  double getMinTime() const;
  double getMaxTime() const;
  double getMean() const          {return((m_max_val-m_min_val)/2);}
  double getMinVal() const        {return(m_min_val);}
  double getMaxVal() const        {return(m_max_val);}
  double getMinVal(double mintime, double maxtime) const;
  double getMaxVal(double mintime, double maxtime) const;

  std::string getVName() const    {return(m_vname);}
  std::string getVarName() const  {return(m_varname);}
  unsigned int  size() const      {return(m_time.size());}

  bool   empty() const            {return(m_time.size() == 0);}
  void   print() const;

  std::string getSpec(unsigned int tprec=3, unsigned vprec=3) const;
  bool   setFromSpec(std::string);
  
protected:
  std::string m_vname;           // Vehicle or Node name
  std::string m_varname;         // Variable name to be plotted
  std::vector<double> m_time;
  std::vector<double> m_value;

  double m_min_val;
  double m_max_val;

  bool   m_median_set;
  double m_median;

  double m_avg_time_gap;
};
#endif 






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
  IPF_Plot() {m_vname="name-unknown"; m_source="src-unknown";};
  ~IPF_Plot() {};

  void   setVName(std::string s)   {m_vname  = s;};
  void   setSource(std::string s)  {m_source = s;};
  bool   addEntry(double, const std::string& str);

  double getTimeByIndex(unsigned int) const;

  std::string getIPFByIndex(unsigned int) const;
  std::string getIPFByTime(double) const;

  std::string getSource() const {return(m_source);};
  std::string getVName()  const {return(m_vname);};

  double getMinTime() const;
  double getMaxTime() const;
  int    size() const          {return(m_time_stamps.size());};
  void   print() const;

  std::string  nullHeadingSpeedIPF() const;

protected:
  int getIndexByTime(double) const;

protected:
  std::string              m_vname;       // Vehicle Name
  std::string              m_source;      // Vehicle Behavior

  // One entry for each BHV_IPF string. All entries should be from
  // the same vehicle and same behavior.
  std::vector<double>      m_time_stamps; 
  std::vector<std::string> m_ipf_strings;  
};
#endif 









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
#include "IvPFunction.h"

class IPF_Plot
{
public:
  IPF_Plot() {m_vname="name-unknown"; m_source="src-unknown";};
  ~IPF_Plot();

  const IPF_Plot &operator=(const IPF_Plot&);

  void   setVName(std::string s)   {m_vname  = s;};
  void   setSource(std::string s)  {m_source = s;};
  bool   addEntry(IvPFunction*, double, unsigned int);

  double getTimeByIndex(unsigned int) const;

  IvPFunction* getIPFByIndex(unsigned int) const;
  IvPFunction* getIPFByTime(double) const;
  IvPFunction* getIPFByHelmIteration(unsigned int) const;

  std::string getSource() const {return(m_source);};
  std::string getVName()  const {return(m_vname);};

  double getMinTime() const;
  double getMaxTime() const;
  int    size() const          {return(m_time_stamp.size());};

  std::string  nullHeadingSpeedIPF() const;

protected:
  int getIndexByTime(double) const;
  int getIndexByHelmIter(unsigned int) const;

protected:
  std::string   m_vname;       // Vehicle Name
  std::string   m_source;      // Vehicle Behavior

  // One entry for each BHV_IPF string. All entries should be from
  // the same vehicle and same behavior.
  std::vector<IvPFunction*> m_ivp_function;
  std::vector<double>       m_time_stamp; 
  std::vector<unsigned int> m_helm_iteration;
};
#endif 

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
#include "IvPDomain.h"

class IPF_Plot
{
public:
  IPF_Plot() {m_vname="name-unknown"; m_source="src-unknown";};
  ~IPF_Plot() {};

  void   setVName(std::string s)   {m_vname  = s;};
  void   setSource(std::string s)  {m_source = s;};
  bool   addEntry(double timestamp, 
		  const std::string& ipf_str, 
		  unsigned int iteration, 
		  unsigned int piece_count);

  double getTimeByIndex(unsigned int) const;

  std::string getIPFByIndex(unsigned int) const;
  std::string getIPFByTime(double) const;
  std::string getIPFByHelmIteration(unsigned int) const;
  unsigned int getPcsByHelmIteration(unsigned int) const;
  unsigned int getHelmIterByTime(double);

  std::string getSource() const {return(m_source);};
  std::string getVName()  const {return(m_vname);};

  double getMinTime() const;
  double getMaxTime() const;
  int    size() const          {return(m_time_stamp.size());};
  void   print() const;

  std::string  nullHeadingSpeedIPF() const;

  void      setIvPDomain(IvPDomain dom) {m_ivp_domain = dom;};
  IvPDomain getIvPDomain()   {return(m_ivp_domain);};

protected:
  unsigned int getIndexByTime(double) const;
  int getIndexByHelmIter(unsigned int) const;

protected:
  std::string              m_vname;       // Vehicle Name
  std::string              m_source;      // Vehicle Behavior

  // One entry for each BHV_IPF string. All entries should be from
  // the same vehicle and same behavior.
  std::vector<std::string>  m_ipf_string;  
  std::vector<double>       m_time_stamp; 
  std::vector<unsigned int> m_helm_iteration;
  std::vector<unsigned int> m_piece_count;

  IvPDomain                 m_ivp_domain;
};
#endif 

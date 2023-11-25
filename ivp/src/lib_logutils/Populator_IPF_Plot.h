/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_IPF_Plot.h                                 */
/*    DATE: June 5th, 2005 (Sun in Kenmorre)                     */
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

#ifndef POPULATOR_IPF_PLOT_HEADER
#define POPULATOR_IPF_PLOT_HEADER

#include <string>
#include "IPF_Plot.h"
#include "Demuxer.h"
#include "ALogEntry.h"

class Populator_IPF_Plot 
{
public:
  Populator_IPF_Plot() {}
  ~Populator_IPF_Plot() {}

  bool  populateFromEntries(const std::vector<ALogEntry>&);
  bool  setIvPDomain(const std::string&);

  void  setVName(std::string s)   {m_vname = s;}
  int   size()                    {return(m_ipf_plots.size());}

  IPF_Plot    getPlotIPF(unsigned int ix=0);
  std::string getTagIPF(unsigned int ix=0);
  
  void  print();

protected:
  void handleEntry(double, const std::string&);

protected:
  std::string              m_vname;
  std::vector<std::string> m_ipf_tags;
  std::vector<IPF_Plot>    m_ipf_plots;
  Demuxer                  m_demuxer;

  IvPDomain                m_ivp_domain;
};

#endif 






/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_NavPlot.h                                  */
/*    DATE: June 5th, 2005 (Sun in Kenmorre)                     */
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

#ifndef POPULATOR_IPF_PLOT_HEADER
#define POPULATOR_IPF_PLOT_HEADER

#include <string>
#include "IPF_Plot.h"
#include "Demuxer.h"

class Populator_IPF_Plot 
{
public:
  Populator_IPF_Plot() {};
  ~Populator_IPF_Plot() {};

  void  populate(std::string);
  void  set_vname(std::string s)  {m_vname = s;};
  int   size()                    {return(m_ipf_plots.size());};

  IPF_Plot    getPlotIPF(int ix=0);
  std::string getTagIPF(int ix=0);
  
  void  print();

protected:
  void handleEntry(double, const std::string&);

protected:
  std::string              m_vname;
  std::vector<std::string> m_ipf_tags;
  std::vector<IPF_Plot>    m_ipf_plots;

  Demuxer                  m_demuxer;
};
#endif 









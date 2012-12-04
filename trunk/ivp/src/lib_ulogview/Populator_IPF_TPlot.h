/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_IPF_TPlot.h                                */
/*    DATE: Feb2407, Sep2811                                     */
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
#include "IPF_TPlot.h"
#include "IterTPlot.h"
#include "Demuxer.h"
#include "ALogEntry.h"

class Populator_IPF_TPlot 
{
public:
  Populator_IPF_TPlot() {m_logstart_utc=0;};
  ~Populator_IPF_TPlot() {};

  // Setters
  bool  populateFromEntries(const std::vector<ALogEntry>&);
  void  setVName(std::string s)   {m_vname = s;};
  int   size()                    {return(m_ipf_plots.size());};

  void  setIvPDomain(IvPDomain dom) {m_ivp_domain = dom;};
  void  setLogStartUTC(double time);

  // Getters
  const IterTPlot&  getIterPlot() const    {return(m_iter_plot);};
  const IPF_TPlot&  getPlotIPF(unsigned int ix=0);
  std::string       getTagIPF(unsigned int ix=0);
  
  void  print();

protected:
  void handleEntry(double, const std::string&);

protected:
  std::string              m_vname;
  std::vector<std::string> m_ipf_tags;
  std::vector<IPF_TPlot>   m_ipf_plots;
  Demuxer                  m_demuxer;

  IterTPlot                m_iter_plot;

  IvPDomain                m_ivp_domain;
  double                   m_logstart_utc;

  IPF_TPlot                m_null_plot;
};
#endif 



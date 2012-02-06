/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_HelmTPlots.h                               */
/*    DATE: Jul1509 (at GLINT'09), Sep2811                       */
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

#ifndef POPULATOR_HELM_TPLOTS_HEADER
#define POPULATOR_HELM_TPLOTS_HEADER

#include <string>
#include <vector>
#include "HelmTPlot.h"
#include "HelmTPlotX.h"
#include "ALogEntry.h"

class Populator_HelmTPlots 
{
public:
  Populator_HelmTPlots() {m_logstart_utc=0;};
  ~Populator_HelmTPlots() {};

  bool   populateFromEntries(const std::vector<ALogEntry>&);
  void   setLogStartUTC(double time);
  void   setVName(std::string);

  const HelmTPlot& getHelmPlot() const {return(m_helm_plot);};
  const HelmTPlotX& getHelmPlotX() const {return(m_helm_plotx);};

protected:
  std::string  m_vname;
  HelmTPlot    m_helm_plot;
  HelmTPlotX   m_helm_plotx;
  double       m_logstart_utc;
};
#endif 

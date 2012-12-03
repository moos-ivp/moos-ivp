/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotElement.h                                     */
/*    DATE: Sep 27th, 2011                                       */
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

#ifndef NAVPLOT_ELEMENT_HEADER
#define NAVPLOT_ELEMENT_HEADER

#include <vector>
#include <string>
#include "LogTPlot.h"
#include "VPlugTPlot.h"

class NavPlotElement
{
 public:
  NavPlotElement();
  virtual ~NavPlotElement() {};

  void   initialize();
  
  void   setLogPlotNAVX(const LogTPlot& lp)  {m_navx_plot=lp;};
  void   setLogPlotNAVY(const LogTPlot& lp)  {m_navy_plot=lp;}; 
  void   setLogPlotHDG(const LogTPlot& lp)   {m_hdg_plot=lp;}; 
  void   setVPlugPlot(const VPlugTPlot& vp)  {m_vplug_plot=vp;}; 
  void   setVName(const std::string& s)      {m_vname=s;};
  void   setVType(const std::string& s)      {m_vtype=s;};
  void   setVLength(double len)              {m_vlength=len;};
  void   setLogStartUTC(double v)            {m_logstart_utc=v;};

  const  LogTPlot&   getLogPlotX() const    {return(m_navx_plot);};
  const  LogTPlot&   getLogPlotY() const    {return(m_navy_plot);};
  const  LogTPlot&   getLogPlotHDG() const  {return(m_hdg_plot);};
  const  VPlugTPlot& getVPlugPlot() const   {return(m_vplug_plot);};

  std::string getVName() const   {return(m_vname);};
  std::string getVType() const   {return(m_vtype);};
  double      getVLength() const {return(m_vlength);};

  double      getMinTime() const     {return(m_min_time_local);};
  double      getMaxTime() const     {return(m_max_time_local);};
  double      getMinTimeUTC() const  {return(m_min_time_utc);};
  double      getMaxTimeUTC() const  {return(m_max_time_utc);};
  double      getLogStartUTC() const {return(m_logstart_utc);};

  double      getMinX() const {return(m_min_xpos);};
  double      getMaxX() const {return(m_max_xpos);};
  double      getMinY() const {return(m_min_ypos);};
  double      getMaxY() const {return(m_max_ypos);};

 protected:
  LogTPlot     m_navx_plot;
  LogTPlot     m_navy_plot;
  LogTPlot     m_hdg_plot;
  VPlugTPlot   m_vplug_plot;

  // Time Bounding boxes across all TimePlots
  double m_min_time_local;
  double m_max_time_local;
  double m_min_time_utc;
  double m_max_time_utc;
  double m_logstart_utc;

  double m_min_xpos;
  double m_min_ypos;
  double m_max_xpos;
  double m_max_ypos;

  std::string m_vname;
  std::string m_vtype;
  double      m_vlength;
};

#endif 


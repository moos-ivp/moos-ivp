/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPV_Viewer.h                                         */
/*    DATE: May 18th, 2012                                       */
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

#ifndef PLOT_VIEWER_HEADER
#define PLOT_VIEWER_HEADER

// Defined to silence GL deprecation warnings in OSX 10.14+
#define GL_SILENCE_DEPRECATION

#include <string>
#include <map>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "IvPDomain.h"
#include "PartitionRecord.h"

class UPV_Viewer : public Fl_Gl_Window
{
 public:
  UPV_Viewer(int x,int y,int w,int h,const char *l=0);
  ~UPV_Viewer() {}
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  addPartition(std::string, double min, double max, unsigned int bars,
		     std::string color="blue");
  void  addValue(std::string varname, double value);

  void  setParam(std::string, std::string);
  
  void  setHotVar(std::string varname);
  bool  hideVar(std::string varname);
  void  showVar(std::string varname);

  std::string getHotVar()    const {return(m_hotvar);}
  double      getHotVarAvg() const;

  void  printPartitions() const;

 protected:
  void  drawAxes();
  void  drawGrids();
  void  drawLabels();
  void  drawText(int, int, std::string);
  void  drawPartitions() const;
  void  drawPartition(const PartitionRecord&) const;
  void  drawBar(double, double, double, double, 
		double, double, double, double, ColorPack) const;

 protected:
  int    m_color_scheme;
  int    m_curr_mode;
  int    m_grid_block_size;
  double m_back_shade;
  double m_grid_shade;
  double m_line_shade;

  int    m_x_offset;
  int    m_y_offset;
  int    m_y_grid_height;
  int    m_x_grid_width;

  std::string m_hotvar;

  std::map<std::string, bool>            m_map_render;
  std::map<std::string, PartitionRecord> m_map_partitions;


};

#endif 










/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPV_Viewer.cpp                                       */
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

#include <string>
#include <iostream>
#include <cmath>
#include <cstring>
#include "UPV_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "IO_Utilities.h"

using namespace std;

UPV_Viewer::UPV_Viewer(int gx, int gy, int gw, int gh, const char *gl)
  : Fl_Gl_Window(gx,gy,gw,gh,gl)
{
  m_x_offset  = 50;
  m_y_offset  = 50;

  m_x_grid_width    = gw - (m_x_offset*2);
  m_y_grid_height   = gh - (m_x_offset*2);

  setParam("gridsize",  "default");
  setParam("gridshade", "default");
  setParam("backshade", "default");
  setParam("lineshade", "default");

  // The use_high_res_GL function is supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  Fl::use_high_res_GL(1);
#endif
}

//-------------------------------------------------------------
// Procedure: draw()

void UPV_Viewer::draw()
{
  glClearColor(0.5,0.5,0.5 ,0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // The pixel_w/h() functions are supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  glViewport(0, 0, pixel_w(), pixel_h());
#else
  glViewport(0, 0, w(), h());
#endif

  drawGrids();
  drawPartitions();
  drawAxes();
}

//-------------------------------------------------------------
// Procedure: handle()

int UPV_Viewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: resize()

void UPV_Viewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);

  m_x_grid_width   = gw - (2*m_x_offset);
  m_y_grid_height  = gh - (2*m_x_offset);
}

//-------------------------------------------------------------
// Procedure: drawGrids

void UPV_Viewer::drawGrids()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(m_x_offset, m_y_offset, 0);
  
  // Draw Grid Background
  double r=(0.43 * m_back_shade);
  double g=(0.43 * m_back_shade);
  double b=(0.58 * m_back_shade);
  glColor4f(r,g,b,0);
  glBegin(GL_POLYGON);
  glVertex2f(0,  0);
  glVertex2f(m_x_grid_width, 0);
  glVertex2f(m_x_grid_width, m_y_grid_height);
  glVertex2f(0, m_y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  double hashc = 0.6 * m_grid_shade;
  // Draw Vertical Hashmarks
  for(int i=0; i<m_x_grid_width; i+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(i, 0);
    glVertex2f(i, m_y_grid_height);
    glEnd();
  }

  // Draw Horizontal Hashmarks
  for(int j=0; j<m_y_grid_height; j+=m_grid_block_size) {
    glColor4f(hashc,  hashc,  hashc,  0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, j);
    glVertex2f(m_x_grid_width,  j);
    glEnd();
  }

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawAxes

void UPV_Viewer::drawAxes()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(m_x_offset, m_y_offset, 0);
  
  // Draw Grid outline
  glColor4f(1.0,  1.0,  1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0,  0);
  glVertex2f(m_x_grid_width, 0);
  glVertex2f(m_x_grid_width, m_y_grid_height);
  glVertex2f(0, m_y_grid_height);
  glVertex2f(0, 0);
  glEnd();
  
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawLabels

void UPV_Viewer::drawLabels()
{
  if(m_color_scheme == 0)
      glColor3f(0, 0, 0.6);

  gl_font(1, 12);

  //---------------------------------- Draw the two zeros
  drawText(m_x_offset+3,  m_y_offset-12, "0");
  drawText(m_x_offset-12, m_y_offset+3, "0");

  //---------------------------------- Draw the domain max
  int dom_size = m_x_grid_width;
  string dom_str = intToString(dom_size-1);
  drawText(m_x_offset+dom_size-10, m_y_offset-12, dom_str);

  //---------------------------------- Draw the domain label
  drawText(m_x_offset+(dom_size/2)-30, m_y_offset-16, "DOMAIN  x");

  //---------------------------------- Draw the util axis
  drawText(m_x_offset-23, m_y_offset+98, "100");
  drawText(m_x_offset-23, m_y_offset+198, "200");
  drawText(m_x_offset-35, m_y_offset+233, "UTIL");
  drawText(m_x_offset-33, m_y_offset+218, "f(x)");
}

//-------------------------------------------------------------
// Procedure: drawText

void UPV_Viewer::drawText(int x, int y, string str)
{
  glRasterPos3f(x, y, 0);
  gl_draw(str.c_str());
}

//-------------------------------------------------------------
// Procedure: setParam

void UPV_Viewer::setParam(string param, string value)
{
  if(param == "gridsize") {
    if((value == "reset") || (value == "default"))
      m_grid_block_size = 50;
    else if(value == "up")
      m_grid_block_size++;
    else if(value == "down") {
      m_grid_block_size--;
      if(m_grid_block_size < 10)
	m_grid_block_size = 10;
    }
  }
  else if(param == "backshade") {
    if((value == "reset") || (value == "default")) 
      m_back_shade = 1.0;
    else if(value == "down")
      m_back_shade -= 0.02;
    else if(value == "up")
      m_back_shade += 0.02;
    if(m_back_shade < 0)
      m_back_shade = 0;
    if(m_back_shade > 10)
      m_back_shade = 10;
  }
  else if(param == "gridshade") {
    if((value == "reset") || (value == "default")) 
      m_grid_shade = 1.0;
    else if(value == "down")
      m_grid_shade -= 0.02;
    else if(value == "up")
      m_grid_shade += 0.02;
    if(m_grid_shade < 0)
      m_grid_shade = 0;
    if(m_grid_shade > 10)
      m_grid_shade = 10;
  }
  else if(param == "lineshade") {
    if((value == "reset") || (value == "default")) 
      m_line_shade = 1.0;
    else if(value == "down")
      m_line_shade -= 0.02;
    else if(value == "up")
      m_line_shade += 0.02;
    if(m_line_shade < 0)
      m_line_shade = 0;
    if(m_line_shade > 10)
      m_line_shade = 10;
  }
}
      
//-------------------------------------------------------------
// Procedure: drawPartitions

void UPV_Viewer::drawPartitions() const
{
  map<string, PartitionRecord>::const_iterator p;
  for(p=m_map_partitions.begin(); p!= m_map_partitions.end(); p++) {
    string varname = p->first;
    map<string, bool>::const_iterator q = m_map_render.find(varname);
    if(q != m_map_render.end()) {
      bool render = q->second;
      if(render) {
	const PartitionRecord& partition = p->second;
	drawPartition(partition);
      }
    }
  }
}

//-------------------------------------------------------------
// Procedure: drawPartition

void UPV_Viewer::drawPartition(const PartitionRecord& partition) const
{
  unsigned int parts = partition.getPartitions();
  if(parts == 0)
    return;

  ColorPack cpak = partition.getColorPack();

  double xdelta = (double)(m_x_grid_width) / (double)(parts);
  
  unsigned int total_entries = partition.getTotalEntries();
  
  for(unsigned int i=0; i<parts; i++) {
    unsigned int part_count = partition.getEntryCount(i);
    if(part_count > 0) {
      double pct = (double)(part_count) / (double)(total_entries);
      double x1 = (double)(i) * xdelta;
      double y1 = 0;
      double x2 = (double)(i+1) * xdelta;
      double y2 = 0;
      double x3 = x2;
      double y3 = pct * m_y_grid_height * 2;
      if(y3 > m_y_grid_height)
	y3 = m_y_grid_height;
      double x4 = x1;
      double y4 = y3;
      drawBar(x1, y1, x2, y2, x3, y3, x4, y4, cpak);
    }
  }
}


//-------------------------------------------------------------
// Procedure: drawBar

void UPV_Viewer::drawBar(double x1, double y1, double x2, double y2,
			 double x3, double y3, double x4, double y4,
			 ColorPack cpak) const
{
  double red_val = cpak.red() * m_line_shade;
  double grn_val = cpak.grn() * m_line_shade;
  double blu_val = cpak.blu() * m_line_shade;

  if(red_val < 0) red_val=0;
  if(red_val > 1) red_val=1;
  if(grn_val < 0) grn_val=0;
  if(grn_val > 1) grn_val=1;
  if(blu_val < 0) blu_val=0;
  if(blu_val > 1) blu_val=1;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  //glTranslatef(0, 0, 0);
  glTranslatef(m_x_offset, m_y_offset, 0);

  // Draw the inside of the bar
  glEnable(GL_BLEND);
  glColor4f(cpak.red(), cpak.grn(), cpak.blu(), 0.5);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glVertex2f(x3, y3);
  glVertex2f(x4, y4);
  glVertex2f(x1, y1);
  glEnd();
  glDisable(GL_BLEND);

  // Draw the lines of the bar
  glColor4f(1.0, 1.0, 1.0,  0.1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glVertex2f(x3, y3);
  glVertex2f(x4, y4);
  glVertex2f(x1, y1);
  glEnd();

  glFlush();
  glPopMatrix();  
}



//-------------------------------------------------------------
// Procedure: addPartition
//   Example: var=GTEST_VAL, min=0, max=400, bars=20

void UPV_Viewer::addPartition(string varname, double min, double max,
			      unsigned int bars, string color)
{
  if((max - min) <= 0)
    return;
  if(bars == 0)
    return;

  // If a partition for this var already exists, just return
  if(m_map_partitions.count(varname) == 1)
    return;

  PartitionRecord new_partition;
  new_partition.init(min, max, bars);
  new_partition.setColor(color);
  m_map_partitions[varname] = new_partition;
  m_map_render[varname] = true;

  if(m_hotvar == "")
    m_hotvar = varname;
}  


//-------------------------------------------------------------
// Procedure: addValue()

void UPV_Viewer::addValue(string varname, double val)
{
  if(m_map_partitions.count(varname) == 0)
    return;

  m_map_partitions[varname].addValue(val);
}

//-------------------------------------------------------------
// Procedure: printPartitions()

void UPV_Viewer::printPartitions() const
{
  map<string, PartitionRecord>::const_iterator p;
  for(p=m_map_partitions.begin(); p!=m_map_partitions.end(); p++)
    p->second.print();
}


//-------------------------------------------------------------
// Procedure: setHotVar()

void UPV_Viewer::setHotVar(string varname)
{
  if(m_map_partitions.count(varname) == 1)
    m_hotvar = varname;
}

//-------------------------------------------------------------
// Procedure: showVar()

void UPV_Viewer::showVar(string varname)
{
  if(m_map_render.count(varname) == 1)
    m_map_render[varname] = true;
}

//-------------------------------------------------------------
// Procedure: hideVar()

bool UPV_Viewer::hideVar(string varname)
{
  // If the variable to be hidden is the hotvar, deny the request
  if(varname == m_hotvar)
    return(false);

  if(m_map_render.count(varname) == 1)
    m_map_render[varname] = false;

  bool something_is_rendered = false;
  map<string, bool>::iterator p;
  for(p=m_map_render.begin(); p!=m_map_render.end(); p++) {
    if(p->second)
      something_is_rendered = true;
  }

  // Ok, we didn't turn off (hide) the last item
  if(something_is_rendered)
    return(true);

  // Woops, we turned off (hid) the last item. Turn it back on
  // and return false to indicate the hide was unsuccessful!
  m_map_render[varname] = true;
  return(false);
}

//-------------------------------------------------------------
// Procedure: getHotVarAvg()

double UPV_Viewer::getHotVarAvg() const
{
  map<string, PartitionRecord>::const_iterator p;
  for(p=m_map_partitions.begin(); p!=m_map_partitions.end(); p++)
    if(p->first == m_hotvar)
      return(p->second.getAverage());

  return(0);
}










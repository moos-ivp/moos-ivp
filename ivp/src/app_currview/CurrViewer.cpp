/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CurrViewer.cpp                                       */
/*    DATE: January 4th, 2011                                    */
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

#include <string>
#include <iostream>
#include <cstdio>
#include <cmath>
#include "CurrViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "FL/fl_ask.H"

using namespace std;

CurrViewer::CurrViewer(int x, int y, 
			 int width, int height, const char *l)
  : MarineViewer(x,y,width,height,l)
{
  m_snap_val      = 1.0;
  m_active_vector = 0;

  // Modes:   0:insert 1:edit 2:delete
  m_drop_mode = 0;     
  m_hash_delta = 10;
  m_hash_shade = 0.3;
  m_hash_offon = true;
}

//-------------------------------------------------------------
// Procedure: handle()

int CurrViewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
    break;
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: draw()

void CurrViewer::draw()
{
  MarineViewer::draw();
  if(m_hash_offon)
    drawHash();

  vector<XYVector>  mvectors = m_current_field.getVectorsMarked();
  vector<XYVector>  uvectors = m_current_field.getVectorsUnMarked();

  unsigned int j, jsize = mvectors.size();
  for(j=0; j<jsize; j++)
    mvectors[j].set_color("edge", "light_blue");

  drawVectors(mvectors);
  drawVectors(uvectors);
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse

//      Left click - select
// Shft Left click - New vector (only new vector marked)
// Ctrl Left click - New vector (keep all prior marked)

void CurrViewer::handle_left_mouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);
  
  if(Fl::event_state(FL_CTRL)) {
    double new_ang = 45;
    double new_mag = 1.0;
    bool active = m_current_field.hasActiveVertex();
    if(active) {
      unsigned int active_ix = m_current_field.getActiveIX();
      XYVector avector = m_current_field.getVector(active_ix);
      new_ang = avector.ang();
      new_mag = avector.mag();
    }
    XYVector new_vector(sx, sy, new_mag, new_ang);
    new_vector.setHeadSize(2); //mikerb
    m_current_field.addVector(new_vector);
    unsigned int last_ix = m_current_field.size() - 1;
    if(Fl::event_state(FL_SHIFT))
      m_current_field.markupVector(last_ix);
    else
      m_current_field.markVector(last_ix);
  }
    
  else {  // Select a vector
    double dist = 0;
    unsigned int ix = m_current_field.selectVector(sx, sy, dist);
    if(dist < 5) {
      if(m_current_field.isMarked(ix)) {
	m_current_field.unmarkVector(ix);
      }
      else {
	if(Fl::event_state(FL_SHIFT))
	  m_current_field.markupVector(ix);
	else
	  m_current_field.markVector(ix);
      }
    }
  }

  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse

void CurrViewer::handle_right_mouse(int vx, int vy)
{
}


//-------------------------------------------------------------
// Procedure: incrementActive

void CurrViewer::incrementActive()
{
  unsigned int curr_size = m_current_field.size();
  if(curr_size == 0)
    return;

  unsigned int curr_ix = m_current_field.getActiveIX() + 1;
  if(curr_ix >= curr_size)
    curr_ix = 0;

  m_current_field.markVector(curr_ix);
}

//-------------------------------------------------------------
// Procedure: decrementActive

void CurrViewer::decrementActive()
{
  unsigned int curr_size = m_current_field.size();
  if(curr_size == 0)
    return;

  unsigned int curr_ix = m_current_field.getActiveIX();
  if(curr_ix > 0)
    curr_ix -= 1;
  else
    curr_ix = curr_size - 1;

  m_current_field.markVector(curr_ix);
}

//-------------------------------------------------------------
// Procedure: setParam

bool CurrViewer::setParam(string param, double pval)
{
  if(MarineViewer::setParam(param, pval))
    return(true);

  else if(param == "mode") {
    m_drop_mode = (int)(pval);
    if(m_drop_mode == 2)
      m_current_field.unmarkAllVectors();
    return(true);
  }
  else if(param == "snap") {
    m_snap_val = pval;
    return(true);
  }
  else
    return(false);
}

// ----------------------------------------------------------
// Procedure: fileSaveAs
//   Purpose: 

bool CurrViewer::fileSaveAs()
{
  string filename;
  const char *str = fl_input("Enter a filename:", m_filename.c_str());
  if(str != 0) {
    string filestr = str;
    filestr = stripBlankEnds(filestr);
    if(!strContainsWhite(filestr))
      filename = filestr;
  }

  if(filename == "")
    return(false);

  if(okFileToRead(filename)) {
    string question = "File " + filename + " already exists. Overwrite?";
    int res = fl_choice(question.c_str(), "No", "YES", 0);
    if(res == 0) // User selects No
      return(false);
  }

  FILE *f = fopen(filename.c_str(), "w");
  if(!f)
    return(false);

  vector<string> vlisting = m_current_field.getListing();
  unsigned int i, vsize = vlisting.size();
  for(i=0; i<vsize; i++) {
    fprintf(f, "%s\n", vlisting[i].c_str());
  }
  fclose(f);
  return(true);
}

// ----------------------------------------------------------
// Procedure: fileSave
//   Purpose: 

bool CurrViewer::fileSave()
{
  FILE *f = fopen(m_filename.c_str(), "w");
  if(!f)
    return(false);

  vector<string> vlisting = m_current_field.getListing();
  unsigned int i, vsize = vlisting.size();
  for(i=0; i<vsize; i++) {
    fprintf(f, "%s\n", vlisting[i].c_str());
  }

  fclose(f);
  return(true);
}

// ----------------------------------------------------------
// Procedure: shiftHorzVector
//   Purpose: 

void CurrViewer::shiftHorzVector(double shift_val)
{
  m_current_field.modMarkedVectors("aug_x", shift_val);
}

// ----------------------------------------------------------
// Procedure: shiftVertVector
//   Purpose: 

void CurrViewer::shiftVertVector(double shift_val)
{
  m_current_field.modMarkedVectors("aug_y", shift_val);
}

// ----------------------------------------------------------
// Procedure: rotateVector
//   Purpose: 

void CurrViewer::rotateVector(double value)
{
  m_current_field.modMarkedVectors("aug_ang", value);
}

// ----------------------------------------------------------
// Procedure: clearMarked
//   Purpose: 

void CurrViewer::clearMarked()
{
  m_current_field.unmarkAllVectors();
}

// ----------------------------------------------------------
// Procedure: deleteMarked
//   Purpose: 

void CurrViewer::deleteMarked()
{
  m_current_field.deleteMarkedVectors();
}

// ----------------------------------------------------------
// Procedure: deleteLastAdded
//   Purpose: Remove the vector at the highest index, which 
//            presumably is the latest one added by the user
//            or the latest one read from the configuration file. 

void CurrViewer::deleteLastAdded()
{
  unsigned int cf_size = m_current_field.size();
  if(cf_size == 0)
    return;

  m_current_field.deleteVector(cf_size-1);
}

// ----------------------------------------------------------
// Procedure: growVector
//   Purpose: 

void CurrViewer::growVector(double value)
{
  m_current_field.modMarkedVectors("aug_mag", value);
}

// ----------------------------------------------------------
// Procedure: applySnap
//   Purpose: 

void CurrViewer::applySnap()
{
  m_current_field.applySnap(m_snap_val);
}

// ----------------------------------------------------------
// Procedure: initCurrentField
//   Purpose: 

bool CurrViewer::initCurrentField(string filename)
{
  bool ok = m_current_field.populate(filename);
  m_current_field.print();

  if(ok)
    m_filename = filename;
  return(ok);
}


// ----------------------------------------------------------
// Procedure: getInfo
//   Purpose: 

string CurrViewer::getInfo(string param, int precision)
{
  param = tolower(stripBlankEnds(param));

  if(param == "v_cnt") {
    unsigned int cnt = m_current_field.size();
    string rval = uintToString(cnt);
    return(rval);
  }

  bool active = m_current_field.hasActiveVertex();
  if(!active)
    return("n/a");

  unsigned int active_ix = m_current_field.getActiveIX();
  XYVector avector = m_current_field.getVector(active_ix);

  if(param == "v_idx") {
    string rstr = uintToString(active_ix);
    return(rstr);
  }
  else if(param == "v_x") {
    double xpos = avector.xpos();
    string rstr = doubleToString(xpos, precision);
    return(rstr);
  }
  else if(param == "v_y") {
    double ypos = avector.ypos();
    string rstr = doubleToString(ypos, precision);
    return(rstr);
  }
  else if(param == "v_lat") {
    double vlat = avector.ypos();
    string rstr = doubleToString(vlat, precision);
    return(rstr);
  }
  else if(param == "v_lon") {
    double vlon = avector.xpos();
    string rstr = doubleToString(vlon, precision);
    return(rstr);
  }
  else if(param == "v_ang") {
    double vang = avector.ang();
    string rstr = doubleToString(vang, precision);
    return(rstr);
  }
  else if(param == "v_mag") {
    double vmag = avector.mag();
    string rstr = doubleToString(vmag, precision);
    return(rstr);
  }

  return("????");
}



